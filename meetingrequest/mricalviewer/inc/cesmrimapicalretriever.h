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


#ifndef CESMRIMAPICALRETRIEVER_H
#define CESMRIMAPICALRETRIEVER_H

#include <e32base.h>
//<cmail>
#include "mesmragnimportobserver.h"
#include "CFSMailCommon.h"
//</cmail>
#include "cesmricalviewerasynccommand.h"

class CFSMailClient;
class CCalEntry;
class CESMRAgnExternalInterface;
class CFSMailMessagePart;
class CESMRImapMailFetcherCmd;
class TESMRInputParams;
class CESMRAttachmentInfo;

/**
 * CESMRImapIcalRetriever is responsible for parsing iCal information
 * from email message and importing it to calendar entry.
 */
NONSHARABLE_CLASS( CESMRImapIcalRetrieverCmd ) :
        public CESMRIcalViewerAsyncCommand,
        public MESMRAgnImportObserver,
        public MESMRIcalViewerObserver
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CESMRInfoIcalRetriever object. Ownership
     * is transferred to caller.
     * @param aCalSession Reference to calendar database session.
     * @param aMailClient Reference to mail client.
     * @param aInputParametes Reference to input parameter structure.
     * @return Pointer to CESMRInfoIcalRetriever object.
     */
    static CESMRImapIcalRetrieverCmd* NewL(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRInputParams& aInputParameters );

    /**
     * C++ destructor.
     */
    ~CESMRImapIcalRetrieverCmd();

public: // From MESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

public: // From MAgnImportObserver
    MESMRAgnImportObserver::TImpResponse AgnImportErrorL(
            MESMRAgnImportObserver::TImpError aType,
            const TDesC8& aUid,
            const TDesC& aContext);

public: // From MESMRAgnImportObserver
    void OperationCompleted(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );
    void OperationError(
            MESMRIcalViewerObserver::TIcalViewerOperationResult aResult );

private: // Implementation
    CESMRImapIcalRetrieverCmd(
            CCalSession& aCalSession,
            CFSMailClient& aMailClient,
            TESMRInputParams& aInputParameters );
    void ConstructL();
    void CreateEntryL(
            CFSMailMessagePart& aMessagePart,
            CFSMailMessage& aMsg);
    void CheckAttachmentDataL(
            CFSMailMessage& message );
    void HandleMailContentL();

private: // Data
    /// Own: Calendar entry object
    CCalEntry* iConvertedEntry;
    /// Own: ICal importer object
    CESMRAgnExternalInterface* iCalImporter;
    /// Own: Mail fetcher
    CESMRImapMailFetcherCmd* iMailFetcher;
    /// Ref: Reference to mail client object
    CFSMailClient& iMailClient;
    /// Ref: Reference to input parameter structure
    TESMRInputParams& iInputParameters;
    /// Own: Command result
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    /// Own: ICS filename
    HBufC* iIcsFilename;
    /// Own: Calendar part message id
    TFSMailMsgId iCalendarPartId;
    /// Own
    CESMRAttachmentInfo* iAttachmentInfo;
    };

#endif // CESMRIMAPICALRETRIEVER_H

// EOF
