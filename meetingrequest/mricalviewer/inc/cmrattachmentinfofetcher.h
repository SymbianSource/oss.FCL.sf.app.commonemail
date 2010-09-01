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
* Description:  MR attachment informatio fetcher definition
*
*/

#ifndef CMRATTACHMENTINFOFETCHER_H
#define CMRATTACHMENTINFOFETCHER_H

#include <e32base.h>
#include "mfsmailrequestobserver.h"
#include "mesmricalviewerobserver.h"
#include "cesmricalviewerasynccommand.h"

class CFSMailMessage;
class CFSMailClient;
class CCalEntry;

/**
 * CMRAttachmentInfoFetcher is responsible for fetching attachment
 * information and storing URI attachment information to
 * calendar entry.
 */
NONSHARABLE_CLASS( CMRAttachmentInfoFetcher ) :
        public CESMRIcalViewerAsyncCommand,
        public MFSMailRequestObserver
    {
public:
public: // Construction and destruction
    /**
     * Creates and initializes new CMRAttachmentInfoFetcher object. 
     * Ownership is transferred to caller.
     * 
     * @param aMailClient Reference to mail client
     * @param aCalSession Reference to calendar session
     * @return Pointer to CESMRImapMailFetcher object.
     */
    static CMRAttachmentInfoFetcher* NewL(
            CFSMailClient& aMailClient,
            CCalSession& aCalSession,
            CCalEntry& aCalEntry );

    /**
     * C++ destructor.
     */
    ~CMRAttachmentInfoFetcher();

private: // From MFSMailRequestObserver
    void RequestResponseL(
            TFSProgress aEvent,
            TInt aRequestId );

private: // From MESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

private: // Implementation
    CMRAttachmentInfoFetcher(
            CFSMailClient& aMailClient,
            CCalSession& aCalSession,
            CCalEntry& aCalEntry );
    void ConstructL();
    void FetchMessageStructureL();
    TBool ConstructAttachmentInformationL();
    void HandleRequestResponseL(
            TFSProgress aEvent,
            TInt aRequestId );
    void NotifyCompletion();

private:
    /** Enumeration for object internal state */
    enum TMRAttachmentFetcherState
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
    TMRAttachmentFetcherState iState;
    /// Ref: Reference to mail client object
    CFSMailClient& iMailClient;
    /// Ref: Reference to messageobject
    CFSMailMessage* iMailMessage;
    /// Own: Operation result
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    /// Ref: Reference to calendar entry
    CCalEntry& iCalEntry;
    };

#endif // CMRATTACHMENTINFOFETCHER_H

// EOF
