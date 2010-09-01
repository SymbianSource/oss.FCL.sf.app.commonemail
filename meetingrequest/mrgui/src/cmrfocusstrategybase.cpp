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
* Description:  Focus strategy base class implementation
*
*/

#include "cmrfocusstrategybase.h"

#include "mesmrfieldstorage.h"
#include "cesmrfield.h"
#include "emailtrace.h"

// ---------------------------------------------------------------------------
// CMRFocusStrategyBase::CMRFocusStrategyBase
// ---------------------------------------------------------------------------
//
CMRFocusStrategyBase::CMRFocusStrategyBase( MESMRFieldStorage& aFactory )
: iFactory( aFactory )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyBase::~CMRFocusStrategyBase
// ---------------------------------------------------------------------------
//
CMRFocusStrategyBase::~CMRFocusStrategyBase()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyBase::InitializeFocus
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyBase::InitializeFocus()
    {
    FUNC_LOG;
    
    HideFocus();
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyBase::EventOccured
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyBase::EventOccured( TFocusEventType aEventType )
    {
    FUNC_LOG;
    
    // Default implementation
    if ( aEventType == MMRFocusStrategy::EFocusKeyEvent )
        {
        ShowFocus();
        }
    if ( aEventType == MMRFocusStrategy::EFocusPointerEvent )
        {
        HideFocus();
        }
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyBase::HideFocus
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyBase::HideFocus()
    {
    FUNC_LOG;
    
    if ( iVisibleFocusOn )
        {
        const TInt count( iFactory.Count() );
        for ( TInt i = 0; i < count; i++ )
            {
            CESMRField* field = iFactory.Field( i );

            // At the start there is no focus in hybrid or touch device.
            // The hw key provokest the focus again
            field->SetFocusType( EESMRNoFocus );
            }
        iVisibleFocusOn = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyBase::ShowFocus
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyBase::ShowFocus()
    {
    FUNC_LOG;
    
    if ( !iVisibleFocusOn )
        {
        const TInt count( iFactory.Count() );
        for ( TInt i = 0; i < count; i++ )
            {
            CESMRField* field = iFactory.Field( i );

            // At the start there is no focus in hybrid or touch device.
            // The hw key provokes the focus again
            field->SetFocusType( EESMRHighlightFocus );
            }
        iVisibleFocusOn = ETrue;
        }
    }
        
// End of file
