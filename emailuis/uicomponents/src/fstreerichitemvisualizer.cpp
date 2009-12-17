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
* Description:  A visualizer for data items with rich text.
*
*/



#include "emailtrace.h"
#include <txtrich.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

// <cmail> SF
#include <alf/alfcontrol.h>
#include <alf/alflayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfimage.h>
#include <alf/alfimagevisual.h>
#include <alf/alfbrusharray.h>
#include <alf/alfenv.h>
// </cmail>

#include "fstreerichitemvisualizer.h"
#include "fstreerichitemdata.h"
#include "fsgenericpanic.h"
#include "fsrichtext.h"
#include "fstreelist.h"
#include "fsscrollbarset.h"
#include "fsalftextstylemanager.h"
#include "fslayoutmanager.h"

// ======== MEMBER FUNCTIONS ========



// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreeRichItemVisualizer::CFsTreeRichItemVisualizer(
    CAlfControl& aOwnerControl, CFsAlfTextStyleManager& aTextStyleManager )
    : CFsTreeItemVisualizerBase(aOwnerControl)
    {
    FUNC_LOG;
    iTextStyleManager = &aTextStyleManager;
    }



// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemVisualizer::ConstructL()
    {
    FUNC_LOG;
    }



// ---------------------------------------------------------------------------
// Two-phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeRichItemVisualizer* CFsTreeRichItemVisualizer::NewL(
    CAlfControl& aOwnerControl, CFsAlfTextStyleManager& aTextStyleManager )
    {
    FUNC_LOG;
    CFsTreeRichItemVisualizer* self =
        new( ELeave ) CFsTreeRichItemVisualizer(aOwnerControl, aTextStyleManager);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreeRichItemVisualizer::~CFsTreeRichItemVisualizer()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Returns visualizer's type.
// ---------------------------------------------------------------------------
//
TFsTreeItemVisualizerType CFsTreeRichItemVisualizer::Type() const
    {
    FUNC_LOG;
    return EFsTreeRichItemVisualizer;
    }



// ---------------------------------------------------------------------------
//  Create a visual based on the text object.
// ---------------------------------------------------------------------------
//
CAlfFlowLayout* CFsTreeRichItemVisualizer::CreateTextLineL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout,
    const CFsTreeRichItemData* aText,
    TSize aSize )
    {
    FUNC_LOG;
    CAlfFlowLayout* textLayout(
            CAlfFlowLayout::AddNewL(aOwnerControl, aParentLayout));
    textLayout->SetFlowDirection(CAlfFlowLayout::EFlowHorizontal);
    textLayout->SetCentering( EFalse );
    textLayout->EnableBrushesL();
    textLayout->SetSize( aSize );
    textLayout->SetClipping(ETrue);

    CAlfTextVisual* textVisual;
    TCharFormat charFormat;
    TPtrC16 text;
    TInt textPos( 0 );
    TInt xPos( 0 );

    textLayout->SetFlag(EAlfVisualFlagFreezeLayout);

    while ( aText->Text().DocumentLength() > textPos )
        {
        // Need to create a new visual for every text block with different
        // markup.
        aText->Text().GetChars( text, charFormat, textPos );
        textVisual = CAlfTextVisual::AddNewL( aOwnerControl, textLayout );
        textVisual->EnableShadow( iFlags & KFsTreeListItemTextShadow );
        textVisual->EnableBrushesL();
        textVisual->SetTextL( text );

        TInt styleId( iTextStyleManager->GetStyleIDL( charFormat ) );
        textVisual->SetTextStyle( styleId );
        textVisual->SetColor( charFormat.iFontPresentation.iTextColor );

        if ( TFontPresentation::EFontHighlightNone
            != charFormat.iFontPresentation.iHighlightStyle )
            {
            CAlfGradientBrush* brush( CAlfGradientBrush::NewLC(textLayout->Env()));
            brush->SetColor( charFormat.iFontPresentation.iHighlightColor );
            textVisual->Brushes()->AppendL(brush, EAlfHasOwnership);
            CleanupStack::Pop( brush );
            }

        textVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );

        textVisual->SetSize( TSize(textVisual->TextExtents().iWidth,
                iSize.iHeight) );

        TInt visualWidth( textVisual->Size().Target().iX );
        if ( xPos + visualWidth >= aSize.iWidth )
            {
            // The width limit has been reached and the last item need to be
            // resized.
            TAlfRealSize size;
            size.iWidth = aSize.iWidth - xPos;
            size.iHeight = textVisual->Size().iY.Target();
            textVisual->SetSize(size, 0);
            break;
            }
        xPos += visualWidth;
        textPos += text.Length();
        }
    textLayout->SetFlag(EAlfVisualFlagFreezeLayout);
    textLayout->UpdateChildrenLayout(0);

    return textLayout;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets the size of the item visualization when item is not in expanded
// state. The value is taken from layout manager.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemVisualizer::UpdateLayout(
        const CFsTreeRichItemData* aData,
        const TInt aIndentation )
    {
    FUNC_LOG;
    TRect rect;

    TAlfTimedPoint tpMainIconTl, tpMainIconBr;
    TAlfTimedPoint tpTextTl, tpTextBr;
    TAlfTimedPoint tpMarkIconTl, tpMarkIconBr;
    TAlfTimedPoint tpMenuIconTl, tpMenuIconBr;

    CFsLayoutManager::TFsLayoutMetrics
        mainIconMetrics = CFsLayoutManager::EFsLmListMediumLineG1,
        markIconMetrics = CFsLayoutManager::EFsLmListMediumLineG3G2,
        menuIconMetrics = CFsLayoutManager::EFsLmListMediumLineG3G3,
        textMetrics     = CFsLayoutManager::EFsLmListMediumLineG3T1,
        sizeMetrics     = CFsLayoutManager::EFsLmListSingleDycRowPane;

    if (aData->IsIconSet())
        {
        if (iFlags & KFsTreeListItemMarked)
            {
            if (iMenu && (iFlags & KFsTreeListItemFocused))
                {
                //mainIcon + markIcon + menuIcon
                //the metrics defined when declared
                }
            else
                {
                //mainIcon + markIcon
                mainIconMetrics = CFsLayoutManager::EFsLmListMediumLineG2G1;
                markIconMetrics = CFsLayoutManager::EFsLmListMediumLineG2G2;
                textMetrics     = CFsLayoutManager::EFsLmListMediumLineG2T1;
                }
            }
        else
            {
            if ((iFlags & KFsTreeListItemHasMenu)
                    && (iFlags & KFsTreeListItemFocused))
                {
                //mainIcon + menuIcon
                mainIconMetrics = CFsLayoutManager::EFsLmListMediumLineG2G1;
                menuIconMetrics = CFsLayoutManager::EFsLmListMediumLineG2G2;
                textMetrics     = CFsLayoutManager::EFsLmListMediumLineG2T1;
                }
            else
                {
                //mainIcon
                mainIconMetrics = CFsLayoutManager::EFsLmListMediumLineG1;
                textMetrics     = CFsLayoutManager::EFsLmListMediumLineT1;
                }
            }
        }
    else
        {
        if (iFlags & KFsTreeListItemMarked)
            {
            if (iMenu && (iFlags & KFsTreeListItemFocused))
                {
                //markIcon + menuIcon
                markIconMetrics =
                    CFsLayoutManager::EFsLmListMediumLineRightIconx2G1;
                menuIconMetrics =
                    CFsLayoutManager::EFsLmListMediumLineRightIconx2G2;
                textMetrics     =
                    CFsLayoutManager::EFsLmListMediumLineRightIconx2T1;
                }
            else
                {
                //markIcon
                markIconMetrics =
                    CFsLayoutManager::EFsLmListMediumLineRightIconG1;
                textMetrics     =
                    CFsLayoutManager::EFsLmListMediumLineRightIconT1;
                }
            }
        else
            {
            if ((iFlags & KFsTreeListItemHasMenu)
                    && (iFlags & KFsTreeListItemFocused))
                {
                //menuIcon
                menuIconMetrics =
                    CFsLayoutManager::EFsLmListMediumLineRightIconG1;
                textMetrics     =
                    CFsLayoutManager::EFsLmListMediumLineRightIconT1;
                }
            else
                {
                //plain item
                textMetrics     =
                    CFsLayoutManager::EFsLmListMediumLinePlainT1;
                }
            }
        }

    //layout manager is queried for medium values - LM returnes rects based on phone's global text settings

    TRect parentLayoutRect = TRect(iParentLayout->Size().Target().AsSize());
    parentLayoutRect.Resize(-(iParentLayout->HorizontalPadding()*2), 0);

    if (iFlags & KFsTreeListItemManagedLayout)
        {
        CFsLayoutManager::LayoutMetricsSize(
                parentLayoutRect,
                sizeMetrics,
                iSize );
        }

    TRect currentSize = iSize;
    if (CFsLayoutManager::IsMirrored())
        {
        currentSize.iBr = currentSize.iBr - TPoint(aIndentation, 0);
        }
    else
        {
        currentSize.iTl = currentSize.iTl + TPoint(aIndentation, 0);
        }

    CFsLayoutManager::LayoutMetricsRect(
            currentSize,
            mainIconMetrics,
            rect );
    tpMainIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpMainIconBr.SetTarget(TAlfRealPoint(rect.iBr));

    CFsLayoutManager::LayoutMetricsRect(
            currentSize,
            markIconMetrics,
            rect );
    tpMarkIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpMarkIconBr.SetTarget(TAlfRealPoint(rect.iBr));

    CFsLayoutManager::LayoutMetricsRect(
            currentSize,
            menuIconMetrics,
            rect );
    tpMenuIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpMenuIconBr.SetTarget(TAlfRealPoint(rect.iBr));

    CFsLayoutManager::TFsText textInfo;
    CFsLayoutManager::LayoutMetricsText(currentSize, textMetrics, textInfo);

    tpTextTl.SetTarget(TAlfRealPoint(textInfo.iTextRect.iTl));
    tpTextBr.SetTarget(TAlfRealPoint(textInfo.iTextRect.iBr));

    TInt visualIconIndex = iLayout->FindVisual(iIconVisual);
    if ( visualIconIndex != KErrNotFound )
        {
        iLayout->SetAnchor( EAlfAnchorTopLeft,
                visualIconIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpMainIconTl );
        iLayout->SetAnchor( EAlfAnchorBottomRight,
                visualIconIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute,
                EAlfAnchorMetricAbsolute,
                tpMainIconBr );
        }

    if ((iFlags & KFsTreeListItemHasMenu)
            && (iFlags & KFsTreeListItemFocused))
        {
        TInt iconMenuVisIndex = iLayout->FindVisual(iIconMenu);
        if ( iconMenuVisIndex != KErrNotFound )
            {
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                    iconMenuVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMenuIconTl );
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                    iconMenuVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMenuIconBr );

            TAlfTimedValue opacity;
            opacity.SetValueNow(1.0f);
            iIconMenu->SetOpacity(opacity);
            }
        }
    else
        {
        TAlfTimedValue opacity;
        opacity.SetValueNow(0.0f);
        iIconMenu->SetOpacity(opacity);
        }

    if (iTextLayout)
        {
        TInt textVisIndex = iLayout->FindVisual(iTextLayout);
        if ( textVisIndex != KErrNotFound )
            {
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                    textVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    tpTextTl );
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                    textVisIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    tpTextBr );
            }
        }

    if ( iFlags & KFsTreeListItemMarked )
        {
        TInt iconMarkedVisIndex =
                            iLayout->FindVisual(iIconMarked);
        if ( iconMarkedVisIndex != KErrNotFound )
            {
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                    iconMarkedVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    tpMarkIconTl );
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                    iconMarkedVisIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,
                    EAlfAnchorMetricAbsolute,
                    tpMarkIconBr );

            TAlfTimedValue opacity;
            opacity.SetValueNow(1.0f);
            iIconMarked->SetOpacity(opacity);
            }
        }
    else
        {
        if (iIconMarked)
            {
            TAlfTimedValue opacity;
            opacity.SetValueNow(0.0f);
            iIconMarked->SetOpacity(opacity);
            }
        }
    iLayout->UpdateChildrenLayout();
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item is requested to draw itself
// within specified parent layout.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemVisualizer::ShowL( CAlfLayout& aParentLayout, const TUint aTimeout )
    {
    FUNC_LOG;
    if ( !iLayout )
        {
        iParentLayout = &aParentLayout;

        iLayout = CAlfAnchorLayout::AddNewL(iOwnerControl, iParentLayout);
        iLayout->EnableBrushesL();
        iLayout->SetClipping(ETrue);

        if ( !iIconVisual )
            {
            iIconVisual =
                CAlfImageVisual::AddNewL( iOwnerControl, iLayout );
            iIconVisual->SetScaleMode( CAlfImageVisual::EScaleFit );
            }

        if ( !iIconMarked )
            {
            iIconMarked =
                CAlfImageVisual::AddNewL( iOwnerControl, iLayout );
            iIconMarked->SetScaleMode( CAlfImageVisual::EScaleFit );
            }

        if ( !iIconMenu )
            {
            iIconMenu =
                CAlfImageVisual::AddNewL( iOwnerControl, iLayout );
            iIconMenu->SetScaleMode( CAlfImageVisual::EScaleFit );
            }
        }
    else
        {
        //visuals are already created but not attached to the list layout
        if (!iLayout->Layout())
            {
            aParentLayout.Append(iLayout, aTimeout);
            iParentLayout = &aParentLayout;
            }
        }
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item is requested to update its
// visual content.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemVisualizer::UpdateL(
    const MFsTreeItemData& aData,
    TBool /*aFocused*/,
    const TUint aLevel,
    CAlfTexture*& aMarkIcon,
    CAlfTexture*& aMenuIcon,
    const TUint /*aTimeout*/,
    TBool aUpdateData)
    {
    FUNC_LOG;
    if ( iLayout )
        {
        iSize.iWidth = iParentLayout->Size().Target().iX -
                       iParentLayout->PaddingInPixels().Width();
        iExtendedSize.iWidth = iSize.iWidth;

        if ( KFsTreeRichItemDataType == aData.Type() )
            {
            const CFsTreeRichItemData* data(
                static_cast<const CFsTreeRichItemData*>(&aData));

            if (iIconVisual)
                {
                if ( data->IsIconSet() )
                    {
                    iIconVisual->SetScaleMode( CAlfImageVisual::EScaleFit );
                    iIconVisual->SetImage( TAlfImage(data->Icon()));
                    }
                }

            if (iFlags & KFsTreeListItemMarked)
                {
                iIconMarked->SetScaleMode( CAlfImageVisual::EScaleFit );
                iIconMarked->SetImage(TAlfImage(*aMarkIcon));
                }

            if (iFlags & KFsTreeListItemHasMenu)
                {
                iIconMenu->SetScaleMode( CAlfImageVisual::EScaleFit );
                iIconMenu->SetImage(TAlfImage(*aMenuIcon));
                }

            iLayout->Env().SetRefreshMode(EAlfRefreshModeManual);

            if (iTextLayout)
                {
                iTextLayout->RemoveAndDestroyAllD();
                }
            iTextLayout = CAlfFlowLayout::AddNewL(iOwnerControl, iLayout);
            iTextLayout->SetFlowDirection(CAlfFlowLayout::EFlowHorizontal);

            const TInt indent( iTextIndentation * ( aLevel - 1 ) );
            UpdateLayout(data, indent);

            if (aUpdateData)
                {
                CreateTextLineL(iOwnerControl, iTextLayout, data,
                        iTextLayout->Size().Target().AsSize());
                }

            iLayout->Env().SetRefreshMode(EAlfRefreshModeAutomatic);

            }
        }
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item goes out of the visible items
// scope.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemVisualizer::Hide( const TInt aTimeout )
    {
    FUNC_LOG;
    if ( iLayout )
        {
        iParentLayout->Remove( iLayout, aTimeout );
        iLayout->RemoveAndDestroyAllD();
        iLayout = NULL;
        iParentLayout = NULL;
        iTextLayout = NULL;
        iIconMarked = NULL;
        iIconMenu = NULL;
        iIconVisual = NULL;
        }
    }


// ---------------------------------------------------------------------------
//  From MFsTreeItemVisualizer.
//  This method marquees the text in tree item if it is too long.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemVisualizer::MarqueeL(const TFsTextMarqueeType /*aMarqueeType*/,
                                         const TUint /*aMarqueeSpeed*/,
                                         const TInt /*aMarqueeStartDelay*/,
                                         const TInt /*aMarqueeCycleStartDelay*/,
                                         const TInt /*aMarqueeRepetitions*/)
    {
    FUNC_LOG;
    }

