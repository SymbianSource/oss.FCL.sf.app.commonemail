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

#ifndef CCALREMOTEATTACHMENTAPI_H
#define CCALREMOTEATTACHMENTAPI_H

#include <e32base.h>
#include <bamdesca.h>

class MCalRemoteAttachment;
class MCalRemoteAttachmentInfo;
class MCalRemoteAttachmentOperation;
class MCalRemoteAttachmentOperationObserver;

/**
 * CCalRemoteAttachmentApi defines calendar attachment download operations.
 */
class CCalRemoteAttachmentApi : public CBase
    {
public: // Construction and destruction

    /**
	 * Creates and initializes new CCalRemoteAttachmentApi object. Ownership 
	 * is transferred to caller. 
	 * @param aUri
	 */
	static CCalRemoteAttachmentApi* NewL( const TDesC8& aUri );
	
	/**
	 * Destructor
	 */
	virtual ~CCalRemoteAttachmentApi();
	
public: // Interface
	
	/**
	 * Fetches attachment information.
	 * @param aAttachmentInfo Reference to attachment info array
	 * @param aAttachmentUrlArray List of attachment URIs which information is wanted 
	 */
	virtual void AttachmentInfo(
	        MCalRemoteAttachmentInfo& aAttachmentInfo,
	        MDesCArray& aAttachmentUrlArray ) const = 0;
	
	/**
	 * Downloads attachment. Ownership of created operation is transferred
	 * to caller. Created operations are stored to <aOperations> array.
	 * 
	 * @param aAttachments Attachments to be downloaded index
	 * @param aOperations Operation array.
	 * @param aObserver Reference to attachment observer
	 */
	virtual void DownloadAttachmentsL(
	        const RPointerArray<MCalRemoteAttachment>& aAttachments,
	        RPointerArray<MCalRemoteAttachmentOperation>& aOperations,
	        MCalRemoteAttachmentOperationObserver& aObserver ) = 0;

private:
        /** iDtor_ID_Key Instance identifier key. When instance of an
         *               implementation is created by ECOM framework, the
         *               framework will assign UID for it. The UID is used in
         *               destructor to notify framework that this instance is
         *               being destroyed and resources can be released.
         */
        TUid iDtor_ID_Key;	
	};

#include <ccalremoteattachmentapi.inl>
	
#endif // CCALREMOTEATTACHMENTAPI_H

// EOF
