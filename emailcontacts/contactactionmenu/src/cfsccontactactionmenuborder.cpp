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
* Description:  Implementation of the class CFscContactActionMenuBorder.
*
*/


// INCUDES
#include "emailtrace.h"
#include <gdi.h>

#include "cfsccontactactionmenuborder.h"
#include "fsccontactactionmenuuidefines.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuBorder* CFscContactActionMenuBorder::NewL(
    const TRect& aRect,
    TCornerType aCornerType )
    {
    FUNC_LOG;

    CFscContactActionMenuBorder* self = 
        new (ELeave) CFscContactActionMenuBorder( aRect, aCornerType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::~CFscContactActionMenuBorder
// ---------------------------------------------------------------------------
//
CFscContactActionMenuBorder::~CFscContactActionMenuBorder()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::Draw
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuBorder::Draw(const TRect& /*aRect*/) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();
    gc.SetPenColor( iColor );
    gc.SetBrushStyle( CWindowGc::ENullBrush );
    
    TSize ellipse( 0, 0 );
    switch ( iCornerType )
        {
        case EWindowCorner1:
            {
            ellipse = KCORNERSIZE1;
            break;
            }
        case EWindowCorner2:
            {
            ellipse = KCORNERSIZE2;
            break;
            }
        case EWindowCorner3:
            {
            ellipse = KCORNERSIZE3;
            break;
            }
        case EWindowCorner5:
            {
            ellipse = KCORNERSIZE4;
            break;
            }   
        default:
            {
            break;
            }
        }
    gc.DrawRoundRect( iRect, ellipse );
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::SetRect
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuBorder::SetRect( const TRect& aRect )
    {
    FUNC_LOG;
    iRect = aRect;
    CCoeControl::SetRect( iRect );
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::SetCornerType
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuBorder::SetCornerType( TCornerType aCornerType )
    {
    FUNC_LOG;
    iCornerType = aCornerType;
    }  

// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::SetColor
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuBorder::SetColor( TRgb aColor )
    {
    FUNC_LOG;
    iColor = aColor;
    }  
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::CFscContactActionMenuBorder
// ---------------------------------------------------------------------------
//
CFscContactActionMenuBorder::CFscContactActionMenuBorder(
    const TRect& aRect, TCornerType aCornerType )
    : iRect( aRect ), iCornerType( aCornerType ), iColor( KRgbGray )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuBorder::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuBorder::ConstructL()
    {
    FUNC_LOG;
    CCoeControl::SetRect( iRect );
      
    SetNonFocusing();
    MakeVisible( ETrue );
    ActivateL();
    }

