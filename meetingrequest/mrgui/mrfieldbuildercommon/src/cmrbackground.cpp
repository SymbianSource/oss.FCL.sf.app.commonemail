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
* Description:  Background/focus component for fields.
*
*/

#include "cmrbackground.h"

#include "nmrglobalfeaturesettings.h"
#include "nmrlayoutmanager.h"
#include <AknUtils.h>


// unnamed namespace for local definitions
namespace // codescanner::namespace
    {
    const TInt KEdge (8);
    const TInt KGranularity( 9 );

    #ifdef _DEBUG
        enum TPanic
            {
            EErrorItemCount = 1
            };

        void Panic( TPanic aPanic )
            {
            _LIT( KCategory, "CMRBackground" );
            User::Panic( KCategory(), aPanic );
            }
    #endif // _DEBUG

    TBool IsFocusUsed()
        {
        TBool retval( EFalse );
        if ( NMRGlobalFeatureSettings::KeyboardType() !=
                NMRGlobalFeatureSettings::ENoKeyboard )
            {
            // If there is a keyboard then we can show the focus
            retval = ETrue;
            }

        return retval;
        }
    }//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRBackground::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C
CMRBackground* CMRBackground::NewL()
    {
    CMRBackground* self = new (ELeave) CMRBackground();
    CleanupStack::PushL( self );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRBackground::CMRBackground
// ---------------------------------------------------------------------------
//
CMRBackground::CMRBackground()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRBackground::~CMRBackground
// ---------------------------------------------------------------------------
//
EXPORT_C CMRBackground::~CMRBackground()
    {
    }

// ---------------------------------------------------------------------------
// CMRBackground::Draw
// ---------------------------------------------------------------------------
//
EXPORT_C void CMRBackground::Draw(
        CWindowGc& aGc,
        const CCoeControl& aControl,
        const TRect& /*aRect*/ ) const
    {
    if ( IsFocusUsed() )
        {
        const CESMRField& ctrl =
                static_cast<const CESMRField&> ( aControl );
        TBool hasOutlineFocus = ctrl.HasOutlineFocus();
        TESMRFieldFocusType focusType = ctrl.GetFocusType();
        // highlight bitmap target rect:
        TRect rect( ctrl.GetFocusRect() );
        rect.Move( ctrl.Position() );

        if ( hasOutlineFocus )
            {
            if( focusType == EESMRHighlightFocus )
                {
                if( ctrl.FieldMode() == EESMRFieldModeView )
                    {
                    // Focused viewer field
                    DrawFocus( aGc, rect, aControl, EViewerFieldWithFocus );
                    }
                else
                    {
                    // Focused editor field
                    DrawFocus( aGc, rect, aControl, EEditorFieldWithFocus );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRBackground::IconSkinIdL
// ---------------------------------------------------------------------------
//
CArrayFixFlat<NMRBitmapManager::TMRBitmapId>*
CMRBackground::IconSkinIdL( TBgType aType ) const
    {
    CArrayFixFlat<NMRBitmapManager::TMRBitmapId>* array =
        new (ELeave) CArrayFixFlat<NMRBitmapManager::TMRBitmapId>( KGranularity );
    CleanupStack::PushL( array );
    if( aType == EViewerFieldWithFocus )
        {
        array->AppendL( NMRBitmapManager::EMRBitmapListTopLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapListLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapListBottomLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapListTop );
        array->AppendL( NMRBitmapManager::EMRBitmapListCenter );
        array->AppendL( NMRBitmapManager::EMRBitmapListBottom );
        array->AppendL( NMRBitmapManager::EMRBitmapListTopRight );
        array->AppendL( NMRBitmapManager::EMRBitmapListRight );
        array->AppendL( NMRBitmapManager::EMRBitmapListBottomRight );
        }
    else if ( aType == EEditorFieldWithFocus )
        {
        array->AppendL( NMRBitmapManager::EMRBitmapInputTopLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapInputLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapInputBottomLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapInputTop );
        array->AppendL( NMRBitmapManager::EMRBitmapInputCenter );
        array->AppendL( NMRBitmapManager::EMRBitmapInputBottom );
        array->AppendL( NMRBitmapManager::EMRBitmapInputTopRight );
        array->AppendL( NMRBitmapManager::EMRBitmapInputRight );
        array->AppendL( NMRBitmapManager::EMRBitmapInputBottomRight );
        }
    else
        {
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptTopLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptBottomLeft );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptTop );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptCenter );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptBottom );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptTopRight );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptRight );
        array->AppendL( NMRBitmapManager::EMRBitmapSetOptBottomRight );
        }

    CleanupStack::Pop( array );
    return array;
    }

// ---------------------------------------------------------------------------
// CMRBackground::DrawFocus
// ---------------------------------------------------------------------------
//
void CMRBackground::DrawFocus(
        CWindowGc& aGc,
        TRect aRect,
        const CCoeControl& aControl,
        TBgType aType ) const
        {
        CFbsBitmap* selector = NULL;
        CFbsBitmap* selectorMask = NULL;

        CArrayFixFlat<NMRBitmapManager::TMRBitmapId>* iconIds = NULL;
        // Error does not matter, we'll check that array is created and
        // it contains all items.
        TRAP_IGNORE( iconIds = IconSkinIdL( aType ) );

        if( iconIds )
            {
            __ASSERT_DEBUG(
                    iconIds->Count() == KGranularity,
                    Panic(EErrorItemCount) );
            TSize corner(KEdge, KEdge);
            TInt error = NMRBitmapManager::GetSkinBasedBitmap(
                    (*iconIds)[0], selector, selectorMask, corner );

            // Reduce color stripe from background
            TRect rect( aRect );
            TAknLayoutRect stripeLayoutRect =
                    NMRLayoutManager::GetLayoutRect( aRect,
                            NMRLayoutManager::EMRLayoutStripe );
            TInt width( stripeLayoutRect.Rect().Width() );

            // Reduce stripe width from right hand side.
            // This makes rectangle correct for mirrored layout.
            rect.Resize( -width, 0 );

            if ( !AknLayoutUtils::LayoutMirrored() )
                {
                // Move rect to right when layout is not mirrored.
                rect.Move( width, 0 );
                }

            if( selector && selectorMask && error == KErrNone )
                {
                //corner TL
                aGc.BitBltMasked(
                        rect.iTl, selector, corner, selectorMask, EFalse );

                //side L
                TSize side(KEdge, (rect.Height() - 2 * KEdge) );
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[1], selector, selectorMask, side );
                aGc.BitBltMasked( TPoint(rect.iTl.iX, rect.iTl.iY + KEdge),
                                  selector, side, selectorMask, EFalse );

                //corner BL
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[2], selector, selectorMask, corner );
                aGc.BitBltMasked(
                        TPoint(rect.iTl.iX,
                                rect.iTl.iY + KEdge + side.iHeight),
                        selector, corner, selectorMask, EFalse );

                //top
                TSize top( (rect.Width() - 2 * KEdge) , KEdge);
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[3], selector, selectorMask, top );
                aGc.BitBltMasked( TPoint(rect.iTl.iX + KEdge, rect.iTl.iY),
                                  selector, top, selectorMask, EFalse );

                //center
                TSize center( top.iWidth, side.iHeight);
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[4], selector, selectorMask, center );
                aGc.BitBltMasked(
                        TPoint(rect.iTl.iX + KEdge, rect.iTl.iY + KEdge),
                            selector, center, selectorMask, EFalse );

                //bottom
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[5], selector, selectorMask, top );
                aGc.BitBltMasked(
                TPoint(rect.iTl.iX + KEdge,
                        rect.iTl.iY + side.iHeight + KEdge),
                selector, top, selectorMask, EFalse );

                //corner TR
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[6], selector, selectorMask, corner );
                aGc.BitBltMasked(
                        TPoint(rect.iTl.iX + KEdge + top.iWidth,
                                rect.iTl.iY),
                        selector, corner, selectorMask, EFalse );

                //side R
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[7], selector, selectorMask, side );
                aGc.BitBltMasked(
                        TPoint(rect.iTl.iX + KEdge + top.iWidth,
                                rect.iTl.iY + KEdge),
                 selector, side, selectorMask, EFalse );

                //corner Br
                NMRBitmapManager::GetSkinBasedBitmap(
                        (*iconIds)[8], selector, selectorMask, corner );
                aGc.BitBltMasked(
                        TPoint(rect.iTl.iX + KEdge + top.iWidth,
                                rect.iTl.iY + KEdge + side.iHeight),
                                    selector, corner, selectorMask, EFalse );
                }
            else // This should NOT be called ever.
                {
                aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
                aGc.SetPenStyle( CGraphicsContext::ENullPen );
                aGc.SetBrushColor( KRgbGreen );
                aGc.DrawRect( aControl.Rect() );
                }

            delete selector;
            delete selectorMask;
            }
        delete iconIds;
        }
// EOF
