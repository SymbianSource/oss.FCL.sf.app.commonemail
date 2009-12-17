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
* Description:  Definition of the class MPbkxRemoteContactLookupProtocolEnv
*
*/


#ifndef MPBKXREMOTECONTACTLOOKUPPROTOCOLENV_H
#define MPBKXREMOTECONTACTLOOKUPPROTOCOLENV_H

#include <cntitem.h>
#include "cpbkxremotecontactlookupprotocolaccount.h"
#include "cpbkxremotecontactlookupprotocolresult.h"

/**
 *  Interface to construct instances of classes needed by the protocol adaptation.
 *
 *  @since S60 3.1
 */
class MPbkxRemoteContactLookupProtocolEnv
    {
public:

    /**
     * Factory method to create a new protocol account instance. Ownership
     * is given to the caller.
     *
     * @param aId protocol account identifier
     * @param aName protocol account name
     *
     * @return protocol account
     */
    virtual CPbkxRemoteContactLookupProtocolAccount* NewProtocolAccountL( 
        TPbkxRemoteContactLookupProtocolAccountId aId, 
        const TDesC& aName ) = 0;
        
    /**
     * Factory method to create a new protocol result instance. Ownership is
     * given to the caller.
     *
     * @param aIsComplete is search result complete?
     * @param aContactItem contact item, owernship is transfered to this object
     * @param aExtraProtocolData extra protocol data for two-phase search
     *
     * @return protocol result
     */
    virtual CPbkxRemoteContactLookupProtocolResult* NewProtocolResultL(
        TBool aIsComplete, 
        CContactItem& aContactItem, 
        const TDesC& aExtraProtocolData ) = 0;
    };

#endif // MPBKXREMOTECONTACTLOOKUPPROTOCOLENV_H
