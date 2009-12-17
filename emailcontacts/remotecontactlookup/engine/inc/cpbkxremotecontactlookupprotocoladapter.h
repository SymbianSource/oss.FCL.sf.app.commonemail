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
* Description:  Definition of the class CPbkxRemoteContactLookupProtocolAdapter.
*
*/


#ifndef CPBKXREMOTECONTACTLOOKUPADAPTER_H
#define CPBKXREMOTECONTACTLOOKUPADAPTER_H

#include <e32base.h>
#include <cntitem.h>
#include "cpbkxremotecontactlookupprotocolaccount.h"
#include "cpbkxremotecontactlookupprotocolsession.h"
#include "mpbkxremotecontactlookupprotocolenv.h"

/**
*  Adaptation interface for different synchronization protocols.
*
*  @since S60 3.1
*/
class CPbkxRemoteContactLookupProtocolAdapter : public CBase
    {

public:

    /**
    * Two-phased constructor.
    *
    * @param aProtocolUid UID of the protocol ECom plugin
    * @param aProtocolEnv factory for the data objects needed by the adapter
    */
    static CPbkxRemoteContactLookupProtocolAdapter* NewL( 
        TUid aProtocolUid,
        MPbkxRemoteContactLookupProtocolEnv& aProtocolEnv );
                    
    /**
    * Destructor.
    */
    virtual ~CPbkxRemoteContactLookupProtocolAdapter();

    /**
    * Factory method for creating protocol accounts. The creation of the account will
    * be only succesful if protocol account for the given identifier exists. Ownership
    * of the instance is given to the caller.
    *
    * @param aAccountId account identifier
    * @return protocol account
    */
    virtual CPbkxRemoteContactLookupProtocolAccount* NewProtocolAccountL( 
        const TPbkxRemoteContactLookupProtocolAccountId& aAccountId ) = 0;
    
    /**
    * Appends all the available protocol accounts to the array. Owernship of the
    * accounts is transfered to the caller.
    *
    * Given array MUST NOT be reset.
    *
    * @param aAccounts an array containing all the available protocol 
    *                  accounts.
    */
    virtual void GetAllProtocolAccountsL( 
        RPointerArray<CPbkxRemoteContactLookupProtocolAccount>& aAccounts ) = 0;

    /**
    * Factory method for creating protocol sessions that are used to perform remote
    * lookups. Owernship of the instance is given to the caller.
    *
    * @return protocol session
    */
    virtual CPbkxRemoteContactLookupProtocolSession* NewSessionL() = 0;
        
private: // data

	/** iDtor_ID_Key Instance identifier key. When instance of an
    *               implementation is created by ECOM framework, the
    *               framework will assign UID for it. The UID is used in
    *               destructor to notify framework that this instance is
    *               being destroyed and resources can be released.
    */
	TUid iDtor_ID_Key;
    };

#include "cpbkxremotecontactlookupprotocoladapter.inl"

#endif // CPBKXREMOTECONTACTLOOKUPADAPTER_H
