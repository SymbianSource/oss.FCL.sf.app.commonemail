/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

//<cmail>
#include "emailtrace.h"
#include "cesmruifactory.h"
#include "mesmruibase.h"
#include "cesmreditordialog.h"
#include "cesmrviewerdialog.h"
#include "cesmrtrackingviewdialog.h"
#include "cesmrviewerfieldstorage.h"
#include "esmrhelper.h"
#include <esmrgui.rsg>
#include "cesmrpolicy.h"
//</cmail>
#include <e32base.h>
#include <eikenv.h>
#include <ConeResLoader.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

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
    ESMRHelper::LocateResourceFile(
                aFileName,
                KDC_RESOURCE_FILES_DIR,
                pathAndFile,
                &aEikEnv->FsSession() );

    // Find the resource file for the nearest language
    BaflUtils::NearestLanguageFile(aEikEnv->FsSession(), pathAndFile );
    aResourceOffset = CCoeEnv::Static()->AddResourceFileL( pathAndFile );
    }

// ---------------------------------------------------------------------------
// CESMRUiFactory::CreateUIL()
// ---------------------------------------------------------------------------
//
MESMRUiBase* CESMRUiFactory::CreateUIL(
        CESMRPolicy* aPolicy,
        MESMRCalEntry& aEntry,
        MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    MESMRUiBase* ui = NULL;

    TESMRViewMode viewMode = aPolicy->ViewMode();
    switch ( viewMode )
        {
        case EESMREditMR: // Fall through
        case EESMRForwardMR:
            {
            ui = CESMREditorDialog::NewL(
                    aPolicy,
                    aEntry,
                    aCallback );
            break;
            }
        case EESMRViewMR: // Fall through
        case EESMRViewAllDayMR:
            {
            ui = CESMRViewerDialog::NewL(
                    aPolicy,
                    aEntry,
                    aCallback );
            break;
            }
        case EESMRTrackingViewMR:
            {
            ui = CESMRTrackingViewDialog::NewL(
                    aPolicy,
                    aEntry,
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

