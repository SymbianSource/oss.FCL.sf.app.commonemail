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
* Description:  Definition of the class CPbkxRemoteContactLookupProtocolSession.
*
*/


#ifndef CPBKXREMOTECONTACTLOOKUPPROTOCOLSESSION_H
#define CPBKXREMOTECONTACTLOOKUPPROTOCOLSESSION_H

#include "cpbkxremotecontactlookupprotocolresult.h"
#include "mpbkxremotecontactlookupprotocolsessionobserver.h"
#include "tpbkxremotecontactlookupprotocolaccountid.h"

/**
 *  Protocol session.
 *
 *  @since S60 3.1
 */
class CPbkxRemoteContactLookupProtocolSession : public CBase
    {

public:

    /**
     * Initialize session search session.
     *
     * @param aObserver observer for the session
     * @param aAccountId remote account that is searched
     */
    virtual void InitializeL( 
        MPbkxRemoteContactLookupProtocolSessionObserver& aObserver,
        const TPbkxRemoteContactLookupProtocolAccountId& aAccountId ) = 0;
        

    /**
     * Start loose search with given query.     
     *
     * @param aQueryText query text
     * @param aMaxMatches maximum number of matches
     */
    virtual void LooseSearchL( const TDesC& aQueryText, TInt aMaxMatches ) = 0;
     
    /**
     * Retrieves rest of the contact fields. Contact items in the results
     * array are reused and new fields are just added to them.
     *
     * @param aResults set of results for which the 
     */
    virtual void RetrieveContactFieldsL( 
        RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults ) = 0;
        
    /**
     * Cancels either the search or detail retrieval process.
     */
    virtual void Cancel() = 0;            
    };

#endif // CPBKXREMOTECONTACTLOOKUPPROTOCOLSESSION_H
