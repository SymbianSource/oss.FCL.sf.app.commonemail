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
* Description:  Interface definition for policy checker
*
*/


#ifndef MESMRPOLICYCHECKER_H
#define MESMRPOLICYCHECKER_H

class CESMRPolicy;
class TESMRScenarioData;

/**
 * MESMRPolicyChecker defines interface for policy checking operations
 */
class MESMRPolicyChecker
    {
public:
    /**
     * Virtual destructor
     */
    virtual ~MESMRPolicyChecker() {}

public: // Interface

    /**
     * Tests, whether policy matches against scenario data.
     * @param aPolicy Reference to policy to be tested.
     * @param aScenData Reference to scenario data.
     */
    virtual TBool MatchesL(
            const CESMRPolicy& aPolicy,
            const TESMRScenarioData& aScenData ) = 0;
    };

#endif // MESMRPOLICYCHECKER_H
