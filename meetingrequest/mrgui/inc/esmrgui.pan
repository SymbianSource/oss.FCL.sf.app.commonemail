/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Panics for mrgui. 
*
*/


#ifndef ESMRGUI_PAN
#define ESMRGUI_PAN

enum TMeetingRequestPanics
    {
    EMeetingRequestPanic = 1
    // add further panics here
    };

inline void Panic(TMeetingRequestPanics aReason)
    {
    _LIT(applicationName,"MeetingRequestUI");
    User::Panic(applicationName, aReason);
    }

#endif
