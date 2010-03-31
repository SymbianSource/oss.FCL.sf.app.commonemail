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
* Description:  Common variables and defines for all fieldbuilders
 *
*/

 
#ifndef ESMRFIELDBUILDERDEF_H
#define ESMRFIELDBUILDERDEF_H
 
_LIT( KAddressDelimeterSemiColon, ";" );
_LIT( KTimeFieldSeparator, " - ");
_LIT( KEmptySpace, " ");
_LIT(KDefaultMailBoxURI, "mailbox://");
const TText8 KDefaultMailBoxURISeparator = '/';

const TUint KSemiColon = 59;//ascii letter
const TUint KComma = 44;//ascii letter

const TInt KTextLimit( 2000 );
const TInt KMaxAddressFieldLines = 3;
    
//needed to offset the difference that HandleEdwinSizeEventL forces on this control
//compared to layout manager sizes
const TInt KViewerDifference = 3;
const TInt KEditorDifference (2);
const TInt KViewerSmallFontDifference = 2;

#define TTIME_MAXIMUMDATE (TTime(TDateTime(2100, EDecember, 31-1, 23, 59, 59, 999999)))
#define TTIME_MINIMUMDATE (TTime(TDateTime(1900, EJanuary, 1-1, 0, 0, 0, 0)))

#define TTIME_TIMEFIELDMAXIMUMDATE (TTime(TDateTime(9999, EDecember, 31-1, 23, 59, 59, 999999)))
#define TTIME_TIMEFIELDMINIMUMDATE (TTime(TDateTime(0000, EJanuary, 1-1, 0, 0, 0, 0)))

const TInt KBufferLength = 32;
const TInt KDateStringLength( 64 );
const TInt KMaxTextLength (255);

const TInt KAllDayEventDurationInDays( 1 );

#endif//ESMRFIELDBUILDERDEF_H
