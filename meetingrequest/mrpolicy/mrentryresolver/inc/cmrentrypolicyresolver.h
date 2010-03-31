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


#ifndef CMRENTRYPOLICYRESOLVER_H
#define CMRENTRYPOLICYRESOLVER_H

#include "cmrpolicyresolver.h"

class CMRPolicyResourceIterator;

/**
 *  CESMRPolicyChecker is responsible for matching policy
 *  against scenario data. This utility class encapsulates
 *  matching policies with TESMRScenarioData
 *
 *  @lib esmrpolicy.lib
 */
NONSHARABLE_CLASS( CMREntryPolicyResolver ) :
        public CMRPolicyResolver
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRPolicyChecker object.
     * @return Pointer to esmr policy checker object.
     */
    static CMREntryPolicyResolver* NewL();

    /**
     * C++ Destructor.
     */
    ~CMREntryPolicyResolver();

protected: // Interface
    
    CESMRPolicy* ResolveL( const TESMRScenarioData& aScenData );

    TBool SupportsTypeL( TESMRCalendarEventType aType );
    
    TBool MatchesL(
        const CESMRPolicy& aPolicy,
        const TESMRScenarioData& aScenData );

private: // Implementation
    CMREntryPolicyResolver();
    
    void ConstructL();
    
private: // Data
    
    CMRPolicyResourceIterator* iIterator;
    };

#endif // CMRENTRYPOLICYRESOLVER_H

// EOF
