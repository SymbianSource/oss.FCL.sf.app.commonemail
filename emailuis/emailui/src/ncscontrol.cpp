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
* Description:  Implementation of the CNcsEngine
*
*/


#include "emailtrace.h"
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <gulcolor.h>
#include <eikdef.h>

#include "ncscontrol.h"
#include "FreestyleEmailUiLayoutData.h"
// <cmail> Plaform layout change
#include "ncsutility.h"
// </cmail> Plaform layout change

// ---------------------------------------------------------------------------
// MNcsControl::MNcsControl
// ---------------------------------------------------------------------------
//
MNcsControl::MNcsControl( MNcsFieldSizeObserver* aObserver ) :
    iSizeObserver( aObserver )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CNcsLabel::CNcsLabel
// ---------------------------------------------------------------------------
//
// <cmail> Platform layout changes
CNcsLabel::CNcsLabel( const CCoeControl& aParent, MNcsFieldSizeObserver* aSizeObserver ) :
    MNcsControl( aSizeObserver ), iParent( aParent )
    {
    FUNC_LOG;
    UpdateTextColor();
    }
// </cmail> Platform layout changes

// ---------------------------------------------------------------------------
// CNcsLabel::Reposition
// ---------------------------------------------------------------------------
//
void CNcsLabel::Reposition( TPoint& aPt, TInt /*aWidth <Cmail>*/ )
    {
    FUNC_LOG;
    // <cmail> Platform layout changes
    /*
    TInt ht = Font()->HeightInPixels() + Font()->DescentInPixels();
    ht += 2 * KAifLabelMarginHorizontal;
    TSize sz( aWidth, ht );
    if ( Rect() != TRect( aPt, sz ) )
        {
        SetExtent( aPt, sz );
        }
    aPt.iY += Size().iHeight;
    */
    // </cmail> Platform layout changes
    SetPosition( aPt );
    }

// ---------------------------------------------------------------------------
// CNcsLabel::FocusChanged
// ---------------------------------------------------------------------------
//
void CNcsLabel::FocusChanged( TDrawNow aDrawNow )
    {
    FUNC_LOG;
    if( IsFocused() )
        {
        //Check if we need to reposition
        if (Rect().iTl.iY < 0) 
            {
            TPoint pt = TPoint(0,0);
            Reposition(pt,Rect().Width());
            if ( iSizeObserver )
                {
                iSizeObserver->UpdateFieldPosition(this);
                }
            }
        }
    
    if( aDrawNow ) 
        {
        DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// CNcsLabel::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CNcsLabel::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CEikLabel::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikMessageColorSchemeChange )
        {
        UpdateTextColor();
        }
    }

// ---------------------------------------------------------------------------
// CNcsLabel::UpdateTextColor
// ---------------------------------------------------------------------------
//
void CNcsLabel::UpdateTextColor()
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb newColor;
    
    TInt err = AknsUtils::GetCachedColor(
        skin,
        newColor,
        KAknsIIDFsTextColors,
        EAknsCIFsTextColorsCG8 );
    
    if ( err == KErrNone )
        {
        TRAP_IGNORE( OverrideColorL( EColorLabelText, newColor ) );
        }
    }

// ---------------------------------------------------------------------------
// CNcsLabel::LayoutLineCount
// ---------------------------------------------------------------------------
//
TInt CNcsLabel::LayoutLineCount() const
    {
    return IsVisible() ? 1 : 0;
    }
