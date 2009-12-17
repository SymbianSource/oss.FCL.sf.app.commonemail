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
* Description:  Class definition for sending meeting request via FS EMail
*
*/

#ifndef C_ESMRSENDMRRESPFSEMAILTASK_H
#define C_ESMRSENDMRRESPFSEMAILTASK_H

//<cmail>
#include "esmrdef.h"
//</cmail>
#include "cesmrtaskbase.h"

class MESMRCalDbMgr;
class MESMRMeetingRequestEntry;
class CMRMailboxUtils;
class CCalEntry;
class CESMRFSEMailManager;
class CESMRMailPlainTextFormatter;
class CESMRCalUserUtil;

/**
 *  Class definition for sending meeting request via FS EMail.
 *
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRSendMRRespFSMailTask ) : public CESMRTaskBase
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CESMRSendMRRespFSMailTask object.
     * Ownership is transferred to caller.
     *
     * @param aCommand Response command.
     * @param aCalDbMgr Reference to cal db manager.
     * @param aEntry Reference to ES MR meeting request calendar entry.
     * @param aMRMailboxUtils Reference mr mailbox utilities.
     * @param aResponseType Response type
     * @param aResponseMessage Pointer to response message buffer.
     */
    static CESMRSendMRRespFSMailTask* NewL(
            TESMRCommand aCommand,
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TESMRResponseType aResponseType,
            const TDesC& aResponseMessage );

    /**
     * C++ destructor.
     */
    ~CESMRSendMRRespFSMailTask();

public:// From MESMRTask
    void ExecuteTaskL();

private:
    CESMRSendMRRespFSMailTask(
            TESMRCommand aCommand,
            MESMRCalDbMgr& aCalDbMgr,
            MESMRMeetingRequestEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            TESMRResponseType aResponseType );

    void ConstructL( const TDesC& aResponseMessage );
    void ConstructMailL();
    void AddPlainTextPartL();
    void AddCalendarPartL();
    void SendMailViaSyncL();
    TInt SaveICalToFileL( const TDesC& aICal, const TDesC& aFilename );
    void SetAttendeesL();
    
private: // Data
    /**
    * Response command
    */
    TESMRCommand iCommand;

    /**
    * FS Email manager
    * Own.
    */
    CESMRFSEMailManager* iEmailMgr;

    /**
    * Plain text formatter
    * Own.
    */
    CESMRMailPlainTextFormatter* iTextFormatter;

    /**
    * Response type
    */
    TESMRResponseType iResponseType;
    /**
    * Response message
    * Own.
    */
    HBufC* iResponseMessage;

    /**
    * Pointer to calendar entry to be sent
    * Own.
    */
    CCalEntry* iEntryToSend;
    
    /**
    * Caluser utility object
    * Own.
    */
    CESMRCalUserUtil* iCaluserUtil;
    };

#endif  // C_ESMRSENDMRRESPFSEMAILTASK_H
