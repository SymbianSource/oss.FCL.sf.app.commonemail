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
* Description:  ESMR calendar entry conflict checker
*
*/


#ifndef CESMRCONFLICTCHECKER_H
#define CESMRCONFLICTCHECKER_H		

#include <e32base.h>

class CCalEntry;
class MESMRCalDbMgr;

/**
 *  CESMRConflictChecker is responsible for finding
 *  conflicting entries from symbian calendar database.
 *
 *  @lib esmrentry.lib
 */
NONSHARABLE_CLASS(CESMRConflictChecker) :
        public CBase
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRConflictChecker object.
     * @param aDbMgr Reference to cal db manager.
     * @return Pointer to esmr policy checker object.
     */
    static CESMRConflictChecker* NewL(
            MESMRCalDbMgr& aDbMgr );

    /**
     * C++ destructor.
     */
    ~CESMRConflictChecker();

public: // Interface
    /**
     * Finds conflicting entries. Conflicting entries are
     * stored to table given as parameter. Ownership of the
     * conflicting entries are transferred to caller.
     *
     * @param aEntry Reference to calendar entry
     * @param aConflicts Reference to calendar entry table
     */
    void FindConflictsL(
            const CCalEntry& aEntry,
            RPointerArray<CCalEntry>& aConflicts );

    /**
     * Finds instances for this entry occuring between specified
     * time range.
     * @param aStart Start time
     * @param aEnd End time
     * @param aEntry Reference to entry
     * @param aInstances On return contains the instances 
     * 
     */
    void FindInstancesForEntryL(
            TTime aStart, 
            TTime aEnd,
            const CCalEntry& aEntry,
            RPointerArray<CCalEntry>& aInstances );

private: // Implementation
    CESMRConflictChecker(
            MESMRCalDbMgr& aDbMgr );

private: // Data
    /// Ref: Reference to cal db manager
    MESMRCalDbMgr& iDbMgr;
    };

#endif // CESMRCONFLICTCHECKER_H

// EOF
