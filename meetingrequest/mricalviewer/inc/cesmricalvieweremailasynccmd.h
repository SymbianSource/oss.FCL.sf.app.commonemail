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
* Description:  Execuing email specific command asynchronously
*
*/


#ifndef CESMRICALVIEWEREMAILASYNCCMD_H
#define CESMRICALVIEWEREMAILASYNCCMD_H

#include "cesmricalviewerasynccommand.h"
//<cmail>
#include "mesmricalviewerobserver.h"
#include "esmricalvieweropcodes.hrh"
#include "mesmricalviewercallback.h"
//</cmail>

class CFSMailClient;


/**
 * Command for triggering asynchronous fs_email command into execution.
 * FS_Email notifies after commad completion via MESMRIcalViewerObserver
 * interface.
 */
NONSHARABLE_CLASS( CESMRIcalViewerEmailAsyncCmd ) :
    public CESMRIcalViewerAsyncCommand,
    public MESMRIcalViewerObserver
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRIcalViewerEmailAsyncCmd command.
     * @param aOperation Operation to be executed.
     * @param aCalSession Reference to calendar session.
     * @param aCallback Reference to ESMRIcalViewer callback
     */
    static CESMRIcalViewerEmailAsyncCmd* NewL(
            TESMRIcalViewerOperationType aOperation,
            CCalSession& aCalSession,
            MESMRIcalViewerCallback& aCallback );
    
    /**
     * C++ Destructor
     */
    ~CESMRIcalViewerEmailAsyncCmd();

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
    CESMRIcalViewerEmailAsyncCmd(
            TESMRIcalViewerOperationType aOperation,
            CCalSession& aCalSession,
            MESMRIcalViewerCallback& aCallback );
    void ConstructL();

private: // Data
    /// Own: Email operation to be executed
    TESMRIcalViewerOperationType iOperation;
    /// Ref: Reference to ESMRIcalViewer callback
    MESMRIcalViewerCallback& iCallback;
    };

#endif // CESMRICALVIEWEREMAILASYNCCMD_H

// EOF

