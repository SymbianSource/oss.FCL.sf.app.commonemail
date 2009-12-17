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
* Description:  Definition of the class CPbkxRclProtocolAccountImpl.
*
*/


#ifndef CPBKXRCLPROTOCOLACCOUNTIMPL_H
#define CPBKXRCLPROTOCOLACCOUNTIMPL_H

#include "cpbkxremotecontactlookupprotocolaccount.h"

/**
* Class for holding information of protocol account.
*
* @lib pbkxrclengine.lib
* @since 3.1
*/
class CPbkxRclProtocolAccountImpl : public CPbkxRemoteContactLookupProtocolAccount
    {
public: // constructors and destructor

    /**
    * Two-phased constructor.
    *
    * @param aId Protocol account id.
    * @param aName Protocol name.
    * @return Created object.
    */
    static CPbkxRclProtocolAccountImpl* NewL(
        TPbkxRemoteContactLookupProtocolAccountId aId,
        const TDesC& aName );

    /**
    * Two-phased constructor.
    *
    * Leaves pointer to the created object on cleanupstack.
    *
    * @param aId Protocol account id.
    * @param aName Name of the protocol.
    * @return Created object.
    */
    static CPbkxRclProtocolAccountImpl* NewLC(
        TPbkxRemoteContactLookupProtocolAccountId aId,
        const TDesC& aName );
    
    /**
    * Destructor.
    */
    virtual ~CPbkxRclProtocolAccountImpl();

public: // methods from CPbkxRemoteContactLookupProtocolAccount

    /**
     * Returns account identifier.
     *
     * @return account identifier
     */
    virtual TPbkxRemoteContactLookupProtocolAccountId Id() const;

    /**
     * Returns value of the account identifier.
     *
     * @return account name
     */
    virtual const TDesC& Name() const; 

private: // constructors

    /**
    * Constructor.
    *
    * @param aId Protocol account id.
    */
    CPbkxRclProtocolAccountImpl( 
        TPbkxRemoteContactLookupProtocolAccountId aId );

    /**
    * Second-phase constructor.
    *
    * @param aName Name of the account.
    */ 
    void ConstructL( const TDesC& aName );

private: // data
    
    // Protocol account id.
    TPbkxRemoteContactLookupProtocolAccountId iId;

    // Name of the protocol.
    RBuf iName;

    };

#endif
