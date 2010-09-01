/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MR Attachment field for editor and viewer
 *
*/


#ifndef CMRATTACHMENTCOMMANDHANDLER_H
#define CMRATTACHMENTCOMMANDHANDLER_H

#include <e32base.h>
#include <bamdesca.h>

#include "esmrdef.h"
#include "mcalremoteattachmentoperationobserver.h"

// Forward declarations
class CCalEntry;
class CESMRRichTextLink;
class CCalRemoteAttachmentApi;
class CCalRemoteAttachmentInfo;
class CDocumentHandler;
class MESMRFieldEventQueue;
class CMRAttachmentCommand;

/**
 * CMRAttachmentCommandHandler is responsible for handling attachment
 * related commands.
 */
NONSHARABLE_CLASS( CMRAttachmentCommandHandler ):
        public CBase,
        public MCalRemoteAttachmentOperationObserver

    {
public: // Construction and destructions

    /**
     * Creates and initializes new CMRAttachmentCommandHandler object.
     * Ownership is transferred to caller.
     * @param aEntry Reference to calendar entry
     */
    static CMRAttachmentCommandHandler* NewL(
            CCalEntry& aEntry,
            MESMRFieldEventQueue& aEventQueue );

    /**
     * C++ destructor
     */
    ~CMRAttachmentCommandHandler();

public: // Interface

    /**
     * Handles attachment related command for currently selected
     * attachment link.
     * @param aCommandId Defines the command to be executed
     * @param aSelectedLink Reference to selected link.
     */
    void HandleAttachmentCommandL(
            TInt aCommandId,
            const CESMRRichTextLink& aSelectedLink );

    /**
     * Tests whether curerntly selected link is remote or not.
     * @param aSelectedLink Reference to selected link.
     * @return ETrue, if currently selected link is remote attachment
     */
    TBool IsRemoteAttachmentL(
            const CESMRRichTextLink& aSelectedLink );

    /**
     * Sets remote attachment API information for the command handler.
     * @param aRemoteaAttaApi Reference to remote attachment API
     * @param aAttachmentInfo Reference to attachment information structure
     */
    void SetRemoteAttachmentInformationL(
            CCalRemoteAttachmentApi& aRemoteAttaApi,
            CCalRemoteAttachmentInfo& aAttachmentInfo );

    /**
     * Handles remote attachment command.
     * @param aCommandId Defines the command to be executed
     * @param aSelectedLink Reference to selected link.
     */
    void HandleRemoteAttachmentCommandL(
            TInt aCommandId,
            const CESMRRichTextLink& aSelectedLink );

    /**
     * Fetches reference to remote operations
     * @return REference to download operation list.
     */
    RPointerArray<MCalRemoteAttachmentOperation>& RemoteOperations();

    /**
     * Returns the current command id in progress.
     * @return current command id or 0
     */
    TInt CurrentCommandInProgress() const;

private: // From MCalRemoteAttachmentOperationObserver
    void Progress(
             MCalRemoteAttachmentOperation* aOperation,
             TInt aPercentageCompleted );
    void OperationCompleted(
            MCalRemoteAttachmentOperation* aOperation,
            RFile& aAttachment );
    void OperationError(
            MCalRemoteAttachmentOperation* aOperation,
            TInt aErrorCode );

private: // Implementation
    CMRAttachmentCommandHandler(
            CCalEntry& aEntry,
            MESMRFieldEventQueue& aEventQueue );
    void ConstructL();
    TInt ResolveAttachmentIndexL(
            const TDesC& aAttachmentLabel );
    TInt CommandIndex( const TDesC& aAttachmentLabel );
    TInt RemoteAttachmentIndexL(
            const TDesC& aAttachmentLabel );
    void HandleOperationCompletedL(
            MCalRemoteAttachmentOperation* aOperation,
            RFile& aAttachment );
    void HandleAttachmentCommandInternalL(
                TInt aCommandId,
                TInt aAttachmentIndex );
    void UpdateLocalAttachmentL(
            const TDesC& aAttachmentLabel,
            RFile& aAttachment,
            TInt aAttachmentIndex );
    void NotifyProgressL(
            const TDesC& aAttachmentLabel,
            TInt aPercentageCompleted );
    void HandleRemoteAttachmentCommandInternalL(
            TInt aCommandId,
            const TDesC& aRemoteAttachmentLabel );
    void HideDownloadIndicatorL();
    void ExecuteCommandL(
            TInt aCommandId,
            CMRAttachmentCommand& aCommand,
            TInt aAttachmentIndex );
    void SaveAllAttachmentsL();
    void SaveNextRemoteAttachmentL();

private: // Data
    /// Ref: Reference to calendar entry
    CCalEntry& iEntry;
    /// Ref: Remote attachment API
    CCalRemoteAttachmentApi* iRemoteAttachmentApi;
    /// Ref: Remote attachment info
    CCalRemoteAttachmentInfo* iAttachmentInfo;
    /// Own: Download operation
    RPointerArray<MCalRemoteAttachmentOperation> iDownloadOperations;
    /// Own: Remote command in progress
    TInt iCommandInProgress;
    /// Own: Document handler
    CDocumentHandler* iDocHandler;
    /// Ref: Event queue
    MESMRFieldEventQueue& iEventQueue;
    };

#endif // CMRATTACHMENTCOMMANDHANDLER_H

// EOF
