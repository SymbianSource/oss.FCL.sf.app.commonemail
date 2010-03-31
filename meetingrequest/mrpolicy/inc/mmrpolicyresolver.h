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


#ifndef MMRPOLICYRESOLVER_H
#define MMRPOLICYRESOLVER_H

#include "esmrdef.h"

class CESMRPolicy;
class TESMRScenarioData;

/**
 *  MMRPolicyResolver defines interface for policy resolving .
 */
class MMRPolicyResolver
    {
public:
    
    virtual ~MMRPolicyResolver() {}
    
        
public:
    
    /**
     * Resolves policy based on given scenario data.
     * Ownership of the resolved policy is transferred to caller.
     * 
     * @param aScenData scenario data.
     * @return resolved policy instance.
     */
    virtual CESMRPolicy* ResolveL( const TESMRScenarioData& aScenData ) = 0;

    /**
     * Returns ETrue if resolver is capable of resolving specified entry type
     * 
     * @param aType Entry type to test
     */
    virtual TBool SupportsTypeL( TESMRCalendarEventType aType ) = 0;
    
    /**
     * Accessor for interface extensions.
     * 
     * @param aExtensionUid the extension interface UID
     * @return pointer to extension interface or NULL
     */
    virtual TAny* Extension( TUid /*aExtensionUid*/ ) { return NULL; }
    
    };

#endif // MMRPOLICYRESOLVER_H
