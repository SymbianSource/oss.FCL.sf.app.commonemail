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
* Description:  Remote Contact Lookup Service UI interface definition
*
*/


#ifndef MPBKXREMOTECONTACTLOOKUPSERVICEUI_H
#define MPBKXREMOTECONTACTLOOKUPSERVICEUI_H

#include <e32std.h>
#include <cntitem.h>
#include "cpbkxremotecontactlookupserviceuicontext.h"
#include "cpbkxremotecontactlookupprotocolaccount.h"
#include "tpbkxremotecontactlookupprotocolaccountid.h"

/**
 *  Remote Contact Lookup Service UI API
 *
 *  @since S60 3.1
 */
class MPbkxRemoteContactLookupServiceUi
    {

public:

    /**
     * Execution context construction parameters.
     */
    class TContextParams 
        {
        public:
            /** Protocol account identifier */
            TPbkxRemoteContactLookupProtocolAccountId iProtocolAccountId;
            
            /** Execution mode */
            CPbkxRemoteContactLookupServiceUiContext::TMode iMode;
        };

    /**
    * Returns default account identifier.
    *
    * @return default account identifier
    */
    virtual TPbkxRemoteContactLookupProtocolAccountId DefaultAccountIdL() const = 0;

    /**
    * Creates new account with given protocol account identifier.
    *
    * @param aProtocolAccountId protocol account ID.
    * @return protocol account
    */
    virtual CPbkxRemoteContactLookupProtocolAccount* NewAccountL(
        const TPbkxRemoteContactLookupProtocolAccountId& aProtocolAccountId ) const = 0;

    /**
     * Create new execution context.
     *
     * @param aParams construction parameters
     * @return execution context
     */     
    virtual CPbkxRemoteContactLookupServiceUiContext* NewContextL( 
        TContextParams& aParams ) const = 0;    
    };

#endif // MPBKXREMOTECONTACTLOOKUPSERVICEUI_H
