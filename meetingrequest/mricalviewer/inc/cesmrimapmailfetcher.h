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
* Description:  ESMR imap email fetcher definition
*
*/


#ifndef CESMRIMAPMAILFETCHER_H
#define CESMRIMAPMAILFETCHER_H

#include <e32base.h>
//<cmail>
#include "mfsmailrequestobserver.h"
#include "mesmricalviewerobserver.h"
//</cmail>
#include "cesmricalviewerasynccommand.h"

class CFSMailMessage;
class CFSMailClient;

/**
 * CESMRImapMailFetcher is responsible for receiving imap mail
 * to device.
 */
NONSHARABLE_CLASS( CESMRImapMailFetcherCmd ) :
        public CESMRIcalViewerAsyncCommand,
        public MFSMailRequestObserver
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CESMRImapMailFetcher object. Ownership
     * is transferred to caller.
     * @param aMailClient Reference to mail client
     * @param aCalSession Reference to calendar session
     * @return Pointer to CESMRImapMailFetcher object.
     */
    static CESMRImapMailFetcherCmd* NewL(
            CFSMailClient& aMailClient,
            CCalSession& aCalSession );

    /**
     * C++ destructor.
     */
    ~CESMRImapMailFetcherCmd();

public: // From MFSMailRequestObserver
    void RequestResponseL(
            TFSProgress aEvent,
            TInt aRequestId );

private: // From MESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

private: // Implementation
    CESMRImapMailFetcherCmd(
            CFSMailClient& aMailClient,
            CCalSession& aCalSession );
    void ConstructL();
    void FetchMessageStructureL();
    TBool FetchMessageContentsL();
    void HandleRequestResponseL(
            TFSProgress aEvent,
            TInt aRequestId );
    void NotifyCompletion();

private:
    /** Enumeration for object internal state */
    enum TESMRImapMailFetcherState
        {
        // Idle
        EIdle,
        // Fetching message structure
        EFetchingStructure,
        // Fetching message contents
        EFetchingContents
        };

private: // Data
    /// Own: Fetch operation request id
    TInt iStructureRequestId;
    /// Own: Fetch operation request id
    TInt iMessagePartRequestId;
    // Own: Current state
    TESMRImapMailFetcherState iState;
    /// Ref: Reference to mail client object
    CFSMailClient& iMailClient;
    /// Ref: Reference to messageobject
    CFSMailMessage* iMailMessage;
    /// Own: Operation result
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    };

#endif // CESMRIMAPMAILFETCHER_H

// EOF
