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
* Description:  Definitions for ES MR scenario data
*
*/


#ifndef TESMRSCENARIODATA_H
#define TESMRSCENARIODATA_H

//<cmail>
#include "esmrdef.h"
//</cmail>

/**
 *  TESMRScenarioData defines ES MR UI scenario data.
 *
 *  TESMRScenarioData is targeted to be used internally by
 *  ES MR UI classes.
 *
 *  @lib esmrservices.lib
 */
class TESMRScenarioData
    {
public: // data
    /**
     * MR role
     */
    TESMRRole      iRole;

    /**
     * Viewing mode
     */
    TESMRViewMode  iViewMode;

    /**
     * Calling application
     */
    TESMRUsingApp  iCallingApp;

    /**
     * Recurrent mode
     */
    TESMREntryMode iRecurrentMode;

    /**
     * Entry type
     */
    TESMRCalendarEventType iEntryType;
    };

#endif // TESMRSCENARIODATA_H
