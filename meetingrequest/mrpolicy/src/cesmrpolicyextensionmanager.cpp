/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR policy extension manager implementation
*
*/

#include "emailtrace.h"
#include "cesmrpolicyextensionmanager.h"

#include "esmrinternaluid.h"

// System includes
#include <bautils.h>    // BaflUtils
#include <coemain.h>    // CCoeEnv
#include <ecom/implementationinformation.h> // RImplInfoPtrArray
#include <ecom/ecom.h>  // REComSession

/// Unnamed namespace for local definitions
namespace {
const TInt KESMRExtensionResourceFileOffset ( 2 );

/**
 * Custom cleanup function.
 * @param aObj  Object to clean.
 */
void CleanupResetAndDestroy( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<RImplInfoPtrArray*>( aObj )->ResetAndDestroy();
        }
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRExtensionResourceFile::CESMRExtensionResourceFile
// ---------------------------------------------------------------------------
//
CESMRExtensionResourceFile::CESMRExtensionResourceFile( RFs& aFs )
: iFsSession( aFs )
    {
    FUNC_LOG;
    // do nothing
    }

// ---------------------------------------------------------------------------
// CESMRExtensionResourceFile::NewL
// ---------------------------------------------------------------------------
//
CESMRExtensionResourceFile*
CESMRExtensionResourceFile::NewL( const TDesC& aPath, RFs& aFs )
    {
    CESMRExtensionResourceFile* self =
        new (ELeave) CESMRExtensionResourceFile( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aPath );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRExtensionResourceFile::~CESMRExtensionResourceFile
// ---------------------------------------------------------------------------
//
CESMRExtensionResourceFile::~CESMRExtensionResourceFile()
    {
    FUNC_LOG;
    iResFile.Close();
    // Don't close the fssession, we have only reference to CCoeEnv's RFs
    }

// ---------------------------------------------------------------------------
// CESMRExtensionResourceFile::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRExtensionResourceFile::ConstructL( const TDesC& aPath )
    {
    FUNC_LOG;
    TFindFile findFile( iFsSession );
    TInt error = findFile.FindByDir( aPath, KNullDesC() );
    if ( error == KErrNone )
        {
        TFileName fileName;
        fileName.Append( findFile.File() );
        // Solve the FullPath
        BaflUtils::NearestLanguageFile(iFsSession, fileName);
        iResFile.OpenL(iFsSession, fileName);
        iResFile.ConfirmSignatureL();

        // Extension resource should be always the first resource in the
        // rss file.
        iResId = iResFile.Offset() + KESMRExtensionResourceFileOffset;
        }
    }

// ---------------------------------------------------------------------------
// CESMRExtensionResourceFile::ResId
// ---------------------------------------------------------------------------
//
TInt CESMRExtensionResourceFile::ResId() const
    {
    FUNC_LOG;
    return iResId;
    }

// ---------------------------------------------------------------------------
// CESMRExtensionResourceFile::ResFile
// ---------------------------------------------------------------------------
//
RResourceFile& CESMRExtensionResourceFile::ResFile()
    {
    FUNC_LOG;
    return iResFile;
    }



///////////////////////////////////////////////////////////////////////////////
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRPolicyExtensionManager::CESMRPolicyExtensionManager
// ---------------------------------------------------------------------------
//
CESMRPolicyExtensionManager::CESMRPolicyExtensionManager( CCoeEnv& aCoeEnv )
: iCoeEnv( aCoeEnv )
    {
    FUNC_LOG;
    // do nothing
    }

// ---------------------------------------------------------------------------
// CESMRPolicyExtensionManager::NewL
// ---------------------------------------------------------------------------
//
CESMRPolicyExtensionManager* CESMRPolicyExtensionManager::NewL(
        CCoeEnv& aCoeEnv )
    {
    FUNC_LOG;
    CESMRPolicyExtensionManager* self =
        new (ELeave) CESMRPolicyExtensionManager( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRPolicyExtensionManager::~CESMRPolicyExtensionManager
// ---------------------------------------------------------------------------
//
CESMRPolicyExtensionManager::~CESMRPolicyExtensionManager()
    {
    FUNC_LOG;
    iResourceFiles.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMRPolicyExtensionManager::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRPolicyExtensionManager::ConstructL()
    {
    FUNC_LOG;
    // do nothing
    }

// ---------------------------------------------------------------------------
// CESMRPolicyExtensionManager::ReadResourcesFromExtensionsL
// ---------------------------------------------------------------------------
//
void CESMRPolicyExtensionManager::ReadResourcesFromExtensionsL()
    {
    FUNC_LOG;
    RImplInfoPtrArray implementations;
    REComSession::ListImplementationsL(TUid::Uid(
            KESMRSchedulingviewInterfaceUid), implementations);
    CleanupStack::PushL(TCleanupItem(
            CleanupResetAndDestroy, &implementations));

    // loop through UI extensions in reverse order
    const TInt count = implementations.Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);

        TFileName resourceFileName;
        resourceFileName.Copy(implInfo->OpaqueData());
        if ( resourceFileName != KNullDesC() && resourceFileName.Length() != 0 )
            {
            CESMRExtensionResourceFile* resFile = NULL;
            TRAPD( err, resFile = CESMRExtensionResourceFile::NewL(
                    resourceFileName, iCoeEnv.FsSession() ) );
            if ( err == KErrNone  && resFile )
                {
                iResourceFiles.AppendL( resFile );
                }
            }
        CleanupStack::PopAndDestroy( implInfo );
        }
    CleanupStack::PopAndDestroy(); // implementations
    }

// ---------------------------------------------------------------------------
// CESMRPolicyExtensionManager::ReadBufferL
// ---------------------------------------------------------------------------
//
HBufC8* CESMRPolicyExtensionManager::ReadBufferL( TInt aResourceId )
    {
    FUNC_LOG;
    HBufC8* buffer = NULL;
    TInt count( iResourceFiles.Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        CESMRExtensionResourceFile* resFile = iResourceFiles[i];
        if ( resFile->ResId() == aResourceId )
            {
            buffer = resFile->ResFile().AllocReadL(aResourceId);
            }
        }

    return buffer;
    }

// ---------------------------------------------------------------------------
// CESMRPolicyExtensionManager::ExtensionResourceFile
// ---------------------------------------------------------------------------
//
CESMRExtensionResourceFile* CESMRPolicyExtensionManager::ExtensionResourceFile(
        TInt aResourceId )
    {
    FUNC_LOG;
    CESMRExtensionResourceFile* file = NULL;
    TInt count( iResourceFiles.Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        CESMRExtensionResourceFile* resFile = iResourceFiles[i];
        if ( resFile->ResId() == aResourceId )
            {
            file = resFile;
            break;
            }
        }

    return file;
    }

//EOF

