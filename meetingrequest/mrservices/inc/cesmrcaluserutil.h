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
* Description:  MR user utility class definition
*
*/


#ifndef CESMRCALUSERUTIL_H
#define CESMRCALUSERUTIL_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

class CCalEntry;
class CCalAttendee;

/**
* CESMRCalUserUtil provides services for handling
* meeting request calendar users.
*
* @lib esmrservices.lib
*/
NONSHARABLE_CLASS(CESMRCalUserUtil) : public CBase
    {
public: // Construction and destruction
    /**
     * Creates new CESMRCalUserUtil object. Ownership
     * is transferred to caller.
     * @param aEntry Refernce to MR calendar entry
     * @return Pointer to created CESMRCalUserUtil object,
     */
     IMPORT_C static CESMRCalUserUtil* NewL( CCalEntry& aEntry );
    
    /**
     * Creates new CESMRCalUserUtil object. Ownership
     * is transferred to caller.
     * @param aEntry Refernce to MR calendar entry
     * @return Pointer to created CESMRCalUserUtil object,
     */
    IMPORT_C static CESMRCalUserUtil* NewLC( CCalEntry& aEntry );

    /**
     * C++ destructor
     */
    IMPORT_C ~CESMRCalUserUtil();

public: // Interface
    /**
     * Fetches attendees from entry. Attendees are added to
     * aAttendeeArray. Attendees are filtered according to aFilterFlags
     * parameter.
     * Filtering flags is combimnation of enumeration TESMRRole values.
     * For example, aFilterFlags =  EESMRRoleRequiredAttendee --> Required
     * attendees are included to array. Ownership of the attendee objects is
     * not transferred.
     * @param aAttendeeArray Reference to attendee array
     * @param aFilterFlags Attendee filtering flags.
     */
    IMPORT_C void GetAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const;

    /**
     * Fetches the phone owner's attendee role in this calendar entry.
     * @return User role.
     * @see TESMRRole.
     */
    IMPORT_C TESMRRole PhoneOwnerRoleL() const;

private: // Implementation
    CESMRCalUserUtil( CCalEntry& aEntry );
    void ConstructL();

private: // Data
    /// Own: Reference to calendar entry
    CCalEntry& iEntry;
    };

#endif // CESMRCALUSERUTIL_H

// EOF
