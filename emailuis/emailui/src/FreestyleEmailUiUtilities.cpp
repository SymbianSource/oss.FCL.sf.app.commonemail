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
*  Description : General utilities for FS Email UI.
*
*/


#include "emailtrace.h"
#include <msvapi.h>
#include <msvids.h>
#include <eikenv.h>
#include <gulicon.h>
#include <SendUiConsts.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <StringLoader.h>
#include <AknIconUtils.h>
#include <aknlists.h>
#include <aknPopup.h>
#include <akndiscreetpopup.h>
#include <AknGlobalConfirmationQuery.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <AknGlobalNote.h>
#include <commonphoneparser.h>
#include <apgcli.h> // RApaLsSession

#ifndef FF_CMAIL_INTEGRATION
#include <txtclipboard.h>
#endif // FF_CMAIL_INTEGRATION

// <cmail>
//#ifdef __SERIES60_32__
// </cmail>
#include <AiwDialDataTypes.h>
#include <AiwGenericParam.h>
#include <AiwServiceHandler.h>
#include <settingsinternalcrkeys.h>

// <cmail>
//#else
//#include <CPhCltDialer.h>
//#include <TPhCltExtPhoneDialData.h>
//#endif
// </cmail>

#include <aknPopup.h>
#include <AknCommonDialogsDynMem.h>
#include <CommonDialogs.rsg>
#include <pathinfo.h>
#include <AknIconArray.h>
#include <baclipb.h> // for clipboard copy
#include <DocumentHandler.h>
#include <bautils.h>

#include <sendui.h>			// CSendUi
#include <coreapplicationuisdomainpskeys.h> // P&S key for email notification
#include <e32property.h>					// RPoperty
#include <SendUiConsts.h>	// Constants for sendui
#include <CMessageData.h>	// CMessageData
#include <CSendingServiceInfo.h>
#include <CoreApplicationUIsSDKCRKeys.h> // offline mode keys
#include <data_caging_path_literals.hrh>
#include "cmailwidgetcenrepkeys.h"

// <cmail> Removed DISABLE_DEFAULT_EMAIL
//#ifndef DISABLE_DEFAULT_EMAIL
//#include <MessagingDomainCRKeys.h>
//#include <centralrepository.h>
//#endif
//</cmail>

//<cmail>
#include "cfsccontactactionservice.h"
#include "cfsccontactactionmenu.h"
#include "cfsccontactactionmenuitem.h"
#include "mfsccontactactionmenumodel.h"
//</cmail>

#include <FreestyleEmailUi.rsg>
#include <freestyleemailui.mbg>

//<cmail>
#include "cfsmailcommon.h"
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cfsmailmessage.h"
#include "cesmricalviewer.h"
//</cmail>

#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiAppui.h"
#include "FSDelayedLoader.h"
#include "FSEmail.pan"

// CONSTANTS
_LIT( KFsEmailIconFileName, "FreestyleEmailUi.mif" );
_LIT( KFsEmailTempDirName, "temp\\" );
static const TUint KSecondSeparator = 1;
_LIT( KTimeFormatBefore, "%-B" );
_LIT( KTimeFormatAfter, "%+B" );
_LIT( KTimeFormatHour, "%J" );
_LIT( KTimeFormatMinutes, "%T" );
_LIT( KTimeFormatSpace, " ");

// MIME types
_LIT( KPdfMimeString, "application/pdf" );
_LIT( KRtfMimeString, "application/rtf" );
_LIT( KDocMimeString, "application/msword" );
_LIT( KDocMimeString2, "application/vnd.ms-word" );
_LIT( KPptMimeString, "application/vnd.ms-powerpoint" );
_LIT( KXlsMimeString, "application/vnd.ms-excel" );
_LIT( KImageMimeString, "image/" );
_LIT( KHtmlMimeString, "text/html" );
_LIT( KPlainTextMimeString, "text/plain" );
_LIT( KVCalMimeString, "text/x-vcalendar" );
_LIT( KMessageMimeString, "message/rfc822" );

// File name extensions
_LIT( KDocFileExtension, ".doc" );
_LIT( KRtfFileExtension, ".rtf" );
_LIT( KPptFileExtension, ".ppt" );
_LIT( KXlsFileExtension, ".xls" );
_LIT( KPdfFileExtension, ".pdf" );
_LIT( KJpgFileExtension, ".jpg" );
_LIT( KJpegFileExtension, ".jpeg" );
_LIT( KJpgeFileExtension, ".jpge" );
_LIT( KPngFileExtension, ".png" );
_LIT( KGifFileExtension, ".gif" );
_LIT( KBmpFileExtension, ".bmp" );
_LIT( KHtmlFileExtension, ".html" );
_LIT( KHtmFileExtension, ".htm" );
_LIT( KTxtFileExtension, ".txt" );
_LIT( KVCalFileExtension, ".vcs" );
_LIT( KEmailFileExtension, ".eml" );

// If the fetched body size (in bytes) is more than these limits,
// a wait note is used when opening, replying, or forwarding the message
static const TInt KLargePlainTextSizeLimit = 20000;
static const TInt KLargeHtmlTextSizeLimit = 50000;

//<cmail> id is different in cmail
const TUid KFSMailServerUid = { 0x2001F40A };
_LIT( KFSMailServerExe, "\\sys\\bin\\fsmailserver.exe" );
//</cmail>

// Define static members
CAknGlobalNote* TFsEmailUiUtility::iGlobalWaitNote = NULL;
TBool TFsEmailUiUtility::iSaveSelect = ETrue;
TBool TFsEmailUiUtility::iDownloadSave = EFalse;
// <cmail>
CESMRIcalViewer* TFsEmailUiUtility::iMrViewer = NULL;
TFsEmailUiUtility::CMrViewerEmptyCallback* TFsEmailUiUtility::iMrViewerCallback = NULL;
CAknWaitDialog* TFsEmailUiUtility::iOpeningWaitNote = NULL;
// </cmail>

// FUNCTION DEFINITIONS
void TFsEmailUiUtility::LaunchHelpL( const TDesC& aContext )
	{
    FUNC_LOG;
    CArrayFix< TCoeHelpContext >* cntx = new( ELeave ) CArrayFixFlat< TCoeHelpContext >( 1 );
    CleanupStack::PushL( cntx );
    cntx->AppendL( TCoeHelpContext( KFSEmailUiUid, aContext ) );
    CleanupStack::Pop( cntx );
    HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(), cntx );
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsEmailFileHandleShutter* CFsEmailFileHandleShutter::NewL(
    CDocumentHandler& aDocumentHandler )
    {
    FUNC_LOG;
    CFsEmailFileHandleShutter* self = new (ELeave) CFsEmailFileHandleShutter;
    CleanupStack::PushL( self );
    self->ConstructL( aDocumentHandler );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor. File handle is closed, if it has been left open.
// ---------------------------------------------------------------------------
//
CFsEmailFileHandleShutter::~CFsEmailFileHandleShutter()
    {
    FUNC_LOG;
    if (iHandleOpen)
        {
        iFileHandle.Close();
        }
    }

// ---------------------------------------------------------------------------
// Stores the give file handle.
// ---------------------------------------------------------------------------
//
void CFsEmailFileHandleShutter::SetFile( RFile aFile )
    {
    FUNC_LOG;
    // Close previously set handle, if necessary.
    if ( iHandleOpen )
        {
        iFileHandle.Close();
        }

    iFileHandle = aFile;
    iHandleOpen = ETrue;
    }

// ---------------------------------------------------------------------------
// Handles the exit of embedded viewer application.
// ---------------------------------------------------------------------------
//
void CFsEmailFileHandleShutter::HandleServerAppExit( TInt aReason )
    {
    FUNC_LOG;
    if (iHandleOpen)
        {
        // Close open file handle.
        iFileHandle.Close();
        iHandleOpen = EFalse;
        }
    MAknServerAppExitObserver::HandleServerAppExit( aReason );
    
    // Get current AppUi.
    CFreestyleEmailUiAppUi* appUi = (CFreestyleEmailUiAppUi*)CCoeEnv::Static()->AppUi();
    if( appUi->EmbeddedApp() )
        {
        // Set embedded application flag to false when embedded application exit.
        appUi->SetEmbeddedApp( EFalse );
        }
    
    // if email editor is not started from embedded app.
    if( !appUi->EditorStartedFromEmbeddedApp() )
        {
        // Do not set embedded app to previous app.
        appUi->SetEmbeddedAppToPreviousApp( EFalse );
        }
    else
        {
        // Set flag for judging if email editor started from embedded app to false.
        appUi->SetEditorStartedFromEmbeddedApp( EFalse );
        }

    // Clear the temp directory since the closed file might have been copied there
    TFsEmailUiUtility::EraseTempDir();
    }

CFsEmailFileHandleShutter::CFsEmailFileHandleShutter()
    : iHandleOpen(EFalse)
    {
    FUNC_LOG;
    }

void CFsEmailFileHandleShutter::ConstructL(
    CDocumentHandler& aDocumentHandler )
    {
    FUNC_LOG;
    aDocumentHandler.SetExitObserver(this);
    }

// ---------------------------------------------------------------------------
// Clean up any static variables of the class
// ---------------------------------------------------------------------------
//
void TFsEmailUiUtility::DeleteStaticData()
    {
    FUNC_LOG;
    delete iGlobalWaitNote;
    iGlobalWaitNote = NULL;

    // <cmail>
    delete iMrViewer;
    iMrViewer = NULL;

    delete iMrViewerCallback;
    iMrViewerCallback = NULL;
    // </cmail>
    }

// ---------------------------------------------------------------------------
// GetMceDefaultMailboxL
// Returrn the default mailbox. If one the FS account is set as default return
// it else return the first FS account. If no FS account then returns NULL.
// ---------------------------------------------------------------------------
//
CFSMailBox* TFsEmailUiUtility::GetMceDefaultMailboxL( CFSMailClient& aClient, CMsvSession& aMsvSession )
	{
    FUNC_LOG;
// <cmail> S60 UID update
	const TUid KUidMsgValTypeFsMtmVal = { 0x2001F406 };
// </cmail> S60 UID update
	TFSMailMsgId id;
    TBool getFirstInList = EFalse;

    CMsvEntry* root = CMsvEntry::NewL( aMsvSession, KMsvRootIndexEntryId,
	        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CleanupStack::PushL( root );

// <cmail> Removed DISABLE_DEFAULT_EMAIL
/*#ifndef DISABLE_DEFAULT_EMAIL
    TInt entryId;
    TInt rval;
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL(KCRUidSelectableDefaultEmailSettings) );
    if ( ret == KErrNone )
        {
        CleanupStack::PushL( repository );
        rval = repository->Get( KSelectableDefaultMailAccount, entryId );
        if( rval == KErrNone && entryId != -1 )
        	{
	   		root->SetEntryL( entryId );
	        if ( root->Entry().iMtm == KUidMsgValTypeFsMtmVal )
	        	{
	        	// One of The FS is default e-mail account.
	        	id = TFSMailMsgId( root->Entry().iMtmData1, root->Entry().iMtmData2 );
	        	}
	        else
	        	{
	            // If FS is not default then get the first one in the list
	        	getFirstInList = ETrue;
	        	}
        	}
        else
        	{
            // If no mail box is set as default, then get the first one in the list
        	getFirstInList = ETrue;
        	}
        CleanupStack::PopAndDestroy( repository );
        }
    else
    	{
        // If Failed to create repository, then get the first one in the list
    	getFirstInList = ETrue;
    	}
#else*/
    // If the SDK doesn't have default mail box concept.
    getFirstInList = ETrue;
//#endif
// </cmail>
    if( getFirstInList )
    	{
    	// Get the list and return the first one.
    	root->SetEntryL( KMsvRootIndexEntryId );
    	CMsvEntrySelection* childSelection = root->ChildrenWithMtmL(
        	KUidMsgValTypeFsMtmVal );
        CleanupStack::PushL( childSelection );

        if ( childSelection->Count() > 0)
        	{
        	root->SetEntryL( childSelection->At( 0 ) );
        	id = TFSMailMsgId( root->Entry().iMtmData1, root->Entry().iMtmData2 );
        	}
        else
        	{
        	// No FS Mail box.
        	//id.SetNullId();
        	}
        CleanupStack::PopAndDestroy( childSelection );
    	}
    CleanupStack::PopAndDestroy( root );
    if( id.IsNullId() )
    	{
    	// If No FS Mailbox then return NULL.
    	return NULL;
    	}
    else
    	{
    	CFSMailBox* mailbox = aClient.GetMailBoxByUidL( id );
    	return mailbox;
    	}
	}

// ---------------------------------------------------------------------------
// GetMailboxForMtmIdL
// Return the mailbox based on mtm id. If there is an mtm account with aMtmId
// return the mail box of it else returns NULL
// ---------------------------------------------------------------------------
//
CFSMailBox* TFsEmailUiUtility::GetMailboxForMtmIdL( CFSMailClient& aClient,
    CMsvSession& aMsvSession, TMsvId  aMtmId )
	{
    FUNC_LOG;
// <cmail> S60 UID update
	const TUid KUidMsgValTypeFsMtmVal = { 0x2001F406 };
// </cmail> S60 UID update
	CMsvEntry* accountEntry = NULL;
	TRAPD(err, accountEntry = CMsvEntry::NewL( aMsvSession, aMtmId,
            TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue )));
    if(err == KErrNotFound || err == KErrNoMemory ||
        accountEntry->Entry().iMtm != KUidMsgValTypeFsMtmVal)
        {
        // This Mtm Id is not found or the found entry is not FS entry
        return NULL;
        }
	CleanupStack::PushL( accountEntry );
    TFSMailMsgId id = TFSMailMsgId( accountEntry->Entry().MtmData1(),
        accountEntry->Entry().MtmData2() );
    CleanupStack::PopAndDestroy( accountEntry );
    CFSMailBox* mailbox = aClient.GetMailBoxByUidL( id );
    return mailbox;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::IsRemoteLookupSupported
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::IsRemoteLookupSupported( CFSMailBox& aMailBox )
	{
    FUNC_LOG;

	TBool r = aMailBox.HasCapability( EFSMBoxCapaSupportsRCL );

	return r;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowConfirmationQuery
// -----------------------------------------------------------------------------
TInt TFsEmailUiUtility::ShowConfirmationQueryL( TInt aResourceStringId,
												const TDesC& aResourceParameter )
    {
    FUNC_LOG;
    CFSEmailUiCallDialog* queryNote = new ( ELeave ) CFSEmailUiCallDialog();
    CleanupStack::PushL( queryNote );
    HBufC* resourceString = NULL;
    if ( aResourceParameter != KNullDesC )
    	{
    	resourceString = StringLoader::LoadLC( aResourceStringId,
                                               aResourceParameter );
    	}
    else
    	{
    	resourceString = StringLoader::LoadLC( aResourceStringId );
    	}
    queryNote->SetPromptL( *resourceString );
    CleanupStack::PopAndDestroy( resourceString );
    CleanupStack::Pop( queryNote );
    return queryNote->ExecuteLD( R_FSEMAIL_QUERY_DIALOG );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowConfirmationQuery
// -----------------------------------------------------------------------------
TInt TFsEmailUiUtility::ShowConfirmationQueryL( const TDesC& aPromptText )
    {
    FUNC_LOG;
    CFSEmailUiCallDialog* queryNote = new ( ELeave ) CFSEmailUiCallDialog();
    CleanupStack::PushL( queryNote );
    queryNote->SetPromptL( aPromptText );
    CleanupStack::Pop( queryNote );
    return queryNote->ExecuteLD( R_FSEMAIL_QUERY_DIALOG );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowErrorNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowErrorNoteL( TInt aResourceStringId, TBool aWaitingDialog )
	{
    FUNC_LOG;
	HBufC* errMessage = StringLoader::LoadLC( aResourceStringId );
	CAknErrorNote* note = new (ELeave) CAknErrorNote( aWaitingDialog );
	note->ExecuteLD( *errMessage );
	CleanupStack::PopAndDestroy( errMessage );
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowInfoNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowInfoNoteL( TInt aResourceStringId, TBool aWaitingDialog )
    {
    FUNC_LOG;
	HBufC* message = StringLoader::LoadLC( aResourceStringId );
	CAknInformationNote* note = new (ELeave) CAknInformationNote( aWaitingDialog );
	note->ExecuteLD( *message );
	CleanupStack::PopAndDestroy( message );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowGlobalErrorNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowGlobalErrorNoteL( TInt aResourceStringId )
    {
    FUNC_LOG;
    HBufC* noteText = StringLoader::LoadLC( aResourceStringId );
    CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
    globalNote->ShowNoteL( EAknGlobalErrorNote, *noteText );
    CleanupStack::PopAndDestroy( globalNote );
    CleanupStack::PopAndDestroy( noteText );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowGlobalInfoNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowGlobalInfoNoteL( TInt aResourceStringId )
    {
    FUNC_LOG;
    HBufC* noteText = StringLoader::LoadLC( aResourceStringId );
    CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
    globalNote->ShowNoteL( EAknGlobalInformationNote, *noteText );
    CleanupStack::PopAndDestroy( globalNote );
    CleanupStack::PopAndDestroy( noteText );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowDiscreetInfoNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowDiscreetInfoNoteL( TInt aResourceStringId )
    {
    FUNC_LOG;   
    HBufC* noteText = StringLoader::LoadLC( aResourceStringId );
    ShowDiscreetInfoNoteL( *noteText );
    CleanupStack::PopAndDestroy( noteText );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowDiscreetInfoNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowDiscreetInfoNoteL( const TDesC& aNoteText )
    {
    FUNC_LOG;   
    //create a host of dummy parameters in order to change the popup duration flag...
    const TDesC& dummyText = KNullDesC;
    CGulIcon* dummyIcon = NULL;
    const TAknsItemID& dummySkinId = KAknsIIDNone;
    const TDesC& dummyBitmapFile = KNullDesC;
    const TInt dummyBitmapId = 0;
    const TInt dummyMaskId = 0;
    
    //Set timeout flag
    TInt flags = 0;
    flags |= KAknDiscreetPopupDurationLong;
    
    CAknDiscreetPopup::ShowLocalPopupL( aNoteText, 
                                        dummyText, 
                                        dummyIcon, 
                                        dummySkinId,
                                        dummyBitmapFile, 
                                        dummyBitmapId, 
                                        dummyMaskId, 
                                        flags);
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowWaitNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowWaitNoteL( CAknWaitDialog*& aDialog, TInt aTextResourceId,
        TBool aAllowCancel, TBool aVisibilityDelayOff /*= EFalse*/ )
    {
    FUNC_LOG;
    if ( !aDialog )
        {
        aDialog = new (ELeave) CAknWaitDialog(
                reinterpret_cast<CEikDialog**>(&aDialog), aVisibilityDelayOff );
        }

    if ( aAllowCancel )
        {
        aDialog->PrepareLC( R_FSE_WAIT_DIALOG );
        }
    else
        {
        aDialog->PrepareLC( R_FSE_WAIT_DIALOG_NO_CANCEL );
        }

    HBufC* noteText = StringLoader::LoadLC( aTextResourceId );
    aDialog->SetTextL( *noteText );
    CleanupStack::PopAndDestroy( noteText );

    aDialog->RunLD();
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowGlobalWaitNoteLC
// -----------------------------------------------------------------------------
TInt TFsEmailUiUtility::ShowGlobalWaitNoteLC( TInt aTextResourceId )
    {
    FUNC_LOG;
    if ( !iGlobalWaitNote )
        {
        iGlobalWaitNote = CAknGlobalNote::NewL();
        iGlobalWaitNote->SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY );
        }

    HBufC* noteText = StringLoader::LoadLC( aTextResourceId );
    TInt waitNoteId = iGlobalWaitNote->ShowNoteL( EAknGlobalWaitNote, *noteText );
    CleanupStack::PopAndDestroy( noteText );

    TCleanupItem closeNoteItem( CloseGlobalWaitNote,
                                reinterpret_cast<TAny*>(waitNoteId) );
    CleanupStack::PushL( closeNoteItem );

    return waitNoteId;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CloseGlobalWaitNote
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::CloseGlobalWaitNote( TAny* aNoteId )
    {
    FUNC_LOG;
    if ( iGlobalWaitNote )
        {
        TInt noteId = reinterpret_cast<TInt>( aNoteId );
        TRAP_IGNORE( iGlobalWaitNote->CancelNoteL( noteId ) );
        }
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowSelectFileDialogL
// -----------------------------------------------------------------------------
//
TBool TFsEmailUiUtility::ShowSelectFileDialogL( TDes& aFileName )
    {
    FUNC_LOG;
    return ShowFileDialogL( ECFDDialogTypeSelect, aFileName );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowSaveFolderDialogL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::ShowSaveFolderDialogL( TDes& aFileName )
	{
    FUNC_LOG;
    return ShowFileDialogL( ECFDDialogTypeSave, aFileName );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowSaveFolderDialogL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::ShowFileDialogL( TInt aType, TDes& aFileName )
	{
    FUNC_LOG;
	TInt retVal = EFalse;

	TInt allDrives =
        AknCommonDialogsDynMem::EMemoryTypePhone |
        AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage |
        AknCommonDialogsDynMem::EMemoryTypeMMCExternal |
        AknCommonDialogsDynMem::EMemoryTypeRemote;

	if ( aType == ECFDDialogTypeSelect )
	    {
	    retVal = AknCommonDialogsDynMem::RunSelectDlgLD( allDrives, aFileName, 0 );
	    }
	else if ( aType == ECFDDialogTypeSave )
	    {
	    // We can't use the SaveDlg of CommonDialogs because it launches also the file name query
	    // which we do not want. Instead, we use move-to-folder dialog with the title and softkeys
	    // of the save dialog.
	    retVal = AknCommonDialogsDynMem::RunMoveDlgLD( allDrives, aFileName,
	            R_CFD_DEFAULT_SAVE_MEMORY_SELECTION, R_CFD_DEFAULT_SAVE_FILE_SELECTION );
	    }
	else
	    {
	    __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
	    }

	return retVal;
	}


// -----------------------------------------------------------------------------
// TFsEmailUiUtility::OkToSaveFileL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::OkToSaveFileL( const TDesC& aFilePath, CFSMailMessagePart& aAttachmentPart )
	{
    FUNC_LOG;
	TBool ret( ETrue );
	TDesC& name = aAttachmentPart.AttachmentNameL();
	HBufC* realFilePath = HBufC::NewLC( aFilePath.Length() + name.Length() );
	realFilePath->Des().Append( aFilePath);
	realFilePath->Des().Append( name);
	if ( BaflUtils::FileExists( CEikonEnv::Static()->FsSession(), *realFilePath ) )
		{
		if ( !TFsEmailUiUtility::ShowConfirmationQueryL( R_FREESTYLE_EMAIL_UI_OVERWRITE_QUERY, name  )	)
			{
			ret = EFalse;
			}
		}
	CleanupStack::PopAndDestroy( realFilePath );
	TFsEmailUiUtility::SetSaveSelect( ret );
	return ret;
	}

// -----------------------------------------------------------------------------
// <cmail>
// TFsEmailUiUtility::ShowFileSavedToFolderNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( TInt aCount )
    {
    FUNC_LOG;
    if ( !iDownloadSave || iSaveSelect ) 
        { 
	    if ( aCount == 1 )
	        {
	        HBufC* noteText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_ONE_ATTACHMENT_SAVED );
	        ShowDiscreetInfoNoteL( *noteText );
	        CleanupStack::PopAndDestroy( noteText );
	        }
	    else if ( aCount > 1 )
	        {
	        HBufC* noteText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MULTIPLE_ATTACHMENT_SAVED, aCount );
	        ShowDiscreetInfoNoteL( *noteText );
	        CleanupStack::PopAndDestroy( noteText );
	        }
        }
    TFsEmailUiUtility::SetSaveSelect( ETrue ); 
    TFsEmailUiUtility::SetDownloadSave( EFalse ); 
    }

void TFsEmailUiUtility::SetDownloadSave( TBool aValue )
    {
    FUNC_LOG;
    iDownloadSave = aValue;
    }
void TFsEmailUiUtility::SetSaveSelect( TBool aValue )
    {
    FUNC_LOG;
    iSaveSelect = aValue;
    }
// -----------------------------------------------------------------------------
// TFsEmailUiUtility::OpenFileL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::OpenFileL( RFile& aFileToOpen, TBool aAllowSave  )
    {
    FUNC_LOG;
    TDataType emptyDataType;
    OpenFileL( aFileToOpen, emptyDataType,aAllowSave );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::OpenFileL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::OpenFileL( RFile& aFileToOpen, TDataType& aDataType, TBool /* aAllowSave */)
    {
    FUNC_LOG;
    CFreestyleEmailUiAppUi* appUi = (CFreestyleEmailUiAppUi*)CCoeEnv::Static()->AppUi();

    // Set file handle shutter to close the file handle after the embedded
    // viewer application exits.
    appUi->FileHandleShutter().SetFile( aFileToOpen );

    CDocumentHandler& docHandler = appUi->DocumentHandler();

    TInt res = KErrNone;
    TRAPD( err, res = docHandler.OpenFileEmbeddedL( aFileToOpen, aDataType ) );

    // Try again with an empty data type if opening using the given data type fails
    if ( err != KErrNone || res != KErrNone )
        {
        TDataType emptyType;

        // Open file embedded
        TRAP( err, res = docHandler.OpenFileEmbeddedL( aFileToOpen, emptyType ) );

        // Show an error note if opening the file still didn't succeed
        // Ensure also that the file handle got closed.
        if ( err != KErrNone || res != KErrNone )
            {
            ShowErrorNoteL( R_FREESTYLE_EMAIL_INFO_NOTE_UNABLE_TO_OPEN, EFalse );
            appUi->FileHandleShutter().HandleServerAppExit( KErrCancel );
            }
        else
            {
            // Set embedded app flag to true when embedded app start up.
            appUi->SetEmbeddedApp( ETrue );
            // Set flag for judging if previous app is embedded app.
            appUi->SetEmbeddedAppToPreviousApp( ETrue );
            }
        }
    else
        {
        // Set embedded app flag to true when embedded app start up.
        appUi->SetEmbeddedApp( ETrue );
        // Set flag for judging if previous app is embedded app.
        appUi->SetEmbeddedAppToPreviousApp( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::OpenAttachmentL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::OpenAttachmentL( const TPartData& aAttachmentPart )
    {
    FUNC_LOG;
    CFreestyleEmailUiAppUi* appUi = (CFreestyleEmailUiAppUi*)CCoeEnv::Static()->AppUi();
    CFSMailMessage* mailMessage = appUi->GetMailClient()->GetMessageByUidL(
        aAttachmentPart.iMailBoxId, aAttachmentPart.iFolderId,
        aAttachmentPart.iMessageId, EFSMsgDataStructure );

    CleanupStack::PushL( mailMessage );
    CFSMailMessagePart* messagePart = mailMessage->ChildPartL(
        aAttachmentPart.iMessagePartId );
    CleanupStack::PushL( messagePart );

    OpenAttachmentL( *messagePart );

    CleanupStack::PopAndDestroy( messagePart );
    CleanupStack::PopAndDestroy( mailMessage );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::OpenAttachmentL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::OpenAttachmentL( CFSMailMessagePart& aAttachmentPart,
        TBool aAllowSave /*= ETrue*/)
    {
    FUNC_LOG;
    const TDesC& attName = aAttachmentPart.AttachmentNameL();
    const TDesC* mimeType16 = &aAttachmentPart.GetContentType();
    TFileType fileType = GetFileType( attName, *mimeType16 );

    // Check if attachment is actually an embedded message object. In that case we try to
    // open it using mail viewer.
    TBool openedAsMessage = EFalse;
    if ( fileType == EMessageType )
        {
        CFreestyleEmailUiAppUi* appUi =
            static_cast<CFreestyleEmailUiAppUi*>( CCoeEnv::Static()->AppUi() );

        // First, try to get the message object directly from mail client using the UIDs.
        CFSMailMessage* message = NULL;
        TRAP_IGNORE( message = appUi->GetMailClient()->GetMessageByUidL(
                aAttachmentPart.GetMailBoxId(),
                aAttachmentPart.GetFolderId(),
                aAttachmentPart.GetPartId(),
                EFSMsgDataEnvelope ) );

        // If we still have no message, then try to convert the attachment file to message object (Activesync case).
        if ( !message ) 
            {
            // This takes some time so we show a wait dialog.
            if ( iOpeningWaitNote )
                {
                iOpeningWaitNote->ProcessFinishedL();
                iOpeningWaitNote = NULL;
                }
            ShowWaitNoteL( iOpeningWaitNote, R_FSE_WAIT_OPENING_TEXT, EFalse, ETrue );

            RFile attFile = aAttachmentPart.GetContentFileL();
            CleanupClosePushL( attFile );
            CFSMailBox* mailbox = appUi->GetMailClient()->GetMailBoxByUidL( aAttachmentPart.GetMailBoxId() );
            CleanupStack::PushL( mailbox );
            TRAP_IGNORE( message = mailbox->CreateMessageFromFileL( attFile ) );
            CleanupStack::PopAndDestroy( mailbox );
            CleanupStack::PopAndDestroy( &attFile );

            iOpeningWaitNote->ProcessFinishedL();
            }

        // Open message to viewer if we got it. Otherwise continue with the standard file opening logic.
        if ( message )
            {
            // Check that the embedded message has text body part or html body part. 
            // Otherwise we cannot display it viewer and needs to be handled as a normal attachment.
            
            CFSMailMessagePart* htmlPart = message->HtmlBodyPartL();
            CFSMailMessagePart* textPart = message->PlainTextBodyPartL();
            
            if ( htmlPart || textPart  )
                {
                THtmlViewerActivationData htmlData; 
                htmlData.iActivationDataType = THtmlViewerActivationData::EmbeddedEmailMessage;
                htmlData.iMailBoxId = aAttachmentPart.GetMailBoxId();
                htmlData.iFolderId = aAttachmentPart.GetFolderId();
                htmlData.iMessageId = aAttachmentPart.GetPartId();
                // ownership of message gets transfered to html viewer.
                htmlData.iEmbeddedMessage = message;
                htmlData.iEmbeddedMessageMode = ETrue;
                TPckgBuf<THtmlViewerActivationData> pckgData( htmlData );
                
                appUi->EnterFsEmailViewL( HtmlViewerId, KStartViewerWithEmbeddedMsgPtr, pckgData );
                openedAsMessage = ETrue;
                }
            else
                {
                //delete message object owned by us.
                delete message;
                }
            
            if( htmlPart )
                {
                delete htmlPart;
                }
            if( textPart )
                {
                delete textPart;
                }
            
            }
        }

    // Normal attachment file opening
    if ( !openedAsMessage )
        {
        RFile attachmentFile;

        // Get the temp file provided by plugin
        RFile pluginTempFile = aAttachmentPart.GetContentFileL();
        CleanupClosePushL( pluginTempFile );

        // Check if the temp file has diffrent file extension than the attachment name.
        TFileName tempFileName;
        pluginTempFile.Name( tempFileName );
        const TDesC& attName = aAttachmentPart.AttachmentNameL();
        if ( !DoFileExtensionsMatchL( tempFileName, attName ) )
            {
            // Make a new temporary copy of the attachment file if the temp file
            // provided by the plugin does not have a proper file extension.
            // This is needed because some applications may not
            // allow opening files with improper file extension.
            attachmentFile = CopyFileToTempDirL( pluginTempFile, attName );
            CleanupStack::PopAndDestroy( &pluginTempFile );
            }
        else
            {
            // Otherwise we may use the temp file provided by plugin directly
            attachmentFile = pluginTempFile;
            CleanupStack::Pop( &pluginTempFile );
            }
        CleanupClosePushL( attachmentFile );

        // Use given MIME type if present
        TDataType dataType;
        const TDesC* mimeType16 = &aAttachmentPart.GetContentType();
        if ( mimeType16->Length() )
            {
            // V-calendar attachments sent from MS Outlook seem to have wrong MIME type. Work around
            // this by overriding the MIME type in case the file can be identified to be VCal object
            if ( GetFileType( attName, *mimeType16 ) == EVCalType )
                {
                mimeType16 = &KVCalMimeString;
                }

            // The MIME type must be converted to 8 bit buffer. Just assume it's ASCII data
            // and forget the upper bytes.
            HBufC8* mimeType8 = HBufC8::NewLC( mimeType16->Length() );
            mimeType8->Des().Append( *mimeType16 );
            //cut extra data from MIME string
            TInt pos = mimeType8->Locate( ';' );
            if ( pos >= 0 )
                {
                mimeType8->Des().SetLength(pos);
                }
            dataType = TDataType( *mimeType8 );
            CleanupStack::PopAndDestroy( mimeType8 );
            }

        CleanupStack::Pop( &attachmentFile ); // next function takes ownershp of the handle
        OpenFileL( attachmentFile, dataType, aAllowSave );
        }
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::SetMessageFollowupFlagL
// -----------------------------------------------------------------------------
TFollowUpNewState TFsEmailUiUtility::SetMessageFollowupFlagL( CFSMailMessage& aMsg,
        TBool aShowFlagCompleted )
    {
    FUNC_LOG;
    TFollowUpNewState newFollowUpState = EFollowUpNoChanges;
    if ( TFsEmailUiUtility::RunFollowUpListDialogL( newFollowUpState, aShowFlagCompleted ) )
        {
        SetMessageFollowupStateL( aMsg, newFollowUpState );
        }
    return newFollowUpState;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::SetssageFollowupStateL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::SetMessageFollowupStateL( CFSMailMessage& aMsg,
        TFollowUpNewState aNewFollowUpState )
    {
    FUNC_LOG;
    switch ( aNewFollowUpState )
        {
        case EFollowUp:
            {
            aMsg.SetFlag( EFSMsgFlag_FollowUp );
            aMsg.ResetFlag( EFSMsgFlag_FollowUpComplete );
            }
            break;
        case EFollowUpComplete:
            {
            aMsg.SetFlag( EFSMsgFlag_FollowUpComplete );
            aMsg.ResetFlag( EFSMsgFlag_FollowUp );
            }
            break;
        case EFollowUpClear:
            {
            aMsg.ResetFlag( EFSMsgFlag_FollowUp | EFSMsgFlag_FollowUpComplete );
            }
            break;
        }
    aMsg.SaveMessageL(); // Save flag status
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::RunFollowUpListDialogL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::RunFollowUpListDialogL( TFollowUpNewState& aSelectedOption,
        TBool aShowFlagCompleted )
	{
    FUNC_LOG;
	aSelectedOption = EFollowUpNoChanges;

	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat( 2 );
	CleanupStack::PushL( array );

	// Add follow up text
	_LIT( KListItemIcon0, "0\t" );

    // aShowFlagCompleted is false when we are in the editor/composer view
    TInt followUpResId =
        ( aShowFlagCompleted ? R_FREESTYLE_EMAIL_UI_FLAG_FOLLOW_UP :
                                R_NCS_FLAG_FOLLOW_UP );

    HBufC* followUp = StringLoader::LoadLC( followUpResId );

	HBufC* followUpText = HBufC::NewLC( KListItemIcon0().Length() + followUp->Length() );
    followUpText->Des().Append( KListItemIcon0 );
    followUpText->Des().Append( *followUp );
    array->AppendL( *followUpText );
	CleanupStack::PopAndDestroy( 2, followUp );

	// Add flag complete text if requested
	if ( aShowFlagCompleted )
	    {
	    _LIT( KListItemIcon1, "1\t" );
    	HBufC* completeFlag = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FLAG_COMPLETE );
    	HBufC* completeFlagText = HBufC::NewLC( KListItemIcon1().Length() + completeFlag->Length() );
    	completeFlagText->Des().Append( KListItemIcon1 );
        completeFlagText->Des().Append( *completeFlag );
        array->AppendL( *completeFlagText );
    	CleanupStack::PopAndDestroy( 2, completeFlag );
        }

	// Add clear flag text
	_LIT( KListItemNoIcon, "\t" );

    // aShowFlagCompleted is false when we are in the editor/composer view
    TInt clearResId = ( aShowFlagCompleted ? R_FREESTYLE_EMAIL_UI_FLAG_CLEAR :
                                             R_NCS_FLAG_CLEAR );

	HBufC* clearFlag = StringLoader::LoadLC( clearResId );
	HBufC* clearFlagText = HBufC::NewLC( KListItemNoIcon().Length() + clearFlag->Length() );
	clearFlagText->Des().Append( KListItemNoIcon );
	clearFlagText->Des().Append( *clearFlag );
	array->AppendL( *clearFlagText );
	CleanupStack::PopAndDestroy( 2, clearFlag );

	TInt selectedOption;
	CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedOption );

    // aShowFlagCompleted is false when we are in the editor/composer view
	TInt dlgResId = ( aShowFlagCompleted ? R_MAIL_FLAG_QUERY_DIALOG :
	                                       R_NCS_MAIL_FLAG_QUERY_DIALOG );

	dlg->PrepareLC( dlgResId );

	//Create icon array from correct icons
    TFileName iconFileName;
    GetFullIconFileNameL( iconFileName );

    CArrayPtr<CGulIcon>* icons = new (ELeave) CAknIconArray( 2 );
	CleanupStack::PushL(icons);
    CFbsBitmap* iconBitmap;
    CFbsBitmap* iconMaskBitmap;
    TSize defaultIconSize(20,20);

// <cmail> icons changed
    // Flag icon "followup"
    AknsUtils::CreateColorIconLC(
        AknsUtils::SkinInstance(),
        KAknsIIDNone,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG7,
        iconBitmap,
        iconMaskBitmap,
        iconFileName,
        EMbmFreestyleemailuiQgn_indi_navi_follow_up,
        EMbmFreestyleemailuiQgn_indi_navi_follow_up_mask,
        KRgbBlack );
	AknIconUtils::SetSize(iconBitmap, defaultIconSize );
    AknIconUtils::SetSize(iconMaskBitmap, defaultIconSize);
    CGulIcon* flagIcon = CGulIcon::NewL(iconBitmap, iconMaskBitmap);
    CleanupStack::Pop( 2, iconBitmap );
    CleanupStack::PushL( flagIcon );
 	icons->AppendL( flagIcon );
 	CleanupStack::Pop( flagIcon );

    // Flag icon "completed"
 	AknsUtils::CreateColorIconLC(
        AknsUtils::SkinInstance(),
        KAknsIIDNone,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG7,
        iconBitmap,
        iconMaskBitmap,
        iconFileName,
        EMbmFreestyleemailuiQgn_indi_navi_follow_up_complete,
        EMbmFreestyleemailuiQgn_indi_navi_follow_up_complete_mask,
        KRgbBlack );
	AknIconUtils::SetSize(iconBitmap, defaultIconSize );
    AknIconUtils::SetSize(iconMaskBitmap, defaultIconSize);
    CGulIcon* flagCompleteIcon = CGulIcon::NewL(iconBitmap, iconMaskBitmap);
    CleanupStack::Pop( 2, iconBitmap );
    CleanupStack::PushL( flagCompleteIcon );
	icons->AppendL( flagCompleteIcon );
    CleanupStack::Pop( flagCompleteIcon );
// </cmail>

    // Run the dialog
    dlg->SetItemTextArray( array );
	dlg->SetIconArrayL( icons );
	CleanupStack::Pop( icons );
	dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
	TBool retVal = dlg->RunLD();
	CleanupStack::PopAndDestroy( array );

	if ( retVal )
	    {
    	// Convert the returned index to enumeration. This mapping is different depending
    	// if "flag completed" is shown or not
        aSelectedOption = static_cast<TFollowUpNewState>( selectedOption );
    	if ( !aShowFlagCompleted && aSelectedOption == EFollowUpComplete )
    	    {
    	    aSelectedOption = EFollowUpClear;
    	    }
	    }

	return retVal;
	}


// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ShowCreateMessageQueryL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ShowCreateMessageQueryL( const TDesC& aAddressData, TBool aSendingToEmail )
	{
    FUNC_LOG;
	if ( aAddressData.Length() ) // Sending needs address data in any case
		{
		CSendUi* sendUi = CSendUi::NewLC();
		TSendingCapabilities noCapabilities(0,0,0);

		CArrayFix<TUid>* showedServicesUidArray = new( ELeave ) CArrayFixFlat<TUid>(4);
		CleanupStack::PushL( showedServicesUidArray );

		CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( 2 );
		CleanupStack::PushL( array );

		if( !aSendingToEmail && sendUi->ValidateServiceL( KSenduiMtmSmsUid, noCapabilities ) )
			{
			HBufC* textMessage = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_TEXT_MESSAGE );
		    array->AppendL( *textMessage );
			CleanupStack::PopAndDestroy( textMessage );
			showedServicesUidArray->AppendL( KSenduiMtmSmsUid );
			}
		if( sendUi->ValidateServiceL( KSenduiMtmMmsUid, noCapabilities ) )
			{
			HBufC* multimediaMessage = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_MULTIMEDIA_MESSAGE );
		    array->AppendL( *multimediaMessage );
			CleanupStack::PopAndDestroy( multimediaMessage );
			showedServicesUidArray->AppendL( KSenduiMtmMmsUid );
			}
		if( sendUi->ValidateServiceL( KSenduiMtmAudioMessageUid, noCapabilities ) )
			{
			HBufC* voiceMessage = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_VOICE_MESSAGE );
		    array->AppendL( *voiceMessage );
			CleanupStack::PopAndDestroy( voiceMessage );
			showedServicesUidArray->AppendL( KSenduiMtmAudioMessageUid );
			}

		TInt selectedOption;
		CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedOption );
		dlg->PrepareLC( R_MAIL_VIEWER_CREATE_MESSAGE_DIALOG  );
		dlg->SetItemTextArray( array );
		dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
		if( dlg->RunLD() )
			{
			CMessageData* messageData = CMessageData::NewLC();
			messageData->AppendToAddressL( aAddressData );
			// safety check.
			if( showedServicesUidArray->Count() > selectedOption )
				{
				sendUi->CreateAndSendMessageL( showedServicesUidArray->At( selectedOption ),
												messageData );
				}
			CleanupStack::PopAndDestroy( messageData );
			}

		CleanupStack::PopAndDestroy( array );
		CleanupStack::PopAndDestroy( showedServicesUidArray );
		CleanupStack::PopAndDestroy( sendUi );
		}
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::GetFileType
// Figures out the file type based on the MIME type and the file name extension.
// The MIME type is used first and if that doesn't help, then the file extension
// is used as backup solution.
// -----------------------------------------------------------------------------
TFileType TFsEmailUiUtility::GetFileType( const TDesC& aFileName, const TDesC& aMimeType )
    {
    FUNC_LOG;
	TFileType fileType = EUnidentifiedType;
    TPtrC ext = TParsePtrC( aFileName ).Ext();

    // here we make sure that content type does not have extra parameters
    TPtrC mimeType( aMimeType );
    TInt semiColonPos( aMimeType.Locate(';') );
    if( semiColonPos != KErrNotFound )
    	{
    	mimeType.Set( aMimeType.Left(semiColonPos) );
    	}
    
	if ( !mimeType.CompareF(KPdfMimeString) )
		{
		fileType = EPdfType;
		}
	else if ( !mimeType.CompareF(KDocMimeString) ||
	          !mimeType.CompareF(KDocMimeString2) )
		{
		fileType = EDocType;
		}
	else if ( !mimeType.CompareF(KRtfMimeString) )
		{
		fileType = ERtfType;
		}
	else if ( !mimeType.CompareF(KPptMimeString) )
		{
		fileType = EPptType;
		}
	else if ( !mimeType.CompareF(KXlsMimeString) )
		{
		fileType = EXlsType;
		}
	else if ( !mimeType.Left( KImageMimeString().Length() ).CompareF(KImageMimeString) )
		{
		fileType = EImageType;
		}
	else if ( !mimeType.CompareF(KHtmlMimeString) )
		{
		fileType = EHtmlType;
		}
	else if ( !mimeType.CompareF(KPlainTextMimeString) )
	    {
	    fileType = EPlainTextType;
	    // When v-cal attachment is sent from MS Outlook, it's received with wrong MIME type.
	    // Following is a workaround for this problem
	    if ( !ext.CompareF(KVCalFileExtension) )
	        {
	        fileType = EVCalType;
	        }
	    }
	else if ( !mimeType.CompareF(KVCalMimeString) )
	    {
	    fileType = EVCalType;
	    }
    else if ( !mimeType.CompareF(KMessageMimeString) )
        {
        fileType = EMessageType;
        }
	else
		{
		// File type couldn't be identified from the MIME type. Use the file extension.

    	if ( !ext.CompareF(KDocFileExtension) )
    		{
    		fileType = EDocType;
    		}
    	else if ( !ext.CompareF(KRtfFileExtension) )
    	    {
    	    fileType = ERtfType;
    	    }
    	else if ( !ext.CompareF(KPptFileExtension) )
    		{
    		fileType = EPptType;
    		}
    	else if ( !ext.CompareF(KXlsFileExtension) )
    		{
    		fileType = EXlsType;
    		}
    	else if ( !ext.CompareF(KPdfFileExtension) )
    		{
    		fileType = EPdfType;
    		}
    	else if ( !ext.CompareF(KHtmlFileExtension) ||
    	          !ext.CompareF(KHtmFileExtension) )
    	    {
    	    fileType = EHtmlType;
    	    }
    	else if ( !ext.CompareF(KJpgFileExtension)
    			  || !ext.CompareF(KJpgeFileExtension)
    			  || !ext.CompareF(KJpegFileExtension)
    			  || !ext.CompareF(KPngFileExtension)
    			  || !ext.CompareF(KGifFileExtension)
    			  || !ext.CompareF(KBmpFileExtension) )
    	    {
    	    fileType = EImageType;
    	    }
    	else if ( !ext.CompareF(KTxtFileExtension) )
    	    {
    	    fileType = EPlainTextType;
    	    }
    	else if ( !ext.CompareF(KVCalFileExtension) )
            {
            fileType = EVCalType;
            }
        else if ( !ext.CompareF(KEmailFileExtension) )
            {
            fileType = EMessageType;
            }
    	else
    	    {
    	    fileType = EUnidentifiedType;
    	    }
		}

    return fileType;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::GetAttachmentIcon
// Funtion for getting correct attachment icon
// -----------------------------------------------------------------------------
CAlfTexture& TFsEmailUiUtility::GetAttachmentIcon( TFileType aAttachmentType, CFreestyleEmailUiTextureManager& aTextureManager )
    {
    FUNC_LOG;
	TFSEmailUiTextures textureId = GetAttachmentIconTextureId( aAttachmentType );
	return aTextureManager.TextureByIndex( textureId );
    }

TFSEmailUiTextures TFsEmailUiUtility::GetAttachmentIconTextureId( TFileType aAttachmentType )
    {
    FUNC_LOG;
    switch( aAttachmentType )
    	{
		case EDocType:
     		return EAttachmentsDocFile;
		case ERtfType:
			return EAttachmentsRtfFile;
        case EPptType:
			return EAttachmentsPptFile;
		case EXlsType:
			return EAttachmentsXls;
		case EPdfType:
			return EAttachmentsPdfFile;
		case EImageType:
			return EAttachmentsImageFile;
		case EHtmlType:
			return EAttachmentsHtmlFile;
		case EPlainTextType:
		case EVCalType:
		case EMessageType:
		case EUnidentifiedType:
			return EAttachmentsUnknownFile;
		default:
			ASSERT( EFalse );
    	}
    return EAttachmentsUnknownFile;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::GetMsgIcon
// Funtion for getting correct msg icon, divides into read and unread icons
// -----------------------------------------------------------------------------
CAlfTexture& TFsEmailUiUtility::GetMsgIcon( CFSMailMessage* aMsgPtr,
									 CFreestyleEmailUiTextureManager& aTextureManager )
	{
    FUNC_LOG;
	TFSEmailUiTextures textureId = GetMsgIconTextureId( aMsgPtr );
	return aTextureManager.TextureByIndex( textureId );
	}

TFSEmailUiTextures TFsEmailUiUtility::GetMsgIconTextureId( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	if ( !aMsgPtr->IsFlagSet( EFSMsgFlag_Read ) )
		{
		return GetUnreadMsgIcon( aMsgPtr );
		}
	else
		{
		return GetReadMsgIcon( aMsgPtr );
		}

	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::MoveMessageToDraftsL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::MoveMessageToDraftsL( CFSMailBox& aMailBox, CFSMailMessage& aMsg )
	{
    FUNC_LOG;

	// check that msg is not in drafts folder already
	TFSMailMsgId draftsFolderId = aMailBox.GetStandardFolderId( EFSDraftsFolder );
	TFSMailMsgId msgFolderId = aMsg.GetFolderId();
	if ( draftsFolderId != msgFolderId )
		{
		RArray<TFSMailMsgId> ids;
		ids.Append( aMsg.GetMessageId() );
		aMailBox.MoveMessagesL( ids, msgFolderId, draftsFolderId );
		ids.Reset();
		}

	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::IsMessagePartFullyFetched
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::IsMessagePartFullyFetched( const CFSMailMessagePart& aPart )
	{
    FUNC_LOG;

	TFSPartFetchState fetchState = aPart.FetchLoadState();
	TBool isFetched = ( fetchState == EFSFull );

	return isFetched;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::IsMessageStructureKnown
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::IsMessageStructureKnown( const CFSMailMessagePart& aPart )
	{
    FUNC_LOG;

	TFSPartFetchState fetchState = aPart.FetchLoadState();
	TBool isKnown = !( fetchState == EFSEmailStructureUnknown );//( fetchState == EFSFull || fetchState == EFSMessagePartsKnown );

	return isKnown;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CreatePlainTextPartL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::CreatePlainTextPartL( CFSMailMessage& aMsg, CFSMailMessagePart*& aPart )
	{
    FUNC_LOG;

    aPart = aMsg.PlainTextBodyPartL();
    if ( !aPart )
    	{
        // Do message contain HTML body part
        CFSMailMessagePart* htmlPart = aMsg.HtmlBodyPartL();
        if ( htmlPart )
            {
            CleanupStack::PushL( htmlPart );
            HBufC* htmlData = HBufC::NewLC( htmlPart->FetchedContentSize() );
            TPtr pointer = htmlData->Des();
            htmlPart->GetContentToBufferL( pointer, 0 );

            HBufC* txtData = TFsEmailUiUtility::ConvertHtmlToTxtL( *htmlData );

            CleanupStack::PopAndDestroy( htmlData );

            CleanupStack::PushL( txtData );

            // create new message part for body text
            TFSMailMsgId id;
            aPart = aMsg.NewChildPartL( id, KFSMailContentTypeTextPlain );
            if ( aPart )
                {
                aPart->SetContentType( KFSMailContentTypeTextPlain );
                pointer.Set( txtData->Des() );
                aPart->SetContent( pointer );
                aPart->SaveL();
                }

            CleanupStack::PopAndDestroy( txtData );
            CleanupStack::PopAndDestroy( htmlPart );
            }
        else
            {
    		// create new message part for body text
    		TFSMailMsgId id;
    		aPart = aMsg.NewChildPartL( id, KFSMailContentTypeTextPlain );
            if ( aPart )
                {
                aPart->SetContentType( KFSMailContentTypeTextPlain );
                aPart->SaveL();
                }
            }
    	}

	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::IsCompleteOrCancelEvent
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::IsCompleteOrCancelEvent( TFSProgress aEvent )
	{
    FUNC_LOG;
    return ( TFSProgress::EFSStatus_RequestComplete == aEvent.iProgressStatus ||
    	TFSProgress::EFSStatus_RequestCancelled == aEvent.iProgressStatus );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::HasUnfetchedAttachmentsL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::HasUnfetchedAttachmentsL( CFSMailMessage& aMsg )
	{
    FUNC_LOG;
	RPointerArray<CFSMailMessagePart> attachments;
	aMsg.AttachmentListL( attachments );
	TBool found = EFalse;
	for ( TInt i=0; i<attachments.Count(); i++ )
		{
		if ( !IsMessagePartFullyFetched( *attachments[i] ) )
			{
			found = ETrue;
			break;
			}
		}
	attachments.ResetAndDestroy();
	return found;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::IsFollowUpSupported
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::IsFollowUpSupported( const CFSMailBox& aMailBox )
	{
    FUNC_LOG;
	TBool supported = aMailBox.HasCapability( EFSMBoxCapaSupportsFollowUp );
	return supported;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::DateTextFromMsgLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::DateTextFromMsgLC( const CFSMailMessage* aMessage,
	TBool aAddYearNumer )
	{
    FUNC_LOG;
	HBufC* ret = HBufC::NewLC( 20 );
	TTime eventTimeInHomeTime = aMessage->GetDate();
    TLocale currentLocaleSettings;
    eventTimeInHomeTime = eventTimeInHomeTime +
    					  (currentLocaleSettings.UniversalTimeOffset());
    eventTimeInHomeTime += currentLocaleSettings.QueryHomeHasDaylightSavingOn()
    					  ? TTimeIntervalHours(1) : TTimeIntervalHours(0);
	TDateFormat dateFormat = currentLocaleSettings.DateFormat();
	TChar secondDateSeparator = currentLocaleSettings.DateSeparator( 1 );
	TChar thirdDateSeparator = currentLocaleSettings.DateSeparator( 2 );

	if ( aAddYearNumer )
		{
		switch ( dateFormat )
			{
			case EDateAmerican: // US format (mm/dd/yyyy)
				{
				ret->Des().Format(_L("%02d%c%02d%c%4d"),
			 		eventTimeInHomeTime.DateTime().Month()+1,
			 		(TUint)secondDateSeparator,
			 		eventTimeInHomeTime.DateTime().Day()+1,
			 		(TUint)thirdDateSeparator,
			 		eventTimeInHomeTime.DateTime().Year());
				}
				break;
			case EDateJapanese: // Japanese format (yyyy/mm/dd)
				{
				ret->Des().Format( _L("%4d%c%02d%c%02d"),
			 		eventTimeInHomeTime.DateTime().Year(),
			 		(TUint)secondDateSeparator,
			 		eventTimeInHomeTime.DateTime().Month()+1,
			 		(TUint)thirdDateSeparator,
			 		eventTimeInHomeTime.DateTime().Day()+1 );
				}
				break;
			case EDateEuropean: // European format (dd/mm/yyyy)
			default:
				{
				ret->Des().Format(_L("%02d%c%02d%c%4d"),
			 		eventTimeInHomeTime.DateTime().Day()+1,
			 		(TUint)secondDateSeparator,
			 		eventTimeInHomeTime.DateTime().Month()+1,
			 		(TUint)thirdDateSeparator,
			 		eventTimeInHomeTime.DateTime().Year());
				}
				break;
			}
		}
	else
		{
		switch ( dateFormat )
			{
			case EDateAmerican: // US format (mm/dd/yyyy)
				{
				ret->Des().Format(_L("%02d%c%02d"),
			 		eventTimeInHomeTime.DateTime().Month()+1,
			 		(TUint)secondDateSeparator,
			 		eventTimeInHomeTime.DateTime().Day()+1 );
				}
				break;
			case EDateJapanese: // Japanese format (yyyy/mm/dd)
				{
				ret->Des().Format( _L("%02d%c%02d"),
			 		eventTimeInHomeTime.DateTime().Month()+1,
			 		(TUint)thirdDateSeparator,
			 		eventTimeInHomeTime.DateTime().Day()+1 );
				}
				break;
			case EDateEuropean: // European format (dd/mm/yyyy)
			default:
				{
				ret->Des().Format(_L("%02d%c%02d"),
			 		eventTimeInHomeTime.DateTime().Day()+1,
			 		(TUint)secondDateSeparator,
			 		eventTimeInHomeTime.DateTime().Month()+1 );
				}
				break;
			}
		}
   	TPtr dataPtr( ret->Des() );
   	AknTextUtils::LanguageSpecificNumberConversion( dataPtr );
	return ret;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::TimeTextFromMsgLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::TimeTextFromMsgLC( const CFSMailMessage* aMessage )
	{
    FUNC_LOG;
    TBuf<20> timeStr;
    TBuf<20> timeStrFormat;
	TTime eventTimeInHomeTime = aMessage->GetDate();
    TLocale currentLocaleSettings;
    eventTimeInHomeTime = eventTimeInHomeTime +
    					  (currentLocaleSettings.UniversalTimeOffset());
    eventTimeInHomeTime += currentLocaleSettings.QueryHomeHasDaylightSavingOn()
    					  ? TTimeIntervalHours(1) : TTimeIntervalHours(0);
    if( currentLocaleSettings.AmPmSymbolPosition() == ELocaleBefore )
        {
        timeStrFormat.Append( KTimeFormatBefore );
        timeStrFormat.Append( KTimeFormatSpace );
        timeStrFormat.Append( KTimeFormatHour );
        timeStrFormat.Append( currentLocaleSettings.TimeSeparator( KSecondSeparator ) );
        timeStrFormat.Append( KTimeFormatMinutes );
        eventTimeInHomeTime.FormatL( timeStr, timeStrFormat );
        }
    else
        {
        timeStrFormat.Append( KTimeFormatHour );
        timeStrFormat.Append( currentLocaleSettings.TimeSeparator( KSecondSeparator ) );
        timeStrFormat.Append( KTimeFormatMinutes );
        timeStrFormat.Append( KTimeFormatSpace );
        timeStrFormat.Append( KTimeFormatAfter );
        eventTimeInHomeTime.FormatL( timeStr, timeStrFormat );
        }
    HBufC* dataStr = HBufC::NewL( 1 + timeStr.Length() );
    TPtr dataPtr( dataStr->Des() );
    dataPtr.Append( timeStr );
    CleanupStack::PushL( dataStr );
    AknTextUtils::LanguageSpecificNumberConversion( dataPtr );
    return dataStr;
	}


// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ListMsgTimeTextFromMsgLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::ListMsgTimeTextFromMsgLC( const CFSMailMessage* aMessage, TBool aDividersInUse )
	{
    FUNC_LOG;
	// If dividers are in use, returjn normal time text
    if ( aDividersInUse )
    	{
    	return TimeTextFromMsgLC( aMessage );
    	}

	// If dividers are not in use, today's emails need to use time, others need to use date.
 	TTime eventTimeInHomeTime = aMessage->GetDate();
	TTime currentTime;
	currentTime.HomeTime();
 	TLocale currentLocaleSettings;
    eventTimeInHomeTime = eventTimeInHomeTime +
    					  (currentLocaleSettings.UniversalTimeOffset());
    eventTimeInHomeTime += currentLocaleSettings.QueryHomeHasDaylightSavingOn()
    					  ? TTimeIntervalHours(1) : TTimeIntervalHours(0);
 	if ( currentTime.DayNoInYear() == eventTimeInHomeTime.DayNoInYear() &&
 		 currentTime.DateTime().Year() == eventTimeInHomeTime.DateTime().Year() )
 		{
		// Event is today, use time text from msg
		return TimeTextFromMsgLC( aMessage );
    	}
    else
    	{
    	// Event is not from today, use date text without year number
		return DateTextFromMsgLC( aMessage, EFalse );
    	}
	}


// -----------------------------------------------------------------------------
// TFsEmailUiUtility::WeekDayTextFromMsgLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::WeekDayTextFromMsgLC( const CFSMailMessage* aMessage,
													 TBool aUseToday,
													 TBool aUseYesterday,
													 TBool& aWasToday,
													 TBool& aWasYesterday )
	{
    FUNC_LOG;
	HBufC* ret(0);
	TTime eventTimeInHomeTime = aMessage->GetDate();
	TTime currentTime;
	currentTime.HomeTime();
    TLocale currentLocaleSettings;

	// Correct current time and event time UTC offset and daylight saving time
    eventTimeInHomeTime = eventTimeInHomeTime +
    					  (currentLocaleSettings.UniversalTimeOffset());
    eventTimeInHomeTime += currentLocaleSettings.QueryHomeHasDaylightSavingOn()
    					  ? TTimeIntervalHours(1) : TTimeIntervalHours(0);

 	TBool eventTimeIsToday(EFalse);
 	TBool eventTimeIsYesterday(EFalse);
 	// Check if year and day number in year are the same, e.g. today
 	if ( currentTime.DayNoInYear() == eventTimeInHomeTime.DayNoInYear() &&
 		 currentTime.DateTime().Year() == eventTimeInHomeTime.DateTime().Year() )
 		{
		eventTimeIsToday = ETrue;
  		}
 	// Check if year and current day number minus one in year are the same, e.g. yesterday
 	else if ( currentTime.DayNoInYear()-1 == eventTimeInHomeTime.DayNoInYear() &&
 		currentTime.DateTime().Year() == eventTimeInHomeTime.DateTime().Year() )
 		{
		eventTimeIsYesterday = ETrue;
		}
	if ( aUseToday && eventTimeIsToday )
		{
		ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_TODAY );
		aWasToday = ETrue;
		}
	else if ( aUseYesterday && eventTimeIsYesterday )
		{
		ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_YESTERDAY );
		aWasYesterday = ETrue;
		}
	else
		{
		switch ( eventTimeInHomeTime.DayNoInWeek() )
			{
			case EMonday:
				ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_MONDAY );
				break;
			case ETuesday:
				ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_TUESDAY );
				break;
			case EWednesday:
				ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_WEDNESDAY );
				break;
			case EThursday:
				ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_THURSDAY );
				break;
			case EFriday:
				ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_FRIDAY );
				break;
			case ESaturday:
				ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_SATURDAY );
				break;
			case ESunday:
				ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_WEEEKDAY_SUNDAY );
				break;
			}
		}
	return ret;
	}


// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CreateSizeDescLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::CreateSizeDescLC( TInt aSizeInBytes,
                                            TBool aShowSizeInBytes )
    {
    FUNC_LOG;
    HBufC* sizeDesc = NULL;

    if ( aSizeInBytes >= KMega ) // Show in MB with one decimal
        {
        // The largest possible file size in 32bit architecture is 2^32 bytes = 4096 MB.
        // Thus, we need to reserve 4 characters for the megabyte part, one for decimal
        // separator, and one for decimal part.
        HBufC* numberBuf = HBufC::NewLC(4+1+1);
        TPtr numberDes = numberBuf->Des();

        TInt megaBytePart = aSizeInBytes / KMega;
        TInt remainder = aSizeInBytes % KMega;
        TInt decimalPart = (remainder*10 + KMega/2) / KMega; // The +KMega/2 is used to get normal rounding rules
        // if decimal part gets rounded up to 10, we need to increment the megabyte part
        if ( decimalPart == 10 )
            {
            megaBytePart++;
            decimalPart = 0;
            }

        TLocale locale;
        TChar decimalSeparator = locale.DecimalSeparator();

        numberDes.Num( megaBytePart );
        numberDes.Append( decimalSeparator );
        numberDes.AppendNum( decimalPart );

        if( aShowSizeInBytes )
            {
            sizeDesc = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_SIZE_IN_MB_WITH_BYTES,
                                            *numberBuf, aSizeInBytes );
            }
        else
            {
            sizeDesc = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_SIZE_IN_MB, *numberBuf );
            }
        CleanupStack::PopAndDestroy( numberBuf );
        CleanupStack::PushL( sizeDesc );
        }
    else if ( aSizeInBytes >= KKilo || aSizeInBytes == 0 ) // Show in kB without decimals
        {
        TInt sizeInKB = (aSizeInBytes + KKilo/2) / KKilo; // The +KKilo/2 is used to get normal rounding rules
        if( aShowSizeInBytes )
            {
            HBufC* numberBuf = HBufC::NewLC(8);
            TPtr numberDes = numberBuf->Des();
            numberDes.Num( sizeInKB );

            sizeDesc = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_SIZE_IN_KB_WITH_BYTES,
                                            *numberBuf, aSizeInBytes );
            CleanupStack::PopAndDestroy( numberBuf );
            CleanupStack::PushL( sizeDesc );
            }
        else
            {
            sizeDesc = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SIZE_IN_KB, sizeInKB );
            }
        }
    else if ( aSizeInBytes > 0 ) // show "less than kilobyte"
        {
        if( aShowSizeInBytes )
            {
            sizeDesc = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SIZE_LESS_THAN_KB_WITH_BYTES,
                                            aSizeInBytes );
            }
        else
            {
            sizeDesc = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_SIZE_LESS_THAN_KB );
            }
        }
    else // negative numbers are not supported
        {
        __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
        sizeDesc = KNullDesC().AllocLC();
        }

    // Do number conversion to Arabic/Eastern Arabic/Indic digits when necessary
    TPtr des = sizeDesc->Des();
    AknTextUtils::LanguageSpecificNumberConversion( des );

    return sizeDesc;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CompareMailSubjectsL
// -----------------------------------------------------------------------------
TInt TFsEmailUiUtility::CompareMailSubjectsL( const CFSMailMessage* aMessage1,
                                              const CFSMailMessage* aMessage2 )
	{
    FUNC_LOG;
	TInt result( 0 );

	if ( aMessage1 &&  aMessage2 )
		{
		HBufC* compareSubject1 = CreateSubjectWithoutLocalisedPrefixLC( aMessage1, ETrue );
		compareSubject1->Des().LowerCase();
		HBufC* compareSubject2 = CreateSubjectWithoutLocalisedPrefixLC( aMessage2, ETrue );
		compareSubject2->Des().LowerCase();

		result = compareSubject1->CompareC( *compareSubject2 );

		CleanupStack::PopAndDestroy( compareSubject2 );
		CleanupStack::PopAndDestroy( compareSubject1 );
		}
	// Check if one subject pointer is valid instead of other -> result is non-zero.
	else if ( aMessage1 && !aMessage2  )
	    {
	    result = 1;
	    }
	else if ( !aMessage1 && aMessage2 )
		{
		result = -1;
		}
	return result;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CreateSubjectWithoutLocalisedPrefixLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::CreateSubjectWithoutLocalisedPrefixLC(
        const CFSMailMessage* aMessage,
        TBool aSuppressNotAvailableText /*= EFalse*/ )
	{
    FUNC_LOG;
    TDesC* subject = &aMessage->GetSubject();
	TPtrC croppedSubject;
    if ( subject )
        {
        croppedSubject.Set( *subject );
        }

	TBool prefixFound = EFalse;

	do
	    {
	    prefixFound = EFalse;

	    // Remove leading white space before trying to find the prefix
	    while( croppedSubject.Length() &&
	           TChar(croppedSubject[0]).IsSpace() )
	        {
	        croppedSubject.Set( croppedSubject.Mid(1) );
	        }

    	// try to find ":" at the beginning
		// Locate : character on location 1,2 and 3
	    static const TInt KPrefixMinLength = 1;
	    static const TInt KPrefixMaxLength = 3;
	    static const TText KPrefixSeparator = ':';
		TInt separatorPosition = croppedSubject.Locate( KPrefixSeparator );
		if ( separatorPosition >= KPrefixMinLength &&
		     separatorPosition <= KPrefixMaxLength )
			{
			TPtrC prefixCandidate = croppedSubject.Left( separatorPosition );
			// Only fully alphabetic prefixes are cropped
			TBool isAlpha = ETrue;
			for ( TInt i = 0 ; i < prefixCandidate.Length() ; ++i )
			    {
			    if ( !TChar( prefixCandidate[i] ).IsAlpha() )
			        {
			        isAlpha = EFalse;
			        }
			    }
			if ( isAlpha )
			    {
    			croppedSubject.Set( croppedSubject.Mid( separatorPosition+1 ) );
    			prefixFound = ETrue;
			    }
			}
	    }
	while ( prefixFound );

	HBufC* ret = croppedSubject.AllocLC();

	// Filter out undesirable characters and remove extra white space
	TPtr retPtr = ret->Des();
	FilterListItemTextL( retPtr );

	// Use "(no subject available)" text if subject is emtpy and this is not forbidden by caller
	if ( !ret->Length() && !aSuppressNotAvailableText )
	    {
	    CleanupStack::PopAndDestroy( ret );
	    ret = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_NO_SUBJECT );
	    }

	return ret;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CreateSubjectTextLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::CreateSubjectTextLC( const CFSMailMessage* aMessage )
    {
    FUNC_LOG;
    HBufC* subjectText = NULL;

    if ( aMessage )
        {
        const TDesC* subject = &aMessage->GetSubject();
        if ( subject )
            {
            subjectText = subject->AllocLC();
            TPtr ptr( subjectText->Des() );
            FilterListItemTextL( ptr );
            // discard the subject text if it's empty after the filtering
            if ( !subjectText->Length() )
                {
                CleanupStack::PopAndDestroy( subjectText );
                subjectText = NULL;
                }
            }
        }

    // Use "(no subject available)" text if no other visible subject got
    if ( !subjectText )
        {
        subjectText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_NO_SUBJECT );
        }

    return subjectText;
    }


// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CreateBodyTextLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::CreateBodyTextLC( const CFSMailMessage* aMessage )
    {
    FUNC_LOG;
    HBufC* bodyText = NULL;

    if ( aMessage )
        {
        CFSMailMessage* msg = const_cast<CFSMailMessage*> (aMessage);
        CFSMailMessagePart* bodyPart = msg->PlainTextBodyPartL();
        if ( bodyPart )
            {
            CleanupStack::PushL( bodyPart );
            bodyText = HBufC::NewLC(bodyPart->ContentSize());
            TPtr ptr =  bodyText->Des();
            bodyPart->GetContentToBufferL(ptr, 0);

            FilterListItemTextL( ptr );
            // discard the subject text if it's empty after the filtering
            if ( !bodyText->Length() )
                {
                CleanupStack::PopAndDestroy( bodyText );
                bodyText = NULL;
                }
            CleanupStack::PopAndDestroy( bodyPart );
            }
        }

    // Use "(no subject available)" text if no other visible subject got
    if ( !bodyText )
        {
        bodyText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_NO_SUBJECT );
        }

    return bodyText;
    }



// ---------------------------------------------------------------------------
// TFsEmailUiUtility::FilterPreviewPaneTextL
// This function crops out tabulator and line feed characters from the given data.
// After that, it will makes sure that there are no consecutive space characters for
// preview pane text
// ---------------------------------------------------------------------------
//
void TFsEmailUiUtility::FilterListItemTextL( TDes& aText )
    {
    FUNC_LOG;
    _LIT( KCharsToReplace, "\r\n\t\x2028\x2029" );

    AknTextUtils::ReplaceCharacters( aText, KCharsToReplace, ' ' );
    aText.TrimAll();
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::MessageSizeClass
// -----------------------------------------------------------------------------
TMessageSizeClass TFsEmailUiUtility::MessageSizeClass( const CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    TUint msgSize = aMessage.ContentSize();
    TMessageSizeClass sizeClass;

    if ( msgSize < ETiny )
        {
        sizeClass = ETiny;
        }
    else if ( msgSize < ESmall )
        {
        sizeClass = ESmall;
        }
    else if ( msgSize < EMedium )
        {
        sizeClass = EMedium;
        }
    else if ( msgSize < ELarge )
        {
        sizeClass = ELarge;
        }
    else if ( msgSize < EVeryLarge )
        {
        sizeClass = EVeryLarge;
        }
    else if ( msgSize < EHuge )
        {
        sizeClass = EHuge;
        }
    else
        {
        sizeClass = EColossal;
        }

    return sizeClass;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ConvertHtmlToTxtL
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::ConvertHtmlToTxtL( const TDesC& aHtml )
    {
    FUNC_LOG;
    enum TPrevious
        {
        ENone, // Nothing special in previous character
        EHeadTag, // We are in head tag
        ETagEnd, // Previous character contained tag ending
        ESpace // Previous character contained space
        };

    _LIT( KTagStart, "<" );
    _LIT( KTagEnd, ">" );
    _LIT( KHeadStart, "head" );
    _LIT( KHeadEnd, "/head>" );

    TInt tagCount = 0; // Amount of nested tags
    TInt sizeToParse = aHtml.Length(); // How much we have to parse
    TPrevious previous = ENone; // Previous characters type.
    HBufC* txtBuffer = HBufC::NewLC( sizeToParse );

    // HTML parsing.
    TInt lineFeedCount = 0;
    TInt index = 0; // Parsing index
    for ( index = 0; index < sizeToParse; index++ )
        {
        // Get the next character
        TPtrC ptr = aHtml.Mid( index, 1 );

        // Are we in header
        if ( previous == EHeadTag )
            {
            // Is this a start of end tag
            if ( ptr.Compare( KTagStart ) == 0 &&
                    index + KHeadEnd().Length() < sizeToParse )
                {
                ptr.Set( aHtml.Mid( index + 1, KHeadEnd().Length() ) );
                if ( ptr.Compare( KHeadEnd ) == 0 )
                    {
                    previous = ETagEnd;
                    index += KHeadEnd().Length();
                    }
                }

            }
        // Is this a start of the tag
        else if ( ptr.Compare( KTagStart ) == 0 )
            {
            lineFeedCount = 0;

            if ( index + KHeadStart().Length() < sizeToParse )
                {
                ptr.Set( aHtml.Mid( index + 1, KHeadStart().Length() ) );
                if ( ptr.Compare( KHeadStart ) == 0 )
                    {
                    previous = EHeadTag;
                    index += KHeadStart().Length();
                    }
                }

            if ( previous != EHeadTag )
                {
                // Increase the number of nested tags
                tagCount++;
                previous = ENone;
                }
            }
        // Are we inside of the tag
        else if ( tagCount > 0 )
            {
            // Is this the end of the tag
            if ( ptr.Compare( KTagEnd ) == 0 )
                {
                tagCount--;
                if ( tagCount < 0 )
                    {
                    // To avoid unnecessary errors convert negative
                    // values to zero.
                    tagCount = 0;
                    }
                if ( tagCount == 0 )
                    {
                    previous = ETagEnd;
                    }
                }
            }
        // We are not inside of the tag
        else
            {
            if ( previous == ETagEnd )
                {
                // Skip multiple line feed and carriage return characters
                if ( ptr.Compare( KCarriageReturn ) != 0 &&
                     ptr.Compare( KLineFeed ) != 0 )
                    {
                    if ( lineFeedCount > 0 )
                        {
                        txtBuffer->Des().Append( KLineFeed );
                        lineFeedCount = 0;
                        }
                    txtBuffer->Des().Append( ptr );
                    previous = ENone;
                    }
                else if ( ptr.Compare( KLineFeed ) == 0 )
                    {
                    lineFeedCount++;
                    }
                }
            else
                {
                if ( previous == ESpace )
                    {
                    // Skip consecutive spaces
 //                   if ( ptr.Compare( KSpace ) != 0 )
 //                       {
                            txtBuffer->Des().Append( ptr );
 //                       }
                    }
                else
                    {
                    txtBuffer->Des().Append( ptr );
                    }
                if ( ptr.Compare( KSpace ) == 0 )
                    {
                    previous = ESpace;
                    }
                else
                    {
                    previous = ENone;
                    }
                }
            }
        }

    CleanupStack::Pop( txtBuffer );

    return txtBuffer;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CompareMailAddressesL
// -----------------------------------------------------------------------------
TInt TFsEmailUiUtility::CompareMailAddressesL( CFSMailAddress* aAddr1,
    CFSMailAddress* aAddr2 )
	{
    FUNC_LOG;
	TInt result( 0 );
	if ( aAddr1 && aAddr2 )
		{
		HBufC* compareName1 = CreateCompareNameForAddressLC( *aAddr1 );
        HBufC* compareName2 = CreateCompareNameForAddressLC( *aAddr2 );

        result = compareName1->CompareC( *compareName2 );

        CleanupStack::PopAndDestroy( compareName2 );
        CleanupStack::PopAndDestroy( compareName1 );
		}
		// Check if one address pointer is valid instead of other -> result is non-zero.
	else if ( ( aAddr1 && !aAddr2  ) )
	    {
	    result = 1;
	    }
	else if ( !aAddr1 && aAddr2  )
		{
		result = -1;
		}
	return result;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CreateCompareNameForAddressLC
// -----------------------------------------------------------------------------
HBufC* TFsEmailUiUtility::CreateCompareNameForAddressLC( const CFSMailAddress& aAddress )
    {
    FUNC_LOG;
    HBufC* result = NULL;

    TDesC* displayName = &aAddress.GetDisplayName();
    TDesC* emailAddress = &aAddress.GetEmailAddress();

    if ( displayName && displayName->Length() )
        {
        result = displayName->AllocLC();
        }
    else if ( emailAddress && emailAddress->Length() )
        {
        result = emailAddress->AllocLC();
        }
    else
        {
        result = KNullDesC().AllocLC();
        }

    result->Des().LowerCase();
    return result;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CountRecepients
// -----------------------------------------------------------------------------
TInt TFsEmailUiUtility::CountRecepients( CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;
    return aMsgPtr->GetToRecipients().Count() + aMsgPtr->GetCCRecipients().Count();
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CountRecipientsSmart
// Calculates recipient count from To and Cc recipient as well as tries to 
// see if the message is sent via message list.
// -----------------------------------------------------------------------------
TInt TFsEmailUiUtility::CountRecipientsSmart( CFreestyleEmailUiAppUi& aAppUi, CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;
    TInt numRecipients( CountRecepients( aMsgPtr ) );
    if ( numRecipients == 1 )
        {
        CFSMailBox* mailBox = NULL;
        
        TRAPD( error, mailBox = aAppUi.GetMailClient()->GetMailBoxByUidL( aMsgPtr->GetMailBoxId() ) );
        
        if (mailBox && !error)
            {
            //check if the malbox ownmailaddress is same as the recipients email address. If not, then assume that the
            //email is a distribution list and we need to inc num of Recipients so that "Reply ALL" option appears in UI.
            if ( aMsgPtr->GetToRecipients().Count() )
                {
                if ( mailBox->OwnMailAddress().GetEmailAddress().CompareF(aMsgPtr->GetToRecipients()[0]->GetEmailAddress()) )
                    {
                    numRecipients++;
                    }
                }
            if ( aMsgPtr->GetCCRecipients().Count() )
                {
                if ( mailBox->OwnMailAddress().GetEmailAddress().CompareF(aMsgPtr->GetCCRecipients()[0]->GetEmailAddress()) )
                    {
                    numRecipients++;
                    }
                }
            if ( aMsgPtr->GetBCCRecipients().Count() )
                {
                if ( mailBox->OwnMailAddress().GetEmailAddress().CompareF(aMsgPtr->GetBCCRecipients()[0]->GetEmailAddress()) )
                    {
                    numRecipients++;
                    }
                }
            }
        
        delete mailBox;
        }
    
    return numRecipients;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::IsMessageBodyLargeL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::IsMessageBodyLargeL( CFSMailMessage* aMessage )
    {
    FUNC_LOG;
    TBool ret = EFalse;
    if ( aMessage )
        {
        CFSMailMessagePart* plainTextBodyPart = aMessage->PlainTextBodyPartL();
        CleanupStack::PushL( plainTextBodyPart );
        CFSMailMessagePart* htmlBodyPart = aMessage->HtmlBodyPartL();
        CleanupStack::PushL( htmlBodyPart );

        if ( (plainTextBodyPart && plainTextBodyPart->FetchedContentSize() > KLargePlainTextSizeLimit) ||
             (htmlBodyPart && htmlBodyPart->FetchedContentSize() > KLargeHtmlTextSizeLimit) )
            {
            ret = ETrue;
            }

        CleanupStack::PopAndDestroy( htmlBodyPart );
        CleanupStack::PopAndDestroy( plainTextBodyPart );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::GetUnreadMsgIconL
// -----------------------------------------------------------------------------
TFSEmailUiTextures TFsEmailUiUtility::GetUnreadMsgIcon( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	TFSEmailUiTextures textureId;
	// Unread calendar invitation
	if ( aMsgPtr->IsFlagSet( EFSMsgFlag_CalendarMsg ))
		{
		if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Important ) )
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments ) )
				{
				textureId = EMessageCalInvitationAttachmentsHighPrio ;
				}
			else
				{
				textureId = EMessageCalInvitationHighPrio;
				}
			}
		else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Low ) )
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments ) )
				{
				textureId = EMessageCalInvitationAttachmentsLowPrio;
				}
			else
				{
				textureId = EMessageCalInvitationLowPrio;
				}
			}
		else
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments ) )
				{
				textureId = EMessageCalInvitationAttachments;
				}
			else
				{
				textureId = EMessageCalInvitation;
				}
			}
        // <cmail>
		TBool cancellationMsg = EFalse;
		TRAP_IGNORE( cancellationMsg = IsMrCancellationMsgL( aMsgPtr ) );
		if ( cancellationMsg )
		    {
		    textureId = EMessageCalInvitationCancelled;
		    }
        // </cmail>
		}
	else	// Normal message icons
		{
		// Check whether msg has attachment or not
		if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments )) // Has attachments
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Important )) // High priority, has attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageUnreadRepliedHighPrioIconAttachment;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageUnreadForwardedHighPrioIconAttachment;
					}
				else
					{
					textureId = EMessageHighPrioUnreadIconAttachment;
					}
				}
			else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, has attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageUnreadRepliedLowPrioIconAttachment;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageUnreadForwardedLowPrioIconAttachment;
					}
				else
					{
					textureId = EMessageLowPrioUnreadIconAttachment;
					}
				}
			else // Normal priority, has attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageUnreadRepliedIconAttachment;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageUnreadForwardedIconAttachment;
					}
				else
					{
					textureId = EMessageUnreadIconAttachment;
					}
				}
			}

		else // No attachments
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Important )) // High priority, no attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageUnreadRepliedHighPrioIcon;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageUnreadForwardedHighPrioIcon;
					}
				else
					{
					textureId = EMessageHighPrioUnreadIcon;
					}
				}
			else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, no attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageUnreadRepliedLowPrioIcon;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageUnreadForwardedLowPrioIcon;
					}
				else
					{
					textureId = EMessageLowPrioUnreadIcon;
					}
				}
			else // Normal priority, no attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageUnreadRepliedIcon;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageUnreadForwardedIcon;
					}
				else
					{
					textureId = EMessageUnreadIcon;
					}
				}
			}
		}
	return textureId;
	}


// -----------------------------------------------------------------------------
// TFsEmailUiUtility::GetReadMsgIconL
// -----------------------------------------------------------------------------
TFSEmailUiTextures TFsEmailUiUtility::GetReadMsgIcon( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	TFSEmailUiTextures textureId;
	// Check for calendar invitation first
	if ( aMsgPtr->IsFlagSet( EFSMsgFlag_CalendarMsg ))
		{
		if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Important ) )
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments ) )
				{
				textureId = EMessageCalInvitationReadAttachmentsHighPrio ;
				}
			else
				{
				textureId = EMessageCalInvitationReadHighPrio;
				}
			}
		else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Low ) )
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments ) )
				{
				textureId = EMessageCalInvitationReadAttachmentsLowPrio;
				}
			else
				{
				textureId = EMessageCalInvitationReadLowPrio;
				}
			}
		else
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments ) )
				{
				textureId = EMessageCalInvitationReadAttachments;
				}
			else
				{
				textureId = EMessageCalInvitationRead;
				}
			}
		// <cmail>
		TBool cancellationMsg = EFalse;
		TRAP_IGNORE( cancellationMsg = IsMrCancellationMsgL( aMsgPtr ) );
		if ( cancellationMsg )
		    {
            textureId = EMessageCalInvitationCancelled;
            }
        // </cmail>
		}
	else	// Normal message icons
		{
		// Check whether msg has attachment or not
		if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Attachments )) // Has attachments
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Important )) // High priority, has attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageReadRepliedHighPrioIconAttachment;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageReadForwardedHighPrioIconAttachment;
					}
				else
					{
					textureId = EMessageHighPrioReadIconAttachment;
					}
				}
			else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, has attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageReadRepliedLowPrioIconAttachment;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageReadForwardedLowPrioIconAttachment;
					}
				else
					{
					textureId = EMessageLowPrioReadIconAttachment;
					}
				}
			else // Normal priority, has attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageReadRepliedIconAttachment;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageReadForwardedIconAttachment;
					}
				else
					{
					textureId = EMessageReadIconAttachment;
					}
				}
			}
		else // No attachments
			{
			if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Important )) // High priority, no attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageReadRepliedHighPrioIcon;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageReadForwardedHighPrioIcon;
					}
				else
					{
					textureId = EMessageHighPrioReadIcon;
					}
				}
			else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Low ) ) // Low priority, no attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageReadRepliedLowPrioIcon;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageReadForwardedLowPrioIcon;
					}
				else
					{
					textureId = EMessageLowPrioReadIcon;
					}
				}
			else // Normal priority, no attachments
				{
				if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Answered ) )
					{
					textureId = EMessageReadRepliedIcon;
					}
				else if ( aMsgPtr->IsFlagSet( EFSMsgFlag_Forwarded ) )
					{
					textureId = EMessageReadForwardedIcon;
					}
				else
					{
					textureId = EMessageReadIcon;
					}
				}
			}
		}
	return textureId;
	}

// <cmail>
// -----------------------------------------------------------------------------
// TFsEmailUiUtility::IsMrCancellationMsgL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::IsMrCancellationMsgL( CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;
    TBool isCancel = EFalse;

    if ( !iMrViewer )
        {
        if ( !iMrViewerCallback )
            {
            iMrViewerCallback = new (ELeave) CMrViewerEmptyCallback;
            }
        iMrViewer = CESMRIcalViewer::NewL( *iMrViewerCallback );
        }

    TESMRMeetingRequestMethod method = iMrViewer->ResolveMeetingRequestMethodL(*aMsgPtr);
    if ( method == EESMRMeetingRequestMethodCancellation )
        {
        isCancel = ETrue;
        }

    return isCancel;
    }
// </cmail>

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CopyToClipboardL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::CopyToClipboardL( const TDesC &aBuf )
	{
    FUNC_LOG;
	CClipboard* cb = CClipboard::NewForWritingLC( CCoeEnv::Static()->FsSession() );

	cb->StreamDictionary().At( KClipboardUidTypePlainText );

	CPlainText* plainText = CPlainText::NewL();
	CleanupStack::PushL( plainText );

	plainText->InsertL( 0 , aBuf );

	plainText->CopyToStoreL( cb->Store(), cb->StreamDictionary(), 0, plainText->DocumentLength() );

	CleanupStack::PopAndDestroy( plainText );
	cb->CommitL();
	CleanupStack::PopAndDestroy( cb );
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ToggleEmailIconL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ToggleEmailIconL(TBool aIconOn )
	{
    FUNC_LOG;
    //Toggle email status indicator
	if(aIconOn)
		{
		RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsNewEmailStatus, ECoreAppUIsNewEmail );
		}
	else
		{
		RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsNewEmailStatus, ECoreAppUIsNoNewEmail );
		}
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::ToggleEmailIconL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::ToggleEmailIconL( TBool aIconOn, const TFSMailMsgId& aMailBox )
	{
    FUNC_LOG;
    //Toggle email status indicator
	//This is only used for mailwidget status updating
	/*if(aIconOn)
		{
		RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsNewEmailStatus, ECoreAppUIsNewEmail );
		}
	else
		{
		RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsNewEmailStatus, ECoreAppUIsNoNewEmail );
		}*/

    //Toggle new mail icon in widget
    if (aMailBox.Id())
        {
        CRepository* repository(NULL);
        TRAPD( err, repository = CRepository::NewL( KCRUidCmailWidget ) );
        if ( err == KErrNone )
            {
            TBuf<256> mailbox;
            mailbox.Num(aMailBox.Id());

            TBuf<256> str;
            str.Copy(_L("<"));    
            str.Append(mailbox);
            str.Append(_L(">"));    

            TBuf<256> stored;
            TUint32 key(KCMailMailboxesWithNewMail);
            repository->Get( key, stored );
    
            TInt result = stored.Find(str);
    
            if (aIconOn)
                {
                if (result < 0) // Not found
                    {
                    stored.Append(str);
                    repository->Set( key, stored );
                    }
                }
            else
                {
                if (result >= 0)
                    {
                    stored.Delete(result, str.Length());
                    repository->Set( key, stored );
                    }
                }
            }
        delete repository;
        }
	}
	
// -----------------------------------------------------------------------------
// TFsEmailUiUtility::DisplayMsgsMovedNoteL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::DisplayMsgsMovedNoteL( TInt aMsgCount, const TFSMailMsgId aDestinationFolderId,
										   	   TBool /*aIsWaitingNote*/ )
	{
    FUNC_LOG;
	if ( aDestinationFolderId.Id() != 0 )
		{
		CFreestyleEmailUiAppUi* appUi = (CFreestyleEmailUiAppUi*)CCoeEnv::Static()->AppUi();
		CFSMailFolder* folder = appUi->GetMailClient()->GetFolderByUidL( appUi->GetActiveMailboxId(), aDestinationFolderId );
		if ( folder )
			{
			CleanupStack::PushL( folder );
			HBufC* folderName(0);
			TInt folderType = folder->GetFolderType();
			switch ( folderType )
				{
				case EFSInbox:
					{
					folderName = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_INBOX );
					}
					break;
				case EFSOutbox:
					{
					folderName = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_OUTBOX );
					}
					break;
				case EFSDraftsFolder:
					{
					folderName = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_DRAFTS );
					}
					break;
				case EFSSentFolder:
					{
					folderName = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_SENT );
					}
					break;
				case EFSDeleted:
					{
					folderName = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FOLDER_LIST_DELETED );
					}
					break;
				default:
					{
					folderName = HBufC::NewLC( folder->GetFolderName().Length() );
					folderName->Des().Append( folder->GetFolderName() );
					}
					break;
				}
			if ( aMsgCount == 1)
				{
				HBufC* noteText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MESSAGE_TO_FOLDER, *folderName  );
				// <cmail> replace global note with aknnote to allow exiting with red key
			    //CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
			    //globalNote->ShowNoteL( EAknGlobalInformationNote, *noteText );
            	CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
            	note->ExecuteLD( *noteText );
            	CleanupStack::PopAndDestroy( noteText );
			    //CleanupStack::PopAndDestroy( 2 );
			    // </cmail>
				}
			else if ( aMsgCount > 1 )
				{
				HBufC* noteText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MESSAGES_TO_FOLDER, *folderName, aMsgCount );
			    // <cmail> replace global note with aknnote to allow exiting with red key
			    //CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
			    //globalNote->ShowNoteL( EAknGlobalInformationNote, *noteText );
            	CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
            	note->ExecuteLD( *noteText );
            	CleanupStack::PopAndDestroy( noteText );
			    // CleanupStack::PopAndDestroy( 2 );
			    // </cmail>
				}
			CleanupStack::PopAndDestroy( folderName );
			CleanupStack::PopAndDestroy( folder );
			}
		}
	}

// ---------------------------------------------------------------------------
// TFsEmailUiUtility::IsOfflineModeL
// ---------------------------------------------------------------------------
//
TBool TFsEmailUiUtility::IsOfflineModeL()
    {
    FUNC_LOG;
    TBool isOffline( EFalse );
    TInt status( ECoreAppUIsNetworkConnectionAllowed );

    CRepository* cenRep = CRepository::NewL( KCRUidCoreApplicationUIs );
    if ( cenRep )
        {
        cenRep->Get( KCoreAppUIsNetworkConnectionAllowed, status );
        }
    delete cenRep;

    if ( status == ECoreAppUIsNetworkConnectionNotAllowed )
	    {
	    isOffline = ETrue;
	    }

    return isOffline;
    }

// ---------------------------------------------------------------------------
// TFsEmailUiUtility::StripDisplayName
// Drop out unwanted characters from display name such as <> and ""
// ---------------------------------------------------------------------------
void TFsEmailUiUtility::StripDisplayName( HBufC& aDisplayName )
	{
    FUNC_LOG;
	// Drop out unwanted chars from display name
	if ( aDisplayName.Length() > 2)
		{
		// Drop out <> and "" characters.
		if ( aDisplayName.Locate('"') == 0 ) // check if  first char is quation mark
			{
			if ( aDisplayName.LocateReverse('"') == aDisplayName.Length()-1 ) // last char
				{
				// Remove first and last
				aDisplayName.Des().Delete(0,1);
				aDisplayName.Des().Delete(aDisplayName.Length()-1,1);
				}
			}
		else if ( aDisplayName.Locate('<') == 0 ) // Else check if first char is bracket
			{
			if ( aDisplayName.LocateReverse('>') == aDisplayName.Length()-1 ) // last char bracket
				{
				// Remove first and last
				aDisplayName.Des().Delete(0,1);
				aDisplayName.Des().Delete(aDisplayName.Length()-1,1);
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// TFsEmailUiUtility::DisplayCreateMailboxQueryL
// @return ETrue if user answered "Yes", EFalse if user answered "No"
// ---------------------------------------------------------------------------
//
TBool TFsEmailUiUtility::DisplayCreateMailboxQueryL()
    {
    FUNC_LOG;
    // load text from resource
    HBufC* question = StringLoader::LoadLC( R_FS_EMAIL_SETTINGS_ADD_MAILBOX_TXT );

    // show dialog
    TRequestStatus status = KRequestPending;
    CAknGlobalConfirmationQuery* query = CAknGlobalConfirmationQuery::NewL();
    CleanupStack::PushL( query );
    query->ShowConfirmationQueryL( status, *question, R_AVKON_SOFTKEYS_YES_NO );
    User::WaitForRequest( status );

    CleanupStack::PopAndDestroy( 2 ); // query question

    return ( status.Int() == EAknSoftkeyYes );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::BringFsEmailToForeground
// -----------------------------------------------------------------------------
//
void TFsEmailUiUtility::BringFsEmailToForeground()
    {
    FUNC_LOG;
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( KFSEmailUiUid );
    if ( task.Exists() )
        {
        task.BringToForeground();
        }
    }

// -----------------------------------------------------------------------------
// Creates a displayname of firstname and lastname components
// -----------------------------------------------------------------------------
//
HBufC* TFsEmailUiUtility::CreateDisplayNameLC( const TDesC& aFirstname,
    const TDesC& aLastname )
    {
    FUNC_LOG;
    return CreateDisplayNameLC( aFirstname, aLastname, KNullDesC );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CreateDisplayNameLC
// -----------------------------------------------------------------------------
//
HBufC* TFsEmailUiUtility::CreateDisplayNameLC( const TDesC& aFirstname,
    const TDesC& aLastname, const TDesC& aEmailField )
	{
    FUNC_LOG;
	HBufC* displayname = NULL;

	if ( aFirstname.Length() == 0 )
		{
		if ( aLastname.Length() == 0 )
			{
			//There's just the email address for Displayname
			displayname = aEmailField.AllocLC();
			}
		else
			{
			//Lastname = displayname
			displayname = aLastname.AllocLC();
			}
		}

	else if ( aLastname.Length() == 0 )
		{
		//Firstname = displayname
		displayname = aFirstname.AllocLC();
		}

	else
		{
		//Displayname = "firstname lastname"
		displayname = HBufC::NewLC( aFirstname.Length() +
									KSpace().Length() +
									aLastname.Length() );
		displayname->Des().Copy( aFirstname );
		displayname->Des().Append( KSpace );
		displayname->Des().Append( aLastname );
		}

	return displayname;
	}

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::GetFullIconFileNameL
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::GetFullIconFileNameL( TDes& aFileName )
    {
    FUNC_LOG;
    aFileName.Zero();
    aFileName.Copy( KDC_APP_BITMAP_DIR );
    aFileName.Append( KFsEmailIconFileName );
    User::LeaveIfError( CompleteWithAppPath( aFileName ) );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::DoFileExtensionsMatchL
// -----------------------------------------------------------------------------
TBool TFsEmailUiUtility::DoFileExtensionsMatchL( const TDesC& aFileName1,
                                                 const TDesC& aFileName2 )
    {
    FUNC_LOG;
    TParse parse1;
    TParse parse2;

    parse1.Set( aFileName1, NULL, NULL );
    parse2.Set( aFileName2, NULL, NULL );

    HBufC* ext1 = parse1.Ext().AllocLC();
    HBufC* ext2 = parse2.Ext().AllocLC();

    ext1->Des().LowerCase();
    ext2->Des().LowerCase();

    TBool match = (*ext1 == *ext2);

    CleanupStack::PopAndDestroy( ext2 );
    CleanupStack::PopAndDestroy( ext1 );

    return match;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::CopyFileToTempDirL
// Makes a copy of given file to the temp directory of FsEmailUI.
// Caller is reponsible of closing the returned file handle.
// -----------------------------------------------------------------------------
RFile TFsEmailUiUtility::CopyFileToTempDirL( const RFile& aSourceFile,
                                             const TDesC& aTargetFileName )
    {
    FUNC_LOG;
    RFs fs( CEikonEnv::Static()->FsSession() );

    // Remove any illegal characters from the target file name
    TFileName targetFileName = aTargetFileName.Right( KMaxFileName ); // truncate just in case; make sure that any file extension remains
    _LIT( KIllegalCharacters, "*?\\<>:\"/|\r\n\t\x2028\x2029" );
    const TText KReplacementChar = '_';
    AknTextUtils::ReplaceCharacters( targetFileName, KIllegalCharacters, KReplacementChar );
    targetFileName.Trim();
    // File name can't begin nor end with dot (.) and cannot be empty
    if ( !targetFileName.Length() )
        {
        targetFileName.Append( KReplacementChar );
        }
    else
        {
        if ( targetFileName[0] == '.' )
            {
            targetFileName[0] = KReplacementChar;
            }
        if ( targetFileName[ targetFileName.Length()-1 ] == '.' )
            {
            targetFileName[ targetFileName.Length()-1 ] = KReplacementChar;
            }
        }

    // If we still couldn't create a legal name, then use the original name as fallback solution
    if ( !fs.IsValidName( targetFileName ) )
        {
        aSourceFile.Name( targetFileName );
        }

    // Construct target file full name
    TFileName targetFilePath;
    GetTempPathL( fs, targetFilePath );
    targetFilePath.Append( targetFileName );

    CFileMan* fileMan = CFileMan::NewL( fs );
    TInt err = fileMan->Copy( aSourceFile, targetFilePath );
    delete fileMan;
    // KErrInUse ignoring check is needed because copying might not succeed
    // if file already exists and handle remains open.
    if ( err != KErrNone && err != KErrInUse )
        {
        User::LeaveIfError( err );
        }

    // Open the copied file
    RFile resultFile;
    err = resultFile.Open( fs, targetFilePath, EFileRead | EFileShareReadersOnly );
    User::LeaveIfError( err );

    return resultFile;
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::EraseTempDir
// Deletes all files from our temporary directory
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::EraseTempDir()
    {
    FUNC_LOG;
    RFs fs( CEikonEnv::Static()->FsSession() );

    TFileName deleteNamePattern;
    TRAPD( err, GetTempPathL( fs, deleteNamePattern ) );
    if ( !err )
        {
        deleteNamePattern.Append( '*' );
        BaflUtils::DeleteFile( fs, deleteNamePattern );
        }
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::GetTempPathL
// Get EmailUI temp directory path. Creates the directory if it does not exist.
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::GetTempPathL( RFs& aFs, TDes& aPath )
    {
    FUNC_LOG;
    _LIT( KDriveCPath, "C:" );

    TInt err = aFs.PrivatePath( aPath );
    User::LeaveIfError( err );

    aPath.Insert( 0, KDriveCPath );
    aPath.Append( KFsEmailTempDirName );
    BaflUtils::EnsurePathExistsL( aFs, aPath );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::EnsureFsMailServerIsRunning
// Checks if FSMailServer is running. If not, then launches it.
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::EnsureFsMailServerIsRunning( RWsSession& aWsSession )
    {
    FUNC_LOG;
    TRAP_IGNORE( EnsureFsMailServerIsRunningL( aWsSession ) );
    }

// -----------------------------------------------------------------------------
// TFsEmailUiUtility::EnsureFsMailServerIsRunningL
// Checks if FSMailServer is running. If not, then launches it.
// -----------------------------------------------------------------------------
void TFsEmailUiUtility::EnsureFsMailServerIsRunningL( RWsSession& aWsSession )
    {
    FUNC_LOG;

    TApaTaskList taskList( aWsSession );
    TApaTask task = taskList.FindApp( KFSMailServerUid );

    if ( !task.Exists() )
        {
        // FSMailServer is not running, try to launch it

        // Get this process' file name to get the correct drive letter
        RProcess process;
        TFileName processFileName = process.FileName();
        process.Close();

        // Parse the full path to FSMailServer exe
        TParse parse;
        User::LeaveIfError( parse.Set( KFSMailServerExe, &processFileName, NULL) );

        // Store the full path
        processFileName = parse.FullName();

        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL( processFileName );
        // Launch FSMailServer in background so that it doesn't steal
        // the focus during it's construction
        cmdLine->SetCommandL( EApaCommandBackground );

        RApaLsSession ls;
        User::LeaveIfError( ls.Connect() );
        CleanupClosePushL( ls );

        TThreadId threadId;
        // Is there anything we can do in case of error?
        //       Try to launch from another drive etc.?
        TInt error = ls.StartApp( *cmdLine, threadId );

        CleanupStack::PopAndDestroy( &ls );
        CleanupStack::PopAndDestroy( cmdLine );
        }
    }



/////////////////////////////////////////////////////////////////////////////////
// ACTION MENU HANDLER
//

//Singleton* Singleton::pinstance = 0;
// Initialize static member variables
CFscContactActionMenu* CFSEmailUiActionMenu::iActionMenu = NULL;
TFileName CFSEmailUiActionMenu::iIconFilePath;
RPointerArray<CGulIcon> CFSEmailUiActionMenu::iIconArray;

CFSEmailUiActionMenu* CFSEmailUiActionMenu::NewL( CFreestyleEmailUiAppUi* aAppUi )
	{
    FUNC_LOG;
    CFSEmailUiActionMenu* self = new (ELeave) CFSEmailUiActionMenu( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

CFSEmailUiActionMenu::CFSEmailUiActionMenu( CFreestyleEmailUiAppUi* aAppUi )
	: iAppUi( aAppUi )
	{

	}

CFSEmailUiActionMenu::~CFSEmailUiActionMenu()
	{
    FUNC_LOG;
	if( iActionMenu )
		{
		delete iActionMenu;
		}
	if ( iService )
		{
		delete iService;
		}
	iIconArray.ResetAndDestroy();
	}

void CFSEmailUiActionMenu::ConstructL()
	{
    FUNC_LOG;
    if( !iActionMenu )
        {
        iService = CFscContactActionService::NewL( iAppUi->GetVPbkManagerL() );
        iActionMenu = CFscContactActionMenu::NewL( *iService );
        }
    TFsEmailUiUtility::GetFullIconFileNameL( iIconFilePath );
	}

CFscContactActionMenu* CFSEmailUiActionMenu::GetActionMenu()
	{
	return iActionMenu;
	}

void CFSEmailUiActionMenu::RemoveAllL()
	{
    FUNC_LOG;
    if ( iIconArray.Count() == 0 )
        {
        CreateIconsL();
        }
	iActionMenu->Model().RemoveAll( ETrue );
	}

void CFSEmailUiActionMenu::AddCustomItemL( TActionMenuCustomItemId aItemId )
	{
    FUNC_LOG;
    if ( iIconArray.Count() == 0 )
        {
        CreateIconsL();
        }
	HBufC* itemText = NULL;
	CGulIcon* itemIcon = iIconArray[aItemId-1];
	switch( aItemId )
		{
		case FsEActionMenuOpen:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_OPEN );
			}
			break;
		case FsEActionMenuOpenCalendarEvent:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_OPEN );
			}
			break;
		case FsEActionMenuReply:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_REPLY );
			}
			break;

		case FsEActionMenuReplyAll:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_REPLY_ALL );
			}
			break;

		case FsEActionMenuForward:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_FORWARD );
			}
			break;

		case FsEActionMenuDelete:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_DELETE );
			}
			break;

		case FsEActionMenuAccept:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ACCEPT );
			}
			break;

		case FsEActionMenuTentative:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_TENTATIVE );
			}
			break;

		case FsEActionMenuDecline:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_DECLINE );
			}
			break;

		case FsEActionMenuRemoveFormCal:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_REM_FROM_CAL );
			}
			break;

		case FsEActionMenuMarkRead:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_MARK_READ );
			}
			break;

		case FsEActionMenuMarkUnread:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_MARK_UNREAD );
			}
			break;

		case FsEActionMenuCall:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_CALL );
			}
			break;
        // <cmail> video call
        case FsEActionMenuCallVideo:
            {
            itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_CALL_VIDEO );
            }
            break;
        // </cmail>
		case FsEActionMenuCreateMessage:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_CREATE_MESSAGE );
			}
			break;
		case FsEActionMenuCreateEmail:
		    {
            itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_CREATE_EMAIL );
		    }
			break;
		case FsEActionMenuContactDetails:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_CONTACT_DETAILS );
			}
			break;

		case FsEActionMenuAddToContacts:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ADD_TO_CONTACTS );
			}
			break;

		case FsEActionMenuRemoteLookup:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_REMOTE_LOOKUP );
			}
			break;

		case FsEActionMenuMove:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_MOVE );
			}
			break;

		case FsEActionMenuMoveToDrafts:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_MOVE_TO_DRAFTS );
			}
			break;
		case FsEActionMenuOpenInWeb:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_OPEN_IN_WEB );
			}
			break;
		case FsEActionMenuOpenInIntranet:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_OPEN_IN_INTRA );
			}
			break;
		case FsEActionMenuBookmark:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_BOOKMARK );
			}
			break;
		case FsEActionAttachmentOpen:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_OPEN );
			}
			break;
		case FsEActionAttachmentDownload:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_DOWNLOAD );
			}
			break;
		case FsEActionAttachmentCancelDownload:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_CANCEL );
			}
			break;
		case FsEActionAttachmentSave:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_SAVE );
			}
			break;
        case FsEActionAttachmentAdd:
            {
            itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_ADD );
            }
            break;
        case FsEActionAttachmentRemove:
            {
            itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_REMOVE );
            }
            break;
        case FsEActionAttachmentViewAll:
            {
            itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_VIEW_ALL );
            }
            break;
		case FsEActionAttachmentDownloadAll:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_DOWNLOAD_ALL );
			}
			break;

		case FsEActionAttachmentCancelAllDownloads:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_CANCEL_ALL );
			}
			break;
		case FsEActionAttachmentSaveAll:
			{
			itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_SAVE_ALL );
			}
			break;
		case FsEActionAttachmentRemoveAll:
            {
            itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_REMOVE_ALL );
            }
            break;
		case FsEActionAttachmentClearFetchedContent:
		    {
		    itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_CLEAR_FETCHED );
		    }
		    break;
		case FsEActionAttachmentRemoveUnfetchedAttachment:
		    {
		    itemText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_AM_ATTACH_REMOVE_UNFETCHED );
		    }
		    break;
		default:
		    {
		    itemText = KNullDesC().AllocLC();
		    }
			break;
		}

	TUid itemUid;
	itemUid.iUid = TInt(aItemId);

    CleanupStack::PushL( itemIcon );
	CFscContactActionMenuItem* menuItem = iActionMenu->Model().NewMenuItemL( *itemText, itemIcon, KActionMenuItemNormalPriority, EFalse, itemUid);
    CleanupStack::PushL( menuItem );
    iActionMenu->Model().AddItemL( menuItem );
    CleanupStack::Pop( menuItem );
    CleanupStack::Pop( itemIcon );
    CleanupStack::PopAndDestroy( itemText );
	}

void CFSEmailUiActionMenu::AddCustomItemsL( RFsEActionMenuIdList aItemList )
	{
    FUNC_LOG;
	for( TInt i = 0; i < aItemList.Count(); i++ )
		{
		AddCustomItemL( aItemList[i] );
		}
	}

// <cmail>
void CFSEmailUiActionMenu::Dismiss( TBool aSlide )
    {
    FUNC_LOG;
    if( iActionMenu )
        {
        iActionMenu->Dismiss( aSlide );
        }
    }
// </cmail>

TActionMenuCustomItemId CFSEmailUiActionMenu::ExecuteL(
	TFscContactActionMenuPosition aPosition,
	TInt aIndex,
	MFsActionMenuPositionGiver* aPositionGiver)
	{
    FUNC_LOG;
    if ( iIconArray.Count() == 0 )
        {
        CreateIconsL();
        }
	TFscActionMenuResult menuResult = iActionMenu->ExecuteL( aPosition, aIndex, aPositionGiver );
    if ( menuResult == EFscCustomItemSelected )
    	{
    	return ResolveSelectedCustomItemIdL();
    	}
    else if( menuResult == EFscCasItemSelectedAndExecuted )
    	{
    	return FsEActionMenuCasItemSelectedAndExecuted;
    	}
    else
    	{
    	return FsEActionMenuDismissed;
    	}
	}

TActionMenuCustomItemId CFSEmailUiActionMenu::ExecuteL(
	RFsEActionMenuIdList aItemList,
	TFscContactActionMenuPosition aPosition,
	TInt aIndex,
	MFsActionMenuPositionGiver* aPositionGiver)
	{
    FUNC_LOG;
    if ( iIconArray.Count() == 0 )
        {
        CreateIconsL();
        }
	RemoveAllL();
	AddCustomItemsL( aItemList );
	return ExecuteL( aPosition, aIndex, aPositionGiver );
	}
// </cmail>


TActionMenuCustomItemId CFSEmailUiActionMenu::ResolveSelectedCustomItemIdL()
	{
    FUNC_LOG;
    if ( iIconArray.Count() == 0 )
        {
        CreateIconsL();
        }
	TInt itemIndex = iActionMenu->FocusedItemIndex();
	TUid itemUid = iActionMenu->Model().ItemL( itemIndex ).ImplementationUid();

	return TActionMenuCustomItemId( itemUid.iUid );
	}

void CFSEmailUiActionMenu::CreateIconsL()
	{
    FUNC_LOG;
	iIconArray.ResetAndDestroy();

	// UPDATE ICONS!
// <cmail> icons changed
	// NOTE: Must be appended same order as are in TActionMenuCustomItemIds!
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_indi_cmail_read, EMbmFreestyleemailuiQgn_indi_cmail_read_mask ) ); // FsEActionMenuOpen
  	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read, EMbmFreestyleemailuiQgn_indi_cmail_calendar_event_read_mask ) ); // FsEActionMenuOpen
  	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_reply, EMbmFreestyleemailuiQgn_prop_cmail_action_reply_mask ) ); // FsEActionMenuReply
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_reply_all, EMbmFreestyleemailuiQgn_prop_cmail_action_reply_all_mask ) ); // FsEActionMenuReplyAll
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_forward, EMbmFreestyleemailuiQgn_prop_cmail_action_forward_mask ) ); // FsEActionMenuForward
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_delete, EMbmFreestyleemailuiQgn_prop_cmail_action_delete_mask ) ); // FsEActionMenuDelete
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_indi_cmail_calendar_tracking_accept, EMbmFreestyleemailuiQgn_indi_cmail_calendar_tracking_accept_mask  ) ); // FsEActionMenuAccept
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_indi_cmail_calendar_tracking_tentative, EMbmFreestyleemailuiQgn_indi_cmail_calendar_tracking_tentative_mask  ) ); // FsEActionMenuTentative
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_indi_cmail_calendar_tracking_reject, EMbmFreestyleemailuiQgn_indi_cmail_calendar_tracking_reject_mask ) ); // FsEActionMenuDecline
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_indi_cmail_calendar_delete, EMbmFreestyleemailuiQgn_indi_cmail_calendar_delete_mask ) ); // FsEActionMenuDecline
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_mark_read, EMbmFreestyleemailuiQgn_prop_cmail_action_mark_read_mask ) ); // FsEActionMenuMarkRead
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_mark_unread, EMbmFreestyleemailuiQgn_prop_cmail_action_mark_unread_mask ) ); // FsEActionMenuMarkUnread
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_call, EMbmFreestyleemailuiQgn_prop_cmail_action_call_mask ) ); // FsEActionMenuCall
    // <cmail> video call
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_call_video, EMbmFreestyleemailuiQgn_prop_cmail_action_call_video_mask ) ); // FsEActionMenuCallVideo
    // </cmail>
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_create_msg, EMbmFreestyleemailuiQgn_prop_cmail_action_create_msg_mask ) ); // FsEActionMenuCreateMessage
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_email,  EMbmFreestyleemailuiQgn_prop_cmail_action_email_mask) ); // FsEActionMenuCreateEmail
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_contact_detail, EMbmFreestyleemailuiQgn_prop_cmail_action_contact_detail_mask ) ); // FsEActionMenuContactDetails
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_add_to_contacts, EMbmFreestyleemailuiQgn_prop_cmail_action_add_to_contacts_mask ) ); // FsEActionMenuAddToContacts
 	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_remote_lookup, EMbmFreestyleemailuiQgn_prop_cmail_action_remote_lookup_mask ) ); // FsEActionMenuRemoteLookup
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_move_msg, EMbmFreestyleemailuiQgn_prop_cmail_action_move_msg_mask ) ); // FsEActionMenuMove
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_move_to_drafts, EMbmFreestyleemailuiQgn_prop_cmail_action_move_to_drafts_mask  ) ); // FsEActionMenuMoveToDrafts
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_menu_browserng, EMbmFreestyleemailuiQgn_menu_browserng_mask ) ); // FsEActionMenuOpenInWeb
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_menu_intranet, EMbmFreestyleemailuiQgn_menu_intranet_mask ) ); // FsEActionMenuOpenInIntranet
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_wml_bookmark, EMbmFreestyleemailuiQgn_prop_cmail_wml_bookmark_mask ) ); // FsEActionMenuBookmark
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_open, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_open_mask ) ); // FsEActionAttachmentOpen
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_download, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_download_mask ) ); // FsEActionAttachmentDownload
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_cancel_download, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_cancel_download_mask ) ); // FsEActionAttachmentCancelDownload
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_save, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_save_mask ) ); // FsEActionAttachmentSave
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_add, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_add_mask ) ); // FsEActionAttachmentAdd
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete_mask ) ); // FsEActionAttachmentRemove
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_view_all, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_view_all_mask ) ); // FsEActionAttachmentViewAll
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_download_all, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_download_all_mask ) ); // FsEActionAttachmentDownloadAll
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_cancel_all, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_cancel_all_mask ) ); // FsEActionAttachmentCancelAllDownloads
	iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_save_all, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_save_all_mask ) ); // FsEActionAttachmentSaveAll
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete_all, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete_all_mask ) ); // FsEActionAttachmentRemoveAll
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete_mask ) ); // FsEActionAttachmentClearFetchedContent
    iIconArray.AppendL( CreateIconL( EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete, EMbmFreestyleemailuiQgn_prop_cmail_action_attachment_list_options_delete_mask ) ); // FsEActionAttachmentRemoveUnfetchedAttachment
// </cmail>
	}

CGulIcon* CFSEmailUiActionMenu::CreateIconL( TInt aBitmapId, TInt aMaskId )
	{
    FUNC_LOG;
	// Define default icon object
    CFbsBitmap* iconBitmap;
    CFbsBitmap* iconMaskBitmap;
    TSize defaultIconSize(30,30);

	// Create "CALL" Item
    AknIconUtils::CreateIconLC(iconBitmap, iconMaskBitmap, iIconFilePath, aBitmapId, aMaskId );
    AknIconUtils::SetSize(iconBitmap, defaultIconSize);
    AknIconUtils::SetSize(iconMaskBitmap, defaultIconSize);
    CGulIcon* icon = CGulIcon::NewL(iconBitmap, iconMaskBitmap);
    CleanupStack::Pop(2);
    return icon;
	}

/////////////////////////////////////////////////////////////////////////////////
// GENERIC TIMER
//

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
CFSEmailUiGenericTimer* CFSEmailUiGenericTimer::NewL(
	MFSEmailUiGenericTimerCallback* aCallback,
	const TInt aPriority )
    {
    FUNC_LOG;
    CFSEmailUiGenericTimer* self = NewLC( aCallback, aPriority );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
CFSEmailUiGenericTimer* CFSEmailUiGenericTimer::NewLC(
	MFSEmailUiGenericTimerCallback* aCallback,
	const TInt aPriority )
    {
    FUNC_LOG;
    CFSEmailUiGenericTimer* self = new (ELeave) CFSEmailUiGenericTimer( aCallback, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::NewL
// NewL function. Returns timer object.
// -----------------------------------------------------------------------------
//
void CFSEmailUiGenericTimer::ConstructL()
    {
    FUNC_LOG;
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::~CFSEmailUiGenericTimer
// D'tor
// -----------------------------------------------------------------------------
//
CFSEmailUiGenericTimer::~CFSEmailUiGenericTimer()
    {
    FUNC_LOG;
    Cancel();
    iCallback = NULL;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::CFSEmailUiGenericTimer
// C'tor
// -----------------------------------------------------------------------------
//
CFSEmailUiGenericTimer::CFSEmailUiGenericTimer(
	MFSEmailUiGenericTimerCallback* aCallback,
	const TInt aPriority )
	: CTimer( aPriority ),
	iCallback( aCallback )
    {
    }

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::RunL
// Timer trigger function.
// -----------------------------------------------------------------------------
//
void CFSEmailUiGenericTimer::RunL()
    {
    FUNC_LOG;
    if ( iCallback )
    	{
    	iCallback->TimerEventL( this );
    	}
    }

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::Start
// Timer starting function.
// -----------------------------------------------------------------------------
//
void CFSEmailUiGenericTimer::Start( TInt aInterval )
    {
    FUNC_LOG;
    Cancel();
    if( aInterval < 0 )
    	{
    	aInterval = KGenericTimerDefaultInterval;
    	}
	After( TTimeIntervalMicroSeconds32( aInterval * 1000 ) );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiGenericTimer::Stop
// Timer stopping function
// -----------------------------------------------------------------------------
//
void CFSEmailUiGenericTimer::Stop()
    {
    FUNC_LOG;
    Cancel();
    }

TKeyResponse CFSEmailUiCallDialog::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
    {
    if (aKeyEvent.iScanCode == EStdKeyYes )
        {
        TKeyEvent newEvent;
        newEvent.iScanCode = EStdKeyEnter;
        newEvent.iCode = EKeyEnter;
        return CAknQueryDialog::OfferKeyEventL(newEvent, aType);
        }
    else
        {
        return CAknQueryDialog::OfferKeyEventL(aKeyEvent, aType);
        }
    }


