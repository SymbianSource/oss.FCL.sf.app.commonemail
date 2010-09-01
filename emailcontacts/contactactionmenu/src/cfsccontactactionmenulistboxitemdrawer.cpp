/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class 
*                CFscContactActionMenuListBoxItemDrawer.
*
*/


// INCUDES
#include "emailtrace.h"
#include <e32std.h>
#include <AknUtils.h>
#include <AknsUtils.h>

#include "cfsccontactactionmenulistboxitemdrawer.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuListBoxItemDrawer::
//      CFscContactActionMenuListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CFscContactActionMenuListBoxItemDrawer::
    CFscContactActionMenuListBoxItemDrawer(
    MTextListBoxModel* aTextListBoxModel, 
    const CFont* aFont, 
    CColumnListBoxData* aColumnData )
    : CColumnListBoxItemDrawer( aTextListBoxModel, aFont, aColumnData )
    {
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuListBoxItemDrawer::
//      ~CFscContactActionMenuListBoxItemDrawer
// ---------------------------------------------------------------------------
//
CFscContactActionMenuListBoxItemDrawer::
    ~CFscContactActionMenuListBoxItemDrawer()
    {
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuListBoxItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuListBoxItemDrawer::DrawItemText(
    TInt aItemIndex, 
    const TRect& aItemTextRect, 
    TBool aItemIsCurrent, 
    TBool aViewIsEmphasized, 
    TBool aItemIsSelected ) const
    {
    FUNC_LOG;
    
    // Disable AVKON skining.
    // Skinning needs to be disabled during draw operation because otherwise 
    // listbox's implementation would use SSE text colors instead of set
    // FS colors.
    
    TBool skinEnabled = AknsUtils::AvkonSkinEnabled(); 
    //<cmail>
    //TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( EFalse ) ); 
    //</cmail>

    CColumnListBoxItemDrawer::DrawItemText(
        aItemIndex, aItemTextRect, aItemIsCurrent, aViewIsEmphasized, 
        aItemIsSelected );
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );     
    
    }

