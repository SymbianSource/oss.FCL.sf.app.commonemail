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
     *
     * @param aTaskFactory Reference to task factory.
     */
    IMPORT_C static CESMRTaskExtenstionImpl* NewL( MESMRTaskFactory& aTaskFactory );

    /**
     * C++ destructor.
     */
    IMPORT_C ~CESMRTaskExtenstionImpl();

public: // From MESMRTaskExtension
    void SendAndStoreResponseL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void SendAndStoreMRL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void DeleteAndSendMRL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void StoreEntryToLocalDBL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void ForwardMRAsEmailL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void ReplyAsEmailL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void DeleteEntryFromLocalDBL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void MarkTodoAsDoneL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void MarkTodoAsNotDoneL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );
    void MoveEntryToCurrentDBL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );

private: // Implementation
    CESMRTaskExtenstionImpl( MESMRTaskFactory& aTaskFactory );
    void ConstructL();
    void CreateAndExecuteTaskL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry );

private: // Data
    /**
    * ES MR Task factory
    * Not own.
    */
    MESMRTaskFactory& iTaskFactory;
    };

#endif 
