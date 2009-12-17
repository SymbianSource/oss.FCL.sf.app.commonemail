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
* Description:  Definition of the class MPbkxRemoteContactLookupProtocolSessionObserver.
*
*/



#ifndef MPBKXREMOTECONTACTLOOKUPPROTOCOLSESSIONOBSERVER_H
#define MPBKXREMOTECONTACTLOOKUPPROTOCOLSESSIONOBSERVER_H

#include "cpbkxremotecontactlookupprotocolresult.h"

/**
 *  Observer for the protocol adapter execution.
 *
 *  @since S60 3.1
 */
class MPbkxRemoteContactLookupProtocolSessionObserver 
    {

public:

    /**
     * Notifies that the loose search has been completed.
     * 
     * @param aStatus error code that tells how search went
     * @param aResults search results, ownership is moved to the caller of the
     * loose search.
     */
    virtual void LooseSearchCompleted( 
        TInt aStatus, 
        RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults ) = 0;
    
    /**
     * Notifies that the contact fields retrieval has been completed.
     * 
     * @param aStatus error code that tells how retrieval went
     */
    virtual void ContactFieldsRetrievalCompleted( TInt aStatus ) = 0;
    };

#endif // MPBKXREMOTECONTACTLOOKUPPROTOCOLSESSIONOBSERVER_H
