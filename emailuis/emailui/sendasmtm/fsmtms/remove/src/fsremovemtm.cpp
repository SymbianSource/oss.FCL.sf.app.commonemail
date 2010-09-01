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
* Description:  UnInstall freestyle MTM account. This exe is executed when 
*  Nokia-Email SIS package is uninstalled.
*
*/


#include "emailtrace.h"
#include <msvapi.h>
#include <msvids.h>
#include <msvuids.h>
#include <apgtask.h>
#include <data_caging_path_literals.hrh> 
#include "FSEmailBuildFlags.h"
#include <featurecontrol.h>                // RFeatureControl
#include <centralrepository.h>
#include "fsmtmsconstants.h"

LOCAL_C TFileName UnInstallationFilenameL();
LOCAL_C void KillProcess( const TDesC& aName );
LOCAL_C void RemoveAccountsL();
LOCAL_C void KillAppsL();
LOCAL_C void DoRemoveL(); 
LOCAL_C void RemoveL();
LOCAL_C void UnDefineFWflag();


#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])


class TDummyObserver : public MMsvSessionObserver
    {
public:
    void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, 
        TAny* /*aArg2*/, TAny* /*aArg3*/) {};
    };

// ---------------------------------------------------------------------------
// UnInstallationFilenameL
// Finds mtm resource file name path
// ---------------------------------------------------------------------------
//  
LOCAL_C TFileName UnInstallationFilenameL()
    {
    FUNC_LOG;
    //Construct the path information for the Installation file.
    TFileName fileName;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    TFindFile finder( fs );
    User::LeaveIfError(
        finder.FindByDir( KFsMtmDirAndFile, KDC_MTM_INFO_FILE_DIR ) );
    fileName = finder.File();
    CleanupStack::PopAndDestroy();
    return fileName;
    }  
    
// ---------------------------------------------------------------------------
// KillProcess
// Kill Messaging server if present
// ---------------------------------------------------------------------------
//    
LOCAL_C void KillProcess( const TDesC& aName )
    {
    FUNC_LOG;
    TFullName name;
    TFindProcess find( aName );
    RProcess process;

    while ( find.Next( name ) == KErrNone )
        {
        if ( process.Open( find ) == KErrNone )
            {
            process.Kill( KErrGeneral );
            process.Close();
            }
        } // end while
        
    }
    
// ---------------------------------------------------------------------------
// RemoveAccountL
// Removes the Nokia-Email accounts from MCE and also from MCE default settings
// ---------------------------------------------------------------------------
//  
LOCAL_C void RemoveAccountsL()
    {
    FUNC_LOG;

    // Open a session to the message server.
    TDummyObserver ob;
    CMsvSession* session = CMsvSession::OpenSyncL(ob);    
    CleanupStack::PushL( session );    
    
    CMsvEntry* msvEntry = CMsvEntry::NewL( *session, KMsvRootIndexEntryId, 
    TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );                                       
    CleanupStack::PushL( msvEntry );

    msvEntry->SetEntryL( KMsvRootIndexEntryId );
    // Get Freestyle services.
    CMsvEntrySelection* childSelection = msvEntry->ChildrenWithMtmL( 
            KUidMsgValTypeFsMtmVal );
    CleanupStack::PushL( childSelection );
    // No Need to do any thing if there are no accounts.
    if(childSelection->Count() > 0)
    	{
		TMsvId accountId = NULL;
        // Remove accounts in a loop
		for( TInt i = 0; i < childSelection->Count(); i++ )
	        {
	        msvEntry->SetEntryL( childSelection->At( i ) );
	        if ( msvEntry->Entry().iMtm == KUidMsgValTypeFsMtmVal )
	            {
	            accountId = msvEntry->Entry().Id();
	            msvEntry->SetEntryL( KMsvRootIndexEntryId );
	            msvEntry->DeleteL( accountId );
	            accountId = NULL;
	            }
	        }
    	}
    else
    	{
    	}
	    CleanupStack::PopAndDestroy( childSelection ); 
	    CleanupStack::PopAndDestroy( msvEntry );
	    CleanupStack::PopAndDestroy( session ); 
}

// ---------------------------------------------------------------------------
// KillAppsL
// ---------------------------------------------------------------------------
//  
LOCAL_C void KillAppsL()
    {
    FUNC_LOG;

    RWsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

    TInt appCount = ARRAY_SIZE( KAppsToKill );

    TApaTaskList taskList( session );

    for ( int i = 0 ; i < appCount ; i++ ) 
        {
        TApaTask task = taskList.FindApp( KAppsToKill[i] );
        if ( task.Exists() )
            {
            task.EndTask();
            }
        }
    CleanupStack::PopAndDestroy( &session );

    }

// ---------------------------------------------------------------------------
// RemoveL
// Remove accounts and then Deinstall Nokia-Email
// ---------------------------------------------------------------------------
//  
LOCAL_C void RemoveL()
    {       
    FUNC_LOG; 

    TDummyObserver ob;
    CMsvSession* session = CMsvSession::OpenSyncL( ob );
    CleanupStack::PushL( session );
    // Remove the accounts.
    RemoveAccountsL();
    
    //Construct the path information for the Installation file.            
    TFileName resourceFileName = UnInstallationFilenameL();
 
    // Kill the message server process
    KillProcess( _L( "MSEXE*" ) );
    User::After( KOneSecond );    
    // Uninstall the MTMs.
    TInt retryCount = 0;
    TInt err = 0;
    do 
        {
        err = session->DeInstallMtmGroup( resourceFileName );
        if ( err == KErrNone )
            {
            }
        else if ( err == KErrInUse )
            {
            TRAP_IGNORE( KillAppsL() );
            //wait one second
            User::After( KOneSecond );
            }
        else
        {
        }
    } while ( err == KErrInUse && retryCount++ < KMAxRetry );

    CleanupStack::PopAndDestroy( session ); 
    // Remove all Nokia-Email accounts after uninstalling MTM
    //RemoveAccountsL();
    
    } 

// ---------------------------------------------------------------------------
// UnDefineFWflag
// Framework flag will be undefined.
// ---------------------------------------------------------------------------
//  
LOCAL_C void UnDefineFWflag() 
    { 
    FUNC_LOG; 
    RFeatureControl featureControl; 
    TInt err = featureControl.Connect(); 
    if ( err == KErrNone )
        { 
        featureControl.Close(); 
        } 
    }

LOCAL_C void UnSetCenrepParams()
	{
	}

// ---------------------------------------------------------------------------
// DoRemoveL
// ---------------------------------------------------------------------------
//  
LOCAL_C void DoRemoveL() 
    {    
    FUNC_LOG;
    
    // Undfine FW flag
    UnDefineFWflag();
    UnSetCenrepParams();
    // Construct and install the active scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TRAP_IGNORE(RemoveL());
    
    CleanupStack::PopAndDestroy( scheduler );
    
    } 

// ---------------------------------------------------------------------------
// E32Main
// ---------------------------------------------------------------------------
//  
GLDEF_C TInt E32Main()
    {
    FUNC_LOG;
    __UHEAP_MARK;
    
    CTrapCleanup* cleanup=CTrapCleanup::New(); 
    
    TRAP_IGNORE( DoRemoveL() );
    
    delete cleanup; 
    
    __UHEAP_MARKEND;
    return 0; 
    } 

// End of file
