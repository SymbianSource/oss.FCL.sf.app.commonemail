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
 * Description:
 *
 */

#ifndef CMRCMAILREMOTEATTACHMENTDOWNLOADER_H
#define CMRCMAILREMOTEATTACHMENTDOWNLOADER_H

#include <e32base.h>
#include <bamdesca.h>
#include <f32file.h>

#include "mcalremoteattachmentoperation.h"
#include "mfsmailrequestobserver.h"
#include "tmrcmailremoteattachmentpluginurlparser.h"

class CFSMailMessage;
class CFSMailClient;
class CCalRemoteAttachment;
class MCalRemoteAttachmentOperationObserver;

/**
 * CMRCmailRemoteAttachmentDownloader is responsible for
 * downloading remote attachment.
 */
NONSHARABLE_CLASS( CMRCmailRemoteAttachmentDownloader ) :
        public CActive,
        public MCalRemoteAttachmentOperation,
        public MFSMailRequestObserver
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CMRCmailRemoteAttachmentDownloader
     * object. Ownership is transferred to caller.
     *
     * @param aMailClient Reference to mail client
     * @param aObserver Reference to command observer
     * @param aUri Attachment URI
     */
    static CMRCmailRemoteAttachmentDownloader* NewL(
            CFSMailClient& aMailClient,
            MCalRemoteAttachmentOperationObserver& aObserver,
            const TDesC& aUri );

    /**
     * C++ destructor
     */
    ~CMRCmailRemoteAttachmentDownloader();

public: // From CActive
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

public: // From MCalRemoteAttachmentOperation
    TInt Progress() const;
    const MCalRemoteAttachment& AttachmentInformation() const;

private: // From MFSMailRequestObserver
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

private: // Implementation
    CMRCmailRemoteAttachmentDownloader(
            CFSMailClient& aMailClient,
            MCalRemoteAttachmentOperationObserver& aObserver );
    void ConstructL(
            const TDesC& aUri );
    void IssueRequest();
    void NotifyCompletionL( CFSMailMessagePart& aAttachment );
    void NotifyError( TInt aError );
    TInt ProgressL() const;
    void DoCancelL();

private: // Data
    /// Ref: Reference to mail client
    CFSMailClient& iMailClient;
    /// Ref: Command observer
    MCalRemoteAttachmentOperationObserver& iObserver;
    /// Own: Remote attachment information
    CCalRemoteAttachment* iRemoteAttachment;
    /// Own: Attachment URI
    HBufC* iUri;
    /// Own: Mail message
    CFSMailMessage* iMailMessage;
    /// Own: Mail operation ID
    TInt iFSMailOperationId;
    /// Own: Mail ID parser
    TMRCMailRemoteAttachmentPluginURLParser iMailIdParser;
    /// Own: Attachment file
    RFile iAttachmentFile;
    };

#endif // CMRCMAILREMOTEATTACHMENTDOWNLOADER_H

// EOF
