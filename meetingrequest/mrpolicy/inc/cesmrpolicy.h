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
* Description:  ESMR service policy
*
*/


#ifndef CESMRPOLICY_H
#define CESMRPOLICY_H

#include <e32base.h>
#include <babitflags.h>
//<cmail>
#include "esmrdef.h"

class RResourceFile;
class TResourceReader;
class TESMREntryField;

/**
 *  CESMRPolicy defines ESMR policy.
 *  ES MR policies are relates to different roles and task scenarions.
 *  Policies are used by user interface views and tasks.
 *
 *  Policies can be defined in resource file with STRUCT ESMR_POLICY
 *  resource structure.
 *
 *  @see esmrservices.rh
 *  @lib esmrpolicy.lib
 */
NONSHARABLE_CLASS(CESMRPolicy) :
        public CBase
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates new CESMRPolicy object. Ownership is
     * tranferred to caller.
     *
     * @return Pointer to created and initialized esmr policy object.
     */
    IMPORT_C static CESMRPolicy* NewL();

    /**
     * Two-phased constructor. Reads esmr policy definition from
     * resource. Caller is responsible for filling the resource reader with
     * correct information.
     *
     * @param aRFile Resource file object.
     * @param aReader Resource reader object.
     * @return Pointer to created and initialized esmr policy object.
     */
    IMPORT_C static CESMRPolicy* NewL(
            RResourceFile& aRFile,
            TResourceReader& aReader );

    /**
     * C++ destructor.
     */
    IMPORT_C ~CESMRPolicy();

public: // Interface
    /**
     * Reads policy definition from resource file. Caller is
     * responsible for filling the resource reader with
     * correct information.
     *
     * @param aRFile Resource file object.
     * @param aReader Resource reader object.
     */
    IMPORT_C void ReadFromResourceL(
            RResourceFile& aRFile,
            TResourceReader& aReader );

    /**
     * Checks, if field is included in policy.
     * @return ETrue if field is included in policy.
     * @param aFieldId Field id.
     */
    IMPORT_C TBool IsFieldIncluded(
            TESMREntryFieldId aFieldId ) const;

    /**
     * Fetches field matching field if from policy.
     * @param aFieldId Field id.
     * @return Reference to field id.
     * @error KErrNotFound Field cannot be found.
     */
    IMPORT_C const TESMREntryField& FieldL(
            TESMREntryFieldId aFieldId ) const;

    /**
     * Checks if <aCommand> belongs to default command list
     * @param aCommand Command id
     * @return ETrue if belongs to default commands
     */
    IMPORT_C TBool IsDefaultCommand(
            TInt aCommand ) const;

    /**
     * Checks if command belongs to additional commands
     * @param aCommand Command id
     * @return ETrue if belongs to additional commands
     */
    IMPORT_C TBool IsAdditionalCommand(
            TInt aCommand ) const;

    /**
     * Tests, if specific ES MR role is included in policy.
     * @param aRole Role of user
     * @return ETrue, if role is included in policy
     */
    IMPORT_C TBool IsRoleIncluded(
            TESMRRole aRole ) const;

    /**
    * Returns the event type
    * @return TESMRCalendarEventType
    */
    IMPORT_C TESMRCalendarEventType EventType() const;

    /**
     * Fetches policy id.
     * @return policy id.
     * @see TESMRPolicyID
     */
    inline TESMRPolicyID PolicyId() const;

    /**
     * Fetches policy view mode.
     * @return policy view mode.
     * @see TESMRViewMode
     */
    inline TESMRViewMode ViewMode() const;

    /**
     * Fetches default fields.
     * @return Default fields
     */
    inline const RArray<TESMREntryField>& Fields() const;

    /**
     * Fetches the allowed applicatio for this policy
     * @return TESMRUsingApp Parent application
     */
    inline TESMRUsingApp AllowedApp() const;

private: // Implementation
    CESMRPolicy();
    void ConstructL(
            RResourceFile& aRFile,
            TResourceReader& aReader );

private: // data
    /// Own Event type
    TESMRCalendarEventType iEventType;
    /// Own: Policy id
    TESMRPolicyID iPolicyId;
    /// Own: Policy MR view mode
    TESMRViewMode iViewMode;
    /// Own: Defines role flags
    TBitFlags16 iRoleFlags;
    /// Own: Application allowed in this policy
    TESMRUsingApp iAllowedApp;
    /// Own: Default MR fields
    RArray<TESMREntryField> iMRFields;
    /// Own: Additional MR fields
    RArray<TESMRCommand> iDefaultCommands;
    /// Own: Additional commands
    RArray<TESMRCommand> iAdditionalCommands;
    };

#include "cesmrpolicy.inl"
//</cmail>

#endif // CESMRPOLICY_H

// EOF
