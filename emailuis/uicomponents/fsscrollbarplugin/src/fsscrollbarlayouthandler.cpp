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
* Description: Implements class CFsScrollbarLayoutHandler
*
*/


//<cmail> SF
#include "emailtrace.h"
#include <alf/alfconstants.h>
//</cmail>
#include "fsscrollbarlayouthandler.h"
#include "fsscrollbarpluginconstants.h"
#include "fshuiscrollbarlayout.h"
#include "fsscrollbarset.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// From class CAlfLayoutHandler.
// ---------------------------------------------------------------------------
//
MAlfExtension* CFsScrollbarLayoutHandler::NewL(
    MAlfInterfaceProvider& aResolver,
    CHuiControl* aOwner,
    CHuiLayout* aParentLayout )
    {
    FUNC_LOG;
    CFsScrollbarLayoutHandler* self =
        new( ELeave )CFsScrollbarLayoutHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL( NULL, *aOwner, aParentLayout );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// From class CAlfLayoutHandler.
// ---------------------------------------------------------------------------
//
void CFsScrollbarLayoutHandler::ConstructL(
    CHuiVisual* /*aVisual*/,
    CHuiControl& aOwner,
    CHuiLayout* aParentLayout )
    {
    FUNC_LOG;
    CFsHuiScrollbarLayout* layout(
        CFsHuiScrollbarLayout::AddNewL( aOwner, aParentLayout ) );
    CAlfLayoutHandler::ConstructL( layout, aOwner, aParentLayout );
    }


// ---------------------------------------------------------------------------
// From class CAlfLayoutHandler.
// ---------------------------------------------------------------------------
//
void CFsScrollbarLayoutHandler::HandleCmdL(
    TInt aCommandId,
    const TDesC8& aInputBuffer,
    TDes8& aResponse )
    {
    FUNC_LOG;
    if ( !iVisual )
        {
        return;
        }
    CFsHuiScrollbarLayout* scrollbarLayout(
        static_cast<CFsHuiScrollbarLayout*>( iVisual ) );
    switch ( aCommandId )
        {
        case EFsScrollbarLayoutSetScrollbarRangeCommand:
            {
            TFsScrollbarSet* scrollbarSet(
                (TFsScrollbarSet*)aInputBuffer.Ptr() );
            scrollbarLayout->SetRange( scrollbarSet->iRangeStart,
                scrollbarSet->iRangeEnd, scrollbarSet->iScrollbar );
            break;
            }
        case EFsScrollbarLayoutSetScrollbarPosCommand:
            {
            TFsScrollbarSet* scrollbarSet(
                (TFsScrollbarSet*)aInputBuffer.Ptr() );
            scrollbarLayout->SetScrollbarPos(
                scrollbarSet->iScrollbarPos, scrollbarSet->iScrollbar );
            break;
            }
        case EFsScrollbarLayoutSetScrollbarVisibilityCommand:
            {
            TFsScrollbarSet* scrollbarSet(
                (TFsScrollbarSet*)aInputBuffer.Ptr() );
            scrollbarLayout->SetVisibilityMode(
                scrollbarSet->iVisibility, scrollbarSet->iScrollbar );
            break;
            }
        case EFsScrollbarLayoutGetScrollbarSettingsCommand:
            {
            TFsScrollbarSet responseSet;
            TFsScrollbarSet* scrollbarSet(
                (TFsScrollbarSet*)aInputBuffer.Ptr() );
            responseSet.iScrollbar = scrollbarSet->iScrollbar;
            responseSet.iRangeStart = scrollbarLayout->RangeStart(
                scrollbarSet->iScrollbar );
            responseSet.iRangeEnd = scrollbarLayout->RangeEnd(
                scrollbarSet->iScrollbar );
            responseSet.iScrollbarPos = scrollbarLayout->ScrollbarPos(
                scrollbarSet->iScrollbar );
            responseSet.iVisible = scrollbarLayout->IsScrollbarVisible(
                scrollbarSet->iScrollbar );
            responseSet.iThumbSpan = scrollbarLayout->ThumbSpan(
                scrollbarSet->iScrollbar );
            responseSet.iVisibility = scrollbarLayout->VisibilityMode(
                scrollbarSet->iScrollbar );

            TPckgC<TFsScrollbarSet> outBuf( responseSet );
            aResponse = outBuf;
            break;
            }
        case EFsScrollbarLayoutSetVerticalScrollbarWidthCommand:
            {
            TInt2* params( (TInt2*)aInputBuffer.Ptr() );
            scrollbarLayout->SetVerticalScrollbarWidth(
                params->iInt1, params->iInt2 );
            break;
            }
        case EFsScrollbarLayoutSetHorizontalScrollbarHeightCommand:
            {
            TInt2* params( (TInt2*)aInputBuffer.Ptr() );
            scrollbarLayout->SetHorizontalScrollbarHeight(
                params->iInt1, params->iInt2 );
            break;
            }
        case EFsScrollbarLayoutSetScrollbarThumbSpanCommand:
            {
            TFsScrollbarSet* scrollbarSet(
                (TFsScrollbarSet*)aInputBuffer.Ptr() );
            scrollbarLayout->SetThumbSpan(
                scrollbarSet->iThumbSpan, scrollbarSet->iScrollbar );
            break;
            }
        case EFsScrollbarLayoutSetScrollbarUpdateCommand:
            {
            TInt* layoutTransitionTime = (TInt*)aInputBuffer.Ptr();
            scrollbarLayout->Update( *layoutTransitionTime );
            break;
            }
        case EFsScrollbarLayoutSetScrollbarCustomImagesCommand:
            {
            TFsImageSet* imageSet( (TFsImageSet*)aInputBuffer.Ptr() );

            CHuiImageBrush* barBrush =
                (CHuiImageBrush*)iResolver.GetInterfaceL(
                    EHuiObjectTypeBrush, imageSet->iBarBrushId );
            CHuiImageBrush* thumbBrush =
                (CHuiImageBrush*)iResolver.GetInterfaceL(
                    EHuiObjectTypeBrush, imageSet->iThumbBrushId );
            scrollbarLayout->SetScrollbarImagesL(
                barBrush, thumbBrush, imageSet->iScrollbar );
            break;
            }
        case EFsScrollbarLayoutSetScrollbarClearCustomImagesCommand:
            {
            TFsScrollbarSet* scrollbarSet(
                (TFsScrollbarSet*)aInputBuffer.Ptr() );
            scrollbarLayout->RemoveCustomImages( scrollbarSet->iScrollbar );
            break;
            }
        case EFsScrollbarLayoutGetScrollbarLayoutSizeCommand:
            {
            THuiRealPoint layoutSize( scrollbarLayout->LayoutSize() );
            TPckgC<THuiRealPoint> outBuf( layoutSize );
            aResponse = outBuf;
            break;
            }
        default:
            CAlfLayoutHandler::HandleCmdL(
                aCommandId, aInputBuffer, aResponse );
            break;
        }
    }

