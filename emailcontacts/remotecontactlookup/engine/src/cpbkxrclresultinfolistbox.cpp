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
* Description:  Definition of the class CPbkxRclResultInfoListBox.
*
*/


#include "emailtrace.h"
#include <AknsUtils.h>
#include <eikfrlbd.h>

#include "cpbkxrclresultinfolistbox.h"

////////////////////////////////////////////////////////////////////////////
// CPbkxRclResultInfoListBox
////////////////////////////////////////////////////////////////////////////


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoListBox::CPbkxRclResultInfoListBox
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoListBox::CPbkxRclResultInfoListBox() :
    CAknDoubleStyleListBox()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoListBox::~CPbkxRclResultInfoListBox
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoListBox::~CPbkxRclResultInfoListBox()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoListBox::CreateItemDrawerL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoListBox::CreateItemDrawerL()
    {
    FUNC_LOG;
    CFormattedCellListBoxData* data = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( data );
    iItemDrawer = new ( ELeave ) CPbkxRclResultInfoItemDrawer(
        Model(),
        iEikonEnv->NormalFont(),
        data );
    data->SetSkinEnabledL( ETrue );
    CleanupStack::Pop( data );
    }


////////////////////////////////////////////////////////////////////////////
// CPbkxRclResultInfoItemDrawer
////////////////////////////////////////////////////////////////////////////


// ---------------------------------------------------------------------------
// CPbkxRclResultInfoItemDrawer::CPbkxRclResultInfoItemDrawer
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoItemDrawer::CPbkxRclResultInfoItemDrawer(
    MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CFormattedCellListBoxData* aFormattedCellData ) :
    CFormattedCellListBoxItemDrawer( 
        aTextListBoxModel, 
        aFont, 
        aFormattedCellData )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CPbkxRclResultInfoItemDrawer::~CPbkxRclResultInfoItemDrawer
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoItemDrawer::~CPbkxRclResultInfoItemDrawer()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoItemDrawer::SetHighlightColor
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoItemDrawer::SetHighlightColor( TRgb aColor )
    {
    FUNC_LOG;
    iHighlightColor = aColor;
    iColorsSet = iColorsSet | EHighlighColorSet;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoItemDrawer::SetColor
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoItemDrawer::SetColor( TRgb aColor )
    {
    FUNC_LOG;
    iColor = aColor;
    iColorsSet = iColorsSet | EColorSet;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoItemDrawer::ResetColors
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoItemDrawer::ResetColors( TBool aHighlightColor )
    {
    FUNC_LOG;
    if ( aHighlightColor )
        {
        iColorsSet = iColorsSet & ( ~EHighlighColorSet );
        }
    else
        {
        iColorsSet = iColorsSet & ( ~EColorSet );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoItemDrawer::DrawItemText(
    TInt aItemIndex,
    const TRect& aItemTextRect,
    TBool aItemIsCurrent,
    TBool aViewIsEmphasized,
    TBool aItemIsSelected ) const
    {
    FUNC_LOG;

    // force our own colors to itemdrawer
    CPbkxRclResultInfoItemDrawer* ptr = 
        const_cast<CPbkxRclResultInfoItemDrawer*>( this );
    
    if ( iColorsSet & EColorSet )
        {
        ptr->iTextColor = iColor;
        }

    if ( iColorsSet & EHighlighColorSet )
        {
        ptr->iHighlightedTextColor = iHighlightColor;
        }

    // Disable AVKON skinning so that our own color definitions apply
    TBool skinEnabled = AknsUtils::AvkonSkinEnabled();
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( EFalse ) );
    CFormattedCellListBoxItemDrawer::DrawItemText(
        aItemIndex,
        aItemTextRect,
        aItemIsCurrent,
        aViewIsEmphasized,
        aItemIsSelected );
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );

    }

