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


#ifndef CESMRICALVIEWERREMOVEMRFROMCALCMD_H
#define CESMRICALVIEWERREMOVEMRFROMCALCMD_H

#include "cesmricalviewerasynccommand.h"
#include "tesmrinputparams.h"
//<cmail>
#include "mesmricalviewerobserver.h"
#include "esmricalviewerdefs.hrh"
//</cmail>
#include <MAgnEntryUi.h>

class CFSMailClient;
class CESMRUiLauncher;

/**
 * Command for removing meeting request from calendar
 * Command completion is reported to caller via MESMRIcalViewerObserver
 * interface.
 */
NONSHARABLE_CLASS( CESMRIcalViewerRemoveMRFromCalCmd ) :
    public CESMRIcalViewerAsyncCommand,
    public MESMRIcalViewerObserver,
    public MAgnEntryUiCallback
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRIcalViewerResponseToMRCmd command.
     * @param aCalSession Reference to calendar session.
     * @param aMailClient Reference to FS Email mail client object
     */
    static CESMRIcalViewerRemoveMRFromCalCmd* NewL(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient );
    
    /**
     * C++ Desctruction
     */
    ~CESMRIcalViewerRemoveMRFromCalCmd();

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
    CESMRIcalViewerRemoveMRFromCalCmd(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient );
    void ConstructL();
    void LaunchUiL();
    void HandleOperationCompletedL(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

private: // Data
    /// Ref: Reference to input parameters
    TESMRInputParams iESMRInputParams;
    /// Ref: Reference to mail client
    CFSMailClient& iMailClient;
    /// Own: MR retriever command
    RPointerArray< MESMRIcalViewerCommand > iMRRetrieverCommands;
    /// Own: Current command index
    TInt iCurrentCommand;
    /// Own: UI launcher
    CESMRUiLauncher* iUILauncher;
    };

#endif

// EOF
