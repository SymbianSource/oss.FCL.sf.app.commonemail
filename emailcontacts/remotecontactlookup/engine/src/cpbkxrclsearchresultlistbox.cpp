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
* Description:  Definition of the class CPbkxRclSearchResultListBox.
*
*/


#include "emailtrace.h"
#include <AknsUtils.h>
#include <eikclbd.h>

#include "cpbkxrclsearchresultlistbox.h"

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultListBox
////////////////////////////////////////////////////////////////////////////


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::CPbkxRclSearchResultListBox
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultListBox::CPbkxRclSearchResultListBox() :
    CAknSingleStyleListBox()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::~CPbkxRclSearchResultListBox
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultListBox::~CPbkxRclSearchResultListBox()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::CreateItemDrawerL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultListBox::CreateItemDrawerL()
    {
    FUNC_LOG;
    CColumnListBoxData* data = CColumnListBoxData::NewL();
    CleanupStack::PushL( data );
    iItemDrawer = new ( ELeave ) CPbkxRclSearchResultItemDrawer(
        Model(),
        iEikonEnv->NormalFont(),
        data );
    data->SetSkinEnabledL( ETrue );
    CleanupStack::Pop( data );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListBox::MakeViewClassInstanceL
// ---------------------------------------------------------------------------
//
CListBoxView* CPbkxRclSearchResultListBox::MakeViewClassInstanceL()
    {
    return ( new (ELeave) CPbkxRclSearchResultListView() );
    }

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultItemDrawer
////////////////////////////////////////////////////////////////////////////


// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::CPbkxRclSearchResultItemDrawer
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultItemDrawer::CPbkxRclSearchResultItemDrawer(
    MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CColumnListBoxData* aColumnData ) :
    CColumnListBoxItemDrawer( aTextListBoxModel, aFont, aColumnData )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::~CPbkxRclSearchResultItemDrawer
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultItemDrawer::~CPbkxRclSearchResultItemDrawer()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::SetHighlightColor
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::SetHighlightColor( TRgb aColor )
    {
    FUNC_LOG;
    iHighlightColor = aColor;
    iColorsSet = iColorsSet | EHighlighColorSet;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::SetColor
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::SetColor( TRgb aColor )
    {
    FUNC_LOG;
    iColor = aColor;
    iColorsSet = iColorsSet | EColorSet;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultItemDrawer::ResetColors
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::ResetColors( TBool aHighlightColor )
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
// CPbkxRclSearchResultItemDrawer::DrawItemText
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultItemDrawer::DrawItemText(
    TInt aItemIndex,
    const TRect& aItemTextRect,
    TBool aItemIsCurrent,
    TBool aViewIsEmphasized,
    TBool aItemIsSelected ) const
    {
    FUNC_LOG;

    // force our own text colors to item drawer
    CPbkxRclSearchResultItemDrawer* ptr = 
        const_cast<CPbkxRclSearchResultItemDrawer*>( this );
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
    CColumnListBoxItemDrawer::DrawItemText(
        aItemIndex,
        aItemTextRect,
        aItemIsCurrent,
        aViewIsEmphasized,
        aItemIsSelected );
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );
    }

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultListView
////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultListView::DrawEmptyList
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultListView::DrawEmptyList(const TRect &aClientRect) const
    {
    // Disable AVKON skinning so that our own color definitions apply
    TBool skinEnabled = AknsUtils::AvkonSkinEnabled();
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( EFalse ) );
    CAknColumnListBoxView::DrawEmptyList( aClientRect );
    // Safe to ignore error
    TRAP_IGNORE( AknsUtils::SetAvkonSkinEnabledL( skinEnabled ) );
    }

