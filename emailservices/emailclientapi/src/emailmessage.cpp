/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: This file implements class CEmailMessage.
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#include <vwsdefpartner.h>
#else
#include <viewcli.h>
#include <vwsdef.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include "emailmessage.h"
#include "emailaddress.h"
#include "emailapiutils.h"
#include "emailclientapi.hrh"
#include "emailapiutils.h"
#include "emailinterfacefactoryimpl.h"
#include "emailcontent.h"
#include "emailtextcontent.h"
#include "emailmultipart.h"
#include "emailattachment.h"
#include "CFSMailPlugin.h"
#include "FreestyleEmailUiConstants.h"
#include "CFSMailClient.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessage* CEmailMessage::NewL( CPluginData& aPluginData,
                                    CFSMailMessage* aFsMessage,
                                    const TDataOwner aOwner )
    {
    CEmailMessage* message = new ( ELeave ) CEmailMessage( aPluginData, aFsMessage, aOwner );
    CleanupStack::PushL( message );
    message->ConstructL();
    CleanupStack::Pop();
    return message;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessage::CEmailMessage( 
        CPluginData& aPluginData,
        CFSMailMessage *aFsMessage,
        const TDataOwner aOwner)
        : iPluginData( aPluginData ), iPluginMessage( aFsMessage ), iOwner( aOwner )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::ConstructL()
    {    
    iPlugin = iPluginData.ClaimInstanceL();
    if ( iPluginMessage )
        {
        iMessageId = TMessageId( 
            iPluginMessage->GetMessageId().Id(),
            iPluginMessage->GetFolderId().Id(), 
            iPluginMessage->GetMailBoxId().Id() );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessage::~CEmailMessage()
    {
    delete iPluginMessage;    
    delete iSender;
    delete iReplyTo;
    delete iTextContent;
    delete iContent;
    iPluginData.ReleaseInstance();
    iAttachments.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailMessage::InterfaceId() const
    {
    return KEmailIFUidMessage;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::Release()
    {
    if ( iOwner == EClientOwns )
        {
        delete this;        
        }
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
const TMessageId& CEmailMessage::MessageId() const
    {
    return iMessageId;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAddress* CEmailMessage::SenderAddressL() const
    {
    CFSMailAddress* fsAddress = iPluginMessage->GetSender();
    if (fsAddress)
        {
        if (!iSender)
            {
            iSender = CEmailAddress::NewL( MEmailAddress::ESender, EAPIOwns );
            }
        iSender->SetAddressL(fsAddress->GetEmailAddress());
        iSender->SetDisplayNameL(fsAddress->GetDisplayName());
        }
    else
        {
        // Sender address not defined. Delete client object as well
        delete iSender;
        iSender = NULL;
        }
    return iSender;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAddress* CEmailMessage::ReplyToAddressL() const
    {
    const CFSMailAddress& fsAddress = iPluginMessage->GetReplyToAddress();
    if ( &fsAddress )
        {
        if (!iReplyTo)
            {
            iReplyTo = CEmailAddress::NewL( MEmailAddress::EReplyTo, EAPIOwns );
            }
        iReplyTo->SetAddressL( fsAddress.GetEmailAddress() );
        iReplyTo->SetDisplayNameL( fsAddress.GetDisplayName() );
        }
    else
        {
        delete iReplyTo;
        iReplyTo = NULL;
        }
    return iReplyTo;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetReplyToAddressL( const MEmailAddress& aSender )
    {
    CFSMailAddress *fsAddress = CFSMailAddress::NewLC();
    if (!iReplyTo)
        {
        iReplyTo = CEmailAddress::NewL( MEmailAddress::EReplyTo, EAPIOwns);
        }
    fsAddress->SetDisplayName( aSender.DisplayName() );
    fsAddress->SetEmailAddress( aSender.Address() );
    iPluginMessage->SetReplyToAddress( fsAddress );
    iReplyTo->SetAddressL( fsAddress->GetEmailAddress() );
    iReplyTo->SetDisplayNameL( fsAddress->GetDisplayName() );
    CleanupStack::Pop();
    }       
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMessage::GetRecipientsL( const MEmailAddress::TRole aRole,
        REmailAddressArray& aRecipients ) const
    {
    if( aRole == MEmailAddress::EReplyTo ||
        aRole == MEmailAddress::ESender    )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        if( aRole == MEmailAddress::ETo || 
            aRole == MEmailAddress::EUndefined )
            {
            RPointerArray<CFSMailAddress>& toRecipients = 
                iPluginMessage->GetToRecipients();
            ConvertAddressArrayL( 
                    MEmailAddress::ETo, 
                    toRecipients, aRecipients );
            }
        if( aRole == MEmailAddress::ECc || 
            aRole == MEmailAddress::EUndefined )
            {
            RPointerArray<CFSMailAddress>& ccRecipients = 
                iPluginMessage->GetCCRecipients();
            ConvertAddressArrayL( 
                    MEmailAddress::ECc, 
                    ccRecipients, aRecipients );
            }
        if( aRole == MEmailAddress::EBcc || 
            aRole == MEmailAddress::EUndefined )
            {
            RPointerArray<CFSMailAddress>& bccRecipients = 
                iPluginMessage->GetBCCRecipients();
            ConvertAddressArrayL( 
                    MEmailAddress::EBcc, 
                    bccRecipients, aRecipients );
            }
        }
    return aRecipients.Count();
    }            
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetRecipientsL( const MEmailAddress::TRole aRole, 
        REmailAddressArray& aRecipients )
    {
    TInt count( aRecipients.Count() );
    
    for( TInt i=0;i<count;i++ )
        {
        const MEmailAddress* address = aRecipients[i];
        CFSMailAddress* fsAddress = CFSMailAddress::NewLC();
        fsAddress->SetEmailAddress( address->Address() );
        fsAddress->SetDisplayName( address->DisplayName() );
        
        if( aRole == MEmailAddress::ETo )
            {
            iPluginMessage->AppendToRecipient( fsAddress );
            }
        else if( aRole == MEmailAddress::ECc )
            {
            iPluginMessage->AppendCCRecipient( fsAddress );
            }
        else if( aRole == MEmailAddress::EBcc )
            {
            iPluginMessage->AppendBCCRecipient( fsAddress );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        CleanupStack::Pop( fsAddress );
        }
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::RemoveRecipientL( const MEmailAddress& aRecipient )
    {
    TInt err( KErrNotFound );
    RPointerArray<CFSMailAddress>* recipients = NULL;

    switch (aRecipient.Role())
        {
        case MEmailAddress::ETo:
            recipients = &iPluginMessage->GetToRecipients();
            break;
        case MEmailAddress::ECc:
            recipients = &iPluginMessage->GetCCRecipients();
            break;
        case MEmailAddress::EBcc:
            recipients = &iPluginMessage->GetBCCRecipients();
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }
    
    for( TInt i = 0; i < recipients->Count(); i++ )
        {
        if ( !aRecipient.Address().Compare( (*recipients)[i]->GetEmailAddress() ) )
            {
            recipients->Remove(i);
            err = KErrNone;
            // We could break now. But let's loop if there are several entries
            }
        }
    User::LeaveIfError( err );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMessage::Subject() const
    {
    return iPluginMessage->GetSubject();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void  CEmailMessage::SetSubjectL( const TPtrC& aSubject)
    {
    iPluginMessage->SetSubject( aSubject );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TTime CEmailMessage::Date() const
    {
    return iPluginMessage->GetDate();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMessage::Flags() const
    {
    return iFlags;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetFlag( const TUint aFlag )
    {
    iFlags |= aFlag;
    TUint flag = MapFlags( aFlag );
    iPluginMessage->SetFlag( flag );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::ResetFlag( const TUint aFlag )
    {
    iFlags &= ~aFlag;
    TUint flag = MapFlags( aFlag );
    iPluginMessage->ResetFlag( flag );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMessageContent* CEmailMessage::ContentL() const
    {
    if (iTextContent)
        {
        return iTextContent;        
        }
    if (iContent)
        {
        return iContent;
        }

    RPointerArray<CFSMailMessagePart> parts;
    CleanupResetAndDestroyPushL( parts );
    iPluginMessage->ChildPartsL(parts);
    TInt count( parts.Count() );
    if( count == 0 )
        {
        /* No content, return NULL */
        CleanupStack::Pop(); // parts
        return NULL;
        }
    CFSMailMessagePart* part = parts[0];
    const TDesC& contentType = part->GetContentType();
    TMessageContentId msgContentId = TMessageContentId( 
                        part->GetPartId().Id(),
                        iMessageId.iId,
                        iMessageId.iFolderId.iId,
                        iMessageId.iFolderId.iMailboxId ); 

    if (!contentType.Compare(KFSMailContentTypeTextPlain) || 
        !contentType.Compare(KFSMailContentTypeTextHtml))
        {                                
        iTextContent = CEmailTextContent::NewL(iPluginData, msgContentId, part, EAPIOwns );        
        }
    else if (!contentType.Compare(KFSMailContentTypeMultipartMixed) ||
             !contentType.Compare(KFSMailContentTypeMultipartAlternative) ||
             !contentType.Compare(KFSMailContentTypeMultipartDigest) ||
             !contentType.Compare(KFSMailContentTypeMultipartRelated) ||
             !contentType.Compare(KFSMailContentTypeMultipartParallel))
        {
        iContent = CEmailMultipart::NewL(iPluginData, msgContentId, part, EAPIOwns);
        }
    if (count == 2 )
        {
        CFSMailMessagePart* part = parts[1];
        const TDesC& contentType = part->GetContentType();
        }
    CleanupStack::Pop(); // parts
    
    if (iTextContent)
        {
        return iTextContent;        
        }
    return iContent;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetContentL( const MEmailMessageContent*  aContent )
    {
    MEmailTextContent* textContent = aContent->AsTextContentOrNull();
    if (textContent)
        {
        if (iTextContent)
            {
            delete iTextContent; // Destroy old content
            }
        iTextContent = dynamic_cast<CEmailTextContent*>(textContent);
        iTextContent->SetOwner( EAPIOwns );
        return;
        }
    MEmailMultipart* mPart = aContent->AsMultipartOrNull();
    if (mPart)
        {
        if (iContent)
            {
            delete iContent;
            }
        iContent = dynamic_cast<CEmailMultipart*>(mPart);
        iContent->SetOwner( EAPIOwns );
        }    
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetPlainTextBodyL( const TDesC& aPlainText )
    {
    if (iTextContent)
        {
        iTextContent->SetTextL( MEmailTextContent::EPlainText, aPlainText );
        return;
        }
    CFSMailMessagePart* msgTextPart = iPluginMessage->PlainTextBodyPartL();
    
    if( !msgTextPart )
        {
        msgTextPart = iPluginMessage->NewChildPartL( TFSMailMsgId(), KFSMailContentTypeTextPlain );        
        }
    CleanupStack::PushL( msgTextPart );
    
    TMessageContentId msgContentId = TMessageContentId( 
                        msgTextPart->GetPartId().Id(),
                        iMessageId.iId,
                        iMessageId.iFolderId.iId,
                        iMessageId.iFolderId.iMailboxId );
    
    msgTextPart->SetContentType( KFSMailContentTypeTextPlain );    
    iTextContent = CEmailTextContent::NewL( iPluginData, msgContentId, msgTextPart, EAPIOwns );
    if (iTextContent)
        {
        iTextContent->SetTextL( MEmailTextContent::EPlainText, aPlainText );
        }
    CleanupStack::Pop(); // msgTextPart
    
    return;
    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAttachment* CEmailMessage::AddAttachmentL( const TDesC& aFullPath )
    {
    CFSMailMessagePart* part = iPluginMessage->AddNewAttachmentL(aFullPath, TFSMailMsgId());
    CleanupStack::PushL( part );    
    CEmailAttachment* att = CEmailAttachment::NewLC(iPluginData, iMsgContentId, part, EAPIOwns);
    iAttachments.AppendL( att );
    CleanupStack::Pop(2); // part, att
    
    return att;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------

MEmailAttachment* CEmailMessage::AddAttachmentL( RFile& aFile )
    {
    TBufC8 <1> mime;
    CFSMailMessagePart* part = iPluginMessage->AddNewAttachmentL(aFile, mime);
    CleanupStack::PushL( part );
    CEmailAttachment* att = CEmailAttachment::NewLC(iPluginData, iMsgContentId, part, EAPIOwns);
    iAttachments.AppendL( att );

    CleanupStack::Pop(); // part, att
    
    return att;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMessage::GetAttachmentsL( REmailAttachmentArray& aAttachments )
    {
    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyPushL( attachments );
    iPluginMessage->AttachmentListL(attachments);
    const TInt count( attachments.Count() );
    for (TInt i = 0; i < count; i++)
        {
            TMessageContentId msgContentId = TMessageContentId( 
                            attachments[i]->GetPartId().Id(),
                            iMessageId.iId,
                            iMessageId.iFolderId.iId,
                            iMessageId.iFolderId.iMailboxId ); 

            CEmailAttachment* att = CEmailAttachment::NewL(iPluginData, msgContentId, attachments[i], EClientOwns);
            
            aAttachments.AppendL(att);
        }
    CleanupStack::Pop(); // attachments
    return count;    
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::RemoveAttachmentL( const MEmailAttachment& aAttachment  )
    {
    iPluginMessage->RemoveChildPartL(FsMsgId( iPluginData, aAttachment.Id()));
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
const TFolderId& CEmailMessage::ParentFolderId() const
    {
    return iMessageId.iFolderId;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SaveChangesL()
    {
    TFSMailMsgId mailboxId( 
            FsMsgId( iPluginData, iMessageId.iFolderId.iMailboxId ) );
    
    iPlugin->StoreMessageL( mailboxId, *iPluginMessage );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SendL()
    {
    SaveChangesL();
    iPlugin->SendMessageL( *iPluginMessage );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::ConvertAddressArrayL( 
        const MEmailAddress::TRole aRole,
        RPointerArray<CFSMailAddress>& aSrc, 
        REmailAddressArray& aDst ) const
    {
    for ( TInt i=0; i<aSrc.Count(); i++ )
        {
        CEmailAddress* recipient = CreateAddressLC( aRole, *aSrc[i] );
        aDst.AppendL( recipient );
        CleanupStack::Pop( recipient );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CEmailAddress* CEmailMessage::CreateAddressLC( 
        const MEmailAddress::TRole aRole, 
        CFSMailAddress& aFsAddress ) const
    {
    CEmailAddress* address = CEmailAddress::NewLC( aRole, EClientOwns );
    TDesC& temp1 = aFsAddress.GetEmailAddress();
    if ( &temp1 )
        {
        address->SetAddressL( temp1 );
        }
    TDesC& temp2 = aFsAddress.GetDisplayName();
    if ( &temp2 )
        {
        address->SetDisplayNameL( temp2 );
        }   
    return address;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint CEmailMessage::MapFlags( const TUint& aFlag )
    {
    TUint flag = 0;
    switch( aFlag )
        {
        case EFlag_Read:
            flag = EFSMsgFlag_Read;
            break;
        case EFlag_Read_Locally:
            flag = EFSMsgFlag_Read_Locally;
            break;
        case EFlag_Low:
            flag = EFSMsgFlag_Low;
            break;
        case EFlag_Important:
            flag = EFSMsgFlag_Important;
            break;
        case EFlag_FollowUpComplete:
            flag = EFSMsgFlag_FollowUpComplete;
            break;
        case EFlag_FollowUp:
            flag = EFSMsgFlag_FollowUp;
            break;
        case EFlag_Attachments:
            flag = EFSMsgFlag_Attachments;
            break;
        case EFlag_Multiple:
            flag = EFSMsgFlag_Multiple;
            break;
        case EFlag_CalendarMsg:
            flag = EFSMsgFlag_CalendarMsg;
            break;
        case EFlag_Answered:
            flag = EFSMsgFlag_Answered;
            break;
        case EFlag_Forwarded:
            flag = EFSMsgFlag_Forwarded;
            break;
        case EFlag_OnlyToMe:
            flag = EFSMsgFlag_OnlyToMe;
            break;
        case EFlag_RemoteDeleted:
            flag = EFSMsgFlag_RemoteDeleted;
            break;
        case EFlag_HasMsgSender:
            flag = EFSMsgFlag_HasMsgSender;
            break;
        default:
            break;
        }
    return flag;
    }

void CEmailMessage::ShowMessageViewerL( )
    {   
    THtmlViewerActivationData htmlData;
    htmlData.iActivationDataType = THtmlViewerActivationData::EMailMessage;
    htmlData.iMailBoxId = FsMsgId(iPluginData, iMessageId.iFolderId.iMailboxId);
    htmlData.iFolderId = FsMsgId(iPluginData, iMessageId.iFolderId);
    htmlData.iMessageId = FsMsgId(iPluginData, iMessageId);
    TPckgBuf<THtmlViewerActivationData> pckgData( htmlData );
    CVwsSessionWrapper* viewSrvSession = CVwsSessionWrapper::NewLC();
    viewSrvSession->ActivateView(TVwsViewId(KFSEmailUiUid, HtmlViewerId), KHtmlViewerOpenNew, pckgData);
    CleanupStack::PopAndDestroy();
    }    

/** 
 * Launches Email application and new reply message in editor. 
 * The method follows "fire and forget" pattern, returns immediately.
 */
void CEmailMessage::ReplyToMessageL( const TBool aReplyToAll )
    {
    TEditorLaunchParams editorLaunchData;
    editorLaunchData.iMailboxId = FsMsgId(iPluginData, iMessageId.iFolderId.iMailboxId);
    editorLaunchData.iFolderId = FsMsgId(iPluginData, iMessageId.iFolderId);
    editorLaunchData.iMsgId = FsMsgId(iPluginData, iMessageId);
    editorLaunchData.iActivatedExternally = ETrue; 
    
    TPckgBuf<TEditorLaunchParams> pckgData( editorLaunchData );
    CVwsSessionWrapper* viewSrvSession = CVwsSessionWrapper::NewLC();
    TUid command = TUid::Uid(KEditorCmdReplyAll);
    if ( !aReplyToAll )
        {
        command = TUid::Uid(KEditorCmdReply);
        }
    viewSrvSession->ActivateView(TVwsViewId(KFSEmailUiUid, MailEditorId), command, pckgData);
    CleanupStack::PopAndDestroy();   
    }

void CEmailMessage::ForwardMessageL()
    {
    TEditorLaunchParams editorLaunchData;
    editorLaunchData.iMailboxId = FsMsgId(iPluginData, iMessageId.iFolderId.iMailboxId);
    editorLaunchData.iFolderId = FsMsgId(iPluginData, iMessageId.iFolderId);
    editorLaunchData.iMsgId = FsMsgId(iPluginData, iMessageId);
    editorLaunchData.iActivatedExternally = ETrue; 
    
    TPckgBuf<TEditorLaunchParams> pckgData( editorLaunchData );
    CVwsSessionWrapper* viewSrvSession = CVwsSessionWrapper::NewLC();
    TUid command = TUid::Uid(KEditorCmdForward);
    viewSrvSession->ActivateView(TVwsViewId(KFSEmailUiUid, MailEditorId), command, pckgData);
    CleanupStack::PopAndDestroy();   
    }
// End of file.
