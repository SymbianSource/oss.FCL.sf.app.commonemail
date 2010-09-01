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
* Description:  Interface definition for MR policy resolver ECom interface
*
*/


#ifndef CMRPOLICYRESOLVER_H
#define CMRPOLICYRESOLVER_H

#include <e32base.h>
#include "mmrpolicyresolver.h"
#include "mesmrpolicychecker.h"

#include "esmrdef.h"

/**
 *  CMRPolicyResolver defines ECom plugin interface for policy resolving .
 *  @see MESMRTaskFactory
 *
 *  @lib esmrtasks.lib
 */
class CMRPolicyResolver : public CBase,
                          public MMRPolicyResolver,
                          public MESMRPolicyChecker
    {
public: // Construction and destruction
    /**
     * Creates and initialzes new ES MR task factory.
     * @param aCalDbMgr Reference to cal db manager.
     */
    inline static CMRPolicyResolver* NewL( TESMRCalendarEventType aType );

    /**
     * C++ destructor.
     */
    inline ~CMRPolicyResolver();
    
public: // From MMRPolicyResolver
    
    CESMRPolicy* ResolveL( const TESMRScenarioData& aScenData ) = 0;

    TBool SupportsTypeL( TESMRCalendarEventType aType ) = 0;
    
public: // From MESMRPolicyChecker
   TBool MatchesL(
            const CESMRPolicy& aPolicy,
            const TESMRScenarioData& aScenData ) = 0;

private: // Data
    
    /**
     * iDtor_ID_Key Instance identifier key. When instance of an
     * implementation is created by ECOM framework, the
     * framework will assign UID for it. The UID is used in
     * destructor to notify framework that this instance is
     * being destroyed and resources can be released.
     */
    TUid iDtor_ID_Key;    
    };

#include "cmrpolicyresolver.inl"

#endif // C_ESMRTASKFACTORY_H
