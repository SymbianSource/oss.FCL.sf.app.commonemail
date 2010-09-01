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
* Description:  MR Cal Entry policy resolver ECom plugin definition
*
*/


#ifndef CMRCALENTRYPOLICYRESOLVER_H
#define CMRCALENTRYPOLICYRESOLVER_H

#include "cmrpolicyresolver.h"

class CMRPolicyResourceIterator;

/**
 *  CMRCalEntryPolicyResolver is responsible for resolving policy
 *  against scenario data. This utility class encapsulates
 *  matching policies with TESMRScenarioData
 *
 */
NONSHARABLE_CLASS( CMRCalEntryPolicyResolver ) :
        public CMRPolicyResolver
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initializes
     * CMRCalEntryPolicyResolver object.
     * @return Pointer to esmr policy checker object.
     */
    static CMRCalEntryPolicyResolver* NewL();

    /**
     * C++ Destructor.
     */
    ~CMRCalEntryPolicyResolver();

protected: // Interface
    
    CESMRPolicy* ResolveL( const TESMRScenarioData& aScenData );

    TBool SupportsTypeL( TESMRCalendarEventType aType );
    
    TBool MatchesL(
        const CESMRPolicy& aPolicy,
        const TESMRScenarioData& aScenData );

private: // Implementation
    CMRCalEntryPolicyResolver();
    
    void ConstructL();
    
private: // Data
    
    CMRPolicyResourceIterator* iIterator;
    };

#endif // CMRCALENTRYPOLICYRESOLVER_H

// EOF
