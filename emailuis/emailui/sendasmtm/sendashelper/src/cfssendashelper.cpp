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
* Description:  Implementation of the class CFsSendAsHelper.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <s32mem.h>
#include <aknappui.h>
#include <bautils.h>
#include <centralrepository.h>
#include <e32const.h>
#include <usbman.h>
#include <usbpersonalityids.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <FreestyleEmailUi.rsg>

//<cmail>
#include "cfsmailcommon.h"
#include "cfsmailclient.h"
#include "cfsmailmessage.h"
#include "cfsmailbox.h"
#include "cfsmailaddress.h"
#include "cfsmailmessagebase.h"
#include "cfsmailmessagepart.h"
#include "FreestyleEmailUiConstants.h"
#include "esmailsettingsplugin.h"
//</cmail>
#include "cfssendashelper.h"
#include "fsmtmsconstants.h"
#include "freestyleemailcenrepkeys.h"

_LIT( KFreestyleEmailUiResourceFileName, "FreestyleEmailUi.rsc" );

// ---------------------------------------------------------------------------
// CFsSendAsInfo::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CFsSendAsHelper* CFsSendAsHelper::NewL()
    {
    FUNC_LOG;

    CFsSendAsHelper* self = new(ELeave) CFsSendAsHelper();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::~CFsSendAsHelper
// ---------------------------------------------------------------------------
//
EXPORT_C CFsSendAsHelper::~CFsSendAsHelper()
    {
    FUNC_LOG;

    delete iSubject;
    delete iBody;
    delete iMassStorageNote;
    
    for (int i = 0; i < iFiles.Count() && !isPassed; ++i )
    	{
    	iFiles[i].Close();
    	}
    iFiles.Reset();
    iFileNames.ResetAndDestroy();
    iMimeTypesFile.ResetAndDestroy();
    iMimeTypesFileName.ResetAndDestroy();
    
    iToAddresses.ResetAndDestroy();
    iCcAddresses.ResetAndDestroy();
    iBccAddresses.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::CFsSendAsHelper
// ---------------------------------------------------------------------------
//
CFsSendAsHelper::CFsSendAsHelper() : isPassed(EFalse) 
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::ConstructL
// ---------------------------------------------------------------------------
//
void CFsSendAsHelper::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::AddAttachmentL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsSendAsHelper::AddAttachmentL(const TDesC& aFileName, 
    const TDesC8& aMimeType, TUint /*aCharSet*/)
    {
    FUNC_LOG;

    if ( !iBody )
       	{
       	//Create empty body, as body is required
       	iBody = HBufC::NewL(1);
       	}
    
    if ( aFileName != KNullDesC )
        {
        iFileNames.AppendL( aFileName.AllocL() );
        iMimeTypesFileName.AppendL( aMimeType.AllocL() );
        }
    
    if ( !iSubject )
    	{
    	TBuf<KMaxFileName> fileName( aFileName );
    	TParse parse;
    	User::LeaveIfError(parse.Set( fileName, NULL , NULL) );
    	iSubject = parse.NameAndExt().AllocL();
    	}

    }


// ---------------------------------------------------------------------------
// CFsSendAsHelper::AddAttachmentL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsSendAsHelper::AddAttachmentL( RFile& aFile, 
    const TDesC8& aMimeType, TUint /*aCharSet*/)
    {
    FUNC_LOG;

    iFiles.AppendL( aFile );
    iMimeTypesFile.AppendL( aMimeType.AllocL() );

    if ( !iSubject )
    	{
    	TBuf<KMaxFileName> fileName;
    	TInt error = aFile.FullName( fileName );
    	if ( error != KErrNone )
    		{
    		User::Leave( error );
    		}
    	TParse parse;
    	User::LeaveIfError(parse.Set( fileName, NULL , NULL) );
    	iSubject = parse.NameAndExt().AllocL();

    	if ( !iBody )
    		{
    		//Create empty body, as body is required
    		iBody = HBufC::NewL(1);
    		}
    	}


    }


// ---------------------------------------------------------------------------
// CFsSendAsHelper::AddRecipientL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsSendAsHelper::AddRecipientL(const TDesC& aAddress, 
    const TDesC& aAlias, TMsvRecipientType aType)
    {
    FUNC_LOG;

    CFSMailAddress* to = CFSMailAddress::NewL();
    CleanupStack::PushL( to );
    
    if ( aAddress != KNullDesC )
    	{
    	to->SetEmailAddress( aAddress );
    	}

    if ( aAlias != KNullDesC )
    	{
    	to->SetDisplayName( aAlias );
    	}

    switch ( aType )
        {
        case EMsvRecipientCc:
            iCcAddresses.AppendL( to );  
            break;
        case EMsvRecipientBcc:
            iBccAddresses.AppendL( to );  
            break;
        case EMsvRecipientTo:
        default:
            iToAddresses.AppendL( to );  
            break;        
        }
    
    CleanupStack::Pop( to );
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::SetSubjectL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsSendAsHelper::SetSubjectL(const TDesC& aSubject)
    {
    FUNC_LOG;

    if ( aSubject != KNullDesC )
        {
        delete iSubject;
        iSubject = NULL; // <cmail>
        iSubject = aSubject.AllocL();
        }
    
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::SetBodyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsSendAsHelper::SetBodyL(const TDesC& aBody)
    {
    FUNC_LOG;
     
    if ( aBody != KNullDesC )
        {
        delete iBody;
        iBody = NULL; // <cmail>
        iBody = aBody.AllocL();
        }

    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::StoreDataL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsSendAsHelper::StoreDataL( TUint aPluginId, TUint aId )
    {
    FUNC_LOG;

    User::LeaveIfError( IsMailboxAvailable( aPluginId ) );
    
    CFSMailClient* mailClient = CFSMailClient::NewL();
    CleanupClosePushL(*mailClient); // +1 mailClient
    CFSMailBox* mailBox = mailClient->GetMailBoxByUidL( 
        TFSMailMsgId( aPluginId, aId ));
    
    // If there's no mailbox, we can't create a new email
    User::LeaveIfNull( mailBox );
    
    CleanupStack::PushL( mailBox ); // +2 mailBox
    CFSMailMessage* newMessage = mailBox->CreateMessageToSend();
    CleanupStack::PushL( newMessage ); // +3 newMessage //SCANNER
    
    // newMessage->SetContentType( KFSMailContentTypeMultipartMixed );
    
    // set message subject    
    if(iSubject)
        {
        newMessage->SetSubject( *iSubject );
        }

    // Append To recipients
    for ( int i = 0; i < iToAddresses.Count(); ++i )
    	{
    	newMessage->AppendToRecipient( iToAddresses[i] );
    	}
    
    iToAddresses.Reset();

    // Append Cc recipients
    for ( int i = 0; i < iCcAddresses.Count(); ++i )
        {
        newMessage->AppendCCRecipient( iCcAddresses[i] );
        }   
    
    iCcAddresses.Reset();

    // Append Bcc recipients
    for ( int i = 0; i < iBccAddresses.Count(); ++i )
        {
        newMessage->AppendBCCRecipient( iBccAddresses[i] );
        }   

    iBccAddresses.Reset();  

    // add body
    if(iBody)
    	{
    	CFSMailMessagePart* newMessageTextPart = newMessage->PlainTextBodyPartL();
    	if ( !newMessageTextPart )
    		{                
    		// create new message part for body text
    		TFSMailMsgId insertBefore;
    		newMessageTextPart = newMessage->NewChildPartL( insertBefore, KFSMailContentTypeTextPlain ); 
    		}

    	if ( newMessageTextPart )
    		{
    		CleanupStack::PushL( newMessageTextPart ); // +4 newMessageTextPart
    	
    		// Check that whether new part contains data that needs to be appended
    		// for example signature generated by the mailbox
    		HBufC* createdBodyContent = HBufC::NewLC( newMessageTextPart->FetchedContentSize() );
            TPtr tempPtr = createdBodyContent->Des();
            newMessageTextPart->GetContentToBufferL( tempPtr, 0 );	
    		if ( createdBodyContent->Length() )
    		    {
    		    // Store original body
    		    HBufC* originalBody = iBody->AllocLC();
    		    delete iBody;
    		    iBody = NULL;
    		    // Create new body object, deleted either above, in destructor or in setbody
    		    iBody = HBufC::NewL( originalBody->Length() + createdBodyContent->Length( ));
    		    // append original body and also generated body content
    		    iBody->Des().Append( *originalBody );
                iBody->Des().Append( *createdBodyContent );
    	        CleanupStack::PopAndDestroy( originalBody );
    		    }		
            CleanupStack::PopAndDestroy( createdBodyContent );    		 		
    		newMessageTextPart->SetContentType( KFSMailContentTypeTextPlain );
    		newMessageTextPart->SetMailBoxId( mailBox->GetId() );    
    		TPtr bodyPtr = iBody->Des();
            newMessageTextPart->SetContent( bodyPtr );
    		newMessageTextPart->SaveL();
    		CleanupStack::PopAndDestroy( newMessageTextPart ); // -4 newMessageTextPart
    		}               
    	}        

    for( int i = 0; i < iFiles.Count(); ++i )
    	{
    	CFSMailMessagePart* msgPart = newMessage->AddNewAttachmentL( iFiles[i], *iMimeTypesFile[i] );
    	delete msgPart;
    	}
    isPassed = ETrue;
    
    for ( int i = 0; i < iFileNames.Count(); ++i )
    	{
    	TFSMailMsgId nullId;
    	CFSMailMessagePart* msgPart = newMessage->AddNewAttachmentL( *iFileNames[i], nullId );
    	delete msgPart;
    	}

    newMessage->SaveMessageL();    
    TFSMailMsgId msgId = newMessage->GetMessageId();

// <cmail> S60 UID update
    const TUid KFreestyleEmailCenRep = {0x2001E277};
// </cmail> S60 UID update
    CRepository* repository( NULL );    
    TRAPD( ret, repository = CRepository::NewL( KFreestyleEmailCenRep ) );
    if ( ret == KErrNone )
    	{
        CleanupStack::PushL( repository );
    	TInt rval;
        TInt messageId = msgId.Id();
        rval = repository->Set( KFreestyleMtmMessageId, messageId );
        if( rval != KErrNone)
            {
            }
        CleanupStack::PopAndDestroy( repository );
    	}
    // check that msg is not in drafts folder already
    TFSMailMsgId draftsFolderId = mailBox->GetStandardFolderId( 
        EFSDraftsFolder );
    TFSMailMsgId msgFolderId = newMessage->GetFolderId();
    if ( draftsFolderId != msgFolderId )
        {
        RArray<TFSMailMsgId> ids;
        ids.Append( msgId );
        mailBox->MoveMessagesL( ids, msgFolderId, draftsFolderId );
        ids.Reset();
        }

    CleanupStack::PopAndDestroy( newMessage ); // -3 newMessage
    CleanupStack::PopAndDestroy( mailBox ); // -2 mailBox
    CleanupStack::PopAndDestroy( mailClient ); // -1 mailClient, close() will be called
    mailClient = NULL;

    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::LaunchUiL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsSendAsHelper::LaunchUiL( TUint aPluginId, TUint aId, 
    TBool aIsEmptyMail, TBool aIsSettings)
    {
    FUNC_LOG;

    CEikAppUi* appUi = CEikonEnv::Static()->EikAppUi();
    CCoeAppUi* coeAppUi = static_cast<CCoeAppUi*>( appUi );

    if ( aIsSettings )
    	{

    	CESMailSettingsPlugin::TSubViewActivationData pluginData;
    	pluginData.iAccount = TFSMailMsgId( aPluginId, aId );
    	pluginData.iLaunchedOutsideFSEmail = ETrue;
    	pluginData.iSubviewId=0;
    	const TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> pkgOut( pluginData );
    	coeAppUi->ActivateViewL( TVwsViewId( KFSEmailUiUid, SettingsViewId ),
    	            TUid::Uid(KMailSettingsOpenPluginSettings), pkgOut );
    	}

    else if ( aIsEmptyMail )
        {
        // While starting the empty editor. This would be the case when
        // write message - > Email - > Freestle 
        TUid editorCommand = TUid::Uid( KEditorCmdCreateNew );        
        TEditorLaunchParams params;        
        // ETrue makes editor close application when it exits
        params.iActivatedExternally = ETrue;    
        params.iMailboxId = TFSMailMsgId( aPluginId, aId );
        TPckgBuf<TEditorLaunchParams> buf( params );
        coeAppUi->ActivateViewL( TVwsViewId( KFSEmailUiUid, MailEditorId ),
            editorCommand, buf );
        }
    else
        {
        //While starting the editor with some data. This would be the case when
        // ex: notepadfile-> options- > send -> Email - > Freestle
        CFSMailClient* mailClient = CFSMailClient::NewL();
        CleanupClosePushL(*mailClient); // +1 mailClient
        CFSMailBox* mailBox = mailClient->GetMailBoxByUidL( 
                TFSMailMsgId( aPluginId, aId ));
        CleanupStack::PushL( mailBox ); // +2 mailBox      

        TUid editorCommand = TUid::Uid( KEditorCmdOpen );
        TEditorLaunchParams params;
        params.iExtra=NULL;
        params.iActivatedExternally = ETrue;    
        params.iMailboxId = TFSMailMsgId( aPluginId, aId );
        
// <cmail> S60 UID update
        const TUid KFreestyleEmailCenRep = {0x2001E277};
// </cmail> S60 UID update
        CRepository* repository( NULL );    
        TUint mId = 0;
        TRAPD( ret, repository = CRepository::NewL( KFreestyleEmailCenRep ) );
        if ( ret == KErrNone )
        	{
            CleanupStack::PushL( repository );
        	TInt rval;
            TInt messageId;
            rval = repository->Get( KFreestyleMtmMessageId, messageId );
            if( rval != KErrNone)
            	{
            	}
            mId = messageId;
            CleanupStack::PopAndDestroy( repository );
        	}
        params.iMsgId = TFSMailMsgId( aPluginId, mId );
        params.iFolderId = mailBox->GetStandardFolderId( EFSDraftsFolder );
        
        CleanupStack::PopAndDestroy( mailBox ); // -2 mailBox
        CleanupStack::PopAndDestroy( mailClient ); // -1 mailClient, close() will be called
        
        TPckgBuf<TEditorLaunchParams> buf( params );
        coeAppUi->ActivateViewL( TVwsViewId( KFSEmailUiUid, MailEditorId ),
            editorCommand, buf );

        }
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::DisplayMailboxNotAvailableNote
// ---------------------------------------------------------------------------
//
void CFsSendAsHelper::DisplayMailboxNotAvailableNoteL()
    {
    FUNC_LOG;

    if ( !iMassStorageNote )
        {
        CEikonEnv* env( CEikonEnv::Static() );
        ASSERT( env );
        TParse parse;
        parse.Set( KFreestyleEmailUiResourceFileName, &KDC_APP_RESOURCE_DIR, NULL );
        TFileName fileName( parse.FullName() );
        BaflUtils::NearestLanguageFile( env->FsSession(), fileName );
        TInt offset( env->AddResourceFileL( fileName ) );
        TRAPD( error, iMassStorageNote = StringLoader::LoadL( R_MASS_STORAGE_OFFLINE ) );
        env->DeleteResourceFile( offset );
        User::LeaveIfError( error );
        }
    CAknWarningNote* note = new (ELeave) CAknWarningNote( ETrue );
    note->ExecuteLD( *iMassStorageNote );
    }

// ---------------------------------------------------------------------------
// CFsSendAsHelper::IsMailboxAvailable
// ---------------------------------------------------------------------------
//
TInt CFsSendAsHelper::IsMailboxAvailable( TInt aPluginId )
    {
    const TUint KEasPluginId = 0x2000B020;
    const TUint KOzPluginId = 0x20021362;

    TInt retVal( KErrNone );
    
    if ( aPluginId == KEasPluginId || aPluginId == KOzPluginId )
        {
        RUsb usb;
        if ( usb.Connect() == KErrNone )
            {
            TUsbServiceState state = EUsbServiceIdle;
            usb.GetCurrentState( state );

            TInt personalityId( 0 );
            usb.GetCurrentPersonalityId( personalityId );
            usb.Close();

            if ( state == EUsbServiceStarted && 
                 personalityId == KUsbPersonalityIdMS )
                {
                // The operation cannot be fulfilled in mass storage mode for
                // MfE or Ozone boxes, show note detailing the situation.
                TRAP_IGNORE( DisplayMailboxNotAvailableNoteL() );
                retVal = KErrNotSupported;
                }
            }
        }
    return retVal;
    }
