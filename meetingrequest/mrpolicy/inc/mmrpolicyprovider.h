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


#ifndef MMRPOLICYPROVIDER_H
#define MMRPOLICYPROVIDER_H

class CESMRPolicy;

/**
 * MMRPolicyProvider defines interface for accessing policy from CESMRPolicyManager.
 */
class MMRPolicyProvider
    {
public: // Interface

    /**
     * Gets the current policy resolved by the policy provider implementation
     * 
     * @return reference to the current policy instance.
     */
    virtual const CESMRPolicy& CurrentPolicy() const = 0;

protected:
    /**
     * Virtual destructor
     */
    virtual ~MMRPolicyProvider() {}


    
    };

#endif // MMRPOLICYPROVIDER_H
