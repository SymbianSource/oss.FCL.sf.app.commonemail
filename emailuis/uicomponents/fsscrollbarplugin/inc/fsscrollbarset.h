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
* Description: This file defines classes TFsScrollbarSet, TFsImageSet
*
*/



#ifndef T_FSSCROLLBARSET_H
#define T_FSSCROLLBARSET_H


#include <e32base.h>

/**  Defines the visibility modes for a scrollbar. */
enum TFsScrollbarVisibility
    {
    EFsScrollbarHideAlways,
    EFsScrollbarShowAlways,
    EFsScrollbarAuto
    };

/**  Defines the scrollbar which to use. */
enum TFsScrollbar
    {
    EFsScrollbarVertical,
    EFsScrollbarHorizontal
    };


/**
 *  Collection class of scrollbar's data.
 */
class TFsScrollbarSet
    {

public: // data

    /**
     * The scrollbar at issue.
     */
    TFsScrollbar iScrollbar;

    /**
     * The minimum value of the scrollbar position.
     */
    TInt iRangeStart;

    /**
     * The maximum value of the scrollbar position.
     */
    TInt iRangeEnd;

    /**
     * The size of the visible area in scrollbar values.
     */
    TInt iThumbSpan;

    /**
     * The current scrollbar position.
     */
    TInt iScrollbarPos;

    /**
     * The visibility mode of the scrollbar.
     */
    TFsScrollbarVisibility iVisibility;

    /**
     * Defines if the scrollbar is visible or hidden.
     */
    TBool iVisible;

    };

class TFsImageSet
    {

public: //data

    TInt iBarBrushId;
    TInt iThumbBrushId;
    
    //CHuiImageBrush* iBarBrush;
    //CHuiImageBrush* iThumbBrush;
    TFsScrollbar iScrollbar;

    };
    
#endif // T_FSSCROLLBARSET_H
