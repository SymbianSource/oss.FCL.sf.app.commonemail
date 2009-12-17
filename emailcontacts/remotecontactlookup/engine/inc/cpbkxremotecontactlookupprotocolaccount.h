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
* Description:  Definition of the class CPbkxRemoteContactLookupAccount
*
*/


#ifndef CPBKXREMOTECONTACTLOOKUPACCOUNT_H
#define CPBKXREMOTECONTACTLOOKUPACCOUNT_H

#include <e32base.h>
#include "tpbkxremotecontactlookupprotocolaccountid.h"

/**
 *  Describes a protocol account that is used to identify and account to a
 *  remote server, which has a database of contacts.
 *
 *  @since S60 3.1
 */
class CPbkxRemoteContactLookupProtocolAccount : public CBase
    {

public:

    /**
     * Returns protocol accounts identifier.
     *
     * @return account identifier
     */
    virtual TPbkxRemoteContactLookupProtocolAccountId Id() const = 0;

    /**
     * Returns protocol accounts name.
     *
     * @return account name
     */
    virtual const TDesC& Name() const = 0;    
    };

#endif // CPBKXREMOTECONTACTLOOKUPACCOUNT_H
