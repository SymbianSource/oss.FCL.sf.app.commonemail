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
* Description:  Resolving meeting request method asynchronoysly
*
*/


#ifndef CESMRICALVIEWERRESOLVEMRMETHODASYNC_H
#define CESMRICALVIEWERRESOLVEMRMETHODASYNC_H

//<cmail>
#include "mesmricalviewerobserver.h"
#include "cesmricalviewerasynccommand.h"
#include "tesmrinputparams.h"
#include "esmricalviewerdefs.hrh"
//</cmail>

class CFSMailClient;

/**
 * Command for resolving meeting request method asynchronously
 * Resolved method is reported to caller via MESMRIcalViewerObserver
 * interface.
 */
NONSHARABLE_CLASS( CESMRIcalViewerResolveMRMethodASyncCmd ) :
    public CESMRIcalViewerAsyncCommand,
    public MESMRIcalViewerObserver
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRIcalViewerLoadMRDataCmd command.
     * @param aCalSession Reference to calendar session.
     * @param aESMRInputParams Reference to parameter structure
     */
    static CESMRIcalViewerResolveMRMethodASyncCmd* NewL(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient );
    
    /**
     * C++ Desctruction
     */
    ~CESMRIcalViewerResolveMRMethodASyncCmd();

public: // From MESMRIcalViewerObserver
    void OperationCompleted(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );
    void OperationError(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

private: // From MESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

private: // Implementation
    CESMRIcalViewerResolveMRMethodASyncCmd(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient );
    void ConstructL();

private: // Data
    /// Ref: Reference to input parameters
    TESMRInputParams iESMRInputParams;
    /// Ref: Reference to mail client
    CFSMailClient& iMailClient;
    /// Own: MR retriever command
    RPointerArray< MESMRIcalViewerCommand > iMRRetrieverCommands;
    /// Own: Current command index
    TInt iCurrentCommand;
    /// Own: Meeting request method
    TESMRMeetingRequestMethod iMRMethod;
    };

#endif // CESMRICALVIEWERRESOLVEMRMETHODASYNC_H

// EOF
