/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclProtocolEnvImpl.
*
*/


#ifndef CPBKXRCLPROTOCOLENVIMPL_H
#define CPBKXRCLPROTOCOLENVIMPL_H

#include "mpbkxremotecontactlookupprotocolenv.h"

/**
* Factory class for creating data objects.
*
* @lib pbkxengine.lib
* @since S60 3.1
*/
class CPbkxRclProtocolEnvImpl : 
    public CBase,
    public MPbkxRemoteContactLookupProtocolEnv
    {
public: // constructors and destructor

    /**
    * Two-phased constructor.
    *
    * @return Created object.
    */
    static CPbkxRclProtocolEnvImpl* NewL();
    
    /**
    * Two-phased constructor.
    *
    * Leaves pointer to the created object on cleanupstack.
    *
    * @return Created object.
    */
    static CPbkxRclProtocolEnvImpl* NewLC();

    /**
    * Destructor.
    */
    virtual ~CPbkxRclProtocolEnvImpl();

public: // functions from MPbkxRemoteContactLookupProtocolEnv

    /**
    * Creates a new protocol account instance.
    *
    * Ownership is transferred to caller.
    *
    * @param aId protocol account ID
    * @param aName protocol account name
    * @return Created account.
    */
    virtual CPbkxRemoteContactLookupProtocolAccount* NewProtocolAccountL( 
        TPbkxRemoteContactLookupProtocolAccountId aId, 
        const TDesC& aName );
        
    /**
    * Creates a new protocol result instance.
    *
    * Ownership is transferred to caller.
    *
    * @param aIsComplete is search result complete?
    * @param aContactItem contact item, owernship is transfered to this object
    * @param aExtraProtocolData extra protocol data for two-phase search
    * @return Created protocol result.
    */
    virtual CPbkxRemoteContactLookupProtocolResult* NewProtocolResultL(
        TBool aIsComplete, 
        CContactItem& aContactItem, 
        const TDesC& aExtraProtocolData );

private: // constructors

    /**
    * Constructor.
    */
    CPbkxRclProtocolEnvImpl();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

    };

#endif
