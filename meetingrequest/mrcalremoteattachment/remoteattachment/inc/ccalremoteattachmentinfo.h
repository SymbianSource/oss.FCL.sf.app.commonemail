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

#ifndef CCALREMOTEATTACHMENTINFO_H
#define CCALREMOTEATTACHMENTINFO_H

#include <e32base.h>
#include <mcalremoteattachmentinfo.h>

class CCalRemoteAttachment;

/**
 * CCalRemoteAttachmentInfo contains information
 * about attachments.
 */
NONSHARABLE_CLASS( CCalRemoteAttachmentInfo ) :
        public CBase,    
        public MCalRemoteAttachmentInfo
    {
public: // Construction and destruction
    /**
     * Creates CCalRemoteAttachmentInfo object.
     * Ownership is transferred to caller.
     * 
     * @return Pointer to CCalRemoteAttachmentInfo
     */
    IMPORT_C static CCalRemoteAttachmentInfo* NewL();
    
    /**
     * C++ destructor
     */
    IMPORT_C ~CCalRemoteAttachmentInfo();
    
public: // 
    void AddAttachmentInfoL(
            const TDesC& aAttachmentIdentifier,
            const TDesC& aAttachmentLabel,
            TInt aAttachmentSizeInBytes,
            MCalRemoteAttachment::TCalRemoteAttachmentState aState );
     void AddAttachmentInfoL(
             MCalRemoteAttachment* aAttachment );
     TInt AttachmentCount() const;
     const MCalRemoteAttachment& AttachmentL(
             TInt aIndex ) const;
     void RemoveAttachmentAtL(
             TInt aIndex );
     void RemoveAttachmentL(
             MCalRemoteAttachment& aAttachment );
     const RPointerArray<MCalRemoteAttachment>& Attachments() const;     
    
private:
    CCalRemoteAttachmentInfo();
    void ConstructL();

private:
    /// Own: Attachment array
    RPointerArray<MCalRemoteAttachment> iAttachments;     
    };

#endif // CCALREMOTEATTACHMENTINFO_H

// EOF

