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


#ifndef CESMRPOLICYMANAGER_H
#define CESMRPOLICYMANAGER_H

#include <e32base.h>
//<cmail>
#include "esmrutilsapiext.h"
#include <barsc.h>
#include "esmrdef.h"
#include "tesmrscenariodata.h"

class CESMRPolicy;
class MESMRCalEntry;
class MESMRPolicyChecker;
class CESMRPolicyChecker;
class CESMRPolicyExtensionManager;
class CCoeEnv;

/**
 *  CESMRPolicyManager defines ESMR policymanager.
 *  ES MR policies are relates to different roles and task scenarions.
 *  Policies are used by user interface views and tasks. CESMRPolicyManager
 *  provides interface for accessing these policies.
 *  CESMRPolicyManager uses CCoeEnv.
 *
 *  @see CESMRPolicy
 *  @lib esmrpolicy.lib
 */
NONSHARABLE_CLASS(CESMRPolicyManager) :
        public CBase
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates new CESMRPolicyManager object.
     * Ownership is transferred to caller.
     *
     * @return Pointer to created and initialized esmr policy object.
     */
    IMPORT_C static CESMRPolicyManager* NewL();

    /**
     * C++ destructor.
     */
    IMPORT_C ~CESMRPolicyManager();

public: // Interface
    /**
     * Fetches current policy object.
     * @return Current policy object
     */
    inline CESMRPolicy& CurrentPolicy();

    /**
     * Fetches the extension UID for ES MR policy manager
     * @return Uid of extension
     */
    IMPORT_C static TUid ExtensionUid();

    /**
     * Resolves policy to be used.
     * @param aScenarioData Scenario Data for resolving correct policy
     * @param aESMREntry Reference to ES MR Entry.
     * @param aPolicyChecker to be used. If NULL, default checker is used
     */
    IMPORT_C void ResolvePolicyL(
            const TESMRScenarioData& aScenarioData,
            MESMRCalEntry& aESMREntry,
            MESMRPolicyChecker* aPolicyChecker );

    /**
     * Reads policy from resource file. Policies are appended
     * @param aPolicyFile Policy resource filename.
     * @param aPolicyArrayResourceId Policy array resource id
     */
    IMPORT_C void ReadPolicyFromResourceL(
            const TDesC& aPolicyFile,
            TInt aPolicyArrayResourceId );

    /**
     * Adds the current policy on stack for preserving it if a new policy
     * needs to be resolved and the current policy is still needed afterwards.
     */
    IMPORT_C void PushPolicyL();
    
    /**
     * Restores the topmost policy from the stack as the current policy. 
     * @return the policy from stack or NULL if stack is empty.
     */
    IMPORT_C CESMRPolicy* PopPolicy();
    
private: // Implementation

    CESMRPolicyManager();
    void ConstructL();
    void ReadPolicyL(
            TInt aResourceId,
            MESMRCalEntry& aESMREntry );

private: // data

    /**
    * Current policy object
    * Own.
    */
    CESMRPolicy* iCurrentPolicy;

    /**
    * Policy resource file
    * Own
    */
    RResourceFile iPolicyResourceFile;

    /**
    * ESMR policy resource ids
    * Own.
    */
    RArray<TInt> iPolicyResourceIds;

    /**
     * Default ESMR policy checker.
     * Own.
     */
    CESMRPolicyChecker* iDefaultPolicyChecker;

    // Own: Extension resource manager
    CESMRPolicyExtensionManager* iExtension;

    // Ref: Coe Env
    CCoeEnv* iCoeEnv;
    
    /**
     * Array for preserving policies
     */
    RPointerArray< CESMRPolicy > iPolicyStack;
    };

#include "cesmrpolicymanager.inl"
//</cmail>

#endif // CESMRPOLICYMANAGER_H
