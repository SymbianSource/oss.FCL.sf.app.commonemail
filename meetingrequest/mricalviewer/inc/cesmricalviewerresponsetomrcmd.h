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
* Description:  Responsing to meeting request asynchronously
*
*/


#ifndef CESMRICALVIEWERRESPONSETOMRCMD_H
#define CESMRICALVIEWERRESPONSETOMRCMD_H

#include "cesmricalviewerasynccommand.h"
#include "tesmrinputparams.h"
//<cmail>
#include "mesmricalviewerobserver.h"
#include "mesmricalviewercallback.h"
#include "esmricalviewerdefs.hrh"
//</cmail>
#include <magnentryui.h>

class CFSMailClient;
class CESMRUiLauncher;

/**
 * Command for responding to meeting request asynchronously
 * Command completion is reported to caller via MESMRIcalViewerObserver
 * interface.
 */
NONSHARABLE_CLASS( CESMRIcalViewerResponseToMRCmd ) :
    public CESMRIcalViewerAsyncCommand,
    public MESMRIcalViewerObserver,
    public MAgnEntryUiCallback
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRIcalViewerResponseToMRCmd command.
     * @param aCallback callback interface to MESMRIcalViewerCallback
     * @param aCalSession Reference to calendar session.
     * @param aMailClient Reference to FS Email mail client object
     * @param aAttendeeStatus status of meeting request participant
     */
    static CESMRIcalViewerResponseToMRCmd* NewL(
            MESMRIcalViewerCallback& aCallback,
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRAttendeeStatus aAttendeeStatus );

    /**
     * C++ Destructor
     */
    ~CESMRIcalViewerResponseToMRCmd();

public: // From MESMRIcalViewerObserver
    void OperationCompleted(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );
    void OperationError(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

public: // From MAgnEntryUiCallback
    void ProcessCommandL(TInt aCommandId);
    TInt ProcessCommandWithResultL( TInt aCommandId );
    TBool IsCommandAvailable( TInt aCommandId );

private: // From MESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

private: // Implementation
    CESMRIcalViewerResponseToMRCmd(
            MESMRIcalViewerCallback& aCallback,
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRAttendeeStatus aAttendeeStatus );
    void ConstructL();
    void LaunchUiL();
    void HandleOperationCompletedL(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

private: // Data
    /// Ref: Reference to ESMRIcalviewer callback
    MESMRIcalViewerCallback& iCallback;
    /// Ref: Reference to input parameters
    TESMRInputParams iESMRInputParams;
    /// Ref: Reference to mail client
    CFSMailClient& iMailClient;
    /// Own: MR retriever command
    RPointerArray< MESMRIcalViewerCommand > iMRRetrieverCommands;
    /// Own: Current command index
    TInt iCurrentCommand;
    /// Own: Attendee status
    TESMRAttendeeStatus iAttendeeStatus;
    /// Own: Current callback command
    TESMRIcalViewerOperationType iCurrentCbCommand;
    /// Own: Asynchronous email command
    MESMRIcalViewerCommand* iEmailCommand;
    /// Own: Command received via callback
    TInt iMRViewerCommand;
    /// Own: UI launcher
    CESMRUiLauncher* iUILauncher;
    };

#endif

// EOF
