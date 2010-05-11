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
* Description: Attachment progress info
*
*/

#ifndef CMRATTACHMENTPROGRESSINFO_H
#define CMRATTACHMENTPROGRESSINFO_H

// INCLUDES
#include <AknProgressDialog.h>
#include <f32file.h>

#include "cmrfilemanager.h"

// FORWARD DECLARATIONS
class CAsyncCallBack;

// CLASS DECLRATIONS
/**
 *
 */
NONSHARABLE_CLASS( CMRAttachmentProgressInfo ) 
	: public CBase, public MProgressDialogCallback, public MMRFileManObserver
	{	
	public: 
		/**
		 * static c'tor
		 */
		static CMRAttachmentProgressInfo* NewL( CMRFileManager& aFileMan, MProgressDialogCallback& aObserver );
		/**
		 * d'tor
		 */
		~CMRAttachmentProgressInfo();

	public: // Inteface
		/**
		 * Start the progress note
		 * @param aFinalValue End value of the process
		 */
		void StartProgressNoteL(TInt aFinalValue);
		/**
		 * Stop the progress. The dialog is dismissed.
		 */
		void StopL();

	public: // From MFileManObserver
		void NotifyStartL( const TDesC& aFilename );
		void NotifyProgress( TInt aBytesCopied );
		void NotifyEnd();
		void NotifyError( TInt aError );

	protected: // Inteface
		/**
		 * The observer will be notified
		 */
		void NotifyObserver();
		/**
		 * Static callback function
		 */
		static TInt NotifyObserverCallback( TAny* aThis );

	protected: // From MProgressDialogCallback
		void DialogDismissedL (TInt aButtonId);

	private:
		CMRAttachmentProgressInfo( CMRFileManager& aFileMan, MProgressDialogCallback& aObserver );
			 
	private:	
		/// Ref:
		CAknProgressDialog*  iProgressDialog;
		/// Ref:
		CEikProgressInfo*    iProgressInfo;
		/// Ref: Manages the file copy op
		CMRFileManager& iFileMan;
		/// Own: Total amount of copied bytes
		TInt iBytesTransferred;
		/// Ref: Client observer
		MProgressDialogCallback& iObserver;
		/// Own: To make observer notification async
		CAsyncCallBack* iAsynCallBack;
		/// Own: Size of the attachment file
		TInt iAttachmentSize;
	};

#endif // CMRATTACHMENTPROGRESSINFO_H
// End of file
