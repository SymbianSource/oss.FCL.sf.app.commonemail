/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Install freestyle MTM account. This exe is executed when 
*  Nokia-Email SIS package is installed
*
*/



#include "emailtrace.h"
#include <mtclreg.h>
#include <data_caging_path_literals.hrh> // for KDC_MTM_INFO_FILE_DIR'
#include <featurecontrol.h>
#include <centralrepository.h>
#include "FSEmailBuildFlags.h"


#include "fsmtmsconstants.h"

LOCAL_C TFileName InstallationFilenameL();
LOCAL_C void InstallL();
LOCAL_C void DefineFWflag();
LOCAL_C void DoInstallL(); 



// Dummy observer class to absorb Message Server session events
class TDummyObserver : public MMsvSessionObserver
    {
public:
    void HandleSessionEventL( TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, 
        TAny* /*aArg2*/, TAny* /*aArg3*/) {};
    };

// ---------------------------------------------------------------------------
// InstallationFilenameL
// Finds mtm resource file name path
// ---------------------------------------------------------------------------
//   
LOCAL_C TFileName InstallationFilenameL()
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
// InstallL
// Set up Message Server session and install PushImap MTM group
// ---------------------------------------------------------------------------
//   
LOCAL_C void InstallL()
    {
    FUNC_LOG;
    TDummyObserver ob;
    CMsvSession* session = CMsvSession::OpenSyncL( ob );
    CleanupStack::PushL( session );

    //Construct the path information for the Installation file.            
    TFileName resourceFileName = InstallationFilenameL();

    // Install MTMs.
    TInt err = session->InstallMtmGroup( resourceFileName );
    if( err!=KErrNone && err!=KErrAlreadyExists  )
        {
        User::LeaveIfError( err );
        }
    
    // Check if Installed MTM is present in Registry.
    CClientMtmRegistry* mtmRegistry = CClientMtmRegistry::NewL( *session );
    CleanupStack::PushL( mtmRegistry );
    if( !mtmRegistry->IsPresent( KUidMsgValTypeFsMtmVal ) )
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PopAndDestroy( mtmRegistry );
    CleanupStack::PopAndDestroy( session ); 
    
    }

// ---------------------------------------------------------------------------
// DefineFWflag
// Framework flag will be defined to indicate the FW is installed by SIS
// ---------------------------------------------------------------------------
//  
LOCAL_C void DefineFWflag() 
    {  
    FUNC_LOG;
    RFeatureControl featureControl; 
    TInt err = featureControl.Connect(); 
    if ( err == KErrNone )
        { 
        featureControl.Close(); 
        } 
    }


LOCAL_C void SetUpCenrepParams()
	{
    FUNC_LOG;
	}


// ---------------------------------------------------------------------------
// DoInstallL
// Install the MTM
// ---------------------------------------------------------------------------
//  
LOCAL_C void DoInstallL() 
    {
    FUNC_LOG;

    User::LeaveIfError( User::RenameThread( _L("MtmInstall") ) );
    // Define Framework Flag
    DefineFWflag();  
    SetUpCenrepParams();
    // Construct and install the active scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install( scheduler ); 
    TRAPD(error,InstallL());     
    if (error != KErrNone)
         {
         User::Leave(error);
         }
    CleanupStack::PopAndDestroy(scheduler);
    }


// ---------------------------------------------------------------------------
// E32Main
// Called when FS SIS package is installed
// ---------------------------------------------------------------------------
//  
GLDEF_C TInt E32Main()
    {
    FUNC_LOG;
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New(); 
    TRAP_IGNORE(DoInstallL()); 
    delete cleanup; 
    
    __UHEAP_MARKEND;
    return 0; 
    }

