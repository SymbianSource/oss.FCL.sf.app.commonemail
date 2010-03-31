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

#include "cmrfocusstrategyeditor.h"
#include "emailtrace.h"

// ---------------------------------------------------------------------------
// CMRFocusStrategyEditor::NewL
// ---------------------------------------------------------------------------
//
CMRFocusStrategyEditor* CMRFocusStrategyEditor::NewL( MESMRFieldStorage& aFactory )
    {
    FUNC_LOG;
    
    CMRFocusStrategyEditor* self =
            new ( ELeave ) CMRFocusStrategyEditor( aFactory );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyEditor::CMRFocusStrategyEditor
// ---------------------------------------------------------------------------
//
CMRFocusStrategyEditor::CMRFocusStrategyEditor( MESMRFieldStorage& aFactory )
: CMRFocusStrategyBase( aFactory )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyEditor::~CMRFocusStrategyEditor
// ---------------------------------------------------------------------------
//
CMRFocusStrategyEditor::~CMRFocusStrategyEditor()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyEditor::EventOccured
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyEditor::EventOccured( TFocusEventType aEventType )
    {
    FUNC_LOG;
    
    if ( aEventType == MMRFocusStrategy::EFocusKeyEvent )
        {
        CMRFocusStrategyBase::ShowFocus();
        }
    if ( aEventType == MMRFocusStrategy::EFocusPointerEvent )
        {
        // We don't hide focus in editor
        iVisibleFocusOn = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CMRFocusStrategyEditor::InitializeFocus
// ---------------------------------------------------------------------------
//
void CMRFocusStrategyEditor::InitializeFocus()
    {
    FUNC_LOG;
    
    // Our focus is always on in editor
    CMRFocusStrategyBase::ShowFocus();
    }

// End of file

