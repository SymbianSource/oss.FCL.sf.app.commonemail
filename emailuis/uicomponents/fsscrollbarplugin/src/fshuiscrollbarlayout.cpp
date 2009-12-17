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
* Description:  Implementation on scrollbar layout.
*
*/


//<cmail> SF
#include "emailtrace.h"
#include <uiacceltk/hitchcock.h>
//</cmail>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include "fshuiscrollbarlayout.h"
#include "fslayoutmanager.h"

const TInt KFsScrollbarTextureIdPoolStart = 1000;

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Resolve the greater values from both size arguments.
// ---------------------------------------------------------------------------
//
TSize ExtendSize( const TSize& aSize, const TSize& aSize2 )
    {
    return TSize(
        aSize.iWidth < aSize2.iWidth ? aSize2.iWidth : aSize.iWidth,
        aSize.iHeight < aSize2.iHeight ? aSize2.iHeight : aSize.iHeight );
    }


// ---------------------------------------------------------------------------
// Resolve the greatest size from scrollbar gridlayouts.
// ---------------------------------------------------------------------------
//
TSize GetMaxImageSize(
    CHuiLayout* aLayout1,
    CHuiLayout* aLayout2,
    CHuiLayout* aLayout3 )
    {
    TSize retVal( static_cast<CHuiImageVisual*>(
        &aLayout1->Visual( 0 ) )->Image().Texture().Size() );
    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout1->Visual( 1 ) )->Image().Texture().Size() );
    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout1->Visual( 2 ) )->Image().Texture().Size() );

    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout2->Visual( 0 ) )->Image().Texture().Size() );
    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout2->Visual( 1 ) )->Image().Texture().Size() );
    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout2->Visual( 2 ) )->Image().Texture().Size() );

    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout3->Visual( 0 ) )->Image().Texture().Size() );
    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout3->Visual( 1 ) )->Image().Texture().Size() );
    retVal = ExtendSize( retVal, static_cast<CHuiImageVisual*>(
        &aLayout3->Visual( 2 ) )->Image().Texture().Size() );

    return retVal;
    }


// ---------------------------------------------------------------------------
// Creates a new bitmap based on the given source bitmap. New bitmap will be
// rotated 90 degrees agains clock direction if wanted.
// ---------------------------------------------------------------------------
//
CFbsBitmap* CreateBitmapL( CFbsBitmap* aSourceBitmap, TBool aRotated )
    {
    FUNC_LOG;
    CFbsBitmap* destinationBitmap( new( ELeave )CFbsBitmap() );
    CleanupStack::PushL( destinationBitmap );

    TSize sourceBitmapSize( aSourceBitmap->SizeInPixels() );

    TSize destinationBitmapSize( 0, 0 );
    if ( aRotated )
        {
        destinationBitmapSize =
            TSize( sourceBitmapSize.iHeight, sourceBitmapSize.iWidth );
        }
    else
        {
        destinationBitmapSize =
            TSize( sourceBitmapSize.iWidth, sourceBitmapSize.iHeight );
        }

    User::LeaveIfError( destinationBitmap->Create(
        destinationBitmapSize, aSourceBitmap->DisplayMode() ) );

    CFbsBitmapDevice* destinationDevice(
        CFbsBitmapDevice::NewL( destinationBitmap ) );
    CleanupStack::PushL( destinationDevice );

    CFbsBitGc* destinationGc;
    User::LeaveIfError( destinationDevice->CreateContext( destinationGc ) );

    if ( aRotated )
        {
        // Assume that bitmaps are small and this is called only at
        // construction. Simply just blit the pixels one by one here.
        TInt destinationX( 0 );
        TInt destinationY( 0 );

        for ( TInt sourceY( 0 ); sourceY < sourceBitmapSize.iHeight;
            sourceY++ )
            {
            for ( TInt sourceX( 0 ); sourceX < sourceBitmapSize.iWidth;
                sourceX++ )
                {
                destinationX = sourceY;
                destinationY = sourceX;
                destinationGc->BitBlt( TPoint( destinationX, destinationY ),
                    aSourceBitmap,
                    TRect( sourceX, sourceY, sourceX + 1, sourceY + 1 ) );
                }
            }
        }
    else
        {
        destinationGc->BitBlt( TPoint( 0, 0 ), aSourceBitmap );
        }

    delete destinationGc;  
    CleanupStack::PopAndDestroy( destinationDevice ); // destinationDevice
    CleanupStack::Pop( destinationBitmap ); // destinationBitmap

    return destinationBitmap;
    }


// ---------------------------------------------------------------------------
// Creates a new bitmap and a mask based on the given skin item id.  New
// bitmap will be rotated 90 degrees agains clock direction if wanted.
// ---------------------------------------------------------------------------
//
void CreateSkinnedBitmapAndMaskL( MAknsSkinInstance* aInstance, 
    const TAknsItemID& aID,
    CFbsBitmap*& aBitmap,
    CFbsBitmap*& aMask,
    TBool aRotated )
    {
    FUNC_LOG;
    CFbsBitmap* skinnedBitmap( NULL );
    CFbsBitmap* skinnedMask( NULL );

    // note, real fallback icons are not (yet) available in avkon icon file.
    TRAP_IGNORE( AknsUtils::CreateIconL(
        aInstance, aID, skinnedBitmap, skinnedMask, KNullDesC, -1, -1 ) );

    if ( !skinnedBitmap )
        {
        AknsUtils::CreateIconL(
            aInstance, aID, skinnedBitmap, KNullDesC, -1 );
        }

    TRect screenRect;     
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
    TAknWindowComponentLayout l(
        AknLayoutScalable_Avkon::scroll_bg_pane_g1() );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( screenRect, l.LayoutLine() );
    TSize itemSize( layoutRect.Rect().Size() );
    if ( itemSize.iHeight > itemSize.iWidth )
        {
        itemSize.iHeight = itemSize.iWidth; // make sure item is square
        }
    else
        {
        itemSize.iWidth = itemSize.iHeight; // make sure item is square
        }
    User::LeaveIfError( AknIconUtils::SetSize( skinnedBitmap, itemSize ) );

    if ( aRotated )
        {
        // now create rotated copy of the original bitmaps
        CFbsBitmap* bitmap( CreateBitmapL( skinnedBitmap, aRotated ) );
        CleanupStack::PushL( bitmap );

        CFbsBitmap* mask( NULL );
        if ( skinnedMask )
            {
            mask = CreateBitmapL( skinnedMask, aRotated );
            CleanupStack::PushL( mask );
            }
        if ( mask )
            {
            CAknIcon* icon( CAknIcon::NewL() );
            icon->SetBitmap( bitmap );
            icon->SetMask( mask );
            CAknIcon* resultIcon( AknIconUtils::CreateIconL( icon ) );
            CleanupStack::Pop( mask ); // mask
            
            aBitmap = resultIcon->Bitmap();
            aMask = resultIcon->Mask();
            
            resultIcon->SetBitmap( NULL );
            resultIcon->SetMask( NULL );
            delete resultIcon;
            }
        else
            {
            CFbsBitmap* resultBitmap( AknIconUtils::CreateIconL( bitmap ) );
            aBitmap = resultBitmap;
            }            

        CleanupStack::Pop( bitmap ); // bitmap

        // delete originals
        delete skinnedBitmap;
        delete skinnedMask;
        }
    else
        {
        aBitmap = skinnedBitmap;
        aMask = skinnedMask;
        }
    }


// ---------------------------------------------------------------------------
// Add s specific skin image to visual.
// ---------------------------------------------------------------------------
//
void CFsHuiScrollbarLayout::SetImageL(
    CHuiImageVisual* aVisual,
    const TAknsItemID& aID,
    TFsScrollbar aOrientation )
    {
    CFsScrollbarBitmapProvider* provider = new (ELeave) CFsScrollbarBitmapProvider( aID, aOrientation );
    iBitmapProviderStack.AppendL( provider );
    
    while ( iOwnerControl->Env().TextureManager().IsLoaded( iTextureCounter ) )
    	{
    	++iTextureCounter;
    	}
    
    CHuiTexture& texture = iOwnerControl->Env().TextureManager().CreateTextureL( iTextureCounter, provider, EHuiTextureUploadFlagDefault );
    iTextureStack.AppendL( &texture );
    texture.SetSkinContent( ETrue );

    aVisual->SetImage( texture );
    aVisual->SetScaleMode( CHuiImageVisual::EScaleFit );
    }

CFsScrollbarBitmapProvider::CFsScrollbarBitmapProvider( TAknsItemID aID, TFsScrollbar aOrientation )
	: iID( aID ), iOrientation( aOrientation )
	{
	}

CFsScrollbarBitmapProvider::~CFsScrollbarBitmapProvider()
	{
	}

void CFsScrollbarBitmapProvider::ProvideBitmapL (TInt /*aId*/, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap)
	{
	MAknsSkinInstance* skin( AknsUtils::SkinInstance() );
	CreateSkinnedBitmapAndMaskL(
		skin, iID, aBitmap, aMaskBitmap, EFsScrollbarHorizontal == iOrientation );
	}


// ======== MEMBER FUNCTIONS ========

CFsHuiScrollbarLayout::CFsHuiScrollbarLayout( MHuiVisualOwner& aOwner )
    : CHuiLayout( aOwner )
    {
    FUNC_LOG;
    }


void CFsHuiScrollbarLayout::ConstructL( CHuiControl& aOwnerControl )
    {
    FUNC_LOG;
    CHuiLayout::ConstructL();
    iHorScroll.SetHorizontal( ETrue );

    iOwnerControl = &aOwnerControl;
    iTextureCounter = KFsScrollbarTextureIdPoolStart;
    }


// ---------------------------------------------------------------------------
// Construct a new scrollbar layout and give its ownership to a control.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsHuiScrollbarLayout* CFsHuiScrollbarLayout::AddNewL(
    CHuiControl& aOwnerControl,
    CHuiLayout* aParentLayout )
    {
    FUNC_LOG;
    CFsHuiScrollbarLayout* scrollbar(
        new (ELeave) CFsHuiScrollbarLayout( aOwnerControl ) );
    CleanupStack::PushL( scrollbar );
    scrollbar->ConstructL( aOwnerControl );
    if ( aParentLayout )
        {
        aParentLayout->AppendL( scrollbar );
        }
    // Give ownership to specified owner.
    aOwnerControl.AppendL( scrollbar );
    CleanupStack::Pop( scrollbar );
    return scrollbar;
    }


CFsHuiScrollbarLayout::~CFsHuiScrollbarLayout()
    {
    FUNC_LOG;
    delete iVBarBrush;
    delete iHBarBrush;
    delete iVThumbBrush;
    delete iHThumbBrush;
    iTextureStack.ResetAndDestroy();
    iBitmapProviderStack.ResetAndDestroy();
    }


// ---------------------------------------------------------------------------
// Set the range of the scrollbar. Will not actually move the scrollbar until
// Update() is called.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetRange(
    TInt aStart,
    TInt aEnd,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    EFsScrollbarVertical == aScrollbar
        ? iVertScroll.SetRange( aStart, aEnd )
        : iHorScroll.SetRange( aStart, aEnd );
    }


// ---------------------------------------------------------------------------
// Get scrollbars minimum range value.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsHuiScrollbarLayout::RangeStart( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    return EFsScrollbarVertical == aScrollbar
        ? iVertScroll.RangeStart() : iHorScroll.RangeStart();
    }


// ---------------------------------------------------------------------------
// Get scrollbars maximum range value.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsHuiScrollbarLayout::RangeEnd( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    return EFsScrollbarVertical == aScrollbar
        ? iVertScroll.RangeEnd() : iHorScroll.RangeEnd();
    }


// ---------------------------------------------------------------------------
// Set the count of visible units at the time.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetThumbSpan(
    TInt aThumbSpan,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    EFsScrollbarVertical == aScrollbar
        ? iVertScroll.SetThumbSpan( aThumbSpan )
        : iHorScroll.SetThumbSpan( aThumbSpan );
    }


// ---------------------------------------------------------------------------
// Get the set value for visible units.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsHuiScrollbarLayout::ThumbSpan( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    return EFsScrollbarVertical == aScrollbar
        ? iVertScroll.ThumbSpan() : iHorScroll.ThumbSpan();
    }


// ---------------------------------------------------------------------------
// Adjust the vertical scrollbar's width.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetVerticalScrollbarWidth(
    TInt aWidth,
    TInt aLayoutTransitionTime )
    {
    FUNC_LOG;
    iScrollbarSize.iWidth = aWidth;
    if ( iVScrollbarLayout )
        {
        iVertScroll.SetThumbSize(
            TSize( aWidth, iVertScroll.ThumbSize().iHeight ) );
        UpdateChildrenLayout( aLayoutTransitionTime );
        }
    }


// ---------------------------------------------------------------------------
// Adjust the horizontal scrollbar's height.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetHorizontalScrollbarHeight(
    TInt aHeight,
    TInt aLayoutTransitionTime )
    {
    FUNC_LOG;
    iScrollbarSize.iHeight = aHeight;
    if ( iHScrollbarLayout )
        {
        iHorScroll.SetThumbSize(
            TSize( iHorScroll.ThumbSize().iWidth, aHeight ) );
        UpdateChildrenLayout( aLayoutTransitionTime );
        }
    }


// ---------------------------------------------------------------------------
// Set the position of the scrollbar.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetScrollbarPos(
    const TInt aPos,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    EFsScrollbarVertical == aScrollbar
        ? iVertScroll.SetPos( aPos ) : iHorScroll.SetPos( aPos );
    }


// ---------------------------------------------------------------------------
// Resolve the position of the scrollbar.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsHuiScrollbarLayout::ScrollbarPos( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    return EFsScrollbarVertical == aScrollbar
        ? iVertScroll.Pos() : iHorScroll.Pos();
    }


// ---------------------------------------------------------------------------
// Set the scrollbar visibility.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetVisibilityMode(
    TFsScrollbarVisibility aVisibility,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    EFsScrollbarVertical == aScrollbar
        ? iVertScroll.SetVisibleMode( aVisibility )
        : iHorScroll.SetVisibleMode( aVisibility );
    }


// ---------------------------------------------------------------------------
// Resolve the scrollbar visibility mode.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsScrollbarVisibility CFsHuiScrollbarLayout::VisibilityMode(
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    return EFsScrollbarVertical == aScrollbar
        ? iVertScroll.VisibilityMode() : iHorScroll.VisibilityMode();
    }


// ---------------------------------------------------------------------------
// Resolve if the specified scrollbar is visible.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsHuiScrollbarLayout::IsScrollbarVisible(
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Perform the operation on selected scrollbar.
    return EFsScrollbarVertical == aScrollbar
        ? iVertScroll.IsVisible() : iHorScroll.IsVisible();
    }


// ---------------------------------------------------------------------------
// Change the scrollbar images to custom images.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetScrollbarImagesL(
    CHuiImageBrush* aBarBrush,
    CHuiImageBrush* aThumbBrush,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    SetCustomImageL( aBarBrush, aScrollbar, EFalse );
    SetCustomImageL( aThumbBrush, aScrollbar, ETrue );
    }


// ---------------------------------------------------------------------------
// Update the visibility of scrollbars.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::Update( TInt aLayoutTransitionTime )
    {
    FUNC_LOG;
    MakeScrollbarVisible( iVertScroll.IsVisible(), EFsScrollbarVertical );
    MakeScrollbarVisible( iHorScroll.IsVisible(), EFsScrollbarHorizontal );
    UpdateChildrenLayout( aLayoutTransitionTime );
    }


// ---------------------------------------------------------------------------
// Remove reference to custom image brushes without releasing those.
// This is for cases where ownership of image brushes couldn't be tranfered to
// scrollbar layout.
// ---------------------------------------------------------------------------
//
void CFsHuiScrollbarLayout::RemoveCustomImages( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    // Remove the previously set image brushes.
    for ( TInt i( 0 ); 2 > i; i++ )
        {
        CHuiImageBrush** imageBrush;
        CHuiDeckLayout* layout;

        // Custom scrollbar is built from two image brushes. Both need to be
        // removed.
        if ( i )
            {
            layout = EFsScrollbarVertical == aScrollbar
                ? iVScrollbarLayout : iHScrollbarLayout;
            imageBrush = EFsScrollbarVertical == aScrollbar
                ? &iVBarBrush : &iHBarBrush;
            }
        else
            {
            layout = EFsScrollbarVertical == aScrollbar
                ? iVThumb : iHThumb;
            imageBrush = EFsScrollbarVertical == aScrollbar
                ? &iVThumbBrush : &iHThumbBrush;
            }

        // Remove the brush from the layout.
        if ( layout && layout->Brushes() )
            {
            for ( TInt j( layout->Brushes()->Count() - 1 ); 0 <= j; j-- )
                {
                if ( &layout->Brushes()->At( j ) == *imageBrush )
                    {
                    layout->Brushes()->Remove( j );
                    break;
                    }
                }
            }

        // Mark the brush as released. CFsAlfScrollbarLayout has the ownership
        // of this object.
        *imageBrush = NULL;
        }
    }


// ---------------------------------------------------------------------------
// Resolve the layout's size without the scrollbars.
// ---------------------------------------------------------------------------
//
EXPORT_C THuiRealPoint CFsHuiScrollbarLayout::LayoutSize()
    {
    FUNC_LOG;
    THuiRealPoint layoutSize( Size().RealTarget() );
    if ( iVertScroll.IsVisible() )
        {
        layoutSize.iX = layoutSize.iX - iScrollbarSize.iWidth;
        }
    if ( iHorScroll.IsVisible() )
        {
        layoutSize.iY = layoutSize.iY - iScrollbarSize.iWidth;
        }

    return layoutSize;
    }


// ---------------------------------------------------------------------------
// From class CHuiLayout.
// Change the size of the layout. Children's positions and sizes are updated
// accordingly. Each layout class is responsible for determining how to
// update children's layout.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::SetSize(
    const THuiRealSize& aSize,
    TInt aTransitionTime )
    {
    FUNC_LOG;
    CHuiLayout::SetSize( aSize, aTransitionTime );
    UpdateChildrenLayout( aTransitionTime );
    }


// ---------------------------------------------------------------------------
// From class CHuiLayout.
// Update the layout of one child visual.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsHuiScrollbarLayout::UpdateChildLayout(
    TInt aIndex,
    TInt aTransitionTime )
    {
    FUNC_LOG;
    CHuiVisual* child( &Visual( aIndex ) );
    if ( child == iVScrollbarLayout || child == iHScrollbarLayout )
        {
        // Update vertical or horizontal scrollbar.
        THuiRealRect rect;
        ChildRect( ChildOrdinal( aIndex ), rect );
        child->SetSize( rect.Size(), aTransitionTime );
        child->SetPos( rect.iTl, aTransitionTime );
        TSize size( rect.Size() );

        CFsScrollbarClet &scrollClet(
            child == iVScrollbarLayout ? iVertScroll : iHorScroll );
        if ( ( ( child == iVScrollbarLayout && !iVBarBrush )
            || ( child == iHScrollbarLayout && !iHBarBrush ) )
            && scrollClet.IsVisible() )
            {
            scrollClet.AdjustSize( static_cast<CHuiGridLayout*>(
                &child->Visual( 0 ) ), size, aTransitionTime );
            }
        scrollClet.Update( aTransitionTime );
        }
    else
        {
        // Update other child visuals.
        CHuiLayout::UpdateChildLayout( aIndex, aTransitionTime );
        }
    }


// ---------------------------------------------------------------------------
// From class CHuiLayout.
// Determines the size of a child visual according to the layout.
// ---------------------------------------------------------------------------
//
TBool CFsHuiScrollbarLayout::ChildSize( TInt aOrdinal, TSize& aSize )
    {
    FUNC_LOG;
    THuiRealRect rect;
    ChildRect( aOrdinal, rect );
    aSize = rect.Size();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// From class CHuiLayout.
// Determines the position of a child visual according to the layout.
// ---------------------------------------------------------------------------
//
TBool CFsHuiScrollbarLayout::ChildPos( TInt aOrdinal, TPoint& aPos )
    {
    FUNC_LOG;
    THuiRealRect rect;
    ChildRect( aOrdinal, rect );
    aPos = rect.iTl;
    return ETrue;
    }


// ---------------------------------------------------------------------------
// From class CHuiLayout.
// Update the layout of all children.
// ---------------------------------------------------------------------------
//
void CFsHuiScrollbarLayout::UpdateChildrenLayout( TInt aTransitionTime )
    {
    FUNC_LOG;
    CHuiLayout::UpdateChildrenLayout( aTransitionTime );
    }


// ---------------------------------------------------------------------------
// From class CHuiLayout.
// Determines the position and size of a child visual according to the layout.
// ---------------------------------------------------------------------------
//
TInt CFsHuiScrollbarLayout::ChildRect( TInt aOrdinal, THuiRealRect& aPos )
    {
    FUNC_LOG;
    const CHuiVisual* visual( &Visual( aOrdinal ) );
    TSize size( Size().Target().AsSize() );
    THuiRealRect area( size );

    if ( visual == iVScrollbarLayout && iVertScroll.IsVisible() )
        {
        // Size and position of vertical scrollbar.
        if ( CFsLayoutManager::IsMirrored() )
            {
            area.iBr.iX = iScrollbarSize.iWidth;
            }
        else
            {
            area.iTl.iX = area.iBr.iX - iScrollbarSize.iWidth;
            }

        if ( iHorScroll.IsVisible() )
            {
            area.iBr.iY -= iScrollbarSize.iHeight;
            }
        }
    else if ( visual == iHScrollbarLayout && iHorScroll.IsVisible() )
        {
        // Size and position of horizontal scrollbar.
        area.iTl.iY = area.iBr.iY - iScrollbarSize.iHeight;
        if ( iVertScroll.IsVisible() )
            {
            if ( CFsLayoutManager::IsMirrored() )
                {
                area.iTl.iX = iScrollbarSize.iWidth;
                }
            else
                {
                area.iBr.iX -= iScrollbarSize.iWidth;
                }
            }
        }
    else
        {
        // Other visuals size on position depends on the visible scrollbars.
        if ( iVertScroll.IsVisible() )
            {
            if ( CFsLayoutManager::IsMirrored() )
                {
                area.iTl.iX = iScrollbarSize.iWidth;
                }
            else
                {
                area.iBr.iX -= iScrollbarSize.iWidth;
                }
            }
        if ( iHorScroll.IsVisible() )
            {
            area.iBr.iY -= iScrollbarSize.iHeight;
            }
        }
    aPos.iTl = area.iTl; 
    aPos.iBr = area.iBr;
    return THuiLayoutChildRectLayoutUpdateNeeded;
    }


// ---------------------------------------------------------------------------
// Create a scrollbar visual.
// The scrollbar is only created, but not added to this layout.
// ---------------------------------------------------------------------------
//
void CFsHuiScrollbarLayout::CreateScrollbarL(
    CHuiDeckLayout*& aScrollbarLayout,
    CHuiLayout*& aThumbLayout,
    TFsScrollbar aOrientation )
    {
    FUNC_LOG;
    CHuiDeckLayout* scrollbarLayout(
        CHuiDeckLayout::AddNewL( *iOwnerControl ) );
    CleanupStack::PushL( scrollbarLayout );
    scrollbarLayout->SetClipping( ETrue );

    // Create the scrollbar's background.
    CHuiGridLayout *bg( CreateGridLayoutL(
        scrollbarLayout, KAknsIIDQsnCpScrollBgTop,
        KAknsIIDQsnCpScrollBgMiddle, KAknsIIDQsnCpScrollBgBottom,
        aOrientation ) );

    // Create the thumb.
    CHuiDeckLayout* thumbLayout(
        CHuiDeckLayout::AddNewL( *iOwnerControl, scrollbarLayout ) );
    thumbLayout->SetClipping( ETrue );

    // Background is build from two differend image sets.
    CHuiGridLayout* middle( CreateGridLayoutL( thumbLayout,
        KAknsIIDQsnCpScrollHandleBgTop, KAknsIIDQsnCpScrollHandleBgMiddle,
        KAknsIIDQsnCpScrollHandleBgBottom, aOrientation ) );
    CHuiGridLayout* top( CreateGridLayoutL( thumbLayout,
        KAknsIIDQsnCpScrollHandleTop, KAknsIIDQsnCpScrollHandleMiddle,
        KAknsIIDQsnCpScrollHandleBottom, aOrientation ) );

    const TSize maxSize( GetMaxImageSize( bg, middle, top ) );

    if ( EFsScrollbarHorizontal == aOrientation )
        {
        if ( !iHBarBrush && !iHThumbBrush )
            {
            SetHorizontalScrollbarHeight( maxSize.iHeight );
            }
        }
    else
        {
        if ( !iVBarBrush && !iVThumbBrush )
            {
            SetVerticalScrollbarWidth( maxSize.iWidth );
            }
        }

    CleanupStack::Pop( scrollbarLayout );
    aScrollbarLayout = scrollbarLayout;
    aThumbLayout = thumbLayout;
    }


// ---------------------------------------------------------------------------
// Create or destroy specific scrollbar visual.
// ---------------------------------------------------------------------------
//
void CFsHuiScrollbarLayout::MakeScrollbarVisible(
    TBool aVisible,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    CFsScrollbarClet* scroll( NULL );
    CHuiDeckLayout** scrollbarLayout( NULL );
    CHuiDeckLayout** thumbLayout( NULL );
    CHuiImageBrush* barBrush( NULL );
    CHuiImageBrush* thumbBrush( NULL );

    if ( EFsScrollbarVertical == aScrollbar )
        {
        // Select vertical scrollbar dependent variables.
        scroll = &iVertScroll;
        scrollbarLayout = &iVScrollbarLayout;
        thumbLayout = &iVThumb;
        barBrush = iVBarBrush;
        thumbBrush = iVThumbBrush;
        }
    else
        {
        // Select horizontal scrollbar dependent variables.
        scroll = &iHorScroll;
        scrollbarLayout = &iHScrollbarLayout;
        thumbLayout = &iHThumb;
        barBrush = iHBarBrush;
        thumbBrush = iHThumbBrush;
        }

    if ( aVisible )
        {
        // The scrollbar is visible.
        if ( !*scrollbarLayout )
            {
            // Create the scrollbar as it didn't already exists.
            TRAPD( leaveErr, CreateScrollbarL(
                (CHuiDeckLayout*&)*scrollbarLayout,
                (CHuiLayout*&)*thumbLayout,
                aScrollbar ) );

            if ( !leaveErr )
                {
                // Add just created scrollbar to this layout
                TRAP( leaveErr, AppendL( *scrollbarLayout ) );
                if ( !leaveErr )
                    {
                    (*scrollbarLayout)->SetFlag( EHuiVisualFlagManualSize );
                    (*scrollbarLayout)->SetFlag(
                        EHuiVisualFlagManualPosition );
                    }
                else
                    {
                    (*scrollbarLayout)->RemoveAndDestroyAllD();
                    *scrollbarLayout = NULL;
                    }
                }
            if ( barBrush )
                {
                // Replace the grid layout with custom brush.
                UseCustomImage( *scrollbarLayout, barBrush, *thumbLayout );
                scroll->EnableCustomBarImage();
                }
            if ( thumbBrush )
                {
                // Replace the grid layout with custom brush.
                UseCustomImage( *thumbLayout, thumbBrush, NULL );
                scroll->EnableCustomThumbImage();
                }
            scroll->SetVisuals( *scrollbarLayout, *thumbLayout );
            UpdateChildrenLayout();
            }
        }
    else
        {
        // The scrollbar is hidden.
        if ( *scrollbarLayout )
            {
            // Release the scrollbar object.
            ClearFlag( EHuiVisualFlagLayoutUpdateNotification );
            (*scrollbarLayout)->RemoveAndDestroyAllD();
            SetFlag( EHuiVisualFlagLayoutUpdateNotification );
            *scrollbarLayout = NULL;
            *thumbLayout = NULL;
            scroll->SetVisuals( NULL, NULL );
            }
        }
    }


// ---------------------------------------------------------------------------
// Take the custom set image in use.
// Default scrollbar is build with grid layout and it must be released first.
// ---------------------------------------------------------------------------
//
TInt CFsHuiScrollbarLayout::UseCustomImage(
    CHuiDeckLayout* aLayout,
    CHuiImageBrush* aImage,
    CHuiDeckLayout* aChildLayout )
    {
    FUNC_LOG;
    THuiTimedPoint childSize;
    THuiTimedPoint childPos;
    if ( aChildLayout )
        {
        // Store the current size and position of the layout.
        childSize = aChildLayout->Size();
        childPos = aChildLayout->Pos();
        }
    for ( TInt i( aLayout->Count() - 1); 0 <= i; i-- )
        {
        // Keep the one defined child layout. It is the thumb layout on
        // scrollbar layout.
        if ( &aLayout->Visual( i ) != aChildLayout )
            {
            // Remove all other visuals on this layout.
            aLayout->Visual( i ).RemoveAndDestroyAllD();
            }
        }
    if ( aChildLayout )
        {
        // Set the same size and position it had before the values were
        // reseted on child layout destruction.
        aChildLayout->Size() = childSize;
        aChildLayout->Pos() = childPos;
        }

    // Append the image to this layout only if it is not added before.
    TRAPD( leaveErr,
        {
        aLayout->EnableBrushesL();
        for ( TInt i( aLayout->Brushes()->Count() - 1 ); 0 <= i; i-- )
            {
            if ( &aLayout->Brushes()->At( i ) == aImage )
                {
                // The brush is already set.
                return KErrNone;
                }
            }
        aLayout->Brushes()->AppendL( aImage, EHuiDoesNotHaveOwnership );
        } );

    return leaveErr;
    }


// ---------------------------------------------------------------------------
// Update scrollbar thumb positions.
// ---------------------------------------------------------------------------
//
void CFsHuiScrollbarLayout::UpdateScrollbars( TInt aLayoutTransitionTime )
    {
    FUNC_LOG;
    iVertScroll.Update( aLayoutTransitionTime );
    iHorScroll.Update( aLayoutTransitionTime );
    }


// ---------------------------------------------------------------------------
// Create a gridlayout with three skin images. Layout is used to construct the
// scrollbar image.
// ---------------------------------------------------------------------------
//
CHuiGridLayout* CFsHuiScrollbarLayout::CreateGridLayoutL(
    CHuiLayout* aLayout,
    const TAknsItemID& aIdTop,
    const TAknsItemID& aIdMiddle,
    const TAknsItemID& aIdBottom,
    TFsScrollbar aOrientation )
    {
    FUNC_LOG;
    TInt columns( 1 );
    TInt rows( 3 );
    if ( EFsScrollbarHorizontal == aOrientation )
        {
        columns = 3;
        rows = 1;
        }

    CHuiGridLayout* gridLayout( CHuiGridLayout::AddNewL(
        *iOwnerControl, columns, rows, aLayout ) );

    CHuiImageVisual* top(
        CHuiImageVisual::AddNewL( *iOwnerControl, gridLayout ) );
    CHuiImageVisual* middle(
        CHuiImageVisual::AddNewL( *iOwnerControl, gridLayout ) );
    CHuiImageVisual* bottom(
        CHuiImageVisual::AddNewL( *iOwnerControl, gridLayout ) );

    SetImageL( top, aIdTop, aOrientation );
    SetImageL( middle, aIdMiddle, aOrientation );
    SetImageL( bottom, aIdBottom, aOrientation );

    TRect rectTop( top->Image().Texture().Size() );
    TRect rectMiddle( middle->Image().Texture().Size() );
    TRect rectBottom( bottom->Image().Texture().Size() );

    RArray<TInt> weights;
    if ( EFsScrollbarHorizontal == aOrientation )
        {
        weights.Append( rectTop.Width() );
        weights.Append( rectMiddle.Width() );
        weights.Append( rectBottom.Width() );
        gridLayout->SetColumnsL( weights );
        }
    else
        {
        weights.Append( rectTop.Height() );
        weights.Append( rectMiddle.Height() );
        weights.Append( rectBottom.Height() );
        gridLayout->SetRowsL( weights );
        }
    weights.Close();

    return gridLayout;
    }


// ---------------------------------------------------------------------------
// Start using a custom set scrollbar image.
// Ownership of the new brush is gained.
// ---------------------------------------------------------------------------
//
void CFsHuiScrollbarLayout::SetCustomImageL(
    CHuiImageBrush* aBrush,
    TFsScrollbar aScrollbar,
    TBool aSetThumbImage )
    {
    FUNC_LOG;
    if ( !aBrush )
        {
        // Null pointer could cause problems.
        User::Leave( KErrArgument );
        }

    CHuiImageBrush** imageBrush;
    if ( aSetThumbImage )
        {
        imageBrush = EFsScrollbarVertical == aScrollbar
            ? &iVThumbBrush : &iHThumbBrush;
        }
    else
        {
        imageBrush = EFsScrollbarVertical == aScrollbar
            ? &iVBarBrush : &iHBarBrush;
        }

    if ( *imageBrush
        && ( iVScrollbarLayout && EFsScrollbarVertical == aScrollbar )
        || ( iHScrollbarLayout && EFsScrollbarHorizontal == aScrollbar ) )
        {
        // Remove the previously set image brush.
        CHuiDeckLayout* layout;
        if ( aSetThumbImage )
            {
            layout = EFsScrollbarVertical == aScrollbar ? iVThumb : iHThumb;
            }
        else
            {
            layout = EFsScrollbarVertical == aScrollbar
                ? iVScrollbarLayout : iHScrollbarLayout;
            }
        if ( layout->Brushes() )
            {
            for ( TInt i( layout->Brushes()->Count() - 1 ); 0 <= i; i-- )
                {
                if ( &layout->Brushes()->At( i ) == *imageBrush )
                    {
                    layout->Brushes()->Remove( i );
                    break;
                    }
                }
            }
        }

    // Release the previously set brush.
    delete *imageBrush;
    *imageBrush = aBrush;

    if ( ( iVScrollbarLayout && EFsScrollbarVertical == aScrollbar )
        || ( iHScrollbarLayout && EFsScrollbarHorizontal == aScrollbar ) )
        {
        // The scrollbar is already visible
        CFsScrollbarClet& scrollClet(
            EFsScrollbarVertical == aScrollbar ? iVertScroll : iHorScroll );
        CHuiDeckLayout* thumbLayout(
            EFsScrollbarVertical == aScrollbar ? iVThumb : iHThumb );

        // Take the new image brush in use.
        if ( aSetThumbImage )
            {
            User::LeaveIfError( UseCustomImage( thumbLayout, aBrush, NULL ) );
            scrollClet.EnableCustomThumbImage();
            }
        else
            {
            CHuiDeckLayout* barLayout( EFsScrollbarVertical == aScrollbar
                ? iVScrollbarLayout : iHScrollbarLayout );
            User::LeaveIfError(
                UseCustomImage( barLayout, aBrush, thumbLayout ) );
            scrollClet.EnableCustomBarImage();
            }
        }
    }

