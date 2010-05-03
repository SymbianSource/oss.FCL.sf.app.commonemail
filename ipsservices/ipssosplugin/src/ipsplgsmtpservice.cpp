/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgSmtpService.
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

// <cmail> const TInt KIpsSetUtilsCharAt = '@'; </cmail>
const TInt KIpsSetUtilsCharLessThan = '<';
const TInt KIpsSetUtilsCharMoreThan = '>';
const TInt KIpsSetUtilsCntrCharacters = 2;

// do we need smtp object instead set of static functions

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CIpsPlgSmtpService
// ---------------------------------------------------------------------------
//
CIpsPlgSmtpService::CIpsPlgSmtpService(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin ) :
    iPlugin( aPlugin ),
    iSession( aSession), 
    iMsgMapper( NULL )
    {
    FUNC_LOG;
    }
  
// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::~CIpsPlgSmtpService
// ---------------------------------------------------------------------------
//    
CIpsPlgSmtpService::~CIpsPlgSmtpService() 
    {
    FUNC_LOG;
    delete iMsgMapper;
    }    

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgSmtpService* CIpsPlgSmtpService::NewL(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgSmtpService* self = CIpsPlgSmtpService::NewLC( 
        aSession, aPlugin  );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgSmtpService* CIpsPlgSmtpService::NewLC(
    CMsvSession& aSession,
    CIpsPlgSosBasePlugin& aPlugin )
    {
    FUNC_LOG;
    CIpsPlgSmtpService* self = new( ELeave ) CIpsPlgSmtpService( 
        aSession, aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::ConstructL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::ConstructL()
    {
    FUNC_LOG;
    iMsgMapper = CIpsPlgMsgMapper::NewL( iSession, iPlugin );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CreateDisplayStringLC
// ---------------------------------------------------------------------------
//
HBufC* CIpsPlgSmtpService::CreateDisplayStringLC(
    const TDesC& aDisplayName,
    const TDesC& aEmailName )
    {
    FUNC_LOG;
    TInt length( aDisplayName.Length() );
    HBufC* fullName( NULL );
    
    length += aEmailName.Length() + KIpsSetUtilsCntrCharacters;
    fullName = HBufC::NewLC( length );
    fullName->Des().Append( aDisplayName );
    
    if ( fullName->Length() )
        {
        // We have display name, add angle brackets to address
        fullName->Des().Append( KIpsSetUtilsCharLessThan );
        fullName->Des().Append( aEmailName );
        fullName->Des().Append( KIpsSetUtilsCharMoreThan );
        }
    else
        {
        fullName->Des().Append( aEmailName );
        }
    return fullName;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::StoreMessageL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::StoreMessageL( 
    TFSMailMsgId /* aMailBoxId */, CFSMailMessage& aMessage )
    {
    FUNC_LOG;

    // Update flags i.e. Priority
    iMsgMapper->UpdateMessageFlagsL( aMessage.GetMessageId().Id(), aMessage );
    
    // get message from store
    CMsvEntry* cEntry = CMsvEntry::NewL(
        iSession, aMessage.GetMessageId().Id(), TMsvSelectionOrdering() );
    CleanupStack::PushL( cEntry );  // << cEntry
    TMsvEntry tEntry = cEntry->Entry();    
    
    // creating store object for editing message
    CMsvStore* store = cEntry->EditStoreL();
    CleanupStack::PushL( store );   // << store
    
    // create symbian email header object
    CImHeader* header = CImHeader::NewLC(); // << header
    header->RestoreL( *store );

    // From field, check that sender is set
    if ( aMessage.GetSender() )
        {
        header->SetFromL( aMessage.GetSender()->GetEmailAddress() );
        }
    
    // To Field
    RPointerArray<CFSMailAddress> toArray = aMessage.GetToRecipients();
    // Reseting recipient list, because our client calls us many times
    // and we want to avoid duplicates
    header->ToRecipients().Reset();
    // build new 'to' header
    for( TInt i = 0; i < toArray.Count(); i++ )
        {
        TPtrC displayName = toArray[i]->GetDisplayName();
        TPtrC emailName = toArray[i]->GetEmailAddress();
        HBufC* fullName = CreateDisplayStringLC( displayName, emailName );  // << fullName
        header->ToRecipients().AppendL( *fullName ); // copy created
        // only first receiver 
        if( i == 0 && fullName )
            {
            tEntry.iDetails.Set( *fullName );
            cEntry->ChangeL( tEntry );
            // Load tEntry again
            // because old instance's iDetails member gets wiped
            // in PopAndDestroy below
            tEntry = cEntry->Entry();
            }
        CleanupStack::PopAndDestroy( fullName );    // >>> fullName
        }   
    
    // CC field
    RPointerArray<CFSMailAddress> ccArray 
        = aMessage.GetCCRecipients();
    header->CcRecipients().Reset();
    for( TInt i = 0; i < ccArray.Count(); i++ )
        {
        header->CcRecipients().AppendL( ccArray[i]->GetEmailAddress() ); // copy created
        }
    
    // BCC field
    RPointerArray<CFSMailAddress> bccArray 
        = aMessage.GetBCCRecipients();
    header->BccRecipients().Reset();
    for( TInt i = 0; i < bccArray.Count(); i++ )
        {
        header->BccRecipients().AppendL( bccArray[i]->GetEmailAddress() ); // copy created
        }
    
    // Subject, check null pointer
    if ( &aMessage.GetSubject() )
        {
        header->SetSubjectL( aMessage.GetSubject() );
        }
        
    // store header in cEntry's store
    header->StoreWithoutCommitL(*store);
    // commit header to store
    store->CommitL();
    
    //change preparation state and set subject to description
    tEntry.SetInPreparation( EFalse );
    tEntry.iDescription.Set( aMessage.GetSubject() );
    
    cEntry->ChangeL( tEntry );
    
    // release store
    CleanupStack::PopAndDestroy( 3, cEntry );   // >>> cEntry, header, store
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CreateNewSmtpMessageL
// ---------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgSmtpService::CreateNewSmtpMessageL( 
    const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG;
    TMsvPartList partList( KMsvMessagePartBody );
    
    CIpsPlgOperationWait* wait = CIpsPlgOperationWait::NewLC( );
    
    TMsvEntry mboxEntry;
    TMsvId service;
    iSession.GetEntry( aMailBoxId.Id(), service, mboxEntry );
    
    // new mail creating operation. Mail is created asynchronously
    CImEmailOperation* newMailOp = CImEmailOperation::CreateNewL(
            wait->iStatus, 
            iSession,
            KMsvDraftEntryId,
            mboxEntry.iRelatedId, 
            partList, 
            0, 
            KUidMsgTypeSMTP );
    
    CleanupStack::PushL( newMailOp );

    // wait until new mail is created
    wait->Start();
   
    TMsvId msgId = GetIdFromProgressL( newMailOp->FinalProgress() );
    
    CleanupStack::PopAndDestroy( 2, wait );
    
    return CreateFSMessageAndSetFlagsL( 
            msgId, KErrNotFound, aMailBoxId.Id() );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CreateForwardSmtpMessageL
// ---------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgSmtpService::CreateForwardSmtpMessageL( 
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aOriginalMessageId )
    {
    FUNC_LOG;
    // 1. part of function checs that body text and all
    // attachments are fetched
    TMsvEntry orgMsg;
    TMsvId service;
    User::LeaveIfError( iSession.GetEntry( 
            aOriginalMessageId.Id(), service, orgMsg ) );
    
    if ( orgMsg.Id() ==  KMsvNullIndexEntryIdValue )
        {
        User::Leave(KErrNotFound);
        }
    
    if ( ( orgMsg.Parent() == KMsvSentEntryIdValue ) && 
         ( orgMsg.iMtm == KSenduiMtmSmtpUid ) )
        {
        ChangeServiceIdL( orgMsg );
        }
    TMsvPartList partList( KMsvMessagePartBody | KMsvMessagePartAttachments );
    
    CIpsPlgOperationWait* wait = CIpsPlgOperationWait::NewLC( );
    CImEmailOperation* forwMailOp = CImEmailOperation::CreateForwardL(
            wait->iStatus, 
            iSession,
            orgMsg.Id(),
            KMsvDraftEntryId, 
            partList, 
            0, 
            KUidMsgTypeSMTP );
            
    CleanupStack::PushL( forwMailOp );
    
    wait->Start();
    
    TMsvId msgId = GetIdFromProgressL( forwMailOp->FinalProgress() );
    
    CleanupStack::PopAndDestroy( 2, wait );
    
    return CreateFSMessageAndSetFlagsL( msgId, orgMsg.Id(), aMailBoxId.Id() );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CreateReplySmtpMessageL
// ---------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgSmtpService::CreateReplySmtpMessageL( 
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aOriginalMessageId,
    TBool aReplyToAll )
    {
    FUNC_LOG;
    // find orginal message header and check that body is fetched
    TMsvEntry orgMsg;
    TMsvId service;
    User::LeaveIfError( 
        iSession.GetEntry( aOriginalMessageId.Id(), service, orgMsg ) );
    
    if ( orgMsg.Id() ==  KMsvNullIndexEntryIdValue )
        {
        User::Leave(KErrNotFound);
        }
    
    if ( ( orgMsg.Parent() == KMsvSentEntryIdValue ) && 
         ( orgMsg.iMtm == KSenduiMtmSmtpUid ) )
        {
        ChangeServiceIdL( orgMsg );
        }
    
    // partList flags control e.g. what kind of recipient set is created
    TMsvPartList partList = KMsvMessagePartBody | KMsvMessagePartDescription
    | KMsvMessagePartOriginator;        
    if( aReplyToAll )
        {
        partList |= KMsvMessagePartRecipient;
        }
    
    // Ask Symbian messaging fw to create reply message "skeleton"
    CIpsPlgOperationWait* wait = CIpsPlgOperationWait::NewLC( );
    CImEmailOperation* replMailOp = CImEmailOperation::CreateReplyL(
            wait->iStatus, 
            iSession,
            orgMsg.Id(),
            KMsvDraftEntryId, 
            partList, 
            0, 
            KUidMsgTypeSMTP );
    CleanupStack::PushL( replMailOp );
    wait->Start();
    TMsvId msgId = GetIdFromProgressL( replMailOp->FinalProgress() );
    CleanupStack::PopAndDestroy( 2, wait );
    
    // Start finalising new FS style message
    CFSMailMessage* fsMsg = CreateFSMessageAndSetFlagsL( 
            msgId, orgMsg.Id(), aMailBoxId.Id() );
    CleanupStack::PushL( fsMsg ); // ***

    // dig out new reply message's header
    CMsvEntry* cEntry = iSession.GetEntryL( msgId );
    CleanupStack::PushL( cEntry ); // ***
    CMsvStore* store = cEntry->ReadStoreL();
    CleanupStack::PushL( store ); // ***
    if( store->IsPresentL( KUidMsgFileIMailHeader ) == EFalse )
         {
         User::Leave(KErrCorrupt);
         }
    CImHeader* header = CImHeader::NewLC(); // ***
    header->RestoreL( *store );

    // Start handling recipients
    HBufC* emailAddr( NULL );
    CFSMailAddress* fsAddr( NULL );
    // copy to recipients
    for( TInt i = 0; i < header->ToRecipients().Count(); i++ )
        {
        emailAddr = header->ToRecipients()[i].AllocLC(); // ***
        fsAddr = CFSMailAddress::NewLC(); // ***
        fsAddr->SetEmailAddress( *emailAddr ); // Copy created
        fsMsg->AppendToRecipient( fsAddr ); // No copy
        CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
        CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
        }
    // copy cc recipients
    for( TInt i = 0; i < header->CcRecipients().Count(); i++ )
        {
        emailAddr = header->CcRecipients()[i].AllocLC(); // ***
        fsAddr = CFSMailAddress::NewLC(); // ***
        fsAddr->SetEmailAddress( *emailAddr ); // Copy created
        fsMsg->AppendCCRecipient( fsAddr ); // No copy
        CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
        CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
        }
    // copy bcc recipients
    for( TInt i = 0; i < header->BccRecipients().Count(); i++ )
        {
        emailAddr = header->BccRecipients()[i].AllocLC(); // ***
        fsAddr = CFSMailAddress::NewLC(); // ***
        fsAddr->SetEmailAddress( *emailAddr ); // Copy created
        fsMsg->AppendBCCRecipient( fsAddr ); // No copy
        CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
        CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
        }

    CleanupStack::PopAndDestroy( 3, cEntry ); // header, store, cEntry
    CleanupStack::Pop( fsMsg ); // fsMsg is given to client
    return fsMsg;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::ChangeServiceIdL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::ChangeServiceIdL( TMsvEntry& aEntry )
    {
    FUNC_LOG;
    TMsvEntry serviceEntry;
    TMsvId service;
    CMsvEntry* cEntry = iSession.GetEntryL( aEntry.iServiceId );
    CleanupStack::PushL( cEntry );
    User::LeaveIfError( iSession.GetEntry( cEntry->EntryId(), service, serviceEntry ) );
    if( aEntry.iServiceId != serviceEntry.iRelatedId )
        {
        aEntry.iServiceId = serviceEntry.iRelatedId;
        cEntry->SetEntryL( aEntry.Id() );
        cEntry->ChangeL( aEntry );
        }
    CleanupStack::PopAndDestroy( cEntry );
    }
    
// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::CreateFSMessageAndSetFlagsL
// ---------------------------------------------------------------------------
//
CFSMailMessage* CIpsPlgSmtpService::CreateFSMessageAndSetFlagsL(
       TMsvId aMessageId,
       TMsvId aOriginalMsgId,
       TMsvId aMailboxId,
       TBool aCopyOriginalMsgProperties )
    {
    FUNC_LOG;

    // Default charset
    TUid charset;
    charset.iUid = KCharacterSetIdentifierUtf8;

    // set in preparation flag, altought 
    // not known where this affects
    TMsvId dummy;
    TMsvEmailEntry newEmailMsg;
    iSession.GetEntry( aMessageId, dummy, newEmailMsg );
    newEmailMsg.SetInPreparation( ETrue );
    
    // when creating new message from scratch theres no 
    // orginal message, so it is set to KErrNotFound
    if ( aOriginalMsgId != KErrNotFound )
        {
        TMsvEntry orgMsg;
        //get entry errors not handled
        iSession.GetEntry( aOriginalMsgId, dummy, orgMsg );
        iSession.GetEntry( aMessageId, dummy, newEmailMsg );
        const TMsvEmailEntry& orgEmailMsg(orgMsg);
        
        // symbian not mark same priority to reply / forward messages
        if ( aCopyOriginalMsgProperties && orgEmailMsg.Priority() != newEmailMsg.Priority() )
            {
            newEmailMsg.SetPriority( orgEmailMsg.Priority() );
            }
        
        //get charset from original message
        charset = GetOriginalCharsetL( aOriginalMsgId );
        }
    
    // set in preparation
    CMsvEntry* cEntry = iSession.GetEntryL( aMessageId );
    CleanupStack::PushL( cEntry );
    cEntry->ChangeL( newEmailMsg );

    //no need to set if we for some reason don't have it.
    //system default will be used.
    if ( KNullUidValue != charset.iUid )
        {
        SetCharactersetL( *cEntry, charset );
        }
    // delete operations we don't need anymore
    CleanupStack::PopAndDestroy( cEntry );
    
    // create a Freestyle mail message object and set to it all
    // needed data from just created symbian mail message
    CFSMailMessage* fsMsg = CFSMailMessage::NewL(
        TFSMailMsgId( iPlugin.PluginId(), aMessageId ) );
    fsMsg->SetMailBoxId( TFSMailMsgId( iPlugin.PluginId(), aMailboxId ) );
    fsMsg->SetFolderId( 
        TFSMailMsgId( iPlugin.PluginId(), KMsvDraftEntryId ) );

    // update subject
    fsMsg->SetSubject( newEmailMsg.iDescription );
    
    //update flags
    iMsgMapper->SetFSMessageFlagsL( newEmailMsg, *fsMsg );
    
    return fsMsg;
    }
    

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::GetIdFromProgressL
// inline static
// ---------------------------------------------------------------------------
//
inline TMsvId CIpsPlgSmtpService::GetIdFromProgressL( const TDesC8& aProg )
    {
    FUNC_LOG;
    // Taken from symbian os help example code
    // Create and initialise a temporary TPckg object that 
    // can hold a message Id.
    TMsvId msgId;
    TPckg<TMsvId> param(msgId);
    
    // Copy the message Id returned from the ProgressL() call into the
    // temporary TPckg object.
    param.Copy( aProg ); 

    // Check the value of the returned message Id in the TPckg object.
    if ( msgId == KMsvNullIndexEntryId )
        {
        // Email is not correctly created, so leave
        User::Leave( KErrNotFound );
        }
    return msgId;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::GetOriginalCharsetL
// ---------------------------------------------------------------------------
//
TUid CIpsPlgSmtpService::GetOriginalCharsetL( TMsvId aOriginalMsgId )
    {
    FUNC_LOG;
    TUid ret;
    CMsvEntry* oEntry = iSession.GetEntryL( aOriginalMsgId );
    CleanupStack::PushL( oEntry );
    
    CImEmailMessage* cMessage = CImEmailMessage::NewLC( *oEntry );
    
    // <cmail> Added check for body text because GetCharacterSetL will
    // hang if there is no body text
    CMsvStore* store = oEntry->EditStoreL();
    CleanupStack::PushL(store);

    TUint charId;
    
    if( store->HasBodyTextL() )
        {
        TBool override=EFalse;
        cMessage->GetCharacterSetL( aOriginalMsgId, charId, override );
        ret.iUid = charId;
        }
    else
        {
        ret.iUid = KCharacterSetIdentifierUtf8;
        }
    
    CleanupStack::PopAndDestroy( 3, oEntry );//oEntry, cMessage, store
    // </cmail>
      
    return ret;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSmtpService::SetCharactersetL
// ---------------------------------------------------------------------------
//
void CIpsPlgSmtpService::SetCharactersetL( CMsvEntry& aEntry, TUid aCharset )
    {
    FUNC_LOG;
    //Set character set for outgoing message. We use the one 
    //that was in the original message.     
    CMsvStore* store = aEntry.EditStoreL();
    CleanupStack::PushL(store);

    CImHeader* header = CImHeader::NewLC();
    header->RestoreL( *store );
    
    TImEmailTransformingInfo info;
    info.SetToDefault(ESendAsMimeEmail);    
    info.SetHeaderAndBodyCharset( aCharset.iUid );
    info.StoreL( *store );
    
    header->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( 2, store ); // store, header       
    }
// End of File

