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
* Description:  Mailbox Deleter
*
*/


// SYSTEM INCLUDE FILES
#include <StringLoader.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <avkon.mbg>
#include <FreestyleEmailUi.rsg>
#include <freestyleemailui.mbg>
#include <aknnotewrappers.h>
#include <AknIconUtils.h>  

// INTERNAL INCLUDE FILES
#include "emailtrace.h"
#include "freestyleemailuimailboxdeleter.h"
#include "FreestyleEmailUiUtilities.h"
#include "cfsmailbox.h"
#include "FreestyleEmailUiAppui.h"
#include "cfsmailclient.h"

// CONSTANT VALUES
_LIT( KTabCharacter, "\t" ); 


CFSEmailUiMailboxDeleter* CFSEmailUiMailboxDeleter::NewL( CFSMailClient& aMailClient, 
                                                          MFSEmailUiMailboxDeleteObserver& aObserver  )
    {
    FUNC_LOG;
    CFSEmailUiMailboxDeleter* self = CFSEmailUiMailboxDeleter::NewLC( aMailClient, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

CFSEmailUiMailboxDeleter* CFSEmailUiMailboxDeleter::NewLC( CFSMailClient& aMailClient, 
                                                           MFSEmailUiMailboxDeleteObserver& aObserver )
    {
    FUNC_LOG;
    CFSEmailUiMailboxDeleter* self = new (ELeave) CFSEmailUiMailboxDeleter( aMailClient, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CFSEmailUiMailboxDeleter::CFSEmailUiMailboxDeleter( CFSMailClient& aMailClient, 
                                                    MFSEmailUiMailboxDeleteObserver& aObserver )
    : iMailClient( aMailClient ), iObserver( aObserver )
    {
    FUNC_LOG;
    }

void CFSEmailUiMailboxDeleter::ConstructL()
    {
    FUNC_LOG;
    iIdle = CIdle::NewL (CIdle::EPriorityIdle);
    }

CFSEmailUiMailboxDeleter::~CFSEmailUiMailboxDeleter()
    {
    FUNC_LOG;
    delete iWaitDialog;
    iMailboxesToDelete.Close();
    if (iIdle)
    	{
    	iIdle->Cancel();
    	delete iIdle;
    	}
    }


// ---------------------------------------------------------------------------
// Deletes the given mailbox or displays a list of available mailboxes and
// allows the user to mark multiple mailboxes for deletion if aId is NullId.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailboxDeleter::DeleteMailboxL()
    {
    iMailboxesToDelete.Reset();

    // Get the deletable mailboxes.
    RPointerArray<CFSMailBox> mailboxes;
    const TInt mailboxCount = GetDeletableMailboxesLC( mailboxes );
    
    TBool res( EFalse );    
    
    if( mailboxCount == 1 )
        {
        res = DeleteSingleMailboxL( mailboxes );
        }
    else if( mailboxCount > 1 )
        {
        res = DeleteMultipleMailboxesL( mailboxes );
        }

    CleanupStack::PopAndDestroy(); // mailboxes
    
    if ( res )
        {
        DoDeleteSelectedMailboxesL();
        }
    }

// ---------------------------------------------------------------------------
// DeleteMailboxL()
// Deletes the given mailbox. Displays also a confirmation dialog.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailboxDeleter::DeleteMailboxL( const TFSMailMsgId& aMailboxId )
	{
    iMailboxesToDelete.Reset();

    // Get the deletable mailboxes.
    RPointerArray<CFSMailBox> mailboxes;
    const TInt mailboxCount = GetDeletableMailboxesLC( mailboxes );

    // Make sure that it is allowed to delete the mailbox with the given ID.
    CFSMailBox* mailbox = NULL;

    for ( TInt i( 0 ); i < mailboxCount; ++i )
    	{
    	mailbox = mailboxes[i];

    	if ( mailbox && mailbox->GetId() == aMailboxId )
    		{
    		// The mailbox can be deleted.
    		break;
    		}

    	mailbox = NULL;
    	}

    if ( !mailbox )
    	{
    	// Either no mailbox with the given ID exist or it is not allowed to
    	// be deleted.
    	// Display an error message?
    	CleanupStack::PopAndDestroy(); // mailboxes
    	return;
    	}

    TBool response = ConfirmMailboxDeletionL( 1, mailbox->GetName() );
    CleanupStack::PopAndDestroy(); // mailboxes

    if ( !response )
    	{
    	// User did not confirm the deletion.
    	return;
    	}
    
    iMailboxesToDelete.AppendL( aMailboxId );
    DoDeleteSelectedMailboxesL();
	}


// ---------------------------------------------------------------------------
// GetDeletableMailboxesLC()
// Returns the mailboxes that can be deleted.
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailboxDeleter::GetDeletableMailboxesLC(
	RPointerArray<CFSMailBox>& aMailboxes )
	{
    // Make sure that FSMailServer is running so that the mailboxes will also
    // be removed from MCE.
    TFsEmailUiUtility::EnsureFsMailServerIsRunning(
    	CCoeEnv::Static()->WsSession() );

    CleanupResetAndDestroyClosePushL( aMailboxes );

    // Get the mailboxes.
    TFSMailMsgId id; 
    User::LeaveIfError( iMailClient.ListMailBoxes( id, aMailboxes ) );

    CFSMailBox* mailbox = NULL;
    
    // Filter out mailboxes that can't be deleted.
    const TInt mailboxCount( aMailboxes.Count() );

    for( TInt i( 0 ); i < mailboxCount; )
        {
        mailbox = aMailboxes[i];

        if( !mailbox->HasCapability( EFSMBoxCapaCanBeDeleted ) )
            {
            delete mailbox;
            aMailboxes.Remove( i );
            }
        else
            {
            ++i;
            }
        }

    return aMailboxes.Count();
	}


// ---------------------------------------------------------------------------
// DeleteSingleMailboxL
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailboxDeleter::DeleteSingleMailboxL( 
                                const RPointerArray<CFSMailBox>& aMailboxes )
    {
    CFSMailBox* mailBox = aMailboxes[0];

    TBool res( ConfirmMailboxDeletionL( 1, mailBox->GetName() ) );
    
    if( res )
        {
        iMailboxesToDelete.AppendL( mailBox->GetId() );
        }
    
    return res;
    }

// ---------------------------------------------------------------------------
// DeleteMultipleMailboxesL
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailboxDeleter::DeleteMultipleMailboxesL( 
                                const RPointerArray<CFSMailBox>& aMailboxes )
    {
    // Create array for user selections 
    CArrayFixFlat<TInt>* selections = new(ELeave) CArrayFixFlat<TInt>( aMailboxes.Count() );
    CleanupStack::PushL( selections );    

    // Setup mailbox selection dialog
    CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog( selections );
    dlg->PrepareLC( R_DELETE_MAILBOXES_MULTI_SELECTION_QUERY );
     
    // Create icon array
    CArrayPtr<CGulIcon>* mailboxListIconArray = new(ELeave) CArrayPtrFlat<CGulIcon>(1);
    CleanupStack::PushL( mailboxListIconArray );

    // Create listbox model
    CDesCArrayFlat* mailboxListModel = new(ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL( mailboxListModel );

    CreateListboxModelAndIconArrayL( *mailboxListModel, *mailboxListIconArray, aMailboxes );
     
    dlg->SetOwnershipType( ELbmOwnsItemArray );        
    dlg->SetItemTextArray( mailboxListModel ); // Takes ownership
    CleanupStack::Pop( mailboxListModel );

    dlg->SetIconArrayL( mailboxListIconArray ); // Takes ownership
    CleanupStack::Pop( mailboxListIconArray ); 

    // Display mailbox selection dialog.
    TBool res( dlg->RunLD() );

    if( res && ( selections->Count() > 0 ) )
        {
        res = ConfirmMailboxDeletionL( selections->Count(), 
                                       aMailboxes[selections->At(0)]->GetName() );
         
        if( res )
            {
            for( TInt i( 0 ); i < selections->Count(); ++i )
                {
                iMailboxesToDelete.AppendL( aMailboxes[selections->At(i)]->GetId() );
                }
            }
        }
        
    CleanupStack::PopAndDestroy( selections );

    return res;
    }


// ---------------------------------------------------------------------------
// DoDeleteSelectedMailboxesL()
// Deletes the selected mailboxes i.e. those of which IDs are in the
// iMailboxesToDelete array.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailboxDeleter::DoDeleteSelectedMailboxesL()
	{
    if ( iMailboxesToDelete.Count() < 1 )
    	{
    	// Nothing to delete!
    	return;
    	}
    
    // Start wait note.
    iWaitDialog = new ( ELeave ) CAknWaitDialog(
        ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) ), ETrue );
    iWaitDialog->PrepareLC( R_FS_WAIT_NOTE_REMOVING_MAILBOX );
    iWaitDialog->SetCallback( this );
    iWaitDialog->RunLD();

    // Set email indicator off.
    TFsEmailUiUtility::ToggleEmailIconL( EFalse );
        
    // Delete first mailbox in queue.
	DoDeleteNextMailboxL();
	}


// ---------------------------------------------------------------------------
// CreateMarkIconLC
// ---------------------------------------------------------------------------
//
CGulIcon* CFSEmailUiMailboxDeleter::CreateMarkIconLC()
    {
    TAknsItemID skinId;
    skinId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnIndiMarkedAdd );
    
    TRgb defaultColour( KRgbBlack );
    CFbsBitmap* bmap = NULL;
    CFbsBitmap* mask = NULL;

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor( skin, defaultColour,
               KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13 );
    AknsUtils::CreateColorIconLC( skin, 
                                  skinId,
                                  KAknsIIDQsnIconColors, 
                                  EAknsCIQsnIconColorsCG13, 
                                  bmap, 
                                  mask,
                                  AknIconUtils::AvkonIconFileName(), 
                                  EMbmAvkonQgn_indi_marked_add, 
                                  EMbmAvkonQgn_indi_marked_add_mask,
                                  defaultColour );
    
    CGulIcon* icon = CGulIcon::NewL( bmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2 ); // Icon owns the bitmaps now   
    CleanupStack::PushL( icon );
    return icon;
    }


// ---------------------------------------------------------------------------
// CreateDefaultMailboxIconLC
// ---------------------------------------------------------------------------
//
CGulIcon* CFSEmailUiMailboxDeleter::CreateDefaultMailboxIconLC()
    {
    TFileName iconFileName;
    TFsEmailUiUtility::GetFullIconFileNameL( iconFileName );
   
    CFbsBitmap* bmap = NULL;
    CFbsBitmap* mask = NULL;
    AknIconUtils::CreateIconLC( bmap, mask, iconFileName,
                         EMbmFreestyleemailuiQgn_indi_cmail_drop_inbox,
                         EMbmFreestyleemailuiQgn_indi_cmail_drop_inbox_mask);
    CGulIcon* defaultIcon = CGulIcon::NewL( bmap, mask );
    defaultIcon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2 ); // Icon owns the bitmaps now   
    CleanupStack::PushL( defaultIcon );    

    return defaultIcon;
    }

// ---------------------------------------------------------------------------
// ConfirmMailboxDeletionL
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailboxDeleter::ConfirmMailboxDeletionL( TInt aCount, 
                                                         const TDesC& aMailboxName )
    {
    CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog();
    dlg->PrepareLC( R_FSEMAIL_QUERY_DIALOG );

    HBufC* queryText = NULL;    
    if( aCount == 1 )
        {
        queryText = StringLoader::LoadLC( R_DELETE_MAILBOX_QUERY, aMailboxName );
        }
    else if( aCount > 0 )
        {
        queryText = StringLoader::LoadLC( R_DELETE_MULTIPLE_MAILBOXES_QUERY, 
                                          aCount );
        }

    dlg->SetPromptL( *queryText );
    CleanupStack::PopAndDestroy( queryText );

    return dlg->RunLD();
    }

// ---------------------------------------------------------------------------
// CreateListboxModelAndIconArrayL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailboxDeleter::CreateListboxModelAndIconArrayL( 
                                    CDesCArrayFlat& aModel,
                                    CArrayPtr<CGulIcon>& aIconArray, 
                                    const RPointerArray<CFSMailBox>& aMailboxes )
    {
    // Create and add listbox mark icon
    CGulIcon* markIcon = CreateMarkIconLC();
    aIconArray.AppendL( markIcon );
    CleanupStack::Pop( markIcon );

    // Create and add default (non-branded) mailbox icon
    CGulIcon* defaultIcon = CreateDefaultMailboxIconLC();
    aIconArray.AppendL( defaultIcon );
    CleanupStack::Pop( defaultIcon );
    
    // Add branded mailbox icons
    CFSMailBox* mailBox = NULL;     
    for( TInt i(0); i < aMailboxes.Count(); ++i )
        {
        mailBox = aMailboxes[i];
         
        CGulIcon* mbIcon = NULL;
         
        TRAPD( err, mbIcon = iMailClient.GetBrandManagerL().GetGraphicL( EFSMailboxIcon, 
                                                                         mailBox->GetId() ) ); 
        if ( err == KErrNone && mbIcon )
            {
            CleanupStack::PushL( mbIcon );
            aIconArray.AppendL( mbIcon );
            CleanupStack::Pop( mbIcon );            
            }
        
        TPtrC name( mailBox->GetName() );
        HBufC* buf = HBufC::NewLC( name.Length() + 
                                   KTabCharacter().Length() * 2 + 4 ); // +4 for icon id
        TPtr bufPtr( buf->Des() );

        TInt index( mbIcon ? aIconArray.Count() - 1 : 1 ); // Select branded or default icon
        TBuf<4> indexBuf;
        indexBuf.Num( index );
        bufPtr.Append( indexBuf );

        bufPtr.Append( KTabCharacter );
        bufPtr.Append( name );
        bufPtr.Append( KTabCharacter );
        aModel.AppendL( bufPtr );
        CleanupStack::PopAndDestroy( buf );
        }            
    }

// ---------------------------------------------------------------------------
// RequestResponseL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailboxDeleter::RequestResponseL( TFSProgress aEvent, 
                                                 TInt aRequestId )
    {
    switch( aEvent.iProgressStatus )
        {
        case TFSProgress::EFSStatus_RequestComplete:
        case TFSProgress::EFSStatus_RequestCancelled:
            {
            if( iMailboxesToDelete.Count() > 0 )
            	{
                // Delete next mailbox in queue.
                iIdle->Cancel();
                iIdle->Start(TCallBack(IdleCallbackL,this));
            	}
            else
            	{
                if( iWaitDialog )
                    {
                    iWaitDialog->ProcessFinishedL();
                    }
                // Notify observer that the deletion is complete. 
                iObserver.MailboxDeletionComplete();
            	}          
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// DialogDismissedL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailboxDeleter::DialogDismissedL( TInt /*aButtonId */ )
    {
    iWaitDialog = NULL;
    }
    
// ---------------------------------------------------------------------------
// DoDeleteNextMailboxL
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailboxDeleter::DoDeleteNextMailboxL()
    {
    TFSMailMsgId nextToDelete = iMailboxesToDelete[0]; 
    iMailboxesToDelete.Remove( 0 );
    iMailboxDeleteOperationId = iMailClient.DeleteMailBoxByUidL( nextToDelete, 
                                                                 *this );
    }
// ---------------------------------------------------------------------------
// IdleCallback
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiMailboxDeleter::IdleCallbackL(TAny* aPtr)
	{
	TRAPD( leaveErr,
		   static_cast<CFSEmailUiMailboxDeleter*>
		   (aPtr)->DoDeleteNextMailboxL(); );
	return leaveErr;
	}

// End of file
