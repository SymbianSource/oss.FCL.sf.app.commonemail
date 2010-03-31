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
* Description:  ESMR UI Factory implementation
*
*/

#include "cesmruifactory.h"
#include "mesmruibase.h"
#include "cesmreditordialog.h"
#include "cesmrviewerdialog.h"
#include "cesmrtrackingviewdialog.h"
#include "cesmrviewerfieldstorage.h"
#include "esmrhelper.h"
#include "cesmrpolicy.h"
#include "mmrinfoprovider.h"
#include "mmrpolicyprovider.h"

#include <esmrgui.rsg>
#include <e32base.h>
#include <eikenv.h>
#include <coneresloader.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

// DEBUG
#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace  {

// Definition for ui resource file
_LIT( KResourceFile, "esmrgui.rsc" );

#ifdef _DEBUG

/** Literal for panics */
_LIT( KESMRUIFactoryPanicTxt, "ESMRUIFactory" );

/** Enumeration for panic codes */
enum TESMRUIFactoryPanic
    {
    EESMRUIFactoryInvalidEntry,
    EESMRUIFactoryInvalidPolicyMode
    };

/**
 * Raises system panic
 */
void Panic( TESMRUIFactoryPanic aPanic )
    {
    User::Panic( KESMRUIFactoryPanicTxt, aPanic );
    }

#endif // _DEBUG
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRUiFactory::CESMRUiFactory
// ---------------------------------------------------------------------------
//
CESMRUiFactory::CESMRUiFactory()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRUiFactory::~CESMRUiFactory
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRUiFactory::~CESMRUiFactory()
    {
    FUNC_LOG;
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
    }

// ---------------------------------------------------------------------------
// CESMRUiFactory::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRUiFactory* CESMRUiFactory::NewL()
    {
    FUNC_LOG;
    CESMRUiFactory* self = new (ELeave) CESMRUiFactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRUiFactory::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRUiFactory::ConstructL()
    {
    FUNC_LOG;
    CEikonEnv* eikEnv=CEikonEnv::Static();// codescanner::eikonenvstatic
    LoadResourceFileL(eikEnv, KResourceFile, iResourceOffset);    
    }

// ---------------------------------------------------------------------------
// CESMRUiFactory::LoadResourceFileL
// ---------------------------------------------------------------------------
//
void CESMRUiFactory::LoadResourceFileL(
        CEikonEnv* aEikEnv,
        const TDesC& aFileName,
        TInt& aResourceOffset )
    {
    FUNC_LOG;
    TFileName pathAndFile;
    
    INFO_1( "Locating resource file for %S", &aFileName );
    ESMRHelper::LocateResourceFile(
                aFileName,
                KDC_RESOURCE_FILES_DIR,
                pathAndFile,
                &aEikEnv->FsSession() );

    INFO_1( "Finding nearest language file for %S", &pathAndFile );
    
    // Find the resource file for the nearest language
    BaflUtils::NearestLanguageFile(aEikEnv->FsSession(), pathAndFile );
    
    INFO( "Adding resource file" );
    aResourceOffset = CCoeEnv::Static()->AddResourceFileL( pathAndFile );
    }

// ---------------------------------------------------------------------------
// CESMRUiFactory::CreateUIL()
// ---------------------------------------------------------------------------
//
MESMRUiBase* CESMRUiFactory::CreateUIL(
        MMRInfoProvider& aInfoProvider,
        MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    MESMRUiBase* ui = NULL;

    TESMRViewMode viewMode =
        aInfoProvider.PolicyProvider().CurrentPolicy().ViewMode();
    
    switch ( viewMode )
        {
        case EESMREditMR: // Fall through
        case EESMRForwardMR:
            {
            ui = CESMREditorDialog::NewL(
                    aInfoProvider,
                    aCallback );
            break;
            }
        case EESMRViewMR: // Fall through
        case EESMRViewAllDayMR:
            {
            ui = CESMRViewerDialog::NewL(
                    aInfoProvider,
                    aCallback );
            break;
            }
        case EESMRTrackingViewMR:
            {
            ui = CESMRTrackingViewDialog::NewL(
                    aInfoProvider.PolicyProvider().CurrentPolicy(),
                    *aInfoProvider.EntryL(),
                    aCallback );
            break;
            }
        default:
            { 
            break;
            } 
        }

 #ifdef _DEBUG
    __ASSERT_DEBUG(ui, Panic(EESMRUIFactoryInvalidPolicyMode) );
 #else
    if ( !ui )
        {
        User::Leave( KErrArgument );
        }

 #endif // _DEBUG

    return ui;
    }

// EOF

