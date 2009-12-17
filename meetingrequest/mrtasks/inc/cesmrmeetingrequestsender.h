/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR meeting request sender
*
*/


#ifndef C_ESMRPOLICYCHECKER_H
#define C_ESMRPOLICYCHECKER_H

#include <e32base.h>
#include "mesmrmeetingrequestsender.h"

class CMRMailboxUtils;

/**
 *  Implements interface for constructing and sending meeting requests
 *  over email.
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS(CESMRMeetingRequestSender) :
        public CBase,
        public MESMRMeetingRequestSender
    {
public: // Construction and destruction

    /**
     * Two-phased constructor. Creates and initializes
     * CESMRMeetingRequestSender object.
     * @return Pointer to esmr policy checker object.
     */
    IMPORT_C static CESMRMeetingRequestSender* NewL(
            CMRMailboxUtils& aMailboxUtils );

    /**
     * Destructor.
     */
    IMPORT_C ~CESMRMeetingRequestSender();

public:// From MESMRMeetingRequestSender

    void CreateAndSendMeetingRequestL( MESMRMeetingRequestEntry& aEntry );

    void CreateAndSendMeetingRequestL( MESMRMeetingRequestEntry& aEntry,
                                       const TDesC& aFreePlainText );

private:
    CESMRMeetingRequestSender( CMRMailboxUtils& aMailboxUtils );

    void ConstructL();

private:
    /**
    * Mailbox utilities
    * Not Own.
    */
    CMRMailboxUtils& iMailboxUtils;
    };

#endif // C_ESMRPOLICYCHECKER_H
