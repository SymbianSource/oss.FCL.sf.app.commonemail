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
* Description:  ESMR task extension implementation
*
*/


#ifndef CESMRTASKEXTENSIONIMPL_H
#define CESMRTASKEXTENSIONIMPL_H

#include <e32base.h>
#include "mesmrtaskextension.h"

class MESMRCalDbMgr;
class CMRMailboxUtils;
class CESMRPolicyManager;
class CESMREntryProcessor;
class MESMRTaskFactory;

/**
 * CESMRTaskExtenstionImpl implements ES MR task extension for
 * MR Utils interface.
 *
 * Usage (assuming you have CMRUtils* iUtils):
 * @code
 *    MESMRTaskExtension* ext = ExtensionApiL<MESMRTaskExtension>( iUtils );
 * @endcode
 * @see
 *
 */
NONSHARABLE_CLASS( CESMRTaskExtenstionImpl ) : public CBase,
                                               public MESMRTaskExtension
    {
public: // Construction and destruction
    /**
     * Creates and initialzes new CESMRTaskExtenstionImpl object
     * Ownership is trasferred to caller.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aMRMailboxUtils Reference to mailbox utilities.
     * @param aPolicyManager Reference to policy manager.
     * @param aEntryProcessor Reference to entry processor.
     * @param aTaskFactory Reference to task factory.
     */
    IMPORT_C static CESMRTaskExtenstionImpl* NewL(
            MESMRCalDbMgr& aCalDBMgr,
            CMRMailboxUtils& aMRMailboxUtils,
            CESMRPolicyManager& aPolicyManager,
            CESMREntryProcessor& aEntryProcessor,
            MESMRTaskFactory& aTaskFactory );

    /**
     * C++ destructor.
     */
    IMPORT_C ~CESMRTaskExtenstionImpl();

public: // From MESMRTaskExtension
    void SendAndStoreResponseL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    void SendAndStoreMRL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    void DeleteMRFromLocalDBL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    void DeleteAndSendMRL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    void StoreMRToLocalDBL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    void ForwardMRAsEmailL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );
    void ReplyAsEmailL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );

private: // Implementation
    CESMRTaskExtenstionImpl(
            MESMRCalDbMgr& aCalDBMgr,
            CMRMailboxUtils& aMRMailboxUtils,
            CESMRPolicyManager& aPolicyManager,
            CESMREntryProcessor& aEntryProcessor,
            MESMRTaskFactory& aTaskFactory );
    void ConstructL();
    void CreateAndExecuteTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry );

private: // Data
    /**
    * Cal DB manager
    * Not own.
    */
    MESMRCalDbMgr& iCalDBMgr;

    /**
    * Meeting request mailbox utils
    * Not own.
    */
    CMRMailboxUtils& iMRMailboxUtils;

    /**
    * ESMR policy manager
    * Not own.
    */
    CESMRPolicyManager& iPolicyManager;

    /**
    * MR Entry processor
    * Not own.
    */
    CESMREntryProcessor& iEntryProcessor;

    /**
    * ES MR Task factory
    * Not own.
    */
    MESMRTaskFactory& iTaskFactory;
    };

#endif 
