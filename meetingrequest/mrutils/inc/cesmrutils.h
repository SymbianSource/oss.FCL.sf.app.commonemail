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
* Description:  Implementation of CMRUtils ECom API.
*
*/


#ifndef CESMRUTILS_H
#define CESMRUTILS_H

//  INCLUDE FILES
#include <e32base.h>
#include <CMRUtils.h>
#include "esmrdef.h"

// FORWARD DECLARATIONS
class CCalEntry;
class CMRMailboxUtils;
class CESMRCalDbMgr;
class CESMRPolicyManager;
class CMREntryProcessor;
class MESMRTaskFactory;
class MESMRTaskExtension;

/**
 * ECom implementation for es meeting request utilities.
 */
NONSHARABLE_CLASS( CESMRUtils ) :
        public CBase,
        public MMRUtilsObserver
    {
public: // Constructors and destructors

    /**
     * ECom plug-in instantiation method.
     * @return instantiated ECom plug-in
     */
     IMPORT_C static CESMRUtils* NewL(
             MMRUtilsObserver& aObserver );

    /**
     * Destructor.
     */
     IMPORT_C ~CESMRUtils();

public: // Interface
    
    /**
     * Creates extensions for specified entry type
     */
    IMPORT_C void CreateExtensionsL(
            TESMRCalendarEventType aEntryType );
     
     
public: // Interface is copied from CMRUtils.h 
    /**
     * Deletes given aEntries from the database and possibly allows
     * user to cancel meeting or decline participation, depending on
     * participation type.
     * @param aEntry entry to be deleted
     * @param aMailbox mailbox to use for possible cancel/response
     * @return KErrNone or a system wide error code  
     */
    IMPORT_C TInt DeleteWithUiL(
            const CCalEntry& aEntry,
            TMsvId aMailbox );
    
    /**
     * Deletes given instance from the database and possibly allows
     * user to cancel meeting or decline participation, depending on
     * participation type.   
     * @param aInstance instance to be deleted, ownership transferred
     * @param aMailbox mailbox to use for possible cancel/response
     * @return KErrNone or a system wide error code  
     */ 
    IMPORT_C TInt DeleteWithUiL(
            CCalInstance* aInstance,
            TMsvId aMailbox );
    
    /**
     * Deprecated. Do not use.
     */
    IMPORT_C void SendWithUiL(
            const CCalEntry& aEntry,
            TMsvId aMailbox );
    
    /**
     * Allows extending this API without breaking BC.
     * @param aExtensionUid Extension uid
     * @param return extension
     */
    IMPORT_C TAny* ExtensionL(
            TUid aExtensionUid );
    
    /**        
     * Internal template method called immediatelly after
     * instantiation of the ECom plug-in.
     * @param aCalSession calendar session reference
     * @param aMsvSession, may be NULL
     */
    IMPORT_C void SessionSetupL(
            CCalSession& aCalSession,
            CMsvSession* aMsvSession );

public: // From MMRUtilsObserver
    void HandleCalEngStatus(
            TMRUtilsCalEngStatus aStatus );
    void HandleOperation(
            TInt aType,
            TInt aPercentageCompleted,
            TInt aStatus );

private: // Implementation
    CESMRUtils( MMRUtilsObserver& aObserver );
    void ConstructL();

private: // data
    /// Own: Cal engine status
    TMRUtilsCalEngStatus iCalEngStatus;
    /// Own: Pointer to ES MR UTILS cal db object
    CESMRCalDbMgr* iCalDBMgr;
    // Own: Meeting request mailbox utils
    CMRMailboxUtils* iMRMailboxUtils;
    /// Own: ESMR policy manager
    CESMRPolicyManager* iPolicyManager;
    /// Own: MR Entry processor
    CMREntryProcessor* iEntryProcessor;
    /// Own: ES MR Task factory
    MESMRTaskFactory* iTaskFactory;
    /// Own: ES MR Task extension
    MESMRTaskExtension* iTaskExtension;
    /// Ref:
    MMRUtilsObserver& iObserver;
    };

#endif // CESMRUTILSIMPL_H

// End of File
