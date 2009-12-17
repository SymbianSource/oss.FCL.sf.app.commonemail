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


#ifndef CESMRICALVIEWERCMDHANDLER_H
#define CESMRICALVIEWERCMDHANDLER_H

#include <e32base.h>
//<cmail>
#include "mesmricalviewerobserver.h"
#include "mesmricalviewercallback.h"
//</cmail>

class CCalSession;
class CFSMailClient;
class CFSMailMessage;
class MESMRIcalViewerCommand;

/**
 * CESMRIcalViewerCmdHandler is responsible for creating and
 * executing esmricalviewer commands for CFSMailMessages.
 */
NONSHARABLE_CLASS( CESMRIcalViewerCmdHandler ) :
        public CBase,
        public MESMRIcalViewerObserver
    {
public: // Construction and destruction
    /**
     * Creates and initialzes new CESMRIcalViewerCmdHandler object.
     * @param aCallback Reference to ESMRIcalviewer callback.
     * @return Pointer to CESMRIcalViewerCmdHandler object.
     */
    static CESMRIcalViewerCmdHandler* NewL(
            MESMRIcalViewerCallback& aCallback );

    /**
     * C++ destructor
     */
    ~CESMRIcalViewerCmdHandler();

public: // Interface
    /**
     * Executes commands on based on CFSMailMessages based on  operation types
     * @param aOpType commands operation type
     * @param aObserver observer of the asynchronous command result
     * @param aMessage the command is executed on this CFSMailMessage
     */
    void ExecuteCommandL(
            TESMRIcalViewerOperationType aOpType,
            MESMRIcalViewerObserver* aObserver,
            CFSMailMessage& aMessage );

    /**
     * Executes commands on based on CFSMailMessages based on  operation types
     * @param aOpType commands operation type
     * @param aObserver observer of the asynchronous command result
     * @param aMessage the command is executed on this CFSMailMessage
     * @param aParam generic extension parameter type to extend ical commands
     */
    void ExecuteCommandL(
            TESMRIcalViewerOperationType aOpType,
            MESMRIcalViewerObserver* aObserver,
            CFSMailMessage& aMessage,
            TAny* aParam );

    /**
     * Cancel asynchronous operation execution
     */
    void CancelOperation();    
    
public: // From MESMRIcalViewerObserver
    void OperationCompleted(
                    TIcalViewerOperationResult aResult );
    void OperationError(
                    TIcalViewerOperationResult aResult );
    
private: // Implementation
    CESMRIcalViewerCmdHandler(
            MESMRIcalViewerCallback& aCallback );
    void ConstructL();

    void ExecuteCommandInternalL( 
            TESMRIcalViewerOperationType aOpType,
            MESMRIcalViewerObserver* aObserver,
            CFSMailMessage& aMessage,
            TAny* aParam );
    
    /**
     * Operation state of command execution
     */
    enum TState
        {
        EIdle,
        EExecuting
        };

private: // Data
    TState iState;
    /// Own. Calendar database session
    CCalSession* iCalSession;
    /// Own. FS Email mail client object.
    CFSMailClient* iMailClient;
    /// Own: Copy of the message to be handled.
    CFSMailMessage* iMessage;
    /// Own: Command array
    MESMRIcalViewerCommand* iCommand;
    /// Ref: Reference to command observer
    MESMRIcalViewerObserver* iObserver;
    /// Ref: ESMRIcalViewer callback
    MESMRIcalViewerCallback& iCallback;
    /// Own: Result structure
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    };

#endif

// EOF
