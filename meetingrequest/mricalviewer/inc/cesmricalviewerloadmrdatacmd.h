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
* Description:  ESMR imap calendar entry retriever definition
*
*/


#ifndef CESMRICALVIEWERLOADMRDATACMD_H
#define CESMRICALVIEWERLOADMRDATACMD_H

//<cmail>
#include "mesmricalviewerobserver.h"
//</cmail>
#include "cesmricalviewerasynccommand.h"
#include "tesmrinputparams.h"

class CFSMailClient;

/**
 * Command for loading meeting request data from mail messages.
 * Command stores the find data into TESMRInputParams structure.
 *
 * @see TESMRInputParams
 */
NONSHARABLE_CLASS( CESMRIcalViewerLoadMRDataCmd ) :
    public CESMRIcalViewerAsyncCommand,
    public MESMRIcalViewerObserver
    {
public: // Construction and destruction

    /**
     * Creates and initializes new CESMRIcalViewerLoadMRDataCmd command.
     * @param aCalSession Reference to calendar session.
     * @param aESMRInputParams Reference to parameter structure
     */
    static CESMRIcalViewerLoadMRDataCmd* NewL(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRInputParams& aESMRInputParams );

    /**
     * C++ Destruction
     */
    ~CESMRIcalViewerLoadMRDataCmd();

public: // From MESMRIcalViewerObserver
    void OperationCompleted(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );
    void OperationError(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

private: // From MESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

private: // Implementation
    CESMRIcalViewerLoadMRDataCmd(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRInputParams& aESMRInputParams );
    void ConstructL();

private: // Data
    /// Ref: Reference to input parameters
    TESMRInputParams& iESMRInputParams;
    /// Ref: Reference to mail client
    CFSMailClient& iMailClient;
    /// Own: MR retriever command
    RPointerArray< MESMRIcalViewerCommand > iMRRetrieverCommands;
    /// Own: Current command index
    TInt iCurrentCommand;
    };

#endif // CESMRICALVIEWERLOADMRDATACMD_H

// EOF
