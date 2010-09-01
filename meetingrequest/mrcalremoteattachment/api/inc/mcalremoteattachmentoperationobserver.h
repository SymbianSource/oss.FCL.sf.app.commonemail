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

#ifndef MCALREMOTEATTACHMENTOPERATIONOBSERVER_H
#define MCALREMOTEATTACHMENTOPERATIONOBSERVER_H

#include <e32def.h>

class MCalRemoteAttachmentOperation;
class RFile;

/**
 * MCalRemoteAttachmentOperationObserver is used for indicating
 * attachment download operation progress and completion.
 */
class MCalRemoteAttachmentOperationObserver
    {
public: // Destruction
    
	/**
	 * Virtual desttructor
	 */
	virtual ~MCalRemoteAttachmentOperationObserver() {}
	
public: // Interface	
	
	/**
	 * Indicates about operation progress.
	 * @param aOperation Pointer to operation that was completed.
	 * @param aPercentageCompleted how many percent is done.
	 */
	virtual void Progress(
	         MCalRemoteAttachmentOperation* aOperation,
			 TInt aPercentageCompleted ) = 0;
	
	/**
	 * Called when operation is completed. Command is allowed to close file handle
	 * after calling this method. Observer needs to duplicate file handle if 
	 * needed afterwards.
	 * 
	 * @param aOperation Pointer to operation that was completed.
	 */ 
	virtual void OperationCompleted(
	        MCalRemoteAttachmentOperation* aOperation,
            RFile& aAttachment ) = 0;
			
	/**
	 * Called when operation error occured.
	 * @param aOperation Pointer to operation.
	 * @param aErrorCode Defines operation error code.
	 */ 
	virtual void OperationError(
	        MCalRemoteAttachmentOperation* aOperation,
            TInt aErrorCode ) = 0;	
	};

#endif // MCALREMOTEATTACHMENTOPERATIONOBSERVER_H

// EOF
