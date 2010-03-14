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
#include "emailstoreuids.hrh"
#include <bautils.h>  
#include <s32file.h>

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
				TDriveNumber drive( EDriveC );
     		GetDriveL( drive );
        if ( DriveInfo::GetDriveStatus( iFs, drive, driveStatus ) == KErrNone )
            {
            _LIT( KPrivate, ":\\Private\\" );    
            TChar driveChar;
            RFs::DriveToChar(drive, driveChar);
            TFileName msgStorePath;
            TBuf<5> driveLetter;
            driveLetter.Append(driveChar);
            msgStorePath.Append(driveLetter);
            msgStorePath.Append(KPrivate);                
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
    
// -----------------------------------------------------------------------------
// CRfsMsgStorePlugin::GetDriveL(TDriveNumber&)
// -----------------------------------------------------------------------------
//
TInt CRfsMsgStorePlugin::GetDriveL( TDriveNumber& aDrive )
    {
    _LIT16( KDriveToUseFile, "db_drive.cfg" );
    TInt ret( KErrNotFound );
    RFs fs;
    if( fs.Connect() == KErrNone )
        {
        CleanupClosePushL( fs );               //+fs
        TFileName fileName;
        _LIT( KCDrive, "C:" );
        _LIT( KPrivate, "Private" );   
        fileName.Append(KCDrive());
        fileName.Append( KPathDelimiter ); 
        fileName.Append(KPrivate);
        fileName.Append( KPathDelimiter );                 
        fileName.AppendNum( KUidMessageStoreExe, EHex );
        fileName.Append( KPathDelimiter );        
        fileName.Append( KDriveToUseFile );
        if( BaflUtils::FileExists( fs, fileName ) )
            {
            RFileReadStream reader;
            if ( reader.Open( fs, fileName, EFileRead ) == KErrNone )
          		{
           		CleanupClosePushL( reader );             //+reader
           		TUint drive = reader.ReadUint32L();
              CleanupStack::PopAndDestroy( &reader );  //-reader
              if(drive <=EDriveZ)
	              {
	           		aDrive = static_cast<TDriveNumber>( drive );
           			ret = KErrNone;
           			}
           		}
            }
        CleanupStack::PopAndDestroy( &fs );    //-fs
        } // end if
    return ret;
    }

// END FILE RfsMsgStorePlugin.cpp
