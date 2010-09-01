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
* Description:  Message viewer constants for FS Email UI
*
*/


#ifndef __FREESTYLEEMAILVIEWERCONSTANTS_H__
#define __FREESTYLEEMAILVIEWERCONSTANTS_H__

#include <e32keys.h>

static const TInt KPlainTextLimitationInChars = 153600;

static const TInt KAnimationRefreshTimeIntervalInMilliseconds = 2000;
static const TInt KDownloadProgreeNoteHideDelay = 1000;

_LIT( KThreeLines,"---");
static const TInt KNumberOfLines = 3;
static const TInt KTotalNumberOfLines = 9;

const TInt KKeyCodeArtificialScrollUp = EKeyF13;
const TInt KKeyCodeArtificialScrollDown = EKeyF14;

// Media player UID, for opening rtsp links
static const TUid KUidMediaPlayer = { 0x10005A3E };

// Prefixes
_LIT( KUrlPrefixIdentifier, "://");
_LIT( KHttpUrlPrefix, "http://");
_LIT( KMailtoPrefix, "mailto:");
_LIT( KRtspUrlPrefix, "rtsp://");
_LIT( KCallPrefix, "call:");
_LIT( KTelPrefix, "tel:");
_LIT( KSmsPrefix, "sms:");
_LIT( KMmsPrefix, "mms:");
_LIT( KVoipPrefix, "sip:");

#endif
