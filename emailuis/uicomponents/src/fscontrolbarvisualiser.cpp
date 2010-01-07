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
* Description:  Visualiser class for Control Bar component.
*
*/


//<cmail> SF
#include "emailtrace.h"
#include <alf/alfdecklayout.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfimagevisual.h>
#include <alf/alfborderbrush.h>
#include <alf/alfframebrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfbrusharray.h>
//</cmail>
#include <AknUtils.h>

#include "fslayoutmanager.h"
#include "fscontrolbarvisualiser.h"
#include "fscontrolbar.h"
#include "fscontrolbarmodel.h"
#include "fscontrolbutton.h"
#include "fscontrolbuttonvisualiser.h"
#include "fscontrolbuttonmodel.h"
#include "fsgenericpanic.h"

#define DEFAULT_SELECTOR KAknsIIDQsnFrList

const TReal KFsSelectorOpacity = 1;
const TInt KFsSelectorPaddingTop = 0;
const TInt KFsSelectorPaddingBottom = 0;
const TInt KFsSelectorPaddingLeft = 0;
const TInt KFsSelectorPaddingRight = 0;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFsControlBarVisualiser::CFsControlBarVisualiser(
    CFsControlBar& aParentControl,
    CFsControlBarModel& aModel ) :
    iModel( aModel ),
    iParent( aParentControl ),
    iLastSelectedButton( -1 )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::ConstructL()
    {
    FUNC_LOG;
    // creating layout and visual controls
    iBarLayout = CAlfDeckLayout::AddNewL( iParent );
    iBarLayout->EnableBrushesL();
    iBarLayout->SetFlags( EAlfVisualFlagLayoutUpdateNotification |
            EAlfVisualFlagManualPosition |
            EAlfVisualFlagManualSize );
    iBarLayout->SetSize( iModel.Size() );
    iBarLayout->SetClipping( ETrue );

    // controlbar background color
    CAlfEnv& env( iParent.Env() );
    iBgColor = CAlfGradientBrush::NewL( env );
    iBgColor->SetColor( iModel.BarBgColor() );
    iBarLayout->Brushes()->AppendL( iBgColor, EAlfDoesNotHaveOwnership );

    // Background image
    iBgLayout = CAlfLayout::AddNewL( iParent, iBarLayout );
    iBgLayout->EnableBrushesL();

    TRect parentRect( TPoint( 0, 0 ), iBarLayout->Size().Target() );
    TRect layoutRect( 0, 0, 0, 0 );

    // Adjust the background image size and position.
    CFsLayoutManager::LayoutMetricsRect( parentRect,
        CFsLayoutManager::EFsLmBgSpFsCtrlbarPane, layoutRect,
        0 );
    iBgLayout->SetSize( layoutRect.Size() );
    iBgLayout->SetPos( layoutRect.iTl );

    // Use image from skin.
    iDefaultBgBrush = CAlfFrameBrush::NewL( env, KAknsIIDQsnFrStatusFlat );
    TSize size(
        iBarLayout->Size().Target().iX, iBarLayout->Size().Target().iY );
    layoutRect = size;

    //<CMAIL> As a device user, I want Message List items to follow platform layouts to be consistent with other apps

    /*
    REMOVED: Not needed

    TRect childRect( layoutRect );
    // LAF values needed
    childRect.Shrink( 4, 4 );
    iDefaultBgBrush->SetFrameRectsL( childRect, layoutRect );
    */
    //</CMAIL>

    iBgLayout->Brushes()->AppendL(
        iDefaultBgBrush, EAlfDoesNotHaveOwnership );
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsControlBarVisualiser* CFsControlBarVisualiser::NewL(
    CFsControlBar& aParentControl,
    CFsControlBarModel& aModel )
    {
    FUNC_LOG;
    CFsControlBarVisualiser* self =
        new (ELeave) CFsControlBarVisualiser( aParentControl, aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFsControlBarVisualiser::~CFsControlBarVisualiser()
    {
    FUNC_LOG;
    ClearBackgroundImage();
    delete iBgColor;
    delete iSelectorBrush;
    }


// ---------------------------------------------------------------------------
// Redraws control.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::RefreshL( TBool aScreenSizeChanged )
    {
    FUNC_LOG;
    TInt focusedButton( -1 );
    CFsControlButton* button = NULL;

    if ( aScreenSizeChanged )
        {
        iBarLayout->SetSize( iModel.Size() );
        }

    for( TInt i( 0 ); iModel.Count() > i; i++ )
        {
        button = iModel.ButtonByIndex( i );

        // draw only visible buttons
        if ( button->IsVisible() )
            {
            if ( button->IsFocused() )
                {
                focusedButton = i;
                }

            // size of bar changed -> update buttons
            button->Visualiser()->Refresh();
            }
        }

    if( -1 != focusedButton )
        {
        DrawSelectorL( focusedButton, aScreenSizeChanged );
        }
    else
        {
        HideSelector();
        }
    }


// ---------------------------------------------------------------------------
// Sets background color for controlbar.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::SetBackgroundColor( const TRgb& aColor )
    {
    FUNC_LOG;
    // set color
    iBgColor->SetColor( aColor );
    iBgColor->SetOpacity( 1 );
    }


// ---------------------------------------------------------------------------
// Clears controlbar's background color. Controlbar becomes transparent.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsControlBarVisualiser::ClearBackgroundColor()
    {
    FUNC_LOG;
    iBgColor->SetOpacity( 0 );
    }


// ---------------------------------------------------------------------------
// Sets background image for controlbar.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::SetBackgroundImageL( CAlfTexture& aImage )
    {
    FUNC_LOG;
    if ( iDefaultBgBrush )
        {
        ClearBackgroundImage();
        }

    if ( !iBgImage )
        {
        CAlfEnv& env( iParent.Env() );
        iBgImage = CAlfImageBrush::NewL( env, TAlfImage( aImage ) );
        iBgLayout->Brushes()->AppendL( iBgImage, EAlfDoesNotHaveOwnership );
        }
    else
        {
        iBgImage->SetImage( TAlfImage( aImage ) );
        }
    }


// ---------------------------------------------------------------------------
// Clears background image for controlbar.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::ClearBackgroundImage()
    {
    FUNC_LOG;
    if ( iDefaultBgBrush )
        {
        for ( TInt i( iBarLayout->Brushes()->Count() - 1 ); 0 <= i; i-- )
            {
            if ( &iBgLayout->Brushes()->At( i ) == iDefaultBgBrush )
                {
                iBgLayout->Brushes()->Remove( i );
                delete iDefaultBgBrush;
                iDefaultBgBrush = NULL;
                break;
                }
            }
        }
    if ( iBgImage )
        {
        for ( TInt i( iBarLayout->Brushes()->Count() - 1 ); 0 <= i; i-- )
            {
            if ( &iBgLayout->Brushes()->At( i ) == iBgImage )
                {
                iBgLayout->Brushes()->Remove( i );
                delete iBgImage;
                iBgImage = NULL;
                break;
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// Update the size of the controlbar layout.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::UpdateSizeL()
    {
    FUNC_LOG;
    iBarLayout->SetSize( iModel.Size() );
    iBarLayout->SetPos( iModel.Pos() );
    RefreshL();
    }


// ---------------------------------------------------------------------------
// Retrieves controlbar's layout.
// ---------------------------------------------------------------------------
//
CAlfDeckLayout* CFsControlBarVisualiser::Layout()
    {
    FUNC_LOG;
    return iBarLayout;
    }


// ---------------------------------------------------------------------------
// Sets transition time for selector.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::SetSelectorTransitionTimeL(
    TInt aTransitionTime )
    {
    FUNC_LOG;
    iModel.SetSelectorTransitionTime( aTransitionTime );
    RefreshL();
    }


// ---------------------------------------------------------------------------
// Change the selector image.
// Ownership of the brush is gained.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::SetSelectorImageL(
    CAlfBrush* aSelectorBrush,
    TReal32 aOpacity )
    {
    FUNC_LOG;
    // Update the selector if it is visible.
    if ( iSelector )
        {
        if ( iSelector->Brushes()->Count() )
            {
            iSelector->Brushes()->Remove( iSelector->Brushes()->Count() - 1 );
            }
        iSelector->Brushes()->AppendL(
            aSelectorBrush, EAlfDoesNotHaveOwnership );
        iSelector->SetOpacity( aOpacity );
        }

    // Release the old selector brush.
    delete iSelectorBrush;
    iSelectorBrush = aSelectorBrush;
    iSelectorOpacity = aOpacity;
    }


// ---------------------------------------------------------------------------
// Reorder the visuals. Button visual are build from two separate visual,
// background and content. Those are arranged so that the background visual
// are set at back and contents are to front. Selector visual is placed
// between them.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::ReorderVisuals()
    {
    FUNC_LOG;
    if ( iSelector )
        {
        TInt visualOrder( KErrNotFound );
        if ( KErrNotFound == visualOrder )
            {
            visualOrder = iBarLayout->FindVisual( iBgLayout );
            }

        // Reorder the buttons.
        visualOrder++;
        for ( TInt i( 0 ); iModel.Count() > i; i++ )
            {
            // Reorder visual so that the button background visuals get to
            // bottom and their contents get to the front. Selector will be
            // places between them.
            CFsControlButtonVisualiser* button( NULL );
            button = iModel.ButtonByIndex( i )->Visualiser();
            if ( button->IsVisible() )
                {
                iBarLayout->MoveVisualToFront( *button->ContentLayout() );
                iBarLayout->Reorder( *button->Layout(), visualOrder );
                visualOrder++;
                }
            }

        // Move the selector after every button's background visual.
        iBarLayout->Reorder( *iSelector, visualOrder );
        }
    }

// ---------------------------------------------------------------------------
// Hides\shows selector rectangle over selected button.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::MakeSelectorVisible(
        TBool aShow, TBool aFromTouch )
    {
    FUNC_LOG;
    TReal32 opacity( 0 );
    if( aShow )
        {
        opacity = KFsSelectorOpacity;
        }
    if( iSelector )
        {
        iTouchPressed = aFromTouch;
        CFsControlButton* button = NULL;
        const TInt buttonCount( iModel.Count() );
        for( TInt buttonIndex( 0 ); buttonIndex < buttonCount; ++buttonIndex )
            {
            button = iModel.ButtonByIndex( buttonIndex );
            if( button->IsFocused() )
                {
                button->MakeFocusVisible( aShow );
                break;
                }
            }
        iSelector->SetOpacity( opacity );
        }
    iSelectorOpacity = opacity;
    }

// ---------------------------------------------------------------------------
// Draws selector rectangle over selected button.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::DrawSelectorL(
    const TInt aSelectedButtonIndex,
    TBool aFastDraw )
    {
    FUNC_LOG;
    CFsControlButtonVisualiser* vis = NULL;

    // set selector's delay
    TInt transitionTime( aFastDraw ? 0 : iModel.SelectorTransitionTime() );
    TBool setDefaultBrushBorders( EFalse );

    if ( !iSelector )
        {
        // Flags used because otherwise it would generate a callback here
        // before the image visual is fully created.
        iBarLayout->SetFlag( EAlfVisualFlagFreezeLayout );
        iSelector = iParent.AppendVisualL( EAlfVisualTypeVisual, iBarLayout );

        ReorderVisuals();

        iBarLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
        iSelector->SetFlag( EAlfVisualFlagManualPosition );
        iSelector->SetFlag( EAlfVisualFlagManualSize );

        iSelector->EnableBrushesL();

        if ( !iSelectorBrush )
            {
            // Use the default selector brush.
            CAlfFrameBrush* brush(
                CAlfFrameBrush::NewL( iParent.Env(), KAknsIIDQsnFrList ) );
            // Ownership of the brush is transfered.
            SetSelectorImageL( brush, KFsSelectorOpacity );
            // Brush rects are set later.
            setDefaultBrushBorders = ETrue;
            }
        else
            {
            // Use the custom selector brush.
            iSelector->Brushes()->AppendL(
                iSelectorBrush, EAlfDoesNotHaveOwnership );
            iSelector->SetOpacity( iSelectorOpacity );
            }

        // When making the selector visible then do it without any delays.
        transitionTime = 0;
        }

    // retieve button's visualiser
    vis = iModel.ButtonByIndex( aSelectedButtonIndex )->Visualiser();

    // retieve button's pos and size
    TPoint point( vis->Pos().iX.Target(), vis->Pos().iY.Target() );
    TSize size( vis->Size().iX.Target(), vis->Size().iY.Target() );
    point.operator-=(
        TPoint( KFsSelectorPaddingLeft, KFsSelectorPaddingTop ) );
    size += TSize( KFsSelectorPaddingLeft + KFsSelectorPaddingRight,
        KFsSelectorPaddingTop + KFsSelectorPaddingBottom );

    // Hides transition when item selected via touch
    if( iTouchPressed )
        {
        transitionTime = 0;
        iTouchPressed = EFalse;
        }

    iLastSelectedButton = aSelectedButtonIndex;
    // set new pos and size of selector
    iSelector->SetPos( TAlfRealPoint( point ), transitionTime );
    iSelector->SetSize( TAlfRealSize( size ), transitionTime );

    // Setting the selector skin to fit the selector visual size.
    if ( setDefaultBrushBorders )
        {
        CAlfBrush& brush(
            iSelector->Brushes()->At( iSelector->Brushes()->Count() - 1 ) );

        //<CMAIL> As a device user, I want Message List items to follow platform layouts to be consistent with other apps

        /*

        CAlfFrameBrush* defaultSelectorBrush(
            static_cast<CAlfFrameBrush*>( &brush ) );
        TRect layoutRect( size );


        REMOVED: Not needed

        TRect childRect( layoutRect );
        // LAF values needed
        childRect.Shrink( 1, 1 );
        defaultSelectorBrush->SetFrameRectsL( childRect, layoutRect );
        */
        //</CMAIL>

        }
    }


// ---------------------------------------------------------------------------
// Hides selector rectangle.
// ---------------------------------------------------------------------------
//
void CFsControlBarVisualiser::HideSelector()
    {
    FUNC_LOG;
    iLastSelectedButton = KErrNotFound;
    if ( iSelector )
        {
        // Flags used because otherwise it would generate a callback here
        // before the visual is fully destroyed.
        iBarLayout->SetFlag( EAlfVisualFlagFreezeLayout );
        iSelector->RemoveAndDestroyAllD();
        iSelector = NULL;
        iBarLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
        iBarLayout->UpdateChildrenLayout();
        }
    }

