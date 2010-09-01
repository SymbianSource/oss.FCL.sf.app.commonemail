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
* Description:  Open meeting request command definition
*
*/


#ifndef CESMRICALVIEWEROPENMRCMD_H
#define CESMRICALVIEWEROPENMRCMD_H

#include "cesmricalviewerasynccommand.h"
#include "tesmrinputparams.h"
//<cmail>
#include "mesmricalviewerobserver.h"
#include "mesmricalviewercallback.h"
//</cmail>
#include <MAgnEntryUi.h>


class CESMRUiLauncher;
class CFSMailClient;
class CESMRIcalViewerLoadMRDataCmd;
class MESMRIcalViewerCommand;
class CMRAttachmentInfoFetcher;

/**
 * Command for opening meeting request
 */
NONSHARABLE_CLASS( CESMRIcalViewerOpenMRCmd ) :
    public CESMRIcalViewerAsyncCommand,
    public MESMRIcalViewerObserver,
    public MAgnEntryUiCallback
    {
public: // Construction and destruction
    /**
     *  Creates and initializes new CESMRIcalViewerOpenMRCmd object.
     * @param aCallback Reference to ESMRIcalViewer callback.
     * @param aMailClient Reference to mail client object.
     * @param aCalSession Reference to calendar db session.
     */
    static CESMRIcalViewerOpenMRCmd* NewL(
            MESMRIcalViewerCallback& aCallback,
            CFSMailClient& aMailClient,
            CCalSession& aCalSession );

    /**
     * C++ destructor.
     */
    ~CESMRIcalViewerOpenMRCmd();

public: // From MESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

public: // From MESMRIcalViewerObserver
    void OperationCompleted(
             MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );
    void OperationError(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

public: // From MAgnEntryUiCallback
    void ProcessCommandL(TInt aCommandId);
    TInt ProcessCommandWithResultL( TInt aCommandId );
    TBool IsCommandAvailable( TInt aCommandId );

private: // Implementation
    CESMRIcalViewerOpenMRCmd(
            MESMRIcalViewerCallback& aCallback,
            CFSMailClient& aMailClient,
            CCalSession& aCalSession );
    void ConstructL();
    void HandleOperationCompeletedL(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );
    void HandleError( TInt aError );
    void LaunchUIL();
    void FillAttachmentInfoL();

private: // Data
    /// Ref: Reference to ESMRICalViewer callback
    MESMRIcalViewerCallback& iCallback;
    /// Own: Input parameters
    TESMRInputParams iInputParameters;
    /// Ref: Reference to FS Email mail client object
    CFSMailClient& iMailClient;
    /// Own: Load MR Data command
    CESMRIcalViewerLoadMRDataCmd* iLoadMRDataCmd;
    /// Own: Attachment information fetcher
    CMRAttachmentInfoFetcher* iAttachmentInfoFetcher;
    /// Own: UI launcher
    CESMRUiLauncher* iUILauncher;
    /// Own: Operation result
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    /// Own: Current callback command
    TESMRIcalViewerOperationType iCurrentCbCommand;
    /// Own: Asynchronous email command
    MESMRIcalViewerCommand* iEmailCommand;
    /// Own: Command received via callback
    TInt iMRViewerCommand;
    };

#endif // CESMRICALVIEWEROPENMRCMD_H

// EOF
