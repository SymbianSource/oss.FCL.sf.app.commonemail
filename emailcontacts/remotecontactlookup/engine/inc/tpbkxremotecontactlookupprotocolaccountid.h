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
* Description:  Definition of the class TPbkxRemoteContactLookupProtocolAccountId
*
*/


#ifndef TPBKXREMOTECONTACTLOOKUPPROTOCOLACCOUNTID_H
#define TPBKXREMOTECONTACTLOOKUPPROTOCOLACCOUNTID_H

#include <e32std.h>

/**
 *  Represents a protocol account identifier.
 *
 *  @since S60 3.1
 */
class TPbkxRemoteContactLookupProtocolAccountId 
    {

public:

    /**
     * Constructor.
     *
     * @param aProtocolUid protocol ECOM plugin identifier
     * @param aAccountId account identifier
     */
    TPbkxRemoteContactLookupProtocolAccountId( TUid aProtocolUid, TUint aAccountId ) 
        : iProtocolUid( aProtocolUid ), iAccountId( aAccountId ) {};

    /**
     * Protocol ECOM plugin identifier
     */
    TUid iProtocolUid;

    /**
     * Account identifier
     */
    TUint iAccountId;   
    };

#endif // TPBKXREMOTECONTACTLOOKUPPROTOCOLACCOUNTID_H
