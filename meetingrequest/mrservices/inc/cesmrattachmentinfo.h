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
* Description:  MRUI attachmentinfo definition
*
*/


#ifndef CESMRATTACHMENTINFO_H
#define CESMRATTACHMENTINFO_H

#include <e32base.h>
#include "cesmrattachment.h"

/**
 * CESMRAttachmentInfo encapsulates MR attachment information
 * and handling of CESMRAttachment objects in own RPointerArray
 *
 * @lib esmrservices.lib
 */
NONSHARABLE_CLASS( CESMRAttachmentInfo ) : public CBase
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRAttachmentInfo object. Ownership transferred to caller.
     *
     * @return Pointer to esmr CESMRAttachmentInfo handler object.
     */
     IMPORT_C static CESMRAttachmentInfo* NewL();

     /**
      * C++ Destructor
      */
    IMPORT_C ~CESMRAttachmentInfo();

public:
    /**
     * Creates new CESMRAttachment object and
     * adds it to the array owned by this class
     *
     * @param aAttachmentName File name of attachment
     * @param size of attachment
     * @param aState state of attachment download
     */
    IMPORT_C void AddAttachmentInfoL(
            const TDesC& aAttachmentName,
            TInt aAttachntSizeInBytes,
            CESMRAttachment::TESMRAttachmentState aState );

    /**
     * adds CESMRAttachment to the array owned by this class
     *
     * @param aAttachment object to be added to the array
     */
     IMPORT_C void AddAttachmentInfoL(
             CESMRAttachment* aAttachment );

     /**
      * Counts number of CESMRAttachment objects in array
      * @return number of objects in array
      */
     IMPORT_C TInt AttachmentCount() const;

     /**
      * Returns CESMRAttachment from array index
      *
      *@param aIndex index of CESMRAttachment to be returned
      *@return object in the index
      */
     IMPORT_C const CESMRAttachment& AttachmentL(
             TInt aIndex ) const;

     /**
      * Removes CESMRAttachment from array index
      *
      *@param aIndex index of CESMRAttachment to be removed from array
      */
     IMPORT_C void RemoveAttachmentAtL(
             TInt aIndex );

private: // Implementation

    CESMRAttachmentInfo();
    void ConstructL();

private: // Data
    /// Own: Attachment objects
    RPointerArray<CESMRAttachment> iAttachments;
    };

#endif // CESMRATTACHMENTINFO_H

// EOF
