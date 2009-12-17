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
* Description:  Resource headers for project NCS
*
*/



#ifndef NCSCONSTANTS_H
#define NCSCONSTANTS_H

#include "FreestyleEmailUiLiterals.h"

_LIT( KUnicodeLineFeed, "\x2029");
_LIT( KIMSLineFeed, "\r\n");
_LIT( KEmailAddressSeparator, ";" );
_LIT( KSentLineDateAndTimeSeparatorText, ", " );
_LIT( KLastNameFirstNameSeparator, ", " );
_LIT( KEmailAddressDecorationHead, "<" );
_LIT( KEmailAddressDecorationTail, ">" );

const TInt KAutoSaveTimeDelayMs = 15000;

const TInt KHeaderDetailTextPaneVariety = 4;

enum TMsgPriority 
    {
    EMsgPriorityLow,
    EMsgPriorityNormal,
    EMsgPriorityHigh
    };

enum TNcsEditorUsage
    {
    ENcsEditorDefault,
    ENcsEditorAddress,
    ENcsEditorSubject,
    ENcsEditorBody,
    };

#endif
