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
* Description:  Resolve meeting request method sync command
*
*/


#ifndef CESMRICALVIEWERRESOLVEMRMETHODSYNC_H
#define CESMRICALVIEWERRESOLVEMRMETHODSYNC_H

//<cmail>
#include "mesmricalviewerobserver.h"
#include "cesmricalviewercommandbase.h"
#include "tesmrinputparams.h"

#include "esmricalviewerdefs.hrh"
//</cmail>

class CESMRInfoIcalRetrieverCmd;

/**
 * Command for resolving meeting request method synchronously.
 *
 * @see TESMRInputParams
 */
NONSHARABLE_CLASS( CESMRIcalViewerResolveMRMethodSyncCmd ) :
    public CESMRIcalViewerCommandBase,
    public MESMRIcalViewerObserver
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRIcalViewerLoadMRDataCmd command.
     * @param aCalSession Reference to calendar session.
     * @param aMailClient Reference to mailclient object.
     * @param aMRMethod Reference to MR Method
     */
    static CESMRIcalViewerResolveMRMethodSyncCmd* NewL(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRMeetingRequestMethod& aMRMethod );

    /**
     * C++ destructor.
     */
    ~CESMRIcalViewerResolveMRMethodSyncCmd();

public: // From MESMRIcalViewerObserver
    void OperationCompleted(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );
    void OperationError(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

public: // From MESMRIcalViewerCommand
    void ExecuteCommandL(
            CFSMailMessage& aMessage,
            MESMRIcalViewerObserver& aObserver );
    void CancelCommand();

private: // Implementation
    CESMRIcalViewerResolveMRMethodSyncCmd(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRMeetingRequestMethod& aMRMethod );
    void ConstructL();

private: // Data
    /// Ref: Reference to input parameters
    TESMRInputParams iESMRInputParams;
    /// Ref: Refefence to mail client objecy
    CFSMailClient& iMailClient;
    /// Own: Command for retrieving MRINFO data
    CESMRInfoIcalRetrieverCmd* iMRInfoRetrieverCmd;
    /// Ref: Meeting request method
    TESMRMeetingRequestMethod& iMRMethod;
    };

#endif // CESMRICALVIEWERRESOLVEMRMETHODSYNC_H

// EOF
