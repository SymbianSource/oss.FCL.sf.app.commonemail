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
* Description:  MR grid impl.
*
*/

#include "cmrgrid.h"

#include "nmrbitmapmanager.h"
#include "nmrlayoutmanager.h"
#include "esmrhelper.h"

#include <stringloader.h>
#include <akniconarray.h>
#include <akngrid.h>
#include <akngridm.h>
#include <aknlists.h>
#include <aknutils.h>
#include <aknpopup.h>
#include <avkon.hrh>
#include <e32base.h>
#include <gulicon.h>
#include <aknlayout2scalabledef.h> 
#include <esmrgui.mbg>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>

// DEBUG
#include "emailtrace.h"

namespace { // codescanner::namespace

//CONSTANTS
const TInt KColumns( 3 );
const TInt KRows( 2 );

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRGrid::CMRGrid()
// ---------------------------------------------------------------------------
//
CMRGrid::CMRGrid()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRGrid::NewL
// ---------------------------------------------------------------------------
//
TBool CMRGrid::ExecuteL( TInt& aSelectedOption )
    {
    FUNC_LOG;
    
    CMRGrid* self = new( ELeave )CMRGrid();
    CleanupStack::PushL ( self );
    TBool ret = self->ShowGridL( aSelectedOption );
    CleanupStack::PopAndDestroy( self );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMRGrid::~CMRGrid
// ---------------------------------------------------------------------------
//
CMRGrid::~CMRGrid( )
    {
    FUNC_LOG;
    delete iGrid;
    }

// ---------------------------------------------------------------------------
// CMRGrid::AddGridIconsL
// ---------------------------------------------------------------------------
//
void CMRGrid::AddGridIconsL()
    {
    FUNC_LOG;
    
    CArrayPtr<CGulIcon>* iconArray = new( ELeave ) CAknIconArray( 1 );
    CleanupStack::PushL( iconArray );

    /* 
     * 1. Item: Image
     */
    CFbsBitmap* imageBitmap = NULL;
    CFbsBitmap* imageBitmapMask = NULL;
    GetSkinBasedBitmapLC( NMRBitmapManager::EMRBitmapAttachmentTypeImage,
                                            imageBitmap,
                                            imageBitmapMask,
                                            TSize( 100, 100 ) );

    CGulIcon* imageIcon = CGulIcon::NewL( imageBitmap, imageBitmapMask );
    CleanupStack::Pop( 2 ); // imageBitmap, imageBitmapMask
    CleanupStack::PushL( imageIcon );
    iconArray->AppendL( imageIcon );
    CleanupStack::Pop( imageIcon );

    /* 
     * 2. Item: Video clip
     */
    CFbsBitmap* videoClipBitmap = NULL;
    CFbsBitmap* videoClipBitmapMask = NULL;
    GetSkinBasedBitmapLC( NMRBitmapManager::EMRBitmapAttachmentTypeVideo,
                                            videoClipBitmap,
                                            videoClipBitmapMask,
                                            TSize( 100, 100 ) );

    CGulIcon* videoClipIcon = CGulIcon::NewL( videoClipBitmap, videoClipBitmapMask );
    CleanupStack::Pop( 2 ); // videoClipBitmap, videoClipBitmapMask
    CleanupStack::PushL( videoClipIcon );
    iconArray->AppendL( videoClipIcon );
    CleanupStack::Pop( videoClipIcon );

    /* 
     * 3. Item: Sound clip
     */
    CFbsBitmap* soundClipBitmap = NULL;
    CFbsBitmap* soundClipBitmapMask = NULL;
    GetSkinBasedBitmapLC( NMRBitmapManager::EMRBitmapAttachmentTypeMusic,
                                            soundClipBitmap,
                                            soundClipBitmapMask,
                                            TSize( 100, 100 ) );

    CGulIcon* soundClipIcon = CGulIcon::NewL( soundClipBitmap, soundClipBitmapMask );
    CleanupStack::Pop( 2 ); // soundClipBitmap, soundClipBitmapMask
    CleanupStack::PushL( soundClipIcon );
    iconArray->AppendL( soundClipIcon );
    CleanupStack::Pop( soundClipIcon );

    /* 
     * 4. Item: Note
     */
    CFbsBitmap* noteBitmap = NULL;
    CFbsBitmap* noteBitmapMask = NULL;
    GetSkinBasedBitmapLC( NMRBitmapManager::EMRBitmapAttachmentTypeNote,
                                            noteBitmap,
                                            noteBitmapMask,
                                            TSize( 100, 100 ) );

    CGulIcon* noteIcon = CGulIcon::NewL( noteBitmap, noteBitmapMask );
    CleanupStack::Pop( 2 ); // noteBitmap, noteBitmapMask
    CleanupStack::PushL( noteIcon );
    iconArray->AppendL( noteIcon );
    CleanupStack::Pop( noteIcon );
    
    /* 
     * 5. Item: Other
     */
    CFbsBitmap* otherBitmap = NULL;
    CFbsBitmap* otherBitmapMask = NULL;
    GetSkinBasedBitmapLC( NMRBitmapManager::EMRBitmapAttachmentTypeOther,
                                            otherBitmap,
                                            otherBitmapMask,
                                            TSize( 100, 100 ) );

    CGulIcon* otherIcon = CGulIcon::NewL( otherBitmap, otherBitmapMask );
    CleanupStack::Pop( 2 ); // otherBitmap, otherBitmapMask
    CleanupStack::PushL( otherIcon );
    iconArray->AppendL( otherIcon );
    CleanupStack::Pop( otherIcon );

    iGrid->ItemDrawer()->ColumnData()->SetIconArray( iconArray );

    CleanupStack::Pop( iconArray );
    iGrid->HandleItemAdditionL();
    }

// ---------------------------------------------------------------------------
// CMRGrid::ShowGridL
// ---------------------------------------------------------------------------
//
TBool CMRGrid::ShowGridL( TInt& aSelectedOption )
    {
    FUNC_LOG;
    TRect mainPaneRect( 0, 0, 0, 0 );
    AknLayoutUtils::LayoutMetricsRect( 
            AknLayoutUtils::EMainPane, mainPaneRect );
    
    TAknLayoutRect gridLayoutRect =
        NMRLayoutManager::GetLayoutRect( 
                mainPaneRect, 
                NMRLayoutManager::EMRLayoutAttachmentSelectionGrid );
    
    TRect gridRect( gridLayoutRect.Rect() );
    
    TAknLayoutRect cellLayoutRect =
           NMRLayoutManager::GetLayoutRect( 
                   gridRect, 
                   NMRLayoutManager::EMRLayoutAttachmentSelectionGridCell );
        
    TRect cellRect( cellLayoutRect.Rect() );
    cellRect.Move( -gridRect.iTl);    
    
    iGrid = new( ELeave ) CAknGrid;
    CAknPopupList* gridList =  CAknPopupList::NewL( 
            iGrid, R_AVKON_SOFTKEYS_SELECT_BACK );
    CleanupStack::PushL( gridList );
       
    // Load query heading
    HBufC* buf( StringLoader::LoadLC( 
            R_QTN_MEET_REQ_QUERY_HEADING_SELECT_ATTACHMENT_TYPE ) );
    gridList->SetTitleL( *buf );
    CleanupStack::PopAndDestroy( buf );

    iGrid->ConstructL( gridList, EAknListBoxMenuGrid );
    iGrid->SetLayoutL( 
            EFalse, ETrue, ETrue, KColumns, KRows, cellRect.Size() );
    iGrid->SetPrimaryScrollingType( 
            CAknGridView::EScrollIncrementLineAndLoops );
    iGrid->SetSecondaryScrollingType( 
            CAknGridView::EScrollIncrementLineAndLoops );

    iGrid->ScrollBarFrame()->SetScrollBarVisibilityL( 
            CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EOff );
    
    CDesCArray *gridItemArray = static_cast<CDesCArray*>( 
            iGrid->Model()->ItemTextArray() );
    
    // The order of the items should be in line with 
    // CMRAttachmentUi::ResolveAttachmentTypeL switch case items.
    // TODO: update texts to be read from resource
    HBufC* textImage = StringLoader::LoadLC( 
            R_QTN_MEET_REQ_ATTACHMENT_TYPE_IMAGE );
    gridItemArray->AppendL( _L("0\tImage") /*textImage*/ );
    CleanupStack::PopAndDestroy( textImage );
    
    HBufC* textVideo = StringLoader::LoadLC( 
            R_QTN_MEET_REQ_ATTACHMENT_TYPE_VIDEO );
    gridItemArray->AppendL( _L("1\tVideo clip") /*textVideo*/ );
    CleanupStack::PopAndDestroy( textVideo );
    
    HBufC* textSound = StringLoader::LoadLC( 
            R_QTN_MEET_REQ_ATTACHMENT_TYPE_SOUNDCLIP );
    gridItemArray->AppendL( _L("2\tSound clip") /*textSound*/ );
    CleanupStack::PopAndDestroy( textSound );

    HBufC* textNote = StringLoader::LoadLC( 
            R_QTN_MEET_REQ_ATTACHMENT_TYPE_NOTE );
    gridItemArray->AppendL( _L("3\tNote") /*textNote*/ );
    CleanupStack::PopAndDestroy( textNote );
    
    HBufC* textOther = StringLoader::LoadLC( 
            R_QTN_MEET_REQ_ATTACHMENT_TYPE_NOTE );
    gridItemArray->AppendL( _L("4\tOther") /*textOther*/ );
    CleanupStack::PopAndDestroy( textOther );

    iGrid->HandleItemAdditionL();

    AddGridIconsL();
    
    TAknLayoutRect graphicLayoutRect =
           NMRLayoutManager::GetLayoutRect( 
                   cellRect, 
                   NMRLayoutManager::
                   EMRLayoutAttachmentSelectionGridCellGraphic );
    
    TRect graphicRect( graphicLayoutRect.Rect() );

    // Setup grid graphics
    AknListBoxLayouts::SetupFormGfxCell( 
            *iGrid, iGrid->ItemDrawer(), 
             0 /*Column index*/,
             graphicRect.iTl.iX /*Left pos*/, 
             graphicRect.iTl.iY /*Top pos*/,
             0 /*unused*/, 
             0 /*unused*/, 
             graphicRect.Width() /*Icon width*/, 
             graphicRect.Height() /*Icon height*/,
             graphicRect.iTl /*Start pos*/, 
             graphicRect.iBr /*End pos*/ );
    
    TAknLayoutText gridTextLayout( 
            NMRLayoutManager::GetLayoutText( 
                    cellRect, 
                    NMRLayoutManager::
                    EMRTextLayoutAttachmentSelectionGridCellText ) );
    
    TRect textRect( gridTextLayout.TextRect() );

    // Calculate the text baseline
    TInt baseLine( cellRect.iBr.iY - ( cellRect.iBr.iY - textRect.iBr.iY ) );
    // Reduce font's descent -> Baseline calculated
    baseLine -= gridTextLayout.Font()->DescentInPixels();
    
    // Setup grid texts
    AknListBoxLayouts::SetupFormTextCell( 
            *iGrid, iGrid->ItemDrawer(), 
             1 /*Column index*/,
             gridTextLayout.Font() /*Font type*/,
             gridTextLayout.Color().Value() /*color*/,
             0 /*Left margin*/, 
             0 /*unused*/,
             baseLine/*Baseline*/, 
             0 /*Text width*/,
             gridTextLayout.Align() /*Text alignment*/,
             textRect.iTl /*Start pos*/,
             textRect.iBr /*End pos*/);
    
    TBool popupOk = gridList->ExecuteLD();
    aSelectedOption = iGrid->CurrentDataIndex();
    CleanupStack::Pop( gridList );
    
    return popupOk;
    }

//EOF
