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
* Description:  Build information file for project esmrgui
*
*/


#ifndef MCALREMOTEATTACHMENT_H
#define MCALREMOTEATTACHMENT_H

#include <e32base.h>

/**
 * MCalRemoteAttachment encapsulates calendar entry attachment.
 */
class MCalRemoteAttachment
    {
public:
    /**
     * Attachment state information
     */
    enum TCalRemoteAttachmentState
        {
        /** Unknown attachment state */
        EAttachmentStateUnknown,
        /** Attachment is now downlaoded to device */
        EAttachmentStateNotDownloaded,
        /** Attachment is downloaded to device */
        EAttachmentStateDownloaded
        };

public: // Construction and destruction

     /**
      * C++ Destructor.
      */
      virtual ~MCalRemoteAttachment() {} ;

public: // Interface
    /**
     * Sets name of attachment
     * @param aAttachmentLabel Attachment label
     */
    virtual void SetAttachmentLabelL(
            const TDesC& aAttachmentLabel ) = 0;

    /**
     * Gets label of attachment
     * @return Label name of attachment
     */
    virtual const TDesC& AttachmentLabel() const = 0;

    /**
     * Sets attachment identifier
     *
     * @param aAttachmentIdentifier Attachment identifier
     */
    virtual void SetAttachmentIdentifierL(
            const TDesC& aAttachmentIdentifier ) = 0;

    /**
     * Fetches attachment identifier
     *
     * @return Attachment identifier
     */
    virtual const TDesC& AttachmentIdentifier() const = 0;

    /**
     * Sets size of attachment
     *
     * @param aAttachmentSizeInBytes size of attachment in bytes
     */
    virtual void SetAttachmentSize(
            TInt aAttachmentSizeInBytes ) = 0;

    /**
     * return size of attachment in bytes
     *
     * @return size of attachment
     */
    virtual TInt AttachmentSizeInBytes() const = 0;

    /**
     * Sets download state of attachment
     *
     * @param aAttachmentState download state of attachment
     */
    virtual void SetAttachmentState(
            MCalRemoteAttachment::TCalRemoteAttachmentState aAttachmentState ) = 0;

    /**
     * Gets download state of attachment
     *
     * @return download state of attachment
     */
    virtual MCalRemoteAttachment::TCalRemoteAttachmentState 
            AttachmenState() const = 0;

    /**
     * Returns an extension point for this interface or NULL.
     *
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CalRemoteAttachmentExtension(
            TUid /*aExtensionUid*/ ) { return NULL; }
    };

#endif // MCALREMOTEATTACHMENT_H

// EOF
