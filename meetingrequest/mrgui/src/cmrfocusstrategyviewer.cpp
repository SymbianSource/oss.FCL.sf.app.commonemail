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
* Description:  Focus strategy for editor implementation
*
*/

#include "cmrfocusstrategyviewer.h"

#include "nmrglobalfeaturesettings.h"
#include "mesmrfieldstorage.h"
#include "cesmrfield.h"
#include "emailtrace.h"

// ---------------------------------------------------------------------------
// CMRFocusStrategyViewer::NewL
// ---------------------------------------------------------------------------
//
CMRFocusStrategyViewer* CMRFocusStrategyViewer::NewL( MESMRFieldStorage& aFactory )
    {
    FUNC_LOG;
    
    CMRFocusStrategyViewer* self =
            new ( ELeave ) CMRFocusStrategyViewer( aFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyViewer::CMRFocusStrategyViewer
// ---------------------------------------------------------------------------
//
CMRFocusStrategyViewer::CMRFocusStrategyViewer( MESMRFieldStorage& aFactory )
: CMRFocusStrategyBase( aFactory )
    {
    FUNC_LOG;
    
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyViewer::~CMRFocusStrategyViewer
// ---------------------------------------------------------------------------
//
CMRFocusStrategyViewer::~CMRFocusStrategyViewer()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyViewer::ConstructL
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyViewer::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyViewer::InitializeFocus
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyViewer::InitializeFocus()
    {
    FUNC_LOG;
    
    // Focus is not on by default
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

// ---------------------------------------------------------------------------
// CMRFocusStrategyViewer::EventOccured
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyViewer::EventOccured( TFocusEventType aEventType )
    {
    FUNC_LOG;
    
    if ( aEventType == MMRFocusStrategy::EFocusKeyEvent )
        {
        // If device has the keyboard, we should show the focus
        if ( NMRGlobalFeatureSettings::KeyboardType() !=
                        NMRGlobalFeatureSettings::ENoKeyboard )
            {
            CMRFocusStrategyBase::ShowFocus();            
            }
        }
    if ( aEventType == MMRFocusStrategy::EFocusPointerEvent )
        {
        DoHideFocus();
        }
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyViewer::DoHideFocus
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyViewer::DoHideFocus()
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
            
            // Every field is redrawn due to draw problems after 
            // focus removal
            field->DrawDeferred();
            }

        iVisibleFocusOn = EFalse;
        }
    }
// End of file

