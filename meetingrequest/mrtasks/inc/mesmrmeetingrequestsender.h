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
* Description:  Interface definition for ESMR meeting request sender
*
*/


#ifndef MESMRMEETINGREQUESTSENDER_H
#define MESMRMEETINGREQUESTSENDER_H

class MESMRMeetingRequestEntry;

/**
 *  Defines interface for constructing and sending meeting requests
 *  over email.
 *
 * @lib esmrtasks.lib
 */
class MESMRMeetingRequestSender
    {
public:
    /**
     * Virtual destructor.
     */
    virtual ~MESMRMeetingRequestSender() { }

public: // Interface
    /**
     * Creates email from the meeting request and sends it.
     * @param aEntry reference to ES meeting request ebject.
     * @see MESMRMeetingRequestEntry
     */
    virtual void CreateAndSendMeetingRequestL(
            MESMRMeetingRequestEntry& aEntry ) = 0;

    /**
     * Creates email from the meeting request and sends it.
     * @param aEntry reference to ES meeting request ebject.
     * @param aFreePlainText Free plain text to message plain text part.
     * @see MESMRMeetingRequestEntry
     */
    virtual void CreateAndSendMeetingRequestL(
            MESMRMeetingRequestEntry& aEntry,
            const TDesC& aFreePlainText ) = 0;
    };

#endif
