/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CMRViewers ECom API
*
*/


// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "emailtrace.h"
#include "cesmrviewersimpl.h"

// From ESMRVIEWER
#include "cesmrviewerctrl.h"
#include "caleneditorsplugin.h"

// From MR Mailbox Utils
#include <cmrmailboxutils.h>
#include <CMRUtils.h>

// From System
#include <coemain.h>
#include <bautils.h>
#include <f32file.h>
#include <data_caging_path_literals.hrh>

// Logging utilities
// <cmail> Removed profiling. </cmail>

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CESMRViewersImpl::CESMRViewersImpl
// ----------------------------------------------------------------------------
//
CESMRViewersImpl::CESMRViewersImpl()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CESMRViewersImpl::~CESMRViewersImpl
// ----------------------------------------------------------------------------
//
CESMRViewersImpl::~CESMRViewersImpl()
    {
    FUNC_LOG;
    delete iMtmUid;
    delete iController;
    }

// ----------------------------------------------------------------------------
// CESMRViewersImpl::NewL
// ----------------------------------------------------------------------------
//
CESMRViewersImpl* CESMRViewersImpl::NewL( TAny* aMtmUid )
    {
    FUNC_LOG;
    CESMRViewersImpl* self = new (ELeave) CESMRViewersImpl;
    CleanupStack::PushL( self );
    self->ConstructL(aMtmUid);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CESMRViewersImpl::ConstructL
// ----------------------------------------------------------------------------
//
void CESMRViewersImpl::ConstructL( TAny* aMtmUid ) // codescanner::LFunctionCantLeave
    {
    FUNC_LOG;
    iMtmUid = reinterpret_cast<HBufC8*>( aMtmUid );
    }

// ----------------------------------------------------------------------------
// CESMRViewersImpl::ExecuteViewL
// ----------------------------------------------------------------------------
//
TInt CESMRViewersImpl::ExecuteViewL( // codescanner::intleaves
    RPointerArray<CCalEntry>& aEntries,
    const TAgnEntryUiInParams& aInParams,
    TAgnEntryUiOutParams& aOutParams,
    MAgnEntryUiCallback& aCallback)
    {
    FUNC_LOG;
    TRAPD( err, ExecuteViewInternalL(
					aEntries,
					aInParams,
					aOutParams,
					aCallback ) );
    
    if ( KErrArgument == err )
    	{
    	// ES MR VIEWER controller was unable to show the
    	// passed meeting request. Let's launc normal meeting viewer
    	// for showing the entry
    	
    	CCalenEditorsPlugin* editorsPlugin =  CCalenEditorsPlugin::NewL();
		CleanupStack::PushL( editorsPlugin );

        err = editorsPlugin->ExecuteViewL( aEntries,
                                           aInParams,
                                           aOutParams,
                                           aCallback );
    	
        CleanupStack::PopAndDestroy( editorsPlugin );
    	}
    
    return err;
    }

// ----------------------------------------------------------------------------
// CESMRViewersImpl::SetHelpContext
// ----------------------------------------------------------------------------
//
void CESMRViewersImpl::SetHelpContext( const TCoeHelpContext& /*aContext*/ )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CESMRViewersImpl::ExecuteViewInternalL
// ----------------------------------------------------------------------------
//
TInt CESMRViewersImpl::ExecuteViewInternalL( // codescanner::intleaves
		RPointerArray<CCalEntry>& aEntries,
		const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
		MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
		MAgnEntryUiCallback& aCallback)
	{
    FUNC_LOG;
    // ES MR VIEWER controller is created. Controller will handle the
    // MR UTILS creation and showing the UI to user. Controller will also
    // receive the callback information from user and handles the BLC.
    delete iController; 
    iController = NULL;
    iController =
        CESMRViewerController::NewL( *iMtmUid,
                                     aEntries,
                                     aInParams,
                                     aOutParams,
                                     aCallback );

    iController->ExecuteL();

    return KErrNone;	
	}
// End of file

