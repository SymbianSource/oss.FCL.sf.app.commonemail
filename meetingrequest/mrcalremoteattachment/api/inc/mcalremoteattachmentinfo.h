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

#ifndef MCALREMOTEATTACHMENTINFO_H
#define MCALREMOTEATTACHMENTINFO_H

#include <e32base.h>
#include "mcalremoteattachment.h"

/**
 * MCalRemoteAttachmentInfo encapsulates calendar entry attachment information.
 */
class MCalRemoteAttachmentInfo
    {
public: // Construction and destruction

     /**
      * C++ Destructor
      */
    virtual ~MCalRemoteAttachmentInfo() {};

public:
    /**
     * Creates new CESMRAttachment object and
     * adds it to the array owned by this class
     *
     * @param aAttachmentIdentifier Attachment identifier
	 * @param aAttachmentLabel Attachment label
     * @param aAttachmentSizeInBytes of attachment
     * @param aState state of attachment download
     */
    virtual void AddAttachmentInfoL(
            const TDesC& aAttachmentIdentifier,
			const TDesC& aAttachmentLabel,
            TInt aAttachmentSizeInBytes,
            MCalRemoteAttachment::TCalRemoteAttachmentState aState ) = 0;

    /**
     * Adds MCalRemoteAttachment to the array owned by this class. Ownership is
	 * transferred from the caller. 
     *
     * @param aAttachment object to be added to the array
     */
     virtual void AddAttachmentInfoL(
             MCalRemoteAttachment* aAttachment ) = 0;

     /**
      * Counts number of MCalRemoteAttachment objects in array
      * @return number of objects in array
      */
     virtual TInt AttachmentCount() const = 0;

     /**
      * Returns MCalRemoteAttachment from array index
      *
      *@param aIndex index of MCalRemoteAttachment to be returned
      *@return object in the index
      */
     virtual const MCalRemoteAttachment& AttachmentL(
             TInt aIndex ) const = 0;

     /**
      * Removes MCalRemoteAttachment from array index
      *
      *@param aIndex index of MCalRemoteAttachment to be removed from array
      */
     virtual void RemoveAttachmentAtL(
             TInt aIndex ) = 0;

     /**
      * Removes MCalRemoteAttachment from array index
      *
      *@param aIndex index of MCalRemoteAttachment to be removed from array
      */
     virtual void RemoveAttachmentL(
             MCalRemoteAttachment& aAttachment ) = 0;
     
     /**
      * Fetches attachment array.
      * @return attachment array.
      */
     virtual const RPointerArray<MCalRemoteAttachment>& Attachments() const = 0;
     
     /**
      * Returns an extension point for this interface or NULL.
      *
      * @param aExtensionUid Uid of extension
      * @return Extension point or NULL
      */
     virtual TAny* CalRemoteAttachmentInfoExtension(
             TUid /*aExtensionUid*/ ) { return NULL; }
	 };

#endif // MCALREMOTEATTACHMENTINFO_H

// EOF
