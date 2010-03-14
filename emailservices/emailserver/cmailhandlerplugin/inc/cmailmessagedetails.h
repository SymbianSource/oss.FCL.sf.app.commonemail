/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class CMailMessageDetails.
*
*/


#ifndef CMAILCPSHANDLERUTILS_H_
#define CMAILCPSHANDLERUTILS_H_

#include <e32def.h>
#include <e32base.h>
//<cmail>
#include "cfsmailcommon.h"
//</cmail>

class CMailMailboxDetails;

/**
 *  Class holding data related to message
 * 
 *  @lib fsmailserver.exe
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CMailMessageDetails ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @param aMsgId Id of the message
     * @param aMailboxDetails pointer to the mailbox details object
     * @param aSender data
     * @param aSubject data
     * @param aTimeStamp data
     */ 
    static CMailMessageDetails* NewL(
        TFSMailMsgId aMsgId,
        CMailMailboxDetails* aMailboxDetails,
        const TDesC& aSender, 
        const TDesC& aSubject,
        const TTime aTimeStamp );
    
    /**
    * Destructor.
    */
    virtual ~CMailMessageDetails();

private:
    /**
     * ConstructL
     * @param aSender data
     * @param aSubject data
     */ 
    void ConstructL( const TDesC& aSender, const TDesC& aSubject );
    
    /**
     * Constructor
     * @param aMsgId Id of the message
     * @param aMailboxDetails pointer to the mailbox details object
     * @param aTimeStamp data
     */ 
    CMailMessageDetails(
        TFSMailMsgId aMsgId, 
        CMailMailboxDetails* aMailboxDetails, 
        const TTime aTimeStamp );

public: // data
    // For bookkeeping
    // id of the message in the message store
    TFSMailMsgId     iMsgId;
    // cross reference to the mailbox details object
    CMailMailboxDetails* iMailboxDetails;
    // Data
    // sender of the message
    HBufC*           iSender;
    // subject of the message
    HBufC*           iSubject;
    // timestamp of the message
    TTime            iTimeStamp;
    };

#endif /*CMAILCPSHANDLERUTILS_H_*/
