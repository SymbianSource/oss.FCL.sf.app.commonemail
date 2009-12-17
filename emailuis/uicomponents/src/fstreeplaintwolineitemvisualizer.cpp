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
*  Description : A visualizer for data items with plain text.
*  Version     : %version: tr1sido#12 %
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include "emailtrace.h"
#include <AknsUtils.h>
#include <touchlogicalfeedback.h>

#include "fstreeplaintwolineitemvisualizer.h"
#include "fstreeplaintwolineitemdata.h"
#include "fsgenericpanic.h"
#include "fslayoutmanager.h"
#include "fstextstylemanager.h"
#include "fsmarqueeclet.h"
// <cmail> SF
#include <alf/alfanchorlayout.h>
#include <alf/alfimagevisual.h>
#include <alf/alfviewportlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfimage.h>
#include <alf/alfenv.h>
#include <alf/alfcontrol.h>

#include <alf/alfevent.h>
#include <alf/alfmappingfunctions.h>
// </cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreePlainTwoLineItemVisualizer*
    CFsTreePlainTwoLineItemVisualizer::NewL( CAlfControl& aOwnerControl )
    {
    CFsTreePlainTwoLineItemVisualizer* self =
        new( ELeave ) CFsTreePlainTwoLineItemVisualizer( aOwnerControl );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainTwoLineItemVisualizer::~CFsTreePlainTwoLineItemVisualizer()
    {
    FUNC_LOG;
    delete iSecondaryTextMarquee;
    if ( iKeyWords.Count() > 0 )
        {
        iKeyWords.Close();
        }
    }


// ---------------------------------------------------------------------------
// Returns visualizer's type.
// ---------------------------------------------------------------------------
//
TFsTreeItemVisualizerType CFsTreePlainTwoLineItemVisualizer::Type() const
    {
    FUNC_LOG;
    return EFsTreePlainTwoLineItemVisualizer;
    }


// ---------------------------------------------------------------------------
//  Sets the alignment for text item.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsTreePlainTwoLineItemVisualizer::SetSecondaryTextAlign(
    const TAlfAlignHorizontal aAlign )
    {
    FUNC_LOG;
    iSecondaryTextAlign = aAlign;
    }

// ---------------------------------------------------------------------------
//  Turn on/off displaying preview pane (3rd line).
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::SetPreviewPaneOn( TBool aPreviewOn )
    {
    FUNC_LOG;
    iPreviewPaneOn = aPreviewOn;
    }

// ---------------------------------------------------------------------------
//  Retrurns information whether preview pane is turned on.
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainTwoLineItemVisualizer::IsPreviewPaneOn () const
    {
    FUNC_LOG;
    return iPreviewPaneOn;
    }

// ---------------------------------------------------------------------------
//  Sets the size of the item visualization area when it has preview pane
//  turned on.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::SetPreviewPaneEnabledSize (
                                                            const TSize aSize)
    {
    FUNC_LOG;
    iPreviewPaneEnabledSize = aSize;
    }

// ---------------------------------------------------------------------------
//  Function returns the size of an item when it has preview pane turned on.
// ---------------------------------------------------------------------------
//
TSize CFsTreePlainTwoLineItemVisualizer::PreviewPaneEnabledSize() const
    {
    FUNC_LOG;
    if ( iFlags & KFsTreeListItemManagedLayout )
        {
        CFsLayoutManager::TFsLayoutMetrics previewPaneSizeMetrics(CFsLayoutManager::EFsLmListMediumLineX3);
        TRect parentLayoutRect(0,0,0,0);
        TSize prevPaneSize(0,0);
        TBool retVal(EFalse);

        retVal = CFsLayoutManager::LayoutMetricsSize(
                                    parentLayoutRect,
                                    previewPaneSizeMetrics,
                                    prevPaneSize );

        if ( !retVal )
            {
            prevPaneSize = iPreviewPaneEnabledSize;
            }

        return prevPaneSize;
        }
    else
        {
        return iPreviewPaneEnabledSize;
        }
    }

// ---------------------------------------------------------------------------
//  Function sets wether flag icon should be visible or hidden.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::SetFlagIconVisible( TBool aShowFlag )
    {
    FUNC_LOG;
    iShowFlagIcon = aShowFlag;
    }

// ---------------------------------------------------------------------------
//  Function returns information if flag icon is visible or hidden.
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainTwoLineItemVisualizer::IsFlagIconVisible( ) const
    {
    FUNC_LOG;
    return iShowFlagIcon;
    }

// ---------------------------------------------------------------------------
//  The function sets whether time visual should be visible when item is
//  not focused (in one line layout).
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::SetIsTimeVisibleInFirstLineWhenNotFocused( TBool aIsVisible )
    {
    FUNC_LOG;
    iIsTimeVisibleWhenNotFocused = aIsVisible;
    }

// ---------------------------------------------------------------------------
//  The function returns whether time visual is visible when item is not
//  focused (in one line layout).
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainTwoLineItemVisualizer::IsTimeVisibleInFirstLineWhenNotFocused( ) const
    {
    FUNC_LOG;
    return iIsTimeVisibleWhenNotFocused;
    }


// From class MFsTreeItemVisualizer.


// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns the size of an item in a normal (not extended) state.
// ---------------------------------------------------------------------------
//
TSize CFsTreePlainTwoLineItemVisualizer::Size() const
    {
    FUNC_LOG;
    if ( IsAlwaysExtended() )
        {
        //return iExtendedSize;
        return CFsTreeItemVisualizerBase::ExtendedSize();
        }
    else
        {
        //return iSize;
        return CFsTreeItemVisualizerBase::Size();
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// Returns the size of an item in expanded state.
// ---------------------------------------------------------------------------
//
TSize CFsTreePlainTwoLineItemVisualizer::ExtendedSize() const
    {
    FUNC_LOG;
    if ( IsPreviewPaneOn() )
        {
        //return iPreviewPaneEnabledSize;
        return PreviewPaneEnabledSize();
        }
    else
        {
        //return iExtendedSize;
        return CFsTreeItemVisualizerBase::ExtendedSize();
        }
    }


// ---------------------------------------------------------------------------
// Updates the layout of the item beeing updated. Values are served by
// layout manager.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::UpdateLayout(
        const CFsTreePlainTwoLineItemData* aData,
        const TInt aIndentation
        )
    {
    FUNC_LOG;
    TRect rect;

    TAlfTimedPoint tpMainIconTl, tpMainIconBr;
    TAlfTimedPoint tpMainTextTl, tpMainTextBr;
    TAlfTimedPoint tpDateTextTl, tpDateTextBr;
    TAlfTimedPoint tpMarkIconTl, tpMarkIconBr;
    TAlfTimedPoint tpMenuIconTl, tpMenuIconBr;
    TAlfTimedPoint tpFlagIconTl, tpFlagIconBr;
    TAlfTimedPoint tpSubjTextTl, tpSubjTextBr;
    TAlfTimedPoint tpPrevTextTl, tpPrevTextBr;
    
    // <cmail> Layouts
    
/*
    CFsLayoutManager::TFsLayoutMetrics
        mainIconMetrics = CFsLayoutManager::EFsLmListMediumLineT2G3G1,
        markIconMetrics = CFsLayoutManager::EFsLmListMediumLineT2G3G2,
        menuIconMetrics = CFsLayoutManager::EFsLmListMediumLineT2G3G3,
        //<cmail> ???        
        sizeMetrics     = CFsLayoutManager::EFsLmListSingleDycRowPane,
        //</cmail>
        exSizeMetrics   = CFsLayoutManager::EFsLmListMediumLineX2,
        preSizeMetrics  = CFsLayoutManager::EFsLmListMediumLineX3,
        flagIconMetrics = CFsLayoutManager::EFsLmListMediumLineX2G1;
*/

    CFsLayoutManager::TFsLayoutMetrics
        mainIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1,
        markIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG4,
        menuIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG3,
        sizeMetrics     = CFsLayoutManager::EFsLmListSingleDycRowPane,
        exSizeMetrics   = CFsLayoutManager::EFsLmListSingleFsDycPane,
        preSizeMetrics  = CFsLayoutManager::EFsLmListSingleFsDycPane,
        flagIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
    
    TInt mainIconVariety( 0 );
    TInt mainIconRow( 0 );
    TInt markIconVariety( 0 );
    TInt markIconRow( 0 );
    TInt menuIconVariety( 0 );
    TInt menuIconRow( 0 );
    
    TInt sizeVariety( 0 );
    TInt exSizeVariety( 1 );
    TInt preSizeVariety( 2 );
    TInt flagIconVariety( 0 );
    TInt flagIconRow( 0 );

    
    CFsLayoutManager::TFsLayoutMetrics firstLineTextParentMetrics = 
    CFsLayoutManager::EFsLmListSingleDycRowTextPane;
    TInt firstLineTextParentVariety = 0;

    //values are assigned to avoid compiler warnings
    CFsLayoutManager::TFsLayoutMetrics firstLineTextMetrics[3] =
        {
                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1,
                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2,
                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2
        };
    TInt firstLineTextVarieties[3] =
            {
                    0,
                    0,
                    0
            };
    CFsLayoutManager::TFsLayoutMetrics secondLineTextMetrics =
        CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
    TInt secondLineTextVariety = 0;
    CFsLayoutManager::TFsLayoutMetrics thirdLineTextMetrics =
        CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
    TInt thirdLineTextVariety = 0;

    TInt firstLineTextFields = 0;

    if ((iFlags & KFsTreeListItemAlwaysExtended) ||
            (iFlags & KFsTreeListItemExtended))
        {
        //item extended
        TAlfTimedValue opacity;
        firstLineTextFields = 2;
        if (aData->DataLength()==0)
            {
            firstLineTextFields--;
            }
        if (aData->DateTimeDataLength()==0)
            {
            firstLineTextFields--;
            opacity.SetValueNow(0.0f);
            iDateTimeTextVisual->SetOpacity(opacity);
            }
        else
            {
            opacity.SetValueNow(1.0f);
            iDateTimeTextVisual->SetOpacity(opacity);
            }

        if (aData->IsIconSet())
            {
            if (iFlags & KFsTreeListItemMarked)
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //mainIcon + markIcon + menuIcon
                    mainIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG1;//EFsLmListMediumLineX2T3G4G1;
                    mainIconVariety = 3;
                    if (IsPreviewPaneOn())
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;//EFsLmListMediumLineX3T4G4G4;
                        markIconVariety = 5;
						markIconRow = 2;
                        }
                    else
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG3;//EFsLmListMediumLineX2T3G4G2;
                        markIconVariety = 1;
						markIconRow = 0;
                        }
                    menuIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                    menuIconVariety = 2;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                              CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 3;
                        firstLineTextParentVariety = 3;
                        }
                    else if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 21;
                        firstLineTextParentVariety = 3;
                        if (IsPreviewPaneOn())
                            {
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 13;
                            firstLineTextParentVariety = 4;
                            }
                        else
                            {
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 12;
                            firstLineTextParentVariety = 3;
                            }
                        }
                    }
                else
                    {
                    //mainIcon + markIcon
                    mainIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                    mainIconVariety = 4;
                    markIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                    markIconVariety = 3;
                    firstLineTextParentVariety = 4;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 4;
                        }
                    else if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 13;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 4;
                        }
                    }
                }
            else
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //mainIcon + menuIcon
                    mainIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                    mainIconVariety = 4;
                    menuIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                    menuIconVariety = 3;
                    firstLineTextParentVariety = 4;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 4;
                        }
                    else if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 13;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 4;
                        }
                    }
                else
                    {
                    //mainIcon
                    mainIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                    mainIconVariety = 4;
                    firstLineTextParentVariety = 5;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 5;
                        }
                    else if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 23;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 14;
                        }
                    }
                }
            secondLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
            secondLineTextVariety = 5;
            flagIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
            flagIconVariety = 3;
            }
        else
            {
            if (iFlags & KFsTreeListItemMarked)
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //markIcon + menuIcon
                    if (IsPreviewPaneOn())
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        markIconVariety = 5;
						markIconRow = 2;
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 5;
                        }
                    else
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        markIconVariety = 4;
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
                        menuIconVariety = 2;
                        }
                    firstLineTextParentVariety = 6;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                              CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 6;
                        }
                    else if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 24;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 15;
                        }
                    }
                else
                    {
                    //markIcon
                    if (IsPreviewPaneOn())
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        markIconVariety = 5;
						markIconRow = 2;
                        }
                    else
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        markIconVariety = 5;
                        }
                    if (firstLineTextFields==1)
                        {
                        if (IsPreviewPaneOn())
                            {
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 8;
                            firstLineTextParentVariety = 8;
                            }
                        else
                            {
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 7;
                            firstLineTextParentVariety = 7;
                            }
                        }
                    else if (firstLineTextFields==2)
                        {
                        if (IsPreviewPaneOn())
                            {
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 26;
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 17;
                            firstLineTextParentVariety = 8;
                            }
                        else
                            {
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 25;
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 16;
                            firstLineTextParentVariety = 7;
                            }
                        }
                    }
                }
            else
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //menuIcon
                    if (firstLineTextFields==1)
                        {
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 7;
                        firstLineTextParentVariety = 7;
                        }
                    else if (firstLineTextFields==2)
                        {
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 25;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 16;
                        firstLineTextParentVariety = 7;
                        }
                    }
                else
                    {
                    //plain item
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 8;
                        firstLineTextParentVariety = 8;
                        }
                    else if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 26;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 17;
                        firstLineTextParentVariety = 8;
                        }
                    }
                }

            if (aData->IsFlagIconSet())
                {
                secondLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                secondLineTextVariety = 5;
                flagIconMetrics = CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                flagIconVariety = 5;
				flagIconRow = 1;
                }
            else
                {
                secondLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                secondLineTextVariety = 8;
                }
            }
        //<cmail> ???
        thirdLineTextMetrics = CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
        thirdLineTextVariety = 4;
        //</cmail>
        }
    else
        {

        firstLineTextFields = 3;

        TAlfTimedValue opacity;

        if (aData->DataLength()==0)
            {
            firstLineTextFields--;
            opacity.SetValueNow(0.0f);
            iViewportLayout->SetOpacity(opacity);
            }
        else
            {
            opacity.SetValueNow(1.0f);
            iViewportLayout->SetOpacity(opacity);
            }

        if (aData->SecondaryDataLength()==0)
            {
            firstLineTextFields--;
            opacity.SetValueNow(0.0f);
            iSecViewport->SetOpacity(opacity);
            }
        else
            {
            opacity.SetValueNow(1.0f);
            iSecViewport->SetOpacity(opacity);
            }

        if (aData->DateTimeDataLength()==0)
            {
            firstLineTextFields--;
            opacity.SetValueNow(0.0f);
            iDateTimeTextVisual->SetOpacity(opacity);
            }
        else
            {
            opacity.SetValueNow(1.0f);
            iDateTimeTextVisual->SetOpacity(opacity);
            }

        if (aData->IsIconSet())
            {
            if (iFlags & KFsTreeListItemMarked)
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //mainIcon + markIcon + menuIcon
                    mainIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                    mainIconVariety = 3;
                    markIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
                    markIconVariety = 1;
                    menuIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                    menuIconVariety = 2;
                    firstLineTextParentVariety = 3;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 3;
                        }
                    if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 21;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 12;
                        }
                    if (firstLineTextFields==3)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 30;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 21;
                        firstLineTextMetrics[2] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT3;
                        firstLineTextVarieties[2] = 3;
                        }
                    }
                else
                    {
                    //mainIcon + markIcon
                    mainIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                    mainIconVariety = 4;
                    markIconMetrics =
                        CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                    markIconVariety = 3;
                    firstLineTextParentVariety = 4;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 4;
                        }
                    if (firstLineTextFields==2)
                        {
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 22;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 13;
                        }
                    if (firstLineTextFields==3)
                        {
                        if ( IsTimeVisibleInFirstLineWhenNotFocused() )
                            {
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 31;
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 22;
                            firstLineTextMetrics[2] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT3;
                            firstLineTextVarieties[2] = 4;
                            if ( iDateTimeTextVisual )
                                {
                                opacity.SetValueNow(1.0f);
                                iDateTimeTextVisual->SetOpacity(opacity);
                                }
                            }
                        else
                            { //hide time visual - display sender and subject only
                            firstLineTextFields = 2;
                            //hide time visual
                            if ( iDateTimeTextVisual )
                                {
                                opacity.SetValueNow(0.0f);
                                iDateTimeTextVisual->SetOpacity(opacity);
                                }
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 22;
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 13;
                            }
                        }
                    }
                }
            else
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //mainIcon + menuIcon
                    firstLineTextParentVariety = 4;
                    if (firstLineTextFields==1)
                        {
                        mainIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                        mainIconVariety = 4;
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 3;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 4;
                        }
                    if (firstLineTextFields==2)
                        {
                        mainIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                        mainIconVariety = 4;
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 3;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 22;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 13;
                        }
                    if (firstLineTextFields==3)
                        {
                        mainIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                        mainIconVariety = 4;
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 3;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 31;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 22;
                        firstLineTextMetrics[2] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT3;
                        firstLineTextVarieties[2] = 4;
                        }
                    }
                else
                    {
                    //mainIcon
                    firstLineTextParentVariety = 5;
                    if (firstLineTextFields==1)
                        {
                        mainIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                        mainIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 5;
                        }
                    if (firstLineTextFields==2)
                        {
                        mainIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                        mainIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 23;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 14;
                        }
                    if (firstLineTextFields==3)
                        {
                        if ( IsTimeVisibleInFirstLineWhenNotFocused() )
                            {
                            mainIconMetrics =
                                CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                            mainIconVariety = 5;
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 32; 
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 23;
                            firstLineTextMetrics[2] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT3;
                            firstLineTextVarieties[2] = 5;
                            if ( iDateTimeTextVisual )
                                {
                                opacity.SetValueNow(1.0f);
                                iDateTimeTextVisual->SetOpacity(opacity);
                                }
                            }
                        else
                            { //hide time visual - display sender and subject only
                            firstLineTextFields = 2;
                            mainIconMetrics =
                                CFsLayoutManager::EFsLmListSingleDycRowPaneG1;
                            mainIconVariety = 5;
                            firstLineTextMetrics[0] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                            firstLineTextVarieties[0] = 23; 
                            firstLineTextMetrics[1] =
                                CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                            firstLineTextVarieties[1] = 14;
                            //hide time visual
                            if ( iDateTimeTextVisual )
                                {
                                opacity.SetValueNow(0.0f);
                                iDateTimeTextVisual->SetOpacity(opacity);
                                }
                            }
                        }
                    }
                }
            }
        else
            {
            if (iFlags & KFsTreeListItemMarked)
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //markIcon + menuIcon
                    if (firstLineTextFields==1)
                        {
                        firstLineTextParentVariety = 7;
                        firstLineTextMetrics[0] =
                             CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 7;
                        markIconMetrics =
                             CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
                        markIconVariety = 2;
                        menuIconMetrics =
                             CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 4;
                        }
                    else if (firstLineTextFields==2)
                        {
                        firstLineTextParentVariety = 6;
                        markIconMetrics =
                         CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
                        markIconVariety = 2;
                        menuIconMetrics =
                         CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 4;
                        firstLineTextMetrics[0] =
                         CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 24;
                        firstLineTextMetrics[1] =
                         CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 15;
                        }
                    else if (firstLineTextFields==3)
                        {
                        firstLineTextParentVariety = 6;
                        markIconMetrics =
                             CFsLayoutManager::EFsLmListSingleDycRowPaneG3;
                        markIconVariety = 2;
                        menuIconMetrics =
                             CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 4;
                        firstLineTextMetrics[0] =
                             CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 33;
                        firstLineTextMetrics[1] =
                             CFsLayoutManager::EFsLmListSingleDycRowTextPaneT3;
                        firstLineTextVarieties[1] = 24;
//                        firstLineTextMetrics[2] =
//                             CFsLayoutManager::EFsLmListMediumLineT3RightIconx2T3;
                        }
                    }
                else
                    {
                    //markIcon
                    firstLineTextParentVariety = 7;
                    if (firstLineTextFields==1)
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        markIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 7;
                        }
                    else if (firstLineTextFields==2)
                        {
                        markIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        markIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 25;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 16;
                        }
                    else if (firstLineTextFields==3)
                        {
//                        markIconMetrics =
//                            CFsLayoutManager::EFsLmListMediumLineT3RightIconG1;
//                        firstLineTextMetrics[0] =
//                            CFsLayoutManager::EFsLmListMediumLineT3RightIconT1;
//                        firstLineTextMetrics[1] =
//                            CFsLayoutManager::EFsLmListMediumLineT3RightIconT2;
//                        firstLineTextMetrics[2] =
//                            CFsLayoutManager::EFsLmListMediumLineT32RightIconT2;
                        }
                    }
                }
            else
                {
                if ((iFlags & KFsTreeListItemHasMenu)
                        && (iFlags & KFsTreeListItemFocused))
                    {
                    //menuIcon
                    firstLineTextParentVariety = 7;
                    if (firstLineTextFields==1)
                        {
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 7;
                        }
                    else if (firstLineTextFields==2)
                        {
                        menuIconMetrics =
                            CFsLayoutManager::EFsLmListSingleDycRowPaneG4;
                        menuIconVariety = 5;
                        firstLineTextMetrics[0] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 25;
                        firstLineTextMetrics[1] =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 16;
                        }
                    else if (firstLineTextFields==3)
                        {
//                        menuIconMetrics =
//                            CFsLayoutManager::EFsLmListMediumLineT3RightIconG1;
//                        firstLineTextMetrics[0] =
//                            CFsLayoutManager::EFsLmListMediumLineT3RightIconT1;
//                        firstLineTextMetrics[1] =
//                            CFsLayoutManager::EFsLmListMediumLineT3RightIconT2;
//                        firstLineTextMetrics[2] =
//                            CFsLayoutManager::EFsLmListMediumLineT3RightIconT2;
                        }
                    }
                else
                    {
                    //plain item
                    firstLineTextParentVariety = 8;
                    if (firstLineTextFields==1)
                        {
                        firstLineTextMetrics[0]     =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 8;
                        }
                    if (firstLineTextFields==2)
                        {
                        firstLineTextParentVariety = 8;
                        firstLineTextMetrics[0]     =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 26;
                        firstLineTextMetrics[1]     =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 17;
                        }
                    if (firstLineTextFields==3)
                        {
                        firstLineTextMetrics[0]     =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT1;
                        firstLineTextVarieties[0] = 35;
                        firstLineTextMetrics[1]     =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT2;
                        firstLineTextVarieties[1] = 26;
                        firstLineTextMetrics[2]     =
                            CFsLayoutManager::EFsLmListSingleDycRowTextPaneT3;
                        firstLineTextVarieties[2] = 8;
                        }

                    }
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
                preSizeMetrics,
                iPreviewPaneEnabledSize,
                preSizeVariety);

        CFsLayoutManager::LayoutMetricsSize(
                parentLayoutRect,
                exSizeMetrics,
                iExtendedSize,
                exSizeVariety);

        CFsLayoutManager::LayoutMetricsSize(
                parentLayoutRect,
                sizeMetrics,
                iSize,
                sizeVariety);
        }

    TRect currentSize;
    if ((iFlags & KFsTreeListItemAlwaysExtended) ||
            (iFlags & KFsTreeListItemExtended))
        {
        if (IsPreviewPaneOn())
            {
            currentSize = iPreviewPaneEnabledSize;
            }
        else
            {
            currentSize = iExtendedSize;
            }
        }
    else
        {
        currentSize = iSize;
        }

    if (CFsLayoutManager::IsMirrored())
        {
        currentSize.iBr = currentSize.iBr - TPoint(aIndentation, 0);
        }
    else
        {
        currentSize.iTl = currentSize.iTl + TPoint(aIndentation, 0);
        }

    TAlfTimedValue opacity;

    if ((iFlags & KFsTreeListItemHasMenu)
            && (iFlags & KFsTreeListItemFocused))
        {
        TRect iconRowParent = currentSize;
        if (menuIconRow > 0)
            {
            CFsLayoutManager::LayoutMetricsRect(currentSize, CFsLayoutManager::EFsLmListSingleDycRowPane, iconRowParent, menuIconRow, menuIconRow);
            }
        CFsLayoutManager::LayoutMetricsRect(
                iconRowParent,
                menuIconMetrics,
                rect,
                menuIconVariety);
        tpMenuIconTl.SetTarget(TAlfRealPoint(rect.iTl));
        tpMenuIconBr.SetTarget(TAlfRealPoint(rect.iBr));

        TInt iconMenuVisIndex = iLayout->FindVisual(iIconMenu);
        if ( iconMenuVisIndex != KErrNotFound )
            {
            tpMenuIconTl.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                    iconMenuVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMenuIconTl );
            tpMenuIconBr.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                    iconMenuVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMenuIconBr );
            }
        }


    if (iIconVisual)
        {
        TRect iconRowParent = currentSize;
        if (mainIconRow > 0)
            {
            CFsLayoutManager::LayoutMetricsRect(currentSize, CFsLayoutManager::EFsLmListSingleDycRowPane, iconRowParent, mainIconRow, mainIconRow);
            }
        CFsLayoutManager::LayoutMetricsRect(
                iconRowParent,
                mainIconMetrics,
                rect,
                mainIconVariety);
        tpMainIconTl.SetTarget(TAlfRealPoint(rect.iTl));
        tpMainIconBr.SetTarget(TAlfRealPoint(rect.iBr));

        TInt iconVisIndex = iLayout->FindVisual(iIconVisual);
        if ( iconVisIndex != KErrNotFound )
            {
            tpMainIconTl.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor( EAlfAnchorTopLeft,
                    iconVisIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    tpMainIconTl );
            tpMainIconBr.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor( EAlfAnchorBottomRight,
                    iconVisIndex,
                    EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                    tpMainIconBr );
            }
        }

    TInt firstLineTextIndex = 0;

    if (aData->DataLength()>0)
        {
        TRect textParentRect;
        CFsLayoutManager::LayoutMetricsRect(currentSize, firstLineTextParentMetrics, textParentRect, firstLineTextParentVariety);
        CFsLayoutManager::TFsText mainTextInfo;
        CFsLayoutManager::LayoutMetricsText(textParentRect,
                firstLineTextMetrics[0], mainTextInfo, firstLineTextVarieties[0]);

        firstLineTextIndex++;
        tpMainTextTl.SetTarget(TAlfRealPoint(mainTextInfo.iTextRect.iTl));
        tpMainTextBr.SetTarget(TAlfRealPoint(mainTextInfo.iTextRect.iBr));

        TInt textVisIndex = iLayout->FindVisual(iViewportLayout);
        if ( textVisIndex != KErrNotFound )
            {
            tpMainTextTl.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                    textVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMainTextTl );
            tpMainTextBr.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                    textVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMainTextBr );
            }
        }

    if (aData->SecondaryDataLength()>0)
        {
        CFsLayoutManager::TFsText subjTextInfo;

        if ((iFlags & KFsTreeListItemAlwaysExtended) ||
                (iFlags & KFsTreeListItemExtended))
            {
            TRect secondLineParentRect;
            CFsLayoutManager::LayoutMetricsRect(currentSize, CFsLayoutManager::EFsLmListSingleDycRowPane, secondLineParentRect, 1, 1);
            CFsLayoutManager::LayoutMetricsRect(secondLineParentRect, CFsLayoutManager::EFsLmListSingleDycRowTextPane, secondLineParentRect, secondLineTextVariety);
            CFsLayoutManager::LayoutMetricsText(secondLineParentRect,
                    secondLineTextMetrics, subjTextInfo, secondLineTextVariety);
            }
        else
            {
            TRect textParentRect;
            CFsLayoutManager::LayoutMetricsRect(currentSize, firstLineTextParentMetrics, textParentRect, firstLineTextParentVariety);
            CFsLayoutManager::LayoutMetricsText(textParentRect,
                    firstLineTextMetrics[firstLineTextIndex], subjTextInfo, firstLineTextVarieties[firstLineTextIndex]);
            firstLineTextIndex++;
            }

        tpSubjTextTl.SetTarget(TAlfRealPoint(subjTextInfo.iTextRect.iTl));
        tpSubjTextBr.SetTarget(TAlfRealPoint(subjTextInfo.iTextRect.iBr));
        
        if (tpMainTextTl.iY.Target() == tpSubjTextTl.iY.Target() && tpMainTextBr.iX.Target() > tpSubjTextTl.iX.Target())
            {
            const TReal32 delta(tpMainTextBr.iX.Target() - tpSubjTextTl.iX.Target() + 1); 
            tpSubjTextTl.iX.SetTarget(tpSubjTextTl.iX.Target() + delta, 0);            
            tpSubjTextBr.iX.SetTarget(tpSubjTextBr.iX.Target() - delta, 0);            
            }

        TInt seconTextVisIndex = iLayout->FindVisual(iSecViewport);
        if ( seconTextVisIndex != KErrNotFound )
            {
            tpSubjTextTl.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                    seconTextVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpSubjTextTl );
            tpSubjTextBr.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                    seconTextVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpSubjTextBr );
            }
        }

    if ( iDateTimeTextVisual )
        {
        TRect textParentRect;
        CFsLayoutManager::LayoutMetricsRect(currentSize, firstLineTextParentMetrics, textParentRect, firstLineTextParentVariety);
        CFsLayoutManager::TFsText dateTextInfo;
        CFsLayoutManager::LayoutMetricsText(textParentRect,
                firstLineTextMetrics[firstLineTextIndex], dateTextInfo, firstLineTextVarieties[firstLineTextIndex]);

        tpDateTextTl.SetTarget(TAlfRealPoint(dateTextInfo.iTextRect.iTl));
        tpDateTextBr.SetTarget(TAlfRealPoint(dateTextInfo.iTextRect.iBr));

        TInt textDateTimeIndex =
                        iLayout->FindVisual(iDateTimeTextVisual);
        if ( textDateTimeIndex != KErrNotFound )
            {
            tpDateTextTl.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                    textDateTimeIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpDateTextTl );
            tpDateTextTl.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                    textDateTimeIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpDateTextBr );
            }
        }

    //visual for a flag icon
    if ( IsFlagIconVisible() && ((iFlags & KFsTreeListItemExtended) ||
            (iFlags & KFsTreeListItemAlwaysExtended)))
        {
        TRect iconRowParent = currentSize;
        if (flagIconRow > 0)
            {
            CFsLayoutManager::LayoutMetricsRect(currentSize, CFsLayoutManager::EFsLmListSingleDycRowPane, iconRowParent, flagIconRow, flagIconRow);
            }
        CFsLayoutManager::LayoutMetricsRect(
                iconRowParent,
                flagIconMetrics,
                rect,
                flagIconVariety);
        tpFlagIconTl.SetTarget(TAlfRealPoint(rect.iTl));
        tpFlagIconBr.SetTarget(TAlfRealPoint(rect.iBr));

        TInt iconFlagVisIndex =
                          iLayout->FindVisual(iIconFlagVisual);
        if ( iconFlagVisIndex != KErrNotFound )
            {
            tpFlagIconTl.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorTopLeft,
                iconFlagVisIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                tpFlagIconTl );
            tpFlagIconBr.SetStyle(EAlfTimedValueStyleSineWave);
            iLayout->SetAnchor(EAlfAnchorBottomRight,
                iconFlagVisIndex,
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
                EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
                tpFlagIconBr );
            opacity.SetValueNow(1.0f);
            iIconFlagVisual->SetOpacity(opacity);
            }
        }
    else
        {
        if (iIconFlagVisual)
            {
            opacity.SetValueNow(0.0f);
            iIconFlagVisual->SetOpacity(opacity);
            }
        }

    if ( IsMarked() && iIconMarked )
        {
        if (iIconMarked)
            {
            TRect iconRowParent = currentSize;
            if (markIconRow > 0)
                {
                CFsLayoutManager::LayoutMetricsRect(currentSize, CFsLayoutManager::EFsLmListSingleDycRowPane, iconRowParent, markIconRow, markIconRow);
                }
            CFsLayoutManager::LayoutMetricsRect(
                    iconRowParent,
                    markIconMetrics,
                    rect,
                    markIconVariety);
            tpMarkIconTl.SetTarget(TAlfRealPoint(rect.iTl));
            tpMarkIconBr.SetTarget(TAlfRealPoint(rect.iBr));

            TInt iconMarkedVisIndex =
                                 iLayout->FindVisual(iIconMarked);
            if ( iconMarkedVisIndex != KErrNotFound )
                {
                tpMarkIconTl.SetStyle(EAlfTimedValueStyleSineWave);
                iLayout->SetAnchor(EAlfAnchorTopLeft,
                    iconMarkedVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMarkIconTl );
                tpMarkIconBr.SetStyle(EAlfTimedValueStyleSineWave);
                iLayout->SetAnchor(EAlfAnchorBottomRight,
                    iconMarkedVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpMarkIconBr );
                }
            }
        opacity.SetValueNow(1.0f);
        iIconMarked->SetOpacity(opacity);
        }
    else
        {
        opacity.SetValueNow(0.0f);
        iIconMarked->SetOpacity(opacity);
        }

    if ( iPreviewPaneTextVisual )
        {
        if ( IsPreviewPaneOn() && ((iFlags & KFsTreeListItemAlwaysExtended) ||
                (iFlags & KFsTreeListItemExtended)) )
            {
            TRect thirdLineParentRect;
            CFsLayoutManager::LayoutMetricsRect(currentSize, CFsLayoutManager::EFsLmListSingleDycRowPane, thirdLineParentRect, 2, 2);
            CFsLayoutManager::LayoutMetricsRect(thirdLineParentRect, CFsLayoutManager::EFsLmListSingleDycRowTextPane, thirdLineParentRect, thirdLineTextVariety);
            CFsLayoutManager::TFsText prevTextInfo;
            CFsLayoutManager::LayoutMetricsText(thirdLineParentRect,
                    thirdLineTextMetrics, prevTextInfo, thirdLineTextVariety);

            firstLineTextIndex++;
            tpPrevTextTl.SetTarget(TAlfRealPoint(prevTextInfo.iTextRect.iTl));
            tpPrevTextBr.SetTarget(TAlfRealPoint(prevTextInfo.iTextRect.iBr));

            TInt prevPaneVisIndex =
                      iLayout->FindVisual(iPreviewPaneTextVisual);
            if ( prevPaneVisIndex != KErrNotFound )
                {
                tpPrevTextTl.SetStyle(EAlfTimedValueStyleSineWave);
                iLayout->SetAnchor(EAlfAnchorTopLeft,
                    prevPaneVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpPrevTextTl );
                tpPrevTextBr.SetStyle(EAlfTimedValueStyleSineWave);
                iLayout->SetAnchor(EAlfAnchorBottomRight,
                    prevPaneVisIndex,
                    EAlfAnchorOriginLeft,EAlfAnchorOriginTop,
                    EAlfAnchorMetricAbsolute,EAlfAnchorMetricAbsolute,
                    tpPrevTextBr );
                }
            }

        }
    iLayout->UpdateChildrenLayout();
    // </cmail>
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item is requested to draw itself
// within specified parent layout.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::ShowL( CAlfLayout& aParentLayout,
                                        const TUint aTimeout )
    {
    FUNC_LOG;
    iParentLayout = &aParentLayout;
    if (!iLayout)
        {
        iLayout = CAlfAnchorLayout::AddNewL(iOwnerControl, NULL);
        iLayout->SetTactileFeedbackL( ETouchEventStylusDown, ETouchFeedbackBasic );
        iLayout->EnableBrushesL();
        iLayout->SetSize(TSize(iParentLayout->Size().IntTarget().AsSize().iWidth,0));
        iLayout->SetClipping(ETrue);
        if (!iIconVisual)
            {
            iIconVisual = CAlfImageVisual::AddNewL( iOwnerControl, iLayout );
            iIconVisual->SetScaleMode( CAlfImageVisual::EScaleFit );
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
            iTextVisual = CAlfTextVisual::AddNewL(iOwnerControl,
                    iViewportLayout);
            iTextVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iSecViewport)
            {
            iSecViewport =
                      CAlfViewportLayout::AddNewL(iOwnerControl, iLayout);
            iSecViewport->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iSecondaryTextVisual)
            {
            iSecondaryTextVisual = CAlfTextVisual::AddNewL(iOwnerControl,
                    iSecViewport);
            iSecondaryTextVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iIconMarked)
            {
            iIconMarked = CAlfImageVisual::AddNewL( iOwnerControl, iLayout );
            iIconMarked->SetScaleMode( CAlfImageVisual::EScaleFit );
            iIconMarked->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iIconMenu)
            {
            iIconMenu =
                        CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iIconMenu->SetScaleMode( CAlfImageVisual::EScaleFit );
            iIconMenu->SetFlag( EAlfVisualFlagIgnorePointer );
            }

        if (!iIconFlagVisual)
            {
            iIconFlagVisual =
                          CAlfImageVisual::AddNewL(iOwnerControl, iLayout);
            iIconFlagVisual->SetScaleMode( CAlfImageVisual::EScaleFit );
            iIconFlagVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }

         if ( !iDateTimeTextVisual )
             {
            iDateTimeTextVisual = CAlfTextVisual::AddNewL(iOwnerControl, iLayout);
            iDateTimeTextVisual->SetStyle(EAlfTextStyleSmall);
            iDateTimeTextVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }
         if ( !iPreviewPaneTextVisual )
             {
             iPreviewPaneTextVisual = CAlfTextVisual::AddNewL(iOwnerControl,
                     iLayout);
             iPreviewPaneTextVisual->SetAlign(EAlfAlignHLeft,EAlfAlignVCenter);
             iPreviewPaneTextVisual->SetFlag( EAlfVisualFlagIgnorePointer );
            }
         iTextMarquee = new (ELeave) TFsMarqueeClet(iOwnerControl, *iViewportLayout, TFsMarqueeClet::EScrollFromRight, TFsMarqueeClet::EScrollForth);
         iSecondaryTextMarquee = new (ELeave) TFsMarqueeClet(iOwnerControl, *iSecViewport, TFsMarqueeClet::EScrollFromRight, TFsMarqueeClet::EScrollForth);         
         iParentLayout->Append(iLayout, 0);
        }
    else
        {
        //visuals are already created but not attached to the list layout
        if (!iLayout->Layout())
            {
            aParentLayout.Append(iLayout, aTimeout);
            }
        }
    }

// ---------------------------------------------------------------------------
// A call to this function means that the item is requested to update its
// visual content.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::UpdateL( const MFsTreeItemData& aData,
                                          TBool aFocused,
                                          const TUint aLevel,
                                          CAlfTexture*& aMarkIcon,
                                          CAlfTexture*& aMenuIcon,
                                          const TUint aTimeout,
                                          TBool aUpdateData)
    {
    FUNC_LOG;
    TAlfTimedValue opacity;

    if (iLayout)
        {
        iSize.iWidth = iParentLayout->Size().Target().iX;
        iExtendedSize.iWidth = iSize.iWidth;
        iPreviewPaneEnabledSize.iWidth = iSize.iWidth;
        
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

        if ( aData.Type() == KFsTreePlainTwoLineItemDataType )
            {
            const CFsTreePlainTwoLineItemData* data =
                      static_cast<const CFsTreePlainTwoLineItemData*>(&aData);

            if( iIconMenu )
                {
                opacity.SetValueNow(0.0f);
                iIconMenu->SetOpacity(opacity);
                }
                      
            //update the ancors for current item settings
            const TInt indent( iTextIndentation * ( aLevel - 1 ) );
            UpdateLayout(data, indent);

            //set data to visuals

            //display main icon
            if (data->IsIconSet())
                {
                iIconVisual->SetScaleMode( CAlfImageVisual::EScaleFit );
                iIconVisual->SetImage(TAlfImage(data->Icon()));
                }

            TRgb textColor;
            if ( aFocused )
                {
                if (!iIsCustomTextFocusedColor)
                    {//custom color not set, use color from skin - load it in case theme has changed
                    iFocusedTextColor = FocusedStateTextSkinColor();
                    }
                textColor = iFocusedTextColor;
                }
            else
                {
                if (!iIsCustomTextNormalColor)
                    {//custom color not set, use color from skin - load it in case theme has changed
                    iNormalTextColor = NormalStateTextSkinColor( );
                    }
                textColor = iNormalTextColor;
                }

            TInt styleId = ModifiedStyleIdL( );

            iOwnerControl.Env().CancelCustomCommands(this);

            if( iTextVisual && aUpdateData )
                {
                TAlfTimedPoint tp(0,0);
                tp.SetStyle(EAlfTimedValueStyleLinear);
                iTextVisual->SetPos(tp);
                iTextVisual->SetTextL(data->Data( ));

                iTextVisual->EnableShadow(iFlags & KFsTreeListItemTextShadow);
                iTextVisual->SetColor( textColor );
                iTextVisual->SetTextStyle( styleId );

                iTextVisual->SetWrapping(CAlfTextVisual::ELineWrapManual);

                TAlfAlignHorizontal currTextAlign( EAlfAlignHLocale );
                currTextAlign = CurrentTextAlignmentL( iTextAlign, &data->Data(), iTextVisual );
                iTextVisual->SetAlign( currTextAlign, EAlfAlignVCenter );

                iTextVisual->SetWrapping(CAlfTextVisual::ELineWrapTruncate);

                if ( currTextAlign == EAlfAlignHRight )
                    {
                    iTextMarquee->SetScrollDirection( TFsMarqueeClet::EScrollFromLeft );
                    }
                else
                    {
                    iTextMarquee->SetScrollDirection( TFsMarqueeClet::EScrollFromRight );
                    }
                }

            if ( iSecondaryTextVisual && aUpdateData )
                {
                TAlfTimedPoint stp(0,0);
                stp.SetStyle(EAlfTimedValueStyleLinear);
                iSecondaryTextVisual->SetPos(stp);
                iSecondaryTextVisual->SetTextL(data->SecondaryData());

                iSecondaryTextVisual->EnableShadow(iFlags & KFsTreeListItemTextShadow);
                iSecondaryTextVisual->SetColor( textColor );
                iSecondaryTextVisual->SetTextStyle( styleId );

                iSecondaryTextVisual->SetWrapping(CAlfTextVisual::ELineWrapManual);

                TAlfAlignHorizontal currSecTextAlign( EAlfAlignHLocale );
                currSecTextAlign = CurrentTextAlignmentL( iSecondaryTextAlign, &data->SecondaryData(), iSecondaryTextVisual );
                iSecondaryTextVisual->SetAlign(currSecTextAlign,EAlfAlignVCenter );

                iSecondaryTextVisual->SetWrapping(CAlfTextVisual::ELineWrapTruncate);

                if ( currSecTextAlign == EAlfAlignHRight )
                    {
                    iSecondaryTextMarquee->SetScrollDirection( TFsMarqueeClet::EScrollFromLeft );
                    }
                else
                    {
                    iSecondaryTextMarquee->SetScrollDirection( TFsMarqueeClet::EScrollFromRight );
                    }
                HighlightKeyWords(data->SecondaryData());
                }



            if (( IsExtended() || IsAlwaysExtended() ) && IsPreviewPaneOn() && aUpdateData )
                {
                opacity.SetTarget(1.0f, aTimeout);
                iPreviewPaneTextVisual->SetOpacity(opacity);
                iPreviewPaneTextVisual->SetTextL(data->PreviewPaneData());

                iPreviewPaneTextVisual->EnableShadow(iFlags & KFsTreeListItemTextShadow);
                iPreviewPaneTextVisual->SetColor( textColor );
                iPreviewPaneTextVisual->SetTextStyle( styleId );

                iPreviewPaneTextVisual->SetWrapping(CAlfTextVisual::ELineWrapManual);

                TAlfAlignHorizontal currSecTextAlign( EAlfAlignHLocale );
                currSecTextAlign = CurrentTextAlignmentL( EAlfAlignHLocale, &data->PreviewPaneData(), iPreviewPaneTextVisual );
                iPreviewPaneTextVisual->SetAlign(currSecTextAlign,EAlfAlignVCenter );

                iPreviewPaneTextVisual->SetWrapping(CAlfTextVisual::ELineWrapTruncate);
                }
            else
                {
                opacity.SetTarget(0.0f, aTimeout);
                iPreviewPaneTextVisual->SetOpacity(opacity);
                }

            if ( iDateTimeTextVisual && aUpdateData )
                {
                iDateTimeTextVisual->SetTextL(data->DateTimeData());
                if (CFsLayoutManager::IsMirrored())
                    {
                    iDateTimeTextVisual->SetAlign(EAlfAlignHLeft,
                                                  EAlfAlignVCenter );
                    }
                else
                    {
                    iDateTimeTextVisual->SetAlign(EAlfAlignHRight,
                                                  EAlfAlignVCenter );
                    }
                iDateTimeTextVisual->SetWrapping(
                                          CAlfTextVisual::ELineWrapTruncate);
                iDateTimeTextVisual->EnableShadow(
                                          iFlags & KFsTreeListItemTextShadow);
                iDateTimeTextVisual->SetColor( textColor );
                iDateTimeTextVisual->SetTextStyle( styleId );
                }

            if ( IsMarked() && iIconMarked && aMarkIcon )
                {
                iIconMarked->SetScaleMode( CAlfImageVisual::EScaleFit );
                iIconMarked->SetImage( *aMarkIcon );
                }

            //menu icon - visible only when item is focused
            if ((iFlags & KFsTreeListItemHasMenu) && (iFlags & KFsTreeListItemFocused))
                {
                opacity.SetTarget(1.0f,aTimeout);
                iIconMenu->SetOpacity(opacity);
                iIconMenu->SetScaleMode( CAlfImageVisual::EScaleFit );
                iIconMenu->SetImage( *aMenuIcon );
                }
            else
                {
                opacity.SetTarget(0.0f,0);
                iIconMenu->SetOpacity(opacity);
                }

            if ( iIconFlagVisual )
                {
                if (data->IsFlagIconSet())
                    {
                    iIconFlagVisual->SetScaleMode( CAlfImageVisual::EScaleFit );
                    iIconFlagVisual->SetImage(TAlfImage(data->FlagIcon()));
                    }
                }

           iTextMarquee->StopScrolling();
           iTextVisual->SetWrapping(CAlfTextVisual::ELineWrapTruncate);

           iSecondaryTextMarquee->StopScrolling();
           iSecondaryTextVisual->SetWrapping(CAlfTextVisual::ELineWrapTruncate);
           }
        }
    }

// ---------------------------------------------------------------------------
// From class MFsTreeItemVisualizer.
// A call to this function means that the item goes out of the visible items
// scope.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::Hide( const TInt aTimeout )
    {
    FUNC_LOG;
    if ( iLayout )
        {
        //if marquee command was sent
        iSecondaryTextVisual->Env().CancelCustomCommands(this);

        delete iSecondaryTextMarquee;
        iSecondaryTextMarquee = NULL;

        CFsTreePlainOneLineItemVisualizer::Hide(aTimeout);
        iSecViewport = NULL;
        iSecondaryTextVisual = NULL;
        iIconFlagVisual = NULL;
        iDateTimeTextVisual = NULL;
        iPreviewPaneTextVisual = NULL;
        }
    }


// ---------------------------------------------------------------------------
//  From MFsTreeItemVisualizer.
//  Method to marquee the text when it's too long.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::MarqueeL(const TFsTextMarqueeType aMarqueeType,
                                                 const TUint aMarqueeSpeed,
                                                 const TInt aMarqueeStartDelay,
                                                 const TInt aMarqueeCycleStartDelay,
                                                 const TInt aMarqueeRepetitions)
    {
    FUNC_LOG;
    TInt marqueeSize(0), textWidth(0), marqueIdx(KErrNotFound);

    if ( aMarqueeType == EFsTextMarqueeNone )
        {
        return;
        }

    iTextVisual->SetWrapping(CAlfTextVisual::ELineWrapManual);

    textWidth = iTextVisual->SubstringExtents(0, iTextVisual->Text().Length()).Width();
    marqueeSize = textWidth - iViewportLayout->Size().iX.Target();

    iTextVisual->Env().CancelCustomCommands(this, aMarqueeType);

    if (iTextVisual && (marqueeSize>0))
        {
        TSize size( textWidth, iViewportLayout->Size().iY.Target() );
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

        marqueIdx = 0;
        TAlfCustomEventCommand cmd( aMarqueeType, this, marqueIdx );
        iTextVisual->Env().Send(cmd, aMarqueeStartDelay );
        }

    iSecondaryTextVisual->SetWrapping(CAlfTextVisual::ELineWrapManual);

    textWidth = iSecondaryTextVisual->SubstringExtents(0, iSecondaryTextVisual->Text().Length()).Width();
    marqueeSize = textWidth - iSecViewport->Size().iX.Target();

    if (iSecondaryTextVisual && (marqueeSize>0))
        {
        TSize size( textWidth, iSecViewport->Size().iY.Target() );
        if ( aMarqueeType == EFsTextMarqueeForth )
            {
            iSecondaryTextMarquee->SetScrollType( TFsMarqueeClet::EScrollForth );
            }
        else if ( aMarqueeType == EFsTextMarqueeForthBack )
            {
            iSecondaryTextMarquee->SetScrollType( TFsMarqueeClet::EScrollBounce );
            }
        else if ( aMarqueeType == EFsTextMarqueeLoop )
            {
            iSecondaryTextMarquee->SetScrollType( TFsMarqueeClet::EScrollWrap );
            //duplicate text
            TBufC <1> separator(_L(" "));
            HBufC* textHeap = HBufC::NewLC( (iSecondaryTextVisual->Text().Length()+1)*2+1);
            TPtr doubleString(textHeap->Des());
            doubleString = iSecondaryTextVisual->Text();
            doubleString.Append(_L(" "));
            doubleString.Append(iSecondaryTextVisual->Text());
            doubleString.Append(_L(" "));
            iSecondaryTextVisual->SetTextL( doubleString );
            CleanupStack::PopAndDestroy(textHeap);
            size.iWidth = iSecondaryTextVisual->SubstringExtents(0, iSecondaryTextVisual->Text().Length()).Width();
            }

        iSecondaryTextMarquee->SetScrollRepetitions( aMarqueeRepetitions );
        TInt time(0);
        if ( aMarqueeType == EFsTextMarqueeLoop )
            {
            time = textWidth * 1000 / aMarqueeSpeed;
            }
        else
            {
            time = marqueeSize * 1000 / aMarqueeSpeed;
            }
        iSecondaryTextMarquee->SetScrollSpeed( time );
        iSecondaryTextMarquee->SetScrollableArea( size );
        iSecondaryTextMarquee->SetScrollCycleDelay( aMarqueeCycleStartDelay );

        marqueIdx = 1;
        TAlfCustomEventCommand cmd( aMarqueeType, this, marqueIdx );
        iTextVisual->Env().Send(cmd, aMarqueeStartDelay );
        }
    }


// ---------------------------------------------------------------------------
//  Handles custom Alf event. Used with text marquee.
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainTwoLineItemVisualizer::OfferEventL(const TAlfEvent& aEvent)
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
            TInt idx(KErrNotFound);
            idx = aEvent.CustomEventData();
            ret = ETrue;
            switch(idx)
                {
                case 0:
                    {
                    iTextMarquee->StartScrolling( );
                    break;
                    }
                case 1:
                    {
                    iSecondaryTextMarquee->StartScrolling( );
                    break;
                    }
                }
            }
        }
    return ret;
    }


// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainTwoLineItemVisualizer::CFsTreePlainTwoLineItemVisualizer(
    CAlfControl& aOwnerControl )
    : CFsTreePlainOneLineItemVisualizer( aOwnerControl ),
      iSecondaryTextVisual( NULL ),
      iSecondaryTextAlign( EAlfAlignHLocale ),
      iPreviewPaneOn( EFalse ),
      iShowFlagIcon ( EFalse ),
      iIconFlagVisual( NULL ),
      iDateTimeTextVisual ( NULL ),
      iPreviewPaneTextVisual ( NULL ),
      iIsTimeVisibleWhenNotFocused ( ETrue )
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemVisualizer::ConstructL( )
    {
    FUNC_LOG;
    CFsTreePlainOneLineItemVisualizer::ConstructL();
    }

void CFsTreePlainTwoLineItemVisualizer::SetKeyWordsToHighlight(const RPointerArray<TDesC>& aKeyWords)
    {
    if ( iKeyWords.Count() > 0 )
        {
        iKeyWords.ResetAndDestroy();
        }
    for ( TInt i=0; i< aKeyWords.Count(); i++)
        {
        iKeyWords.Append(aKeyWords[i]);
        }
    }

void CFsTreePlainTwoLineItemVisualizer::HighlightKeyWords(TDesC& aText)
    {
    //iBackgroundColor = FocusedStateBackGroundSkinColor();
    iBackgroundColor = NormalStateTextSkinColor(); //temp fix for build break
      if ( iKeyWords.Count() > 0 )
        {
        TInt pos = aText.FindC(*iKeyWords[0]);
        iSecondaryTextVisual->SetHighlightRange(pos, pos+iKeyWords[0]->Length(), iBackgroundColor, iFocusedTextColor);
        }
    /*
	// Code to Highlight Multiple Key words.
    for ( TInt i=0; i <iKeyWords.Count(); i++ )
        {
        TInt pos = 0 ;
        while(pos != KErrNotFound)
            {
            TPtrC subStr = aText.Mid(pos);
            TInt val = subStr.FindC(*iKeyWords[i]); 
            if ( val != KErrNotFound )
                {
                pos += val;
                if( (pos + iKeyWords[i]->Length()) < aText.Length())
                    {
                    iSecondaryTextVisual->SetHighlightRange(pos,pos+iKeyWords[i]->Length(), color, Textcolor);
                    pos += iKeyWords[i]->Length();
                    }
                }
            else
                {
                pos = val;
                }
            }
        }
        */
    }
