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
* Description:  MRUI attachmen definition
*
*/


#ifndef CESMRATTACHMENT_H
#define CESMRATTACHMENT_H

#include <e32base.h>

/**
 * CESMRAttachment encapsulates MR attachment.
 *
 * @lib esmrservices.lib
 */
NONSHARABLE_CLASS( CESMRAttachment ) : public CBase
    {
public:
    /**
     * Attachment state information
     */
    enum TESMRAttachmentState
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
     * Two-phased constructor. Creates and initializes
     * CESMRAttachment object. Ownership transferred to caller.
     *
     * @return Pointer to esmr CESMRAttachment handler object.
     */
     IMPORT_C static CESMRAttachment* NewL();

     /**
      * Two-phased constructor. Creates and initializes
      * CESMRAttachment object. Ownership transferred to caller and
      * created object is left to cleanup stack.
      * 
      * @param aAttachmentName File name of attachment
      * @param aSizeInBytes size of attachment
      * @param aAttachmentState state of attachment download
      * @return Pointer to esmr CESMRAttachment handler object.
      */
     IMPORT_C static CESMRAttachment* NewL(
             const TDesC& aAttachmentName,
             TInt aSizeInBytes,
             TESMRAttachmentState aAttachmentState );

     /**
      * C++ Destructor.
      */
      IMPORT_C ~CESMRAttachment();

public: // Interface
    /**
     * Sets name of attachment
     *
     * @param aAttachmentName File name of attachment
     */
    IMPORT_C void SetAttachmentNameL(
            const TDes& aAttachmentName );

    /**
     * Gets name of attachment
     *
     * @return File name of attachment
     */
    IMPORT_C const TDesC& AttachmentName() const;

    /**
     * Sets size of attachment
     *
     * @param aAttachmentSizeInBytes size of attachment in bytes
     */
    IMPORT_C void SetAttachmentSize(
            TInt aAttachmentSizeInBytes );

    /**
     * return size of attachment in bytes
     *
     * @return size of attachment
     */
    IMPORT_C TInt AttachmentSizeInBytes() const;

    /**
     * Sets download state of attachment
     *
     * @param aAttachmentState download state of attachment
     */
    IMPORT_C void SetAttachmentState(
            TESMRAttachmentState aAttachmentState );

    /**
     * Gets download state of attachment
     *
     * @return download state of attachment
     */
    IMPORT_C TESMRAttachmentState AttachmenState() const;


private: // Implementation
    CESMRAttachment(
             TInt aSizeInBytes,
             TESMRAttachmentState aAttachmentState );
    void ConstructL(
             const TDesC& aAttachmentName );

private: // data
    /// Own: Attachment name
    HBufC* iAttachmentName;
    /// Own: Attachment size in bytes.
    TInt iSizeInBytes;
    /// Own: AttachmenSize
    TESMRAttachmentState iState;
    };

#endif // CESMRATTACHMENT_H

// EOF
