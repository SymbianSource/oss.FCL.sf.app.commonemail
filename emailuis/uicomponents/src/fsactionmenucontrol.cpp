/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A control for action menu component
*
*/


#include "emailtrace.h"
#include "fsactionmenucontrol.h"
#include "fsactionmenu.h"
// <cmail> SF

#include <alf/alfenv.h>
#include <alf/alfvisual.h>
// </cmail>

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsActionMenuControl::CFsActionMenuControl( CFsActionMenu* aActionMenu )
    :iActionMenu ( aActionMenu )
    {
    FUNC_LOG;
    
    }


// ---------------------------------------------------------------------------
//  Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CFsActionMenuControl::ConstructL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CAlfControl::ConstructL( aEnv );
    }


// ---------------------------------------------------------------------------
//  Destructor.
// ---------------------------------------------------------------------------
//
CFsActionMenuControl::~CFsActionMenuControl()
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CFsActionMenuControl::SetRootVisual( CAlfVisual* aVisual )
    {
    FUNC_LOG;
    iVisual = aVisual;
    }


// ---------------------------------------------------------------------------
//  From CAlfControl.
//  Notifies the owner that the layout of a visual has been recalculated.
// ---------------------------------------------------------------------------
//
void CFsActionMenuControl::VisualLayoutUpdated( CAlfVisual& aVisual )
    {
    FUNC_LOG;
    if ( &aVisual == iVisual )
        {
        iVisual->ClearFlag(EAlfVisualFlagLayoutUpdateNotification);
        TRAP_IGNORE( iActionMenu->RootLayoutUpdatedL() ); // <cmail>
        iVisual->SetFlag(EAlfVisualFlagLayoutUpdateNotification);        
        }
    }

// End of File

