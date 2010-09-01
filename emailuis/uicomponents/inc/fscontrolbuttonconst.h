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
* Description:  Enums for FsControlButton
*
*/


#ifndef FSCONTROLBUTTONCONST_H
#define FSCONTROLBUTTONCONST_H

/** Possible elements of the button */
enum TFsControlButtonElem
    {
    ECBElemIconA = 1,
    ECBElemIconB,
    ECBElemLabelFirstLine,
    ECBElemLabelSndLine
    };

/** Possible layouts of button */
enum TFsControlButtonType
    {
    ECBTypeIconOnly,
    ECBTypeOneLineLabelOnly,
    ECBTypeOneLineLabelIconA,
    ECBTypeOneLineLabelIconB,
    ECBTypeOneLineLabelTwoIcons,
    ECBTypeTwoLinesLabelOnly,
    ECBTypeTwoLinesLabelIconA,
    ECBTypeTwoLinesLabelIconB,
    ECBTypeTwoLinesLabelTwoIcons
    };

/** Registered values of button's ids */
enum TFsRegisteredButtonIdValues
    {
    ECBFirstFocusableButton = -1
    };

#endif // FSCONTROLBUTTONCONST_H
