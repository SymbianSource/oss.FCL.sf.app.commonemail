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

#ifndef MCALREMOTEATTACHMENTOPERATION_H
#define MCALREMOTEATTACHMENTOPERATION_H

#include <e32base.h>

class MCalRemoteAttachment;

/**
 * MCalRemoteAttachmentOperation defines interface for calendar attachment operations
 */
class MCalRemoteAttachmentOperation
	{
public: // Destruction 
	/**
	 * Virtual C++ destructor
	 */
	virtual ~MCalRemoteAttachmentOperation() {}
	
public:	// Interface
   
	/**
	 * fetches progress of the operation.
	 * @return Operation progress
	 */
	virtual TInt Progress() const = 0;

	/**
	 * Fetches the information about the attachment related to this command.
	 * @return Attachment information
	 */
	virtual const MCalRemoteAttachment& AttachmentInformation() const = 0;

    /**
     * Returns an extension point for this interface or NULL.
     *
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CalRemoteAttachmentOperationExtension(
            TUid /*aExtensionUid*/ ) { return NULL; }
	};

#endif // MCALREMOTEATTACHMENTOPERATION_H

// EOF

