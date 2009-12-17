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
* Description:  Implementation of RFS (Restore Factory Settings) Plugin i/f.
*
*/
//  Include Files
#include <driveinfo.h>
#include <f32file.h>

#include "RfsMsgStorePlugin.h"
#include "EmailStoreUids.hrh"

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::NewL()
// -----------------------------------------------------------------------------
//
CRfsMsgStorePlugin* CRfsMsgStorePlugin::NewL()
    {
    CRfsMsgStorePlugin* self = new ( ELeave ) CRfsMsgStorePlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::~CRfsMsgStorePlugin()
// -----------------------------------------------------------------------------
//
CRfsMsgStorePlugin::~CRfsMsgStorePlugin()
    {
    iFs.Close();
    }

// ---------------------------------------------------------------------------
// CRfsMsgStorePlugin::RestoreFactorySettingsL
// Handle rfs.
// ---------------------------------------------------------------------------
//
void CRfsMsgStorePlugin::RestoreFactorySettingsL( const TRfsReason aType )
    {
    if ( aType == ENormalRfs || aType == EDeepRfs || aType == EInitRfs )
        {
        TUint driveStatus( 0 );

        if ( DriveInfo::GetDriveStatus( iFs, EDriveE, driveStatus ) == KErrNone )
            {
            // check if internal E: drive is exists on the device
            if ( ( driveStatus & DriveInfo::EDriveUserVisible ) &&
                    ( driveStatus & DriveInfo::EDriveInternal )  &&
                    ( driveStatus & DriveInfo::EDrivePresent ) )
                {
                // since we are dealing with internal E: drive only,
                // it is ok to hardcoded the E: in the string
                _LIT( KMsgStoreEDrivePrivate, "E:\\Private\\" );

                TFileName msgStorePath( KMsgStoreEDrivePrivate );
                msgStorePath.AppendNum( KUidMessageStoreExe, EHex );
                msgStorePath.Append( KPathDelimiter );
                CFileMan* fileManager = CFileMan::NewL( iFs );
                // this will recursively delete all files in all directories under the msgStorePath
                // but directories under the msgStorePath are not deleted
                fileManager->Delete( msgStorePath, CFileMan::ERecurse );
                delete fileManager;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CRfsMsgStorePlugin::GetScriptL
// ---------------------------------------------------------------------------
//
void CRfsMsgStorePlugin::GetScriptL( const TRfsReason /*aType*/, TDes& aPath )
    {
    // no-op
    // there is no script for RFS Server to execute,
    // just populate the aPath to null string
    aPath.Copy( KNullDesC );
    }

// ---------------------------------------------------------------------------
// CRfsMsgStorePlugin::ExecuteCustomCommandL
// ---------------------------------------------------------------------------
void CRfsMsgStorePlugin::ExecuteCustomCommandL( const TRfsReason /*aType*/, TDesC& /*aCommand*/ )
    {
    // no-op
    // there is no custom command to be executed by this plugin
    }

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::CRfsMsgStorePlugin()
// -----------------------------------------------------------------------------
//
CRfsMsgStorePlugin::CRfsMsgStorePlugin()
    {
    }

// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::ConstructL()
// -----------------------------------------------------------------------------
//
void CRfsMsgStorePlugin::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    }

// END FILE RfsMsgStorePlugin.cpp
