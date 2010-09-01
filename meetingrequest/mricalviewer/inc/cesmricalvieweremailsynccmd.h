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


#ifndef CESMRICALVIEWEREMAILSYNCCMD_H
#define CESMRICALVIEWEREMAILSYNCCMD_H

//<cmail>
#include "mesmricalviewerobserver.h"
#include "mesmricalviewercallback.h"
#include "cesmricalviewercommandbase.h"
#include "tesmrinputparams.h"

#include "esmricalviewerdefs.hrh"
//</cmail>

/**
 * Command for resolving meeting request method synchronously.
 *
 * @see TESMRInputParams
 */
NONSHARABLE_CLASS( CESMRIcalViewerEmailSyncCmd ) :
    public CESMRIcalViewerCommandBase
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRIcalViewerEmailSyncCmd command.
     * @param aOperation Operation to be executed.
     * @param aCalSession Reference to calendar session.
     * @param aCallback Reference to ESMRIcalViewer callback
     */
    static CESMRIcalViewerEmailSyncCmd* NewL(
            TESMRIcalViewerOperationType aOperation,
            CCalSession& aCalSession,
            MESMRIcalViewerCallback& aCallback );

    /**
     * C++ destructor.
     */
    ~CESMRIcalViewerEmailSyncCmd();

public: // From MESMRIcalViewerCommand
    void ExecuteCommandL(
            CFSMailMessage& aMessage,
            MESMRIcalViewerObserver& aObserver );
    void CancelCommand();

private: // Implementation
    CESMRIcalViewerEmailSyncCmd(
            TESMRIcalViewerOperationType aOperation,
            CCalSession& aCalSession,
            MESMRIcalViewerCallback& aCallback );
    void ConstructL();

private: // Data
    /// Ref: Reference to FS Email callback
    MESMRIcalViewerCallback& iCallback;
    /// Own: Email operation
    TESMRIcalViewerOperationType iOperation;
    /// Own: Result structure
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    };

#endif // CESMRICALVIEWEREMAILSYNCCMD_H

// EOF
