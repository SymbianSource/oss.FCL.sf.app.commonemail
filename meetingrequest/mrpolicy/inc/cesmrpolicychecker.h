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
* Description:  ESMR service policy checker definition
*
*/


#ifndef CESMRPOLICYCHECKER_H
#define CESMRPOLICYCHECKER_H

#include <e32base.h>
//<cmail>
#include "mesmrpolicychecker.h"
//</cmail>

/**
 *  CESMRPolicyChecker is responsible for matching policy
 *  against scenario data. This utility class encapsulates
 *  matching policies with TESMRScenarioData
 *
 *  @lib esmrpolicy.lib
 */
NONSHARABLE_CLASS(CESMRPolicyChecker) :
        public CBase,
        public MESMRPolicyChecker
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRPolicyChecker object.
     * @return Pointer to esmr policy checker object.
     */
    static CESMRPolicyChecker* NewL();

    /**
     * C++ Destructor.
     */
    ~CESMRPolicyChecker();

public: // Interface
    /**
     * Tests, whether policy matches against scenario data.
     * @param aPolicy Reference to policy to be tested.
     * @param aScenData Reference to scenario data.
     */
    TBool MatchesL(
        const CESMRPolicy& aPolicy,
        const TESMRScenarioData& aScenData );

private: // Implementation
    CESMRPolicyChecker();
    void ConstructL();
    };

#endif // CESMRPOLICYCHECKER_H

// EOF
