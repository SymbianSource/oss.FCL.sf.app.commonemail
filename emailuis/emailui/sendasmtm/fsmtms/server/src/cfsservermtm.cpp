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
* Description:  Implementation of the class CFsServerMtm.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include <StringLoader.h>
#include <centralrepository.h>
#include "freestyleemailcenrepkeys.h"
#include "cfsservermtm.h"
#include <msventry.h>
#include <mentact.h>
#include "cfsmsgconverter.h"

// ---------------------------------------------------------------------------
// CFsServerMtm::NewL
// Exported factory function
// ---------------------------------------------------------------------------
//
EXPORT_C CFsServerMtm* CFsServerMtm::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aInitialEntry)
    {
    FUNC_LOG;
    
    CleanupStack::PushL( aInitialEntry );
    CFsServerMtm* self=new (ELeave) CFsServerMtm( aRegisteredMtmDll, 
        aInitialEntry);
    CleanupStack::Pop();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::CFsServerMtm
// ---------------------------------------------------------------------------
//
CFsServerMtm::CFsServerMtm( CRegisteredMtmDll& aRegisteredMtmDll, 
    CMsvServerEntry* aInitialEntry )
    : CBaseServerMtm( aRegisteredMtmDll, aInitialEntry ) 
    {
    FUNC_LOG;

    iProcessingNestedEmail = EFalse;
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::ConstructL
// ---------------------------------------------------------------------------
//        
void CFsServerMtm::ConstructL()
    {
    FUNC_LOG;
    
    CActiveScheduler::Add( this );

    // Check configuration status of "email attached in email" from cenrep
    // and set flag according to that.
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KFreestyleEmailCenRep ) );
    if ( ret == KErrNone )
    	{
        CleanupStack::PushL( repository );
    	TInt rval = KErrNone;
        TInt hidefeature = 0;
        rval = repository->Get( KEmailHideFS205UIFeature, hidefeature );
        if( rval == KErrNone && hidefeature == EFalse )
        	{
        	iEmailAttachedInEmail = ETrue;
        	}
        CleanupStack::PopAndDestroy( repository );
    	}
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::CFsServerMtm
// ---------------------------------------------------------------------------
//    
CFsServerMtm::~CFsServerMtm()
    {
    FUNC_LOG;
    
    Cancel();
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::CopyToLocalL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::CopyToLocalL( const CMsvEntrySelection& /*aSelection*/, 
    TMsvId /*aDestination*/, TRequestStatus& /*aStatus*/ )
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::CopyFromLocalL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::CopyFromLocalL(const CMsvEntrySelection& /*aSelection*/, 
    TMsvId /*aDestination*/, TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::CopyWithinServiceL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::CopyWithinServiceL(
    const CMsvEntrySelection& /*aSelection*/, TMsvId /*aDestination*/, 
    TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::MoveToLocalL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::MoveToLocalL(const CMsvEntrySelection& /*aSelection*/, 
    TMsvId /*aDestination*/, TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::MoveFromLocalL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::MoveFromLocalL(const CMsvEntrySelection& /*aSelection*/, 
        TMsvId /*aDestination*/, TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::MoveWithinServiceL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::MoveWithinServiceL(const 
    CMsvEntrySelection& /*aSelection*/, TMsvId /*aDestination*/, 
    TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::DeleteAllL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::DeleteAllL(const CMsvEntrySelection& /*aSelection*/, 
    TRequestStatus& /*aStatus*/)

    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::CreateL
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::CreateL(TMsvEntry /*aNewEntry*/, 
    TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::ChangeL
// This function could change file names in the file system. It would then 
// also have to change the details field of all the children of a changed 
// folder name. Not supported for now.
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::ChangeL(TMsvEntry /*aNewEntry*/, 
    TRequestStatus& /*aStatus*/)
    {
    FUNC_LOG;
    
    User::Leave( KErrNotSupported );
    
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::StartCommandL
// Run MTM-specific command on selection of entries 
// Only command supported is Refresh
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::StartCommandL( CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus )
	{
    FUNC_LOG;

	if ( iEmailAttachedInEmail )
		{
		switch( aCommand )
			{
			// Add an rfc822/MIME email as an attachment.
			case KEasEmailMTMAddEmailAttachment: 
				{
	  	        aStatus = KRequestPending;
	  		    iConvertToNestedEmailRequestStatus = &aStatus;
	  		    TBuf<256> tmp;
	  		    tmp.Copy(aParameter);
				TRAPD( er, AddEmailAsAttachmentL( aSelection[0], tmp ) );
				if ( er != KErrNone )
					{
					TRequestStatus* status = &aStatus;
					User::RequestComplete( status, er );
					}
				break;
				}
			case KEasEmailMTMDeleteEntry:
	    		{
				iServerEntry->SetEntry( aSelection[1] );
			 	iServerEntry->SetEntry( aSelection[0] );
			 	iServerEntry->DeleteEntry( aSelection[1] );
			
			    TRequestStatus* status = &aStatus;
			    User::RequestComplete( status, KErrNone );
	    	 	break;	
				}
			
			default:
	    		User::Leave( KErrNotSupported );
				break;
			}
		}
	else
		{
	    User::Leave( KErrNotSupported );
		}
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::CommandExpected
// Prevent object deletion on command completion
// Allow unloading
// ---------------------------------------------------------------------------
//    
TBool CFsServerMtm::CommandExpected()
    {
    FUNC_LOG;
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::Progress
// Pass-back progress information
// ---------------------------------------------------------------------------
//    
const TDesC8& CFsServerMtm::Progress()
    {
    FUNC_LOG;
    return KNullDesC8;
    }

//
//  Active object completion functions
//

// ---------------------------------------------------------------------------
// CFsServerMtm::DoCancel
// Cancel current operation
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::DoCancel()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::DoComplete
// Active object complete leave handler
// Never expect this to be called as CFsServerMtm::DoRunL() does not leave
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::DoComplete(TInt /*aError*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsServerMtm::DoRunL
// Active object completion
// Run is used in this object to clean up after operations have finished.
// ---------------------------------------------------------------------------
//    
void CFsServerMtm::DoRunL()
    {
    FUNC_LOG;

	if ( iEmailAttachedInEmail )
		{
	    // Is it the AddEmailAsAttachment Command
	    if ( iConvertToNestedEmail && !iConvertToNestedEmail->IsActive() )
		    {
		    // This should always be the case. 
		    if ( *iConvertToNestedEmailRequestStatus != KRequestPending ) 
		        {
		  	    User::Leave( KErrCorrupt );
	            }
		    iProcessingNestedEmail = EFalse;

		    if ( iConverter )
		        {
		        delete iConverter;
		        iConverter = NULL;
		        iConvLibrary.Close();
		        }
		  				
		    // If iStatus.Int() > 0 it contains the new ID of the MTM message	
		    User::RequestComplete( iConvertToNestedEmailRequestStatus, iStatus.Int() );	
		    }
	    
	    if ( iProcessingNestedEmail ) 
		    {
			SetActive();	
		    }
		else 
		    {
		    }
		}
    }


// ---------------------------------------------------------------------------
// CFsServerMtm::AddEmailAsAttachmentL
// 
// ---------------------------------------------------------------------------
//  
void CFsServerMtm::AddEmailAsAttachmentL( const TMsvId& aParentEmailId, const TDesC& aAttachmentPath  )
    {
    FUNC_LOG;
	//LOAD EAS LIBRARY TO CONVERT RFC 822 to MTM
	_LIT( KLauncherFilenameExe, "neasmsgconverter.dll" );
	TInt err = iConvLibrary.Load( KLauncherFilenameExe );
	if ( err != KErrNone )
		{
		User::Leave( err );
		}
	TLibraryFunction constructor = iConvLibrary.Lookup( 1 );
	iConverter = ( CMsgConverter* ) constructor();
 
	iConvertToNestedEmail = iConverter->GetMsgActiveObjectL(
		aParentEmailId,
		aAttachmentPath,
		KNullDesC8,
		*iServerEntry,
		ETrue,
		aParentEmailId );
		
	iConverter->StartL( &iStatus );
	iProcessingNestedEmail = ETrue;
 	 
 	if ( !IsActive() )
 		{
 		SetActive();
 		}
    }

// End of file
