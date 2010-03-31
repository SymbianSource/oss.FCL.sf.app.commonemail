/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for ESMR task factory implementation
*
*/


#ifndef C_ESMRTASKBASE_H
#define C_ESMRTASKBASE_H

#include <e32base.h>
#include "mesmrtask.h"

class MESMRCalDbMgr;
class MESMRMeetingRequestEntry;
class CMRMailboxUtils;

/**
 * CESMRTaskBase is base class for ES MR tasks
 *
 * @lib esmrtasks.lib
 */
class CESMRTaskBase : public CBase,
                      public MESMRTask
    {
public: 

    /**
     * C++ destructor.
     */ 
    virtual ~CESMRTaskBase();
    
protected:// Implementation

    /**
     * C++ constructor
     * @param aCalDbMgr Reference to cal db manager.
     */
    CESMRTaskBase( 
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils );
    
    /**
     * Stores entry to calendar DB
     */
    void StoreEntryToDbL();
    
    /**
     * Base class's EPOC 2nd phase constructor.
     */
    void BaseConstructL();
    
    /**
     * Fetches reference to cal db manager.
     * @return Reference to cal db manager.
     */
    inline MESMRCalDbMgr& CalDbMgr();
    
    /**
     * Fetches reference to es mr calendar entry.
     * @return Reference to es mr calendar entry.
     */
    inline MESMRMeetingRequestEntry& ESMREntry();
    
    /**
     * Fetches the reference to mr mailbox utilities.
     * @return reference to mr mailbox utilities.
     */
    inline CMRMailboxUtils& MailboxUtils();
    
private: // Data

    /**
    * Reference to cal db manager
    * Not own.
    */
    MESMRCalDbMgr& iCalDbMgr;
    
    /**
    * Reference to ES MR calendar entry
    * Not Own.
    */
    MESMRMeetingRequestEntry& iEntry;
    /**
    * Reference to mr mailbox utilities
    * Not own.
    */
    CMRMailboxUtils& iMRMailboxUtils;
    };

#include "cesmrtaskbase.inl"

#endif  // C_ESMRTASKBASE_H
