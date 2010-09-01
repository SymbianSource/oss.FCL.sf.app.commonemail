/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MR Info Provider interface 
*
*/


#ifndef MMRINFOPROVIDER_H
#define MMRINFOPROVIDER_H

//  INCLUDES

// FORWARD DECLARATIONS
class MMRPolicyProvider;
class MESMRCalEntry;

// CLASS DECLARATION

/**
 * MMRInfoProvider defines an accessor interface for UI to access current
 * policy and entry information.
 */
class MMRInfoProvider
    {
public:  // Interface
    
    /**
     * Gets the MMRPolicyProvider interface.
     *
     * @return policy provider 
     */
    virtual MMRPolicyProvider& PolicyProvider() const = 0;
    
    /**
     * Gets the current entry.
     * 
     * @return entry.
     */
    virtual MESMRCalEntry* EntryL() = 0;

protected:
    
    /**
     * Protected destructor to prevent destruction of implementation
     * via this interface.
     */
    virtual ~MMRInfoProvider() {}
    
    };

#endif      // MMRINFOPROVIDER_H

// End of File
