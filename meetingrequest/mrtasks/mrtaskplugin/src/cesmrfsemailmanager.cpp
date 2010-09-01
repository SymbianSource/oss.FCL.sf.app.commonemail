/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for ESMR Email Manager
*
*/

#include "emailtrace.h"
#include "cesmrfsemailmanager.h"

#include <cmrmailboxutils.h>
#include <apgcli.h>
#include <apmrec.h>
//<cmail>
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cfsmailmessage.h"
#include "cfsmailmessagepart.h"
#include "mmrinfoprocessor.h"
#include "cfsmailcommon.h"
#include "cfsmailplugin.h"
//</cmail>
#include <ecom/implementationinformation.h>
#include "tesmrinputparams.h"
#include "esmrconfig.hrh"
#include "esmrhelper.h"
#include "esmrinternaluid.h"

// Unnamed namespace for local definitions
namespace {

// Definition for mime types
_LIT(KMimeTextCalRequest,  "text/calendar; method=REQUEST; name=\"meeting.ics\"");
_LIT(KMimeTextCalResponse, "text/calendar; method=RESPONSE; name=\"response.ics\"");
_LIT(KMimeTextCalCancel,   "text/calendar; method=CANCEL; name=\"cancel.ics\"");
_LIT(KMimeTextCalContentClass, "urn:content-classes:calendarmessage" );

// Definition for temporary calendar file
//<cmail> hardcoded paths removal
//_LIT(KFileAndPath, "c:\\temp\\temp.ics"); //codescanner::driveletters
_LIT(KTempFileName, "temp\\temp.ics");
//</cmail>

// Definition for email address comparison
const TInt KEqualEmailAddress(0);

// Definition for first index
const TInt KFirstIndex(0);

/**
 * Cleanup operations for RPointerArray.
 *
 * @param aArray Pointer to RPointerArray.
 */
void MessagePartPointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CFSMailMessagePart>* messagePartArray =
        static_cast<RPointerArray<CFSMailMessagePart>*>( aArray );

    messagePartArray->ResetAndDestroy();
    messagePartArray->Close();
    }

#ifdef _DEBUG

// Definition for panic text
_LIT( KEMailManagerPanicTxt, "EMailManager" );

/** Enumeration for email manager panic codes */
enum TESMREMailManagerPanic
    {
    EESMRMailboxNotFound = 0,
    EESMRMailClientNotExist,
    EESMRMessageNotExist,
    EESMRInvalidCalMethod
    };

// ---------------------------------------------------------------------------
// Panic wrapper method
// ---------------------------------------------------------------------------
//
void Panic( TESMREMailManagerPanic aPanic )
    {

    User::Panic( KEMailManagerPanicTxt, aPanic );
    }

#endif // _DEBUG

// ---------------------------------------------------------------------------
// Leave wrapper method
// ---------------------------------------------------------------------------
//
inline void DoLeaveIfErrorL( TInt aLeaveCode )
    {
    if ( KErrNone != aLeaveCode )
        {

        User::Leave( aLeaveCode );
        }
    }

// ---------------------------------------------------------------------------
// Cleanup operation for RPointerArray.
// @param aArray Pointer to RPointerArray.
// ---------------------------------------------------------------------------
//
void MailboxPointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CFSMailBox>* entryArray =
        static_cast<RPointerArray<CFSMailBox>*>( aArray );

    entryArray->ResetAndDestroy();
    entryArray->Close();
    }
} // namespace


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::CESMRFSEMailManager
// ---------------------------------------------------------------------------
//
inline CESMRFSEMailManager::CESMRFSEMailManager(
        CMRMailboxUtils& aMRMailboxUtils )
:   iMRMailboxUtils( aMRMailboxUtils )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::§CESMRFSEMailManager
// ---------------------------------------------------------------------------
//
CESMRFSEMailManager::~CESMRFSEMailManager()
    {
    FUNC_LOG;
    
    delete iParentPart;
    delete iMessage;
    delete iMailBox;    
    
    if ( iMailClient )
        {
        iMailClient->Close();
        iMailClient = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::NewL
// ---------------------------------------------------------------------------
//
CESMRFSEMailManager* CESMRFSEMailManager::NewL(
        CMRMailboxUtils& aMRMailboxUtils  )
    {
    FUNC_LOG;

    CESMRFSEMailManager* self =
        new (ELeave) CESMRFSEMailManager(aMRMailboxUtils);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::PrepareForSendingL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::PrepareForSendingL(
        const TDesC& aMailboxOwnerAddr )
    {
    FUNC_LOG;

    iMailClient = CFSMailClient::NewL();

    TPtrC emailAddr(
            ESMRHelper::AddressWithoutMailtoPrefix( aMailboxOwnerAddr ) );

    iMailBox = SelectMailBoxL( emailAddr );
    iMessage = CreateMessageL();
    iParentPart = CreateParentPartL();
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::PrepareForSendingreplyL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::PrepareForSendingReplyL(
        const TDesC& aMailboxOwnerAddr, 
        const TFSMailMsgId& aMsgId, 
        TBool aReplyAll )
    {
    FUNC_LOG;

    iMailClient = CFSMailClient::NewL();

    TPtrC emailAddr(
            ESMRHelper::AddressWithoutMailtoPrefix( aMailboxOwnerAddr ) );

    iMailBox = SelectMailBoxL( emailAddr );

    iMessage = iMailBox->CreateReplyMessage(aMsgId, aReplyAll );

    iParentPart = CreateParentPartL();
    }


// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SetSenderL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::SetSenderL(
        const TDesC& aAddress,
        const TDesC& aCommonName )
    {
    FUNC_LOG;

    CFSMailAddress* sender = CFSMailAddress::NewLC();

    TPtrC emailAddr(
            ESMRHelper::AddressWithoutMailtoPrefix( aAddress ) );

    sender->SetEmailAddress( emailAddr );
    sender->SetDisplayName( aCommonName );
    iMessage->SetSender( sender ); // takes ownership
    CleanupStack::Pop( sender );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SetReplyToAddressL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::SetReplyToAddressL(
        const TDesC& aAddress,
        const TDesC& aCommonName )
    {
    FUNC_LOG;

    CFSMailAddress* replyToAddress = CFSMailAddress::NewLC();

    TPtrC emailAddr(
            ESMRHelper::AddressWithoutMailtoPrefix( aAddress ) );

    replyToAddress->SetEmailAddress( emailAddr );
    replyToAddress->SetDisplayName( aCommonName );
    iMessage->SetReplyToAddress( replyToAddress ); // takes ownership
    CleanupStack::Pop( replyToAddress );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::AppendToRecipientL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::AppendToRecipientL(
        const TDesC& aAddress,
        const TDesC& aCommonName )
    {
    FUNC_LOG;

    CFSMailAddress* toRecipient = CFSMailAddress::NewLC();

    TPtrC emailAddr(
            ESMRHelper::AddressWithoutMailtoPrefix( aAddress ) );

    toRecipient->SetEmailAddress( emailAddr );
    toRecipient->SetDisplayName( aCommonName );
    iMessage->AppendToRecipient( toRecipient ); // takes ownership
    CleanupStack::Pop( toRecipient );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::AppendCCRecipientL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::AppendCCRecipientL(
        const TDesC& aAddress,
        const TDesC& aCommonName )
    {
    FUNC_LOG;

    CFSMailAddress* ccRecipient = CFSMailAddress::NewLC();

    TPtrC emailAddr(
            ESMRHelper::AddressWithoutMailtoPrefix( aAddress ) );

    ccRecipient->SetEmailAddress( emailAddr );
    ccRecipient->SetDisplayName( aCommonName );
    iMessage->AppendCCRecipient( ccRecipient ); // takes ownership
    CleanupStack::Pop( ccRecipient );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SetSubjectL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::SetSubjectL(const TDesC& aSubject)
    {
    FUNC_LOG;

    iMessage->SetSubject( aSubject );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::CreateTextPlainPartL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::CreateTextPlainPartL(const TDesC& aPlainText)
    {
    FUNC_LOG;

    AddPlainTextPartL( aPlainText );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::CreateTextCalendarPartL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::CreateTextCalendarPartL(
        TESMRMethod aCalendarMethod )
    {
    FUNC_LOG;
    //<cmail> removing hardcoded paths
    TFileName fileName(KTempFileName);
    User::LeaveIfError(ESMRHelper::CreateAndAppendPrivateDirToFileName(fileName));
    CreateTextCalendarPartL( aCalendarMethod, fileName );
    //</cmail>

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::CreateTextCalendarPartL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::CreateTextCalendarPartL(
        TESMRMethod aCalendarMethod,
        const TDesC& aFilename  )
    {
    FUNC_LOG;

    TPtrC contentType;
    switch ( aCalendarMethod )
        {
        case EESMRMethodRequest:
            {
            contentType.Set(KMimeTextCalRequest());
            }
            break;

        case EESMRMethodResponse:
            {
            contentType.Set(KMimeTextCalResponse());
            }
            break;

        case EESMRMethodCancel:
            {
            contentType.Set(KMimeTextCalCancel());
            }
            break;

        default:
            __ASSERT_DEBUG( EFalse, Panic(EESMRInvalidCalMethod) );
        }

    AddMessagePartFromFileL(
            contentType,
            KMimeTextCalContentClass,
            KNullDesC(),
            KNullDesC(),
            aFilename );

    }


// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SetAttachmentL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::SetAttachmentL(const TDesC& aAttachmentFile )
    {
    FUNC_LOG;

    TFSMailMsgId nullId;
    CFSMailMessagePart* attachmentPart =
        iMessage->AddNewAttachmentL(aAttachmentFile, nullId );
    CleanupStack::PushL( attachmentPart );
    attachmentPart->SaveL();
    iMessage->SaveMessageL();

    CleanupStack::PopAndDestroy( attachmentPart );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SetAttachmentL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::SetAttachmentL(
            RFile& aFile,
            const TDesC8& aMimeType )
    {
    FUNC_LOG;
    
    CFSMailMessagePart* attachmentPart = iMessage->AddNewAttachmentL( aFile, aMimeType ); 
    CleanupStack::PushL( attachmentPart );
    attachmentPart->SaveL();
    iMessage->SaveMessageL();

    CleanupStack::PopAndDestroy( attachmentPart );
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::StoreMessageL
// ---------------------------------------------------------------------------
//
TInt CESMRFSEMailManager::StoreMessageToDraftsFolderL()
    {
    FUNC_LOG;
    // Stores message automatically to drafts folder
    // check that msg is not in drafts folder already
    TFSMailMsgId draftsFolderId =
            iMailBox->GetStandardFolderId( EFSDraftsFolder );
    TFSMailMsgId msgFolderId = iMessage->GetFolderId();
    if ( draftsFolderId != msgFolderId )
        {
        RArray<TFSMailMsgId> ids;
        CleanupClosePushL( ids );
        ids.Append( iMessage->GetMessageId() );
        iMailBox->MoveMessagesL( ids, msgFolderId, draftsFolderId );
        CleanupStack::PopAndDestroy(); // ids
        }

    iMessage->SaveMessageL();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SendMessageL
// ---------------------------------------------------------------------------
//
TInt CESMRFSEMailManager::SendMessageL()
    {
    FUNC_LOG;

    TRAPD( ret, iMailBox->SendMessageL( *iMessage ) );


    return ret;
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::MailboxId
// ---------------------------------------------------------------------------
//
TFSMailMsgId CESMRFSEMailManager::MailboxId() const
    {
    FUNC_LOG;
    return iMailBox->GetId();
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SendMailViaSyncL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::SendMailViaSyncL(
        TESMRInputParams& aParams,
        MMRInfoObject& aInfoObject,
        MMRInfoProcessor::TMRInfoResponseMode aResponseMode,
        const TDesC& aFreeResponseText )
    {
    FUNC_LOG;

    CFSMailBox* mailBox =
        aParams.iMailClient->GetMailBoxByUidL(
                aParams.iMailMessage->GetMailBoxId() );
    CleanupStack::PushL( mailBox );

    if ( mailBox->IsMRInfoProcessorSet() )
        {
        MMRInfoProcessor& infoProcessor(
                mailBox->MRInfoProcessorL() );


        TRAPD(err, {
                infoProcessor.ReplyToMeetingRequestL(
                            aParams.iMailMessage->GetMailBoxId(),
                            aParams.iMailMessage->GetMessageId(),
                            aInfoObject,
                            aResponseMode,
                            aFreeResponseText );
                    } );



        User::LeaveIfError( err );
        }

    CleanupStack::PopAndDestroy( mailBox );

    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::MessageId
// ---------------------------------------------------------------------------
//
TFSMailMsgId CESMRFSEMailManager::MessageId() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iMessage, Panic(EESMRMessageNotExist) );
    return iMessage->GetMessageId();
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::MessageFolderId
// ---------------------------------------------------------------------------
//
TFSMailMsgId CESMRFSEMailManager::MessageFolderId() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iMessage, Panic(EESMRMessageNotExist) );
    return iMessage->GetFolderId();
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::CreateMessageL
// ---------------------------------------------------------------------------
//
CFSMailMessage* CESMRFSEMailManager::CreateMessageL()
    {
    FUNC_LOG;

    CFSMailMessage* msg = iMailBox->CreateMessageToSend();

    msg->SetFlag( EFSMsgFlag_CalendarMsg );
    msg->SaveMessageL();

    return msg;
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::CreateParentPartL
// ---------------------------------------------------------------------------
//
CFSMailMessagePart* CESMRFSEMailManager::CreateParentPartL()
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iMessage, Panic(EESMRMessageNotExist) );

    CFSMailMessagePart* parent = NULL;
    RPointerArray<CFSMailMessagePart> messageParts;
    CleanupStack::PushL(
            TCleanupItem(
                MessagePartPointerArrayCleanup,
                &messageParts    ) );

    iMessage->ChildPartsL( messageParts );

    if( messageParts.Count() )
        {
        parent = messageParts[ KFirstIndex ];
        messageParts.Remove( KFirstIndex );
        }
    else
        {
        // Message does not contain body part --> Create it
        TFSMailMsgId id;
        parent = iMessage->NewChildPartL(
                        id,
                        KFSMailContentTypeMultipartAlternative );
        }

    CleanupStack::PopAndDestroy(); // messageParts
    CleanupStack::PushL( parent );

    parent->SetContentType( KFSMailContentTypeMultipartAlternative );
    parent->SaveL();
    iMessage->SaveMessageL();

    CleanupStack::Pop( parent );
    return parent;
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::SelectMailBoxL
// ---------------------------------------------------------------------------
//
CFSMailBox* CESMRFSEMailManager::SelectMailBoxL(
        const TDesC& aMailboxOwnerAddr )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iMailClient, Panic(EESMRMailClientNotExist) );

    CFSMailBox* mailbox = NULL;

    // Loop throug all mailboxes in this plug-in
    RPointerArray<CFSMailBox> mailboxes;
    CleanupStack::PushL(
            TCleanupItem(
                    MailboxPointerArrayCleanup,
                    &mailboxes    ) );

    // With null uid we get all mailboxes
    TFSMailMsgId msgId;
    iMailClient->ListMailBoxes(
            msgId, mailboxes );

    TInt mailboxCount( mailboxes.Count() );
    for (TInt j(0); j < mailboxCount && !mailbox; ++j )
        {
        TPtrC mailboxOwnerAddName(
                mailboxes[j]->OwnMailAddress().GetEmailAddress() );
        if ( KEqualEmailAddress == mailboxOwnerAddName.CompareF(aMailboxOwnerAddr) )
            {
            // Correct mailbox is found
            mailbox = mailboxes[j];
            mailboxes.Remove(j);
            }
        }
    CleanupStack::PopAndDestroy( &mailboxes );

    __ASSERT_DEBUG( mailbox, Panic(EESMRMailboxNotFound) );

    if ( !mailbox )
        {
        DoLeaveIfErrorL( KErrNotFound );
        }


    return mailbox;
    }

// ---------------------------------------------------------------------------
// CESMRFSEMailManager::AddMessagePartFromFileL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::AddMessagePartFromFileL(
        const TDesC& aContentType,
        const TDesC& aContentClass,
        const TDesC& aContentDescription,
        const TDesC& aContentDisposition,
        const TDesC& aFileAndPath )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iParentPart, Panic(EESMRMessageNotExist) );

    TFSMailMsgId nullId;

    CFSMailMessagePart* messagePart =
        iParentPart->AddNewAttachmentL(
                aFileAndPath,
                nullId,
                aContentType );

    CleanupStack::PushL( messagePart );

    messagePart->SetMailBoxId( iMailBox->GetId() );
    messagePart->SetContentDescription( KNullDesC() );
    messagePart->SetContentType( KNullDesC() );
    messagePart->SetContentDisposition( KNullDesC() );
    messagePart->SetContentClass( KNullDesC() );

    if ( aContentType.Length() )
        {
        messagePart->SetContentType( aContentType );
        }

    if ( aContentDescription.Length() )
        {
        messagePart->SetContentDescription( aContentDescription );
        }

    if ( aContentDisposition.Length() )
        {
        messagePart->SetContentDisposition( aContentDisposition );
        }

    if ( aContentClass.Length() )
        {
        messagePart->SetContentClass( aContentClass );
        }

    messagePart->SaveL();
    iMessage->SaveMessageL();

    CleanupStack::PopAndDestroy( messagePart );

    }
// ---------------------------------------------------------------------------
// CESMRFSEMailManager::AddPlainTextPartL
// ---------------------------------------------------------------------------
//
void CESMRFSEMailManager::AddPlainTextPartL(
        const TDesC& aContent )
    {
    FUNC_LOG;
    TFSMailMsgId id;
    CFSMailMessagePart* plainTextPart =
        iParentPart->FindBodyPartL(KFSMailContentTypeTextPlain);

    if ( !plainTextPart )
        {
        // text/plain part does not exist --> Create it to body part
        plainTextPart =
            iParentPart->NewChildPartL(id,  KFSMailContentTypeTextPlain );
        }

    CleanupStack::PushL( plainTextPart );

    plainTextPart->SetMailBoxId( iMailBox->GetId() );

    TUint currentContentSize = plainTextPart->ContentSize();
    TUint contentBufferSize  = currentContentSize + aContent.Length();

    // Buffer for current content
    HBufC* currentContentBuffer = HBufC::NewLC( currentContentSize );
    TPtr currentContent( currentContentBuffer->Des() );

    // Buffer for content containing calendar information and current content
    HBufC* contentBuffer = HBufC::NewLC( contentBufferSize );
    TPtr content( contentBuffer->Des() );

    // Appending calendar information to content
    content.Copy( aContent );
    if ( currentContentSize )
        {
        // Appending current content if needed
        plainTextPart->GetContentToBufferL( currentContent, 0 );
        content.Append( currentContent );
        }

    plainTextPart->SetContent(content);
    CleanupStack::PopAndDestroy( contentBuffer ); contentBuffer = NULL;
    CleanupStack::PopAndDestroy( currentContentBuffer ); currentContentBuffer = NULL;

    plainTextPart->SaveL();
    iMessage->SaveMessageL();

    CleanupStack::PopAndDestroy( plainTextPart );

    }

// EOF

