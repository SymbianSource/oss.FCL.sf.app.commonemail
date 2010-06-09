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
* Description:  A visualizer for data items with plain text.
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "emailtrace.h"
#include "fstreeplainonelineitemvisualizer.h"
#include "fstreeplainonelineitemdata.h"
#include "fstreelist.h"
#include "fslayoutmanager.h"
#include "fstextstylemanager.h"
#include "fsalftextstylemanager.h"
#include "fsmarqueeclet.h"
#include <touchlogicalfeedback.h>

// <cmail> SF
#include <alf/alfanchorlayout.h>
#include <alf/alfimagevisual.h>
#include <alf/alfviewportlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfimage.h>
#include <alf/alfenv.h>
#include <alf/alftextstyle.h>

#include <alf/alfevent.h>
#include <alf/alfmappingfunctions.h>
// </cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreePlainOneLineItemVisualizer*
    CFsTreePlainOneLineItemVisualizer::NewL( CAlfControl& aOwnerControl )
    {
    CFsTreePlainOneLineItemVisualizer* self =
        new( ELeave ) CFsTreePlainOneLineItemVisualizer( aOwnerControl );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainOneLineItemVisualizer::~CFsTreePlainOneLineItemVisualizer()
    {
    FUNC_LOG;
    delete iTextMarquee;
    }


// ---------------------------------------------------------------------------
// Returns visualizer's type.
// ---------------------------------------------------------------------------
//
TFsTreeItemVisualizerType CFsTreePlainOneLineItemVisualizer::Type() const
    {
    FUNC_LOG;
    return EFsTreePlainOneLineItemVisualizer;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Sets the size of the item visualization when item is not in expanded
// state. The value is taken from layout manager.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemVisualizer::UpdateLayout(
        const CFsTreePlainOneLineItemData* aData,
        const TInt aIndentation )
    {
    FUNC_LOG;
    TRect rect;

    TAlfTimedPoint tpMainIconTl, tpMainIconBr;
    TAlfTimedPoint tpTextTl, tpTextBr;
    TAlfTimedPoint tpMarkIconTl, tpMarkIconBr;
    TAlfTimedPoint tpMenuIconTl, tpMenuIconBr;

    CFsLayoutManager::TFsLayoutMetrics mainIconMetrics =
        CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
    CFsLayoutManager::TFsLayoutMetrics markIconMetrics =
        CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
    CFsLayoutManager::TFsLayoutMetrics menuIconMetrics =
        CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
    CFsLayoutManager::TFsLayoutMetrics textMetrics =
        CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;

    TInt mainIconVariety( 0 );
    TInt markIconVariety( 0 );
    TInt menuIconVariety( 0 );
    TInt textPaneVariety( 0 );

    if (aData->IsIconSet() || IsLayoutHintSet( CFsTreeItemVisualizerBase::EPopupLayout ))
        {
        if (iFlags & KFsTreeListItemMarked)
            {
            if (iMenu && (iFlags & KFsTreeListItemFocused))
                {
                //mainIcon + markIcon + menuIcon
                //the metrics defined when declared
                mainIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                menuIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
                markIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                mainIconVariety = 3;
                textPaneVariety = 3;
                menuIconVariety = 1;
                markIconVariety = 2;
                }
            else
                {
                //mainIcon + markIcon
                mainIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                markIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                mainIconVariety = 4;
                textPaneVariety = 4;
                markIconVariety = 3;
                }
            }
        else
            {
            if ((iFlags & KFsTreeListItemHasMenu)
                    && (iFlags & KFsTreeListItemFocused))
                {
                //mainIcon + menuIcon
                mainIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                menuIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                mainIconVariety = 4;
                textPaneVariety = 4;
                menuIconVariety = 3;
                }
            else
                {
                if ( IsLayoutHintSet( EFolderLayout ) )
	                {
	                //mainIcon
	                mainIconMetrics = CFsLayoutManager::EFsLmFolderListMediumLineG1;
	                textMetrics     = CFsLayoutManager::EFsLmFolderListMediumLineT1;
                    }
	            else
		            {
	                //mainIcon
                    textMetrics     = CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneT1;
                    mainIconMetrics = CFsLayoutManager::EFsLmMainSpFsCtrlbarDdmenuPaneG2;
                    mainIconVariety = 0;
                    textPaneVariety = 2;
                    }
                }
            }
        }
    else // !aData->IsIconSet()
        {
        if (iFlags & KFsTreeListItemMarked)
            {
            if (iMenu && (iFlags & KFsTreeListItemFocused))
                {
                //markIcon + menuIcon
                menuIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
                markIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                menuIconVariety = 2;
                markIconVariety = 4;
                textPaneVariety = 6;
                }
            else
                {
                //markIcon
                markIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                markIconVariety = 5;
                textPaneVariety = 7;
                }
            }
        else
            {
            if ((iFlags & KFsTreeListItemHasMenu)
                    && (iFlags & KFsTreeListItemFocused))
                {
                //menuIcon
                menuIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                menuIconVariety = 5;
                textPaneVariety = 7;
                }
            else
                {
                //plain item
                textPaneVariety = 8;
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
                CFsLayoutManager::EFsLmListSingleDycRowPane,
                iSize );
        }

    TRect currentSize = iSize;
    if ( !IsLayoutHintSet( EFolderLayout ) )
        {
        CFsLayoutManager::LayoutMetricsRect(currentSize, CFsLayoutManager::EFsLmListSingleDycRowPane, currentSize);
        }
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
                rect,
                mainIconVariety );
    tpMainIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpMainIconBr.SetTarget(TAlfRealPoint(rect.iBr));

    CFsLayoutManager::LayoutMetricsRect(
            currentSize,
            markIconMetrics,
            rect,
            markIconVariety );
    tpMarkIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpMarkIconBr.SetTarget(TAlfRealPoint(rect.iBr));

    CFsLayoutManager::LayoutMetricsRect(
            currentSize,
            menuIconMetrics,
            rect,
            menuIconVariety );
    tpMenuIconTl.SetTarget(TAlfRealPoint(rect.iTl));
    tpMenuIconBr.SetTarget(TAlfRealPoint(rect.iBr));

    TRect textRect( currentSize );
    if (textMetrics == CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1)
        {
        CFsLayoutManager::LayoutMetricsRect(
                    currentSize,
                    CFsLayoutManager::EFsLmListSingleDycRowTextPane,
                    textRect,
                    textPaneVariety );
        }
    CFsLayoutManager::TFsText textInfo;
    /*CFsLayoutManager::LayoutMetricsText(currentSize, textMetrics, textInfo);*/

    CFsLayoutManager::LayoutMetricsText(textRect,
            textMetrics,
            textInfo, textPaneVariety);
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

    if (iViewportLayout)
        {
        TInt textVisIndex = iLayout->FindVisual(iViewportLayout);
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
void CFsTreePlainOneLineItemVisualizer::ShowL( CAlfLayout& aParentLayout,
                                        const TUint aTimeout )
    {
    FUNC_LOG;
    if (!iLayout)
        {
        iParentLayout = &aParentLayout;
        iLayout = CAlfAnchorLayout::AddNewL(iOwnerControl,iParentLayout);
        iLayout->SetTactileFeedbackL( ETouchEventStylusDown, ETouchFeedbackBasic );
        iLayout->EnableBrushesL();
        iLayout->SetClipping(ETrue);

        if (!iIconVisual)
            {
            iIconVisual =
                        CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            // <cmail> aspect ratio preserved
            iIconVisual->SetScaleMode( CAlfImageVisual::EScaleFitInside);
            iIconVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iViewportLayout) 
            {
            iViewportLayout =
                      CAlfViewportLayout::AddNewL(iOwnerControl, iLayout);
            iViewportLayout->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iTextVisual)
            {
            iTextVisual =
                      CAlfTextVisual::AddNewL(iOwnerControl, iViewportLayout);
            iTextVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iIconMarked)
            {
            iIconMarked = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iIconMarked->SetScaleMode( CAlfImageVisual::EScaleFitInside);
            iIconMarked->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iIconMenu)
            {
            iIconMenu = CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iIconMenu->SetScaleMode(CAlfImageVisual::EScaleFitInside);
            iIconMenu->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        iTextMarquee = new (ELeave) TFsMarqueeClet(iOwnerControl, *iViewportLayout, TFsMarqueeClet::EScrollFromRight, TFsMarqueeClet::EScrollForth);
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
// A call to this function means that the item is requested to update its
// visual content.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemVisualizer::UpdateL( const MFsTreeItemData& aData,
                                          TBool aFocused,
                                          const TUint aLevel,
                                          CAlfTexture*& aMarkIcon,
                                          CAlfTexture*& aMenuIcon,
                                          const TUint /*aTimeout*/,
                                          TBool aUpdateData)
    {
    FUNC_LOG;
    if (iLayout)
        {
        iSize.iWidth = iParentLayout->Size().Target().iX;
        iExtendedSize.iWidth = iSize.iWidth;
        
        TSize layoutSize(iLayout->Size().Target().iX, iLayout->Size().Target().iY);
        layoutSize.iWidth = iSize.iWidth;
        iLayout->SetSize(layoutSize);

        if (aFocused)
            {
            if (IsFocusable())
                {
                iFlags |= KFsTreeListItemFocused;
                }
            }
        else
            {
            iFlags &= ~KFsTreeListItemFocused;
            }

        if ( aData.Type() == KFsTreePlainOneLineItemDataType )
            {
            const CFsTreePlainOneLineItemData* data =
                      static_cast<const CFsTreePlainOneLineItemData*>(&aData);

            //update the ancors for current item settings
            const TInt indent( iTextIndentation * ( aLevel - 1 ) );
            UpdateLayout(data, indent);

            if (iIconVisual && data->IsIconSet())
                {
                // <cmail> aspect ratio preserved
                iIconVisual->SetScaleMode( CAlfImageVisual::EScaleFitInside );
                // </cmail>
                iIconVisual->SetImage( TAlfImage( data->Icon() ) );
                }

            if ( iTextVisual && aUpdateData )
                {
                //get text color
                TRgb textColor;
                if ( aFocused )
                    {
                    if (!iIsCustomTextFocusedColor)
                        {//custom color not set, use color from skin - load it in case theme has changed
                        iFocusedTextColor = FocusedStateTextSkinColor();
                        }
                    textColor = iFocusedTextColor;
                    iFlags |= KFsTreeListItemFocused;
                    }
                else
                    {
                    if (!iIsCustomTextNormalColor)
                        {//custom color not set, use color from skin - load it in case theme has changed
                        iNormalTextColor = NormalStateTextSkinColor( );
                        }
                    textColor = iNormalTextColor;
                    iFlags &= ~KFsTreeListItemFocused;
                    }
                //get text style
                TInt styleId = ModifiedStyleIdL( );
                iTextVisual->Env().CancelCustomCommands(this);

                TAlfTimedPoint tp(0,0);
                tp.SetStyle(EAlfTimedValueStyleLinear);
                iTextVisual->SetPos(tp);
                iTextVisual->SetTextL(data->Data( ));

                iTextVisual->EnableShadow(iFlags & KFsTreeListItemTextShadow);
                iTextVisual->SetColor( textColor );
                iTextVisual->SetTextStyle( styleId );

                if ( iManualWrapper )
                	{
                	iTextVisual->SetWrapping( CAlfTextVisual::ELineWrapManual );
                	}
                else
                	{
					iTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
                	}

                TAlfAlignHorizontal currTextAlign( EAlfAlignHLocale );
                currTextAlign = CurrentTextAlignmentL( iTextAlign, &data->Data(), iTextVisual );
                iTextVisual->SetAlign( currTextAlign, EAlfAlignVCenter );

                if ( currTextAlign == EAlfAlignHRight )
                    {
                    iTextMarquee->SetScrollDirection( TFsMarqueeClet::EScrollFromLeft );
                    }
                else
                    {
                    iTextMarquee->SetScrollDirection( TFsMarqueeClet::EScrollFromRight );
                    }
                }

            if ( IsMarked() )
                {
                // <cmail> aspect ratio preserved
                iIconMarked->SetScaleMode( CAlfImageVisual::EScaleFitInside);
                // </cmail>
                iIconMarked->SetImage( *aMarkIcon );
                }

            if ((iFlags & KFsTreeListItemHasMenu)
                    && (iFlags & KFsTreeListItemFocused))
                {
                if ( iIconMenu && aMenuIcon )
                    {
                iIconMenu->SetScaleMode( CAlfImageVisual::EScaleFit );
                iIconMenu->SetImage( *aMenuIcon );
                }
                }

            iTextMarquee->StopScrolling();    
            
            if ( iTextVisual )
            	{
				if ( iManualWrapper )
					{
					iTextVisual->SetWrapping( CAlfTextVisual::ELineWrapManual );
					}
				else
					{
					iTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
					}
            	}
            
            iManualWrapper = EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
//  From MFsTreeItemVisualizer.
//  This method sets flag disable wrapping the text item.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemVisualizer::OffWrapping()
	{
	iManualWrapper = ETrue;
	}

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item goes out of the visible items
// scope.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemVisualizer::Hide( const TInt aTimeout )
    {
    FUNC_LOG;
    if ( iLayout )
        {
        //if marquee command was sent
        iTextVisual->Env().CancelCustomCommands(this);

        delete iTextMarquee;
        iTextMarquee = NULL; //it has to be here

        iParentLayout->Remove(iLayout, aTimeout);
        iLayout->RemoveAndDestroyAllD();
        iLayout = NULL;
        iTextVisual = NULL;
        iIconVisual = NULL;
        iIconMarked = NULL;
        iIconMenu = NULL;
        iParentLayout = NULL;
        iViewportLayout = NULL;
        }
    }

// ---------------------------------------------------------------------------
//  From MFsTreeItemVisualizer.
//  Method to marquee the text when it's too long.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemVisualizer::MarqueeL(const TFsTextMarqueeType aMarqueeType,
                                                 const TUint aMarqueeSpeed,
                                                 const TInt aMarqueeStartDelay,
                                                 const TInt aMarqueeCycleStartDelay,
                                                 const TInt aMarqueeRepetitions)
    {
    FUNC_LOG;
    TInt marqueeSize(0), textWidth(0);

    if ( aMarqueeType == EFsTextMarqueeNone || !iTextVisual )
        {
        return;
        }

    iTextVisual->SetWrapping(CAlfTextVisual::ELineWrapManual);

    textWidth = iTextVisual->SubstringExtents(0, iTextVisual->Text().Length()).Width();
    marqueeSize = textWidth - iViewportLayout->Size().iX.Target();

    if ( iTextVisual && ( marqueeSize>0 ) )
        {
        TSize size(textWidth,iViewportLayout->Size().iY.Target());
        if ( aMarqueeType == EFsTextMarqueeForth )
            {
            iTextMarquee->SetScrollType( TFsMarqueeClet::EScrollForth );
            }
        else if ( aMarqueeType == EFsTextMarqueeForthBack )
            {
            iTextMarquee->SetScrollType( TFsMarqueeClet::EScrollBounce );
            }
        else if ( aMarqueeType == EFsTextMarqueeLoop )
            {
            iTextMarquee->SetScrollType( TFsMarqueeClet::EScrollWrap );
            //duplicate text
            TBufC <1> separator(_L(" "));
            HBufC* textHeap = HBufC::NewLC( (iTextVisual->Text().Length()+1)*2+1);
            TPtr doubleString(textHeap->Des());
            doubleString = iTextVisual->Text();
            doubleString.Append(_L(" "));
            doubleString.Append(iTextVisual->Text());
            doubleString.Append(_L(" "));
            iTextVisual->SetTextL( doubleString );
            CleanupStack::PopAndDestroy(textHeap);
            size.iWidth = iTextVisual->SubstringExtents(0, iTextVisual->Text().Length()).Width();
            }

        iTextMarquee->SetScrollRepetitions( aMarqueeRepetitions );
        TInt time(0);
        if ( aMarqueeType == EFsTextMarqueeLoop )
            {
            time = textWidth * 1000 / aMarqueeSpeed;
            }
        else
            {
            time = marqueeSize * 1000 / aMarqueeSpeed;
            }
        iTextMarquee->SetScrollSpeed( time );
        iTextMarquee->SetScrollableArea( size );
        iTextMarquee->SetScrollCycleDelay( aMarqueeCycleStartDelay );

        TAlfCustomEventCommand cmd( aMarqueeType, this, 0 );
        iTextVisual->Env().CancelCustomCommands( this, aMarqueeType );
        iTextVisual->Env().Send( cmd, aMarqueeStartDelay );
        }
    }



// ---------------------------------------------------------------------------
//  Handles custom Alf event. Used with text marquee.
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainOneLineItemVisualizer::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool ret( EFalse );
    if ( aEvent.IsCustomEvent() )
        {
        TInt param = aEvent.CustomParameter();
        if ( param == EFsTextMarqueeForth ||
             param == EFsTextMarqueeForthBack ||
             param == EFsTextMarqueeLoop )
            {
            ret = ETrue;
            iTextMarquee->StartScrolling( );
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainOneLineItemVisualizer::CFsTreePlainOneLineItemVisualizer
                                                ( CAlfControl& aOwnerControl )
    : CFsTreeItemVisualizerBase( aOwnerControl )
    {
    FUNC_LOG;
    
    iManualWrapper = EFalse;

    }


// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemVisualizer::ConstructL( )
    {
    FUNC_LOG;
    CFsTreeItemVisualizerBase::ConstructL();

    //get default skin values
    iNormalTextColor = NormalStateTextSkinColor();
    iFocusedTextColor = FocusedStateTextSkinColor();
    }

