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
* Description:  ESMR service alarm information handler
*
*/


#ifndef CESMRALARMINFOHANDLER_H
#define CESMRALARMINFOHANDLER_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
#include "tesmralarminfo.h"
//</cmail>

class CCalEntry;

/**
 *  CESMRAlarmInfoHandler is responsible for handling
 *  alarm related issues for calendar entry.
 *
 *  @lib esmrservices.lib
 */
NONSHARABLE_CLASS(CESMRAlarmInfoHandler) :
        public CBase
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRAlarmInfoHandler object. Ownership transferred to caller.
     *
     * @return Pointer to esmr recurrence handler object.
     */
    IMPORT_C static CESMRAlarmInfoHandler* NewL();

    /**
     * Two-phased constructor. Creates and initializes
     * CESMRAlarmInfoHandler object. Ownership transferred to caller and
     * created object is left to cleanup stack.

     * @return Pointer to esmr recurrence handler object.
     */
    IMPORT_C static CESMRAlarmInfoHandler* NewLC();

    /**
     * C++ Destructor.
     */
    IMPORT_C ~CESMRAlarmInfoHandler();

public: // Interface
    /**
     * Reads alarm information from resource file.
     *
     * @param aResourceFile Alarm inforamtion resource file name.
     * @param aResouceId Resource id containing alarm info table.
     */
    IMPORT_C void ReadFromResourceL(
            const TDesC& aResourceFile,
            TInt aResouceId );

    /**
     * Fetches alarm information object matching alarm information
     * in entry.
     *
     * @param aEntry Reference to calendar entry.
     * @param aAlarmInfo On return contains fetched alarm information
     *                   object.
     * @error KErrNotFound Alarm info object cannot be resolved.
     */
    IMPORT_C void GetAlarmInfoObjectL(
            const CCalEntry& aEntry,
            TESMRAlarmInfo& aAlarmInfo );

    /**
     * Fetches absolute alarm time from calendar entry. Absolute alarm
     * time is returned in local time.
     *
     * @param aEntry Reference to calendar entry.
     * @param aAbsoluteAlarmTime On return contains absolute alarm time.
     * @error KErrNotFound Absolute alarm cannot resolved.
     */
    IMPORT_C void GetAbsoluteAlarmTimeL(
            CCalEntry& aEntry,
            TTime& aAbsoluteAlarmTime );

private: // Implementation
    CESMRAlarmInfoHandler();
    void ConstructL();

private:
    /// Own: Alarm information objects
    RArray<TESMRAlarmInfo> iAlarmInfos;
    };

#endif // CESMRALARMINFOHANDLER_H

// EOF
