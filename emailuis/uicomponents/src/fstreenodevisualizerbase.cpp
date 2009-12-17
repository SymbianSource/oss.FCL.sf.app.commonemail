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
* Description:  Base class of node visualizer.
*
*/


#include "emailtrace.h"
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <biditext.h>

#include "fstreenodevisualizerbase.h"
#include "fstreelist.h"
#include "fslayoutmanager.h"
#include "fsalftextstylemanager.h"

// <cmail> SF
#include <alf/alfgradientbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfcontrol.h>
#include <alf/alftexture.h>
#include <alf/alfimage.h>
#include <alf/alfenv.h>
#include <alf/alftextstyle.h>
#include <alf/alftextvisual.h>
#include <alf/alfmappingfunctions.h>
// </cmail>

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreeNodeVisualizerBase::~CFsTreeNodeVisualizerBase()
    {
    FUNC_LOG;
    if (iLayout)
        {
        iLayout->Env().CancelCustomCommands(this);
        iLayout->RemoveAndDestroyAllD();
        }
    }


// ---------------------------------------------------------------------------
// Returns visualizer's type.
// ---------------------------------------------------------------------------
//
TFsTreeItemVisualizerType CFsTreeNodeVisualizerBase::Type() const
    {
    FUNC_LOG;
    return EFsTreeItemVisualizerBase;
    }


// ---------------------------------------------------------------------------
//  Sets the alignment for text item.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetTextAlign(
    const TAlfAlignHorizontal aAlign )
    {
    FUNC_LOG;
    iTextAlign = aAlign;
    }


// ---------------------------------------------------------------------------
//  The function sets item's text color for the focused state.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetFocusedStateTextColor( TRgb& aColor )
    {
    FUNC_LOG;
    iIsCustomTextFocusedColor = ETrue;
    iFocusedTextColor = aColor;
    }


// ---------------------------------------------------------------------------
//  The function returns item's text color for the focused state.
// ---------------------------------------------------------------------------
//
TRgb CFsTreeNodeVisualizerBase::FocusedStateTextColor() const
    {
    FUNC_LOG;
    return iFocusedTextColor;
    }
    
// ---------------------------------------------------------------------------
//  The function sets item's text color for the normal (unfocused) state.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetNormalStateTextColor( TRgb& aColor )
    {
    FUNC_LOG;
    iIsCustomTextNormalColor = ETrue;
    iNormalTextColor = aColor;
    }


// ---------------------------------------------------------------------------
// The function returns item's text color for the normal (unfocused) state.
// ---------------------------------------------------------------------------
//
TRgb CFsTreeNodeVisualizerBase::NormalStateTextColor() const
    {
    FUNC_LOG;

    return iNormalTextColor;
    }


// ---------------------------------------------------------------------------
//  The function sets the style of item's text.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetTextStyleId ( const TInt aTextStyleId )
    {
    FUNC_LOG;
    iIsCustomTextStyle = ETrue;
    iTextStyleId = aTextStyleId;
    }


// ---------------------------------------------------------------------------
//  The function returns the style of item's text.
// ---------------------------------------------------------------------------
//
TInt CFsTreeNodeVisualizerBase::TextStyleId () const
    {
    FUNC_LOG;
    TInt styleId(0);
    if ( iIsCustomTextStyle )
        {
        styleId = iTextStyleId;
        }
    else
        {
        TRAP_IGNORE( styleId = ModifiedStyleIdL() );
        }
    return styleId;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets multiple flags for an item at one time.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetFlags( TUint32 aFlags )
    {
    FUNC_LOG;
    iFlags = aFlags;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns flags of the item visualizer.
// ---------------------------------------------------------------------------
//
TUint32 CFsTreeNodeVisualizerBase::Flags()
    {
    FUNC_LOG;
    return iFlags;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets visualized item's state to marked/unmarked.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetMarked( TBool aFlag )
    {
    FUNC_LOG;
    if ( aFlag )
        {
        iFlags |= KFsTreeListItemMarked;
        }
    else
        {
        iFlags &= ~KFsTreeListItemMarked;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns mark/unmark state of the item's visualization.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsMarked() const
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemMarked ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets visualized item's state to dimmed/not dimmed.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetDimmed( TBool aFlag )
    {
    FUNC_LOG;
    if ( aFlag )
        {
        iFlags |= KFsTreeListItemDimmed;
        }
    else
        {
        iFlags &= ~KFsTreeListItemDimmed;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns dimmed/not dimmed state of the item's visualization.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsDimmed() const
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemDimmed ? ETrue : EFalse;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets the size of the item visualization when item is not in expanded state.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetSize( const TSize aSize )
    {
    FUNC_LOG;
    iSize = aSize;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns the size of an item in a normal (not expanded) state.
// ---------------------------------------------------------------------------
//
TSize CFsTreeNodeVisualizerBase::Size() const
    {
    FUNC_LOG;
    if ( iFlags & KFsTreeListItemManagedLayout )
        {
        CFsLayoutManager::TFsLayoutMetrics normalSizeMetrics (CFsLayoutManager::EFsLmListSingleDycRowPane);
        TRect parentLayoutRect(0,0,0,0);
        TSize size(0,0);
        TBool retVal(EFalse);

        retVal = CFsLayoutManager::LayoutMetricsSize(
                                    parentLayoutRect,
                                    normalSizeMetrics,
                                    size );

        if ( !retVal )
            {
            size = iSize;
            }

        return size;
        }
    else
        {
        return iSize;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets the item's visualization state to expanded.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetExtended( const TBool aFlag )
    {
    FUNC_LOG;
    if ( aFlag )
        {
        iFlags |= KFsTreeListItemExtended;
        }
    else
        {
        iFlags &= ~KFsTreeListItemExtended;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns expanded/not expanded state of the item visualization.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsExtended() const
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemExtended ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets if an item can be in expanded state or not.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetExtendable( TBool aFlag )
    {
    FUNC_LOG;
    if ( aFlag )
        {
        iFlags |= KFsTreeListItemExtendable;
        }
    else
        {
        iFlags &= ~KFsTreeListItemExtendable;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns an information if item can be in expanded state.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsExtendable( )
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemExtendable ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns an information if item can be focusable.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsFocusable( ) const
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemFocusable ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets the item's ability to receive focus.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetFocusable( const TBool aFlag )
    {
    FUNC_LOG;
    if ( aFlag )
        {
        iFlags |= KFsTreeListItemFocusable;
        }
    else
        {
        iFlags &= ~KFsTreeListItemFocusable;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets the size of the item visualization area when item is in expanded
// state.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetExtendedSize(
    const TSize aExtendedSize )
    {
    FUNC_LOG;
    iExtendedSize = aExtendedSize;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns the size of an item in expanded state.
// ---------------------------------------------------------------------------
//
TSize CFsTreeNodeVisualizerBase::ExtendedSize() const
    {
    FUNC_LOG;
    if ( iFlags & KFsTreeListItemManagedLayout )
        {
        CFsLayoutManager::TFsLayoutMetrics extendedSizeMetrics( CFsLayoutManager::EFsLmListMediumLineX2 );
        TRect parentLayoutRect(0,0,0,0);
        TSize extSize(0,0);
        TBool retVal(EFalse);

        retVal = CFsLayoutManager::LayoutMetricsSize(
                                    parentLayoutRect,
                                    extendedSizeMetrics,
                                    extSize );

        if ( !retVal )
            {
            extSize = iExtendedSize;
            }

        return extSize;
        }
    else
        {
        return iExtendedSize;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets visualized item's state to hidden/visible.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetHidden( TBool aFlag )
    {
    FUNC_LOG;
    if ( aFlag )
        {
        iFlags |= KFsTreeListItemHidden;
        }
    else
        {
        iFlags &= ~KFsTreeListItemHidden;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns an information if the item is hidden or not.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsHidden()
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemHidden ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Enables or disables text shadow.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::EnableTextShadow(
        const TBool aShadowOn )
    {
    FUNC_LOG;
    if (aShadowOn)
        {
        iFlags |= KFsTreeListItemTextShadow;
        }
    else
        {
        iFlags &= ~KFsTreeListItemTextShadow;
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets text indentation.
// ---------------------------------------------------------------------------
void CFsTreeNodeVisualizerBase::SetIndentation(
        const TInt aIndentation )
    {
    FUNC_LOG;
    iTextIndentation = aIndentation;
    }


// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Sets item background color.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetBackgroundColor(const TRgb aColor)
    {
    FUNC_LOG;
    iBackgroundColor = aColor;
    iBackgroundState = EFsNodeBackgroundColor;
    }

// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Returns background color if it's set.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::GetBackgroundColor( TRgb& aColor )
    {
    FUNC_LOG;
    TBool retVal(EFalse);

    if ( iBackgroundState == EFsNodeBackgroundColor )
        {
        aColor = iBackgroundColor;
        retVal = ETrue;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Sets item background texture.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetBackgroundTexture(
        const CAlfTexture& aTexture)
    {
    FUNC_LOG;
    iBackgroundTexture = &aTexture;
    iBackgroundState = EFsNodeBackgroundTexture;
    }

// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Returns background texture if it's set.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::GetBackgroundTexture( const CAlfTexture*& aTexture )
    {
    FUNC_LOG;
    TBool retVal(EFalse);
    if ( iBackgroundState == EFsNodeBackgroundTexture )
        {
        aTexture = iBackgroundTexture;
        retVal = ETrue;
        }
    return retVal;
    }

// <cmail>
// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Sets item background brush.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetBackgroundBrush( CAlfBrush* aBrush )
    {
    FUNC_LOG;
    iBackgroundBrush = aBrush;
    iBackgroundState = EFsNodeBackgroundBrush;
    }

// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Returns item background brush and opacity.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::GetBackgroundBrush( CAlfBrush*& aBrush )
    {
    FUNC_LOG;
    TBool retVal( EFalse );
    if ( iBackgroundState == EFsNodeBackgroundBrush && iBackgroundBrush )
        {
        aBrush = iBackgroundBrush;
        retVal = ETrue;
        }

    return retVal;
    }
// </cmail>

// ---------------------------------------------------------------------------
// From MFsTreeItemVisualizer.
// Clears item background.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::ClearBackground( )
    {
    FUNC_LOG;
    iBackgroundState = EFsNodeBackgroundNone;
    }

// ---------------------------------------------------------------------------
//  Returns reference to the item's main layout
// ---------------------------------------------------------------------------
//
CAlfLayout& CFsTreeNodeVisualizerBase::Layout() const
    {
    FUNC_LOG;
    return (static_cast<CAlfLayout&>(*iLayout));
    }

// ---------------------------------------------------------------------------
//  Sets menu for item
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetMenu( CFsTreeList* aMenu )
    {
    FUNC_LOG;
    iMenu = aMenu;
    iFlags |= KFsTreeListItemHasMenu;
    }

// ---------------------------------------------------------------------------
//  Returns CFsTreeList object contaning menu items
// ---------------------------------------------------------------------------
//
CFsTreeList* CFsTreeNodeVisualizerBase::Menu() const
    {
    FUNC_LOG;
    return iMenu;
    }

// ---------------------------------------------------------------------------
//  This functions sets wether an item should always be in extended state.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetAlwaysExtended( TBool aAlwaysExtended )
    {
    FUNC_LOG;
    if ( aAlwaysExtended )
        {
        iFlags |= KFsTreeListItemAlwaysExtended;
        }
    else
        {
        iFlags &= ~KFsTreeListItemAlwaysExtended;
        }
    }

// ---------------------------------------------------------------------------
//  This function returns wether an item is always in extended state.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsAlwaysExtended( ) const
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemAlwaysExtended ? ETrue : EFalse;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
//  A call to this function means that the visualization of empty root
// becomes visible and should draw itself within specified parent layout.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::ShowL( CAlfLayout& /*aParentLayout*/,
                                        const TUint /*aTimeout*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Stub.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::UpdateL( const MFsTreeItemData& /*aData*/,
                                          TBool /*aFocused*/,
                                          const TUint /*aLevel*/,
                                          CAlfTexture*& /*aMarkIcon*/,
                                          CAlfTexture*& /*aMenuIcon*/,
                                          const TUint /*aTimeout*/,
                                          TBool /*aUpdateData*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Stub.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::Hide( const TInt /*aTimeout*/ )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Stub.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::MarqueeL(const TFsTextMarqueeType /*aMarqueeType*/,
                                         const TUint /*aMarqueeSpeed*/,
                                         const TInt /*aMarqueeStartDelay*/,
                                         const TInt /*aMarqueeCycleStartDelay*/,
                                         const TInt /*aMarqueeRepetitions*/)
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
//  From MAlfEventHandler. Stub.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::OfferEventL(const TAlfEvent& /*aEvent*/)
    {
    FUNC_LOG;
    return EFalse;
    }


// ---------------------------------------------------------------------------
//  Sets the text style manager for the item visualizer.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetTextStyleManager(
        CFsAlfTextStyleManager& aManager )
    {
    FUNC_LOG;
    iTextStyleManager = &aManager;
    }


// ---------------------------------------------------------------------------
//  Sets the text stroke weight for the item visualizer.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetTextBold( const TBool aIsBold )
    {
    FUNC_LOG;
    if (aIsBold)
        {
        iFlags |= KFsTreeListItemTextBold;
        }
    else
        {
        iFlags &= ~KFsTreeListItemTextBold;
        }
    }


// ---------------------------------------------------------------------------
//  Gets the text stroke weight for the item visualizer.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsTextBold( ) const
    {
    FUNC_LOG;
    return iFlags & KFsTreeListItemTextBold;
    }


// ---------------------------------------------------------------------------
// Function gets text color for a focused item. The color is taken from the
// skin if skin support is  on, if not then a default value is used.
// ---------------------------------------------------------------------------
//
TRgb CFsTreeNodeVisualizerBase::FocusedStateTextSkinColor( )
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    // First try to get the FS specific text color
    if( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDFsHighlightColors, EAknsCIFsHighlightColorsCG1 ) != KErrNone )
        {
        // If FS specific text color not found, try to get the
        // normal Avkon list highlight text color
        if( AknsUtils::GetCachedColor( skin, textColor,
                KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 ) != KErrNone )
            {
            textColor = KRgbBlack;
            }
        }
    return textColor;
    }

// ---------------------------------------------------------------------------
// Function gets text color for a non focused item. The color is taken from
// the skin if skin support is  on, if not then a default value is used.
// ---------------------------------------------------------------------------
//
TRgb CFsTreeNodeVisualizerBase::NormalStateTextSkinColor( )
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    if ( AknsUtils::GetCachedColor( skin, textColor,
            KAknsIIDFsTextColors, EAknsCIFsTextColorsCG3 ) != KErrNone )
        {
        textColor = KRgbBlack;
        }
    return textColor;
    }

// ---------------------------------------------------------------------------
// Returns horizontal alignment of the displayed text in respect to current layout.
// ---------------------------------------------------------------------------
//
TAlfAlignHorizontal CFsTreeNodeVisualizerBase::CurrentTextAlignmentL( TAlfAlignHorizontal aAlignment,
                                                                      TDesC* aText,
                                                                      CAlfTextVisual* aVisual)
    {
    FUNC_LOG;
    TAlfAlignHorizontal align (aAlignment);
    TBool isMirrored( EFalse );

    isMirrored = CFsLayoutManager::IsMirrored();

    if ( align == EAlfAlignHLocale )
        {
        if ( aText && aVisual )
            {
            TInt textWidth(0), visualWidth(0);
            textWidth = aVisual->SubstringExtents(0, aVisual->Text().Length()).Width();
            visualWidth = aVisual->Size().Target().iX;

            if ( textWidth >= visualWidth )
        {
                TBool found(EFalse);
                TBidiText * bidiText = TBidiText::NewL(aText->Length() + 1, 1);
                TBidiText::TDirectionality direction = bidiText->TextDirectionality( *aText, &found );
                delete bidiText;
                if ( direction == TBidiText::ELeftToRight )
                    {
                    align = EAlfAlignHLeft;
                    }
                else
                    { //direction == TBidiText::ERightToLeft
                    align = EAlfAlignHRight;
                    }
                }
            else
                {
                if ( isMirrored )
                    {
                    align = EAlfAlignHRight;
                    }
                else
                    {
                    align = EAlfAlignHLeft;
                    }
                }
            }
        }
    else if ( isMirrored )
        {//convert left <-> right in mirrored layout
        if ( align == EAlfAlignHLeft )
            {
            align = EAlfAlignHRight;
            }
        else if ( align == EAlfAlignHRight )
            {
            align = EAlfAlignHLeft;
            }
        }

    return align;
    }


// ---------------------------------------------------------------------------
// Returns text style id used in the node.
// ---------------------------------------------------------------------------
//
TInt CFsTreeNodeVisualizerBase::ModifiedStyleIdL( ) const
    {
    FUNC_LOG;
    TInt styleId(iTextStyleId);
        TCharFormat charFormat;

        if ( iIsCustomTextStyle )
            {
            CAlfTextStyle* textStyle( iOwnerControl.Env().TextStyleManager().TextStyle( styleId ) );
            HBufC* name = textStyle->TypefaceNameL();
            charFormat.iFontSpec.iTypeface.iName = name->Left( KMaxTypefaceNameLength );
            delete name;

            charFormat.iFontSpec.iHeight = textStyle->TextSizeInTwips();
            charFormat.iFontPresentation.iTextColor = textStyle->TextColor();

            charFormat.iFontPresentation.iStrikethrough = textStyle->IsStrikeThrough() ? EStrikethroughOn : EStrikethroughOff;
            charFormat.iFontPresentation.iUnderline = textStyle->IsUnderline() ? EUnderlineOn : EUnderlineOff;

            }
        else
            {
            TRect parentRect( Size() );
            CFsLayoutManager::TFsText text;

            CFsLayoutManager::LayoutMetricsText( parentRect,CFsLayoutManager::EFsLmListMediumLineT3T1, text, 0 );

            charFormat.iFontSpec = text.iFont->FontSpecInTwips();
            }

        if (iFontHeight>0)
            {
            charFormat.iFontSpec.iHeight = iFontHeight;
            }

        TBool isBold(EFalse);
        isBold = iFlags & KFsTreeListItemTextBold;
        if ( isBold )
            {
            charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );
            }
        else
            {
            charFormat.iFontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightNormal );
            }

        if ( iTextStyleManager )
            {
            styleId = iTextStyleManager->GetStyleIDL( charFormat );
            }

        return styleId;
    }


// ---------------------------------------------------------------------------
//  Method to set text font height in twips.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetFontHeight(const TInt aHeightInTwips)
    {
    FUNC_LOG;
    iFontHeight = aHeightInTwips;
    }


// ---------------------------------------------------------------------------
//  Gets height of a text font in twips.
// ---------------------------------------------------------------------------
//
TInt CFsTreeNodeVisualizerBase::FontHeight() const
    {
    FUNC_LOG;
    return iFontHeight;
    }


// ---------------------------------------------------------------------------
//  Method to set text font height in pixels.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetFontHeightInPixels(const TInt aHeightInPixels)
    {
    FUNC_LOG;
    TZoomFactor devicemap(CCoeEnv::Static()->ScreenDevice());
    devicemap.SetZoomFactor(TZoomFactor::EZoomOneToOne);
    TInt heightInTwips = devicemap.VerticalPixelsToTwips(aHeightInPixels);

    iFontHeight = heightInTwips;
    }


// ---------------------------------------------------------------------------
//  Gets height of a text font in pixels.
// ---------------------------------------------------------------------------
//
TInt CFsTreeNodeVisualizerBase::FontHeightInPixels() const
    {
    FUNC_LOG;
    TZoomFactor devicemap(CCoeEnv::Static()->ScreenDevice());
    devicemap.SetZoomFactor(TZoomFactor::EZoomOneToOne);

    return devicemap.VerticalTwipsToPixels(iFontHeight);
    }


// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::ConstructL( )
    {
    FUNC_LOG;
    //get default skin values
    iNormalTextColor = NormalStateTextSkinColor();
    iFocusedTextColor = FocusedStateTextSkinColor();
    }


// ---------------------------------------------------------------------------
// From class MFsTreeNodeVisualizer.
// Sets visualized node's state to rolled/unrolled.
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetExpanded( TBool aFlag, const MFsTreeItemData* /*aData*/ )
    {
    FUNC_LOG;
    if ( aFlag )
        {
        iFlags |= KFsTreeListNodeExpanded;
        }
    else
        {
        iFlags &= ~KFsTreeListNodeExpanded;
        }
    }


// ---------------------------------------------------------------------------
// From class MFsTreeNodeVisualizer.
// Returns rolled/unrolled state of the node's visualization.
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsExpanded() const
    {
    FUNC_LOG;
    return iFlags & KFsTreeListNodeExpanded ? ETrue : EFalse;
    }


// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreeNodeVisualizerBase::CFsTreeNodeVisualizerBase(
        CAlfControl& aOwnerControl)
    : iOwnerControl(aOwnerControl),
    iLayout(NULL),
    iFlags(KFsTreeListNodeExpanded|KFsTreeListItemFocusable|
            KFsTreeListItemManagedLayout),
    iTextAlign( EAlfAlignHLocale ),
    iTextIndentation( KFsDefaultIndentation ),
    iIsCustomTextNormalColor(EFalse),
    iIsCustomTextFocusedColor(EFalse),
    iTextStyleId ( EAlfTextStyleNormal ),
    iIsCustomTextStyle( EFalse ),
    iFontHeight(0),
    iBackgroundState( EFsNodeBackgroundNone )
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
//  Set layouting hints
// ---------------------------------------------------------------------------
//
void CFsTreeNodeVisualizerBase::SetLayoutHints( const TUint aHints )
    {   
    FUNC_LOG;
    iLayoutHints |= aHints;
    }

// ---------------------------------------------------------------------------
//  Check if hint is set
// ---------------------------------------------------------------------------
//
TBool CFsTreeNodeVisualizerBase::IsLayoutHintSet( const TLayoutHint aHint ) const
    {
    FUNC_LOG;
    return iLayoutHints & aHint;
    }
