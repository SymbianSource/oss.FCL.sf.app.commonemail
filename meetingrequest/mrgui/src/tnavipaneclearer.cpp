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
* Description:  ESMR UI Container class
*
*/
#include "tnavipaneclearer.h"

#include <eikenv.h> // CEikonEnv
#include <aknnavi.h> // CAknNavigationControlContainer
#include <avkon.hrh> // EEikStatusPaneUidNavi
#include <eikspane.h> // CEikStatusPane

// ---------------------------------------------------------------------------
// TNavipaneClearer::TNavipaneClearer()
// ---------------------------------------------------------------------------
//
TNavipaneClearer::TNavipaneClearer()
: iNaviContainer( NULL ), iFullyInitialized( EFalse )
    {
    CEikStatusPane* sp = 
            CEikonEnv::Static()->AppUiFactory()->StatusPane();// codescanner::eikonenvstatic
    // If this leaves we have to stick with the navi we have.
    TRAP_IGNORE( 
        {
        // Put default, empty Status Pane.
        // Otherwise view's status pane with dates would be shown.
        iNaviContainer = static_cast<CAknNavigationControlContainer *>
                        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
        iNaviContainer->PushDefaultL();
        // If PushDefaultL leaves we know that then we can Pop the container 
        iFullyInitialized = ETrue;
        } );
    }

// ---------------------------------------------------------------------------
// TNavipaneClearer::~TNavipaneClearer()
// ---------------------------------------------------------------------------
//
TNavipaneClearer::~TNavipaneClearer()
    {
    // Remove default status pane
    if ( iNaviContainer && iFullyInitialized )
        {
        iNaviContainer->Pop();
        }    
    iNaviContainer = NULL; // not owned
    }

// End of file

