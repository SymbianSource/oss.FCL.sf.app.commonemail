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
* Description: This file implements class CFsAlfScrollbarLayout.
*
*/


//<cmail> SF
#include "emailtrace.h"
#include <alf/alfcontrol.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfenv.h>
#include <alf/alfimagebrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfconstants.h>
#include <alf/alfgencomponent.h>
#include <alf/alfconstants.h>
//</cmail>

#include "fsscrollbarpluginconstants.h"
#include "fsalfscrollbarlayout.h"


// ======== MEMBER FUNCTIONS ========

CFsAlfScrollbarLayout::CFsAlfScrollbarLayout()
    : CAlfLayout()
    {
    FUNC_LOG;
    }


void CFsAlfScrollbarLayout::ConstructL( CAlfControl& aOwnerControl )
    {
    FUNC_LOG;
    CAlfLayout::ConstructL( aOwnerControl );

    iOwnerControl = &aOwnerControl;
    }


// ---------------------------------------------------------------------------
// Construct a new scrollbar layout and give its ownership to a control.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsAlfScrollbarLayout* CFsAlfScrollbarLayout::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    FUNC_LOG;
    TInt ownerHandle( aOwnerControl.Identifier() );
    TInt parentlayouthandle( 0 );
    if ( aParentLayout )
        {
        parentlayouthandle = aParentLayout->Identifier();
        }

    TInt2 inputParams( aOwnerControl.Identifier(), parentlayouthandle );

    TPckg<TInt2> paramBuf( inputParams );

    CFsAlfScrollbarLayout* layout( new( ELeave )CFsAlfScrollbarLayout() );
    CleanupStack::PushL( layout );

    layout->CAlfVisual::ConstructL(
        aOwnerControl,
        EFsScrollbarCreateScreenLayout,
        KFsScrollbarImplementationId, paramBuf );

    User::LeaveIfError( aOwnerControl.Append( layout, aParentLayout ) );
    CleanupStack::Pop( layout );
    return layout;
    }


CFsAlfScrollbarLayout::~CFsAlfScrollbarLayout()
    {
    FUNC_LOG;
    SetVisibilityMode( EFsScrollbarHideAlways, EFsScrollbarVertical );
    SetVisibilityMode( EFsScrollbarHideAlways, EFsScrollbarHorizontal );
    ReleaseCustomImage( EFsScrollbarVertical );
    ReleaseCustomImage( EFsScrollbarHorizontal );
    }


// ---------------------------------------------------------------------------
// Set the range of the scrollbar. Will not actually move the scrollbar until
// Update() is called.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::SetRange(
    TInt aStart,
    TInt aEnd,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    scrollbarSet.iScrollbar = aScrollbar;
    scrollbarSet.iRangeStart = aStart;
    scrollbarSet.iRangeEnd = aEnd;

    SendCommand( EFsScrollbarLayoutSetScrollbarRangeCommand, scrollbarSet );
    }


// ---------------------------------------------------------------------------
// Get scrollbars minimum range value.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsAlfScrollbarLayout::RangeStart( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    TFsScrollbarSet paramScrollbarSet;
    paramScrollbarSet.iScrollbar = aScrollbar;
    SendCommand( EFsScrollbarLayoutGetScrollbarSettingsCommand,
        paramScrollbarSet, scrollbarSet );
    return scrollbarSet.iRangeStart;
    }


// ---------------------------------------------------------------------------
// Get scrollbars maximum range value.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsAlfScrollbarLayout::RangeEnd( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    TFsScrollbarSet paramScrollbarSet;
    paramScrollbarSet.iScrollbar = aScrollbar;
    SendCommand( EFsScrollbarLayoutGetScrollbarSettingsCommand,
        paramScrollbarSet, scrollbarSet );
    return scrollbarSet.iRangeEnd;
    }


// ---------------------------------------------------------------------------
// Set the count of visible units at the time.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::SetThumbSpan(
    TInt aThumbSpan,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    scrollbarSet.iScrollbar = aScrollbar;
    scrollbarSet.iThumbSpan = aThumbSpan;
    SendCommand( EFsScrollbarLayoutSetScrollbarThumbSpanCommand,
        scrollbarSet );
    }


// ---------------------------------------------------------------------------
// Get the set value for visible units.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsAlfScrollbarLayout::ThumbSpan( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    TFsScrollbarSet paramScrollbarSet;
    paramScrollbarSet.iScrollbar = aScrollbar;
    SendCommand( EFsScrollbarLayoutGetScrollbarSettingsCommand,
        paramScrollbarSet, scrollbarSet );
    return scrollbarSet.iThumbSpan;
    }


// ---------------------------------------------------------------------------
// Adjust the vertical scrollbar's width.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::SetVerticalScrollbarWidth(
    TInt aWidth,
    TInt aTransitionTime )
    {
    FUNC_LOG;
    TInt2 params( aWidth, aTransitionTime );
    TPckgC<TInt2> inBuf( params );
    TBuf8<1> dummy;
    TInt err( Comms()->DoSynchronousCmd(
        EFsScrollbarLayoutSetVerticalScrollbarWidthCommand,
        inBuf,
        dummy ) );

    __ASSERT_ALWAYS( err == KErrNone, User::Invariant() );
    }


// ---------------------------------------------------------------------------
// Adjust the horizontal scrollbar's height.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::SetHorizontalScrollbarHeight(
    TInt aHeight,
    TInt aTransitionTime )
    {
    FUNC_LOG;
    TInt2 params( aHeight, aTransitionTime );
    TPckgC<TInt2> inBuf( params );
    TBuf8<1> dummy;
    TInt err( Comms()->DoSynchronousCmd(
        EFsScrollbarLayoutSetHorizontalScrollbarHeightCommand,
        inBuf,
        dummy ) );

    __ASSERT_ALWAYS( err == KErrNone, User::Invariant() );
    }


// ---------------------------------------------------------------------------
// Set the position of the scrollbar.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::SetScrollbarPos(
    const TInt aPos,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    scrollbarSet.iScrollbar = aScrollbar;
    scrollbarSet.iScrollbarPos = aPos;

    SendCommand( EFsScrollbarLayoutSetScrollbarPosCommand, scrollbarSet );
    }


// ---------------------------------------------------------------------------
// Resolve the position of the scrollbar.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFsAlfScrollbarLayout::ScrollbarPos( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    TFsScrollbarSet paramScrollbarSet;
    paramScrollbarSet.iScrollbar = aScrollbar;
    SendCommand( EFsScrollbarLayoutGetScrollbarSettingsCommand,
        paramScrollbarSet, scrollbarSet );
    return scrollbarSet.iScrollbarPos;
    }


// ---------------------------------------------------------------------------
// Set the scrollbar visibility.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::SetVisibilityMode(
    TFsScrollbarVisibility aVisibility,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    scrollbarSet.iScrollbar = aScrollbar;
    scrollbarSet.iVisibility = aVisibility;

    SendCommand( EFsScrollbarLayoutSetScrollbarVisibilityCommand,
        scrollbarSet );
    }


// ---------------------------------------------------------------------------
// Resolve the scrollbar visibility.
// ---------------------------------------------------------------------------
//
EXPORT_C TFsScrollbarVisibility CFsAlfScrollbarLayout::VisibilityMode(
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    TFsScrollbarSet paramScrollbarSet;
    paramScrollbarSet.iScrollbar = aScrollbar;
    SendCommand( EFsScrollbarLayoutGetScrollbarSettingsCommand,
        paramScrollbarSet, scrollbarSet );
    return scrollbarSet.iVisibility;
    }


// ---------------------------------------------------------------------------
// Resolve if the specified scrollbar is visible.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFsAlfScrollbarLayout::IsScrollbarVisible(
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    TFsScrollbarSet paramScrollbarSet;
    paramScrollbarSet.iScrollbar = aScrollbar;
    SendCommand( EFsScrollbarLayoutGetScrollbarSettingsCommand,
        paramScrollbarSet, scrollbarSet );
    return scrollbarSet.iVisible;
    }


// ---------------------------------------------------------------------------
// Change the bar image to custom image. Ownership of the brushes are changed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::SetScrollbarImagesL(
    CAlfImageBrush* aBarBrush, CAlfImageBrush* aThumbBrush,
    TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    ReleaseCustomImage( aScrollbar );

    if ( EFsScrollbarVertical == aScrollbar )
        {
        iVBarBrush = aBarBrush;
        iVThumbBrush = aThumbBrush;
        }
    else
        {
        iHBarBrush = aBarBrush;
        iHThumbBrush = aThumbBrush;
        }

    TFsImageSet imageSet;
    imageSet.iBarBrushId = aBarBrush->Identifier();
    imageSet.iThumbBrushId = aThumbBrush->Identifier();
    imageSet.iScrollbar = aScrollbar;

    TPckgC<TFsImageSet> inBuf( imageSet );
    TBuf8<1> dummy;

    TInt err = Comms()->DoSynchronousCmd(
        EFsScrollbarLayoutSetScrollbarCustomImagesCommand,
        inBuf,
        dummy );

    __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );
    }


// ---------------------------------------------------------------------------
// Update the visibility of scrollbars.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFsAlfScrollbarLayout::Update( TInt aLayoutTransitionTime )
    {
    FUNC_LOG;
    TPckgC<TInt> inBuf( aLayoutTransitionTime );
    TBuf8<1> outDum;
    TInt err = Comms()->DoSynchronousCmd(
        EFsScrollbarLayoutSetScrollbarUpdateCommand,
        inBuf,
        outDum );

    __ASSERT_ALWAYS( err == KErrNone, User::Invariant() );
    }


// ---------------------------------------------------------------------------
// Get the target size of the scrolbar layout without the scrollbars.
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfRealPoint CFsAlfScrollbarLayout::LayoutSize()
    {
    FUNC_LOG;
    TAlfRealPoint layoutSize;

    TBuf8<1> inBuf;
    TPckg<TAlfRealPoint> outBuf( layoutSize );
    TInt err = Comms()->DoSynchronousCmd(
        EFsScrollbarLayoutGetScrollbarLayoutSizeCommand,
        inBuf,
        outBuf );

    __ASSERT_ALWAYS( KErrNone == err, User::Invariant() );

    return layoutSize;
    }


// ---------------------------------------------------------------------------
// Send a command to server.
// ---------------------------------------------------------------------------
//
void CFsAlfScrollbarLayout::SendCommand(
    TFsScrollbarCommand aCommandId,
    TFsScrollbarSet& aInScrollbarSet )
    {
    FUNC_LOG;
    TFsScrollbarSet dummySet;
    SendCommand( aCommandId, aInScrollbarSet, dummySet );
    }


// ---------------------------------------------------------------------------
// Send a command to server.
// ---------------------------------------------------------------------------
//
void CFsAlfScrollbarLayout::SendCommand(
    TFsScrollbarCommand aCommandId,
    TFsScrollbarSet& aInScrollbarSet,
    TFsScrollbarSet& aOutScrollbarSet )
    {
    FUNC_LOG;
    TPckgC<TFsScrollbarSet> inBuf( aInScrollbarSet );
    TPckg<TFsScrollbarSet> outBuf( aOutScrollbarSet );
    TInt err = Comms()->DoSynchronousCmd(
        aCommandId,
        inBuf,
        outBuf );

    __ASSERT_ALWAYS( err == KErrNone, User::Invariant() );
    }


// ---------------------------------------------------------------------------
// Release custom image reserved memory.
// ---------------------------------------------------------------------------
//
void CFsAlfScrollbarLayout::ReleaseCustomImage( TFsScrollbar aScrollbar )
    {
    FUNC_LOG;
    TFsScrollbarSet scrollbarSet;
    scrollbarSet.iScrollbar = aScrollbar;
    SendCommand( EFsScrollbarLayoutSetScrollbarClearCustomImagesCommand,
        scrollbarSet );
    if ( EFsScrollbarVertical == aScrollbar )
        {
        delete iVBarBrush;
        iVBarBrush = NULL;
        delete iVThumbBrush;
        iVThumbBrush = NULL;
        }
    else
        {
        delete iHBarBrush;
        iHBarBrush = NULL;
        delete iHThumbBrush;
        iHThumbBrush = NULL;
        }
    }

