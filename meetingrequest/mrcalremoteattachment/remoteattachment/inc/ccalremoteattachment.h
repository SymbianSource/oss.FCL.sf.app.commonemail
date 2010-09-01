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
* Description:
*
*/

#ifndef CCALREMOTEATTACHMENT_H
#define CCALREMOTEATTACHMENT_H

#include <e32base.h>
#include "mcalremoteattachment.h"

/**
 * CCalRemoteAttachment encapsulates one attachment information
 */
NONSHARABLE_CLASS( CCalRemoteAttachment ) : 
    public CBase,
    public MCalRemoteAttachment
    {
public: // Construction and destruction
    
    /**
     * Creates and initializes new CCalRemoteAttachment object.
     * Ownership is transferred to caller.
     * 
     * @param aAttachmentIdentifier Attachment identifier
     * @param aAttachmentLabel Attachment label
     * @param aAttachmentSizeInBytes of attachment
     * @param aState state of attachment download
     */
    IMPORT_C static CCalRemoteAttachment* NewL(
            const TDesC& aAttachmentIdentifier,
            const TDesC& aAttachmentLabel,
            TInt aAttachmentSizeInBytes,
            MCalRemoteAttachment::TCalRemoteAttachmentState aState );
    
    /**
     * C++ destructor 
     */
    IMPORT_C ~CCalRemoteAttachment();
    
public: // From MCalRemoteAttachment
    void SetAttachmentLabelL(
            const TDesC& aAttachmentLabel );
    const TDesC& AttachmentLabel() const;
    void SetAttachmentIdentifierL(
            const TDesC& aAttachmentIdentifier );
    const TDesC& AttachmentIdentifier() const;
    void SetAttachmentSize(
            TInt aAttachmentSizeInBytes );
    TInt AttachmentSizeInBytes() const;
    void SetAttachmentState(
            MCalRemoteAttachment::TCalRemoteAttachmentState aAttachmentState );
    MCalRemoteAttachment::TCalRemoteAttachmentState AttachmenState() const;
    
private: // Implementation
    CCalRemoteAttachment(
            TInt aAttachmentSizeInBytes,
            MCalRemoteAttachment::TCalRemoteAttachmentState aState );
    void ConstructL(
            const TDesC& aAttachmentIdentifier,
            const TDesC& aAttachmentLabel );
    
private: // Data
    /// Own: Attachment identifier
    HBufC* iIdentifier;
    /// Own: Attachment label
    HBufC* iLabel;
    /// Own: Attachment size in bytes
    TInt iSizeInBytes;
    /// Own: Attachment download state
    MCalRemoteAttachment::TCalRemoteAttachmentState iState;
    };

#endif // CCALREMOTEATTACHMENT_H

// EOF
