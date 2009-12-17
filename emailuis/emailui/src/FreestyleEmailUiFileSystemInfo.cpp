/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFileSystemInfo.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include <coemain.h>
#include <apgcli.h>

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------
// CFileSystemInfo::NewL()
// ----------------------------------------------------------
//
CFileSystemInfo* CFileSystemInfo::NewL(const TUid & aApplicationUid)
    {
    FUNC_LOG;
    CFileSystemInfo* self = CFileSystemInfo::NewLC(aApplicationUid);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CFileSystemInfo::NewLC()
// ----------------------------------------------------------
//
CFileSystemInfo* CFileSystemInfo::NewLC(const TUid & aApplicationUid)
    {
    FUNC_LOG;
    CFileSystemInfo* self = new (ELeave) CFileSystemInfo();
    CleanupStack::PushL( self );
    self->ConstructL(aApplicationUid);
    return self;
    }

// ---------------------------------------------------------
// CFileSystemInfo::CFileSystemInfo()
// ---------------------------------------------------------
//
CFileSystemInfo::CFileSystemInfo()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CFileSystemInfo::~CFileSystemInfo()
// ---------------------------------------------------------
//
CFileSystemInfo::~CFileSystemInfo()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CFileSystemInfo::PrivatePath()
// ---------------------------------------------------------
//
TFileName CFileSystemInfo::PrivatePath() const
	{
    FUNC_LOG;
	return iPrivatePath;
	}

// ---------------------------------------------------------
// CFileSystemInfo::ConstructL()
// ---------------------------------------------------------
//
void CFileSystemInfo::ConstructL(const TUid & aApplicationUid)
    {
    FUNC_LOG;
    RApaLsSession apaSession;
    
    // Connect to application architecture service.
    User::LeaveIfError( apaSession.Connect() );
        
    // Retrieve the application info
    TApaAppInfo appInfo;
    User::LeaveIfError( apaSession.GetAppInfo( appInfo, aApplicationUid ) );
    
    // Close connection to application architecture service.
    apaSession.Close();
            
    RFs fs;
    User::LeaveIfError(fs.Connect());
    
    // Parse the installation drive from the application path.
    TParse driveParse;
    driveParse.Set( appInfo.iFullName, NULL, NULL );
    iPrivatePath.Append( driveParse.Drive() );
    
    // Append the private path to installation drive.
    TFileName privatePath;
    fs.PrivatePath( privatePath ); 
    iPrivatePath.Append( privatePath );    
    fs.Close();
    
    }
	
// End of file

