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
* Description: Attachment fetch utility class for event viewers and editors
*
*/

#ifndef CMRATTACHMENTUI_H
#define CMRATTACHMENTUI_H

// INCLUDES
#include <e32base.h>
#include <f32file.h> // RFs
#include <mediafiletypes.hrh> // TMediaFileType
#include <aknprogressdialog.h> // MProgressDialogCallback
#include <mmgfetchverifier.h>
#include <maknfileselectionobserver.h>

// FORWARD DECLARATIONS
class MESMRCalEntry;
class CMRAttachmentProgressInfo;
class CMRFileManager;
class CMRGrid;

// CLASS DECLARATIONS
/**
 * Observer for the attachment operation
 */
class MMRAttachmentUiObserver
	{
	public:
		virtual void AttachmentOperationCompleted( MDesC16Array& aArray ) = 0;

	protected:
		virtual ~MMRAttachmentUiObserver() { }
	};

/**
 * Class for attachment operation results
 */
class TAttachmentOpResult
	{
	public:
		inline TAttachmentOpResult() : iFiles( NULL ){}

	public:
		MDesC16Array* iFiles;
	};

/**
 * Facade class for the attachment related ui operations
 */
NONSHARABLE_CLASS( CMRAttachmentUi ): public CBase, public MProgressDialogCallback, 
                         public MMGFetchVerifier, public MAknFileSelectionObserver
	{
	public:
		/**
		 * Static c'tor
		 */
		IMPORT_C static CMRAttachmentUi* NewL();

		/**
		 * D'tor
		 */
		IMPORT_C ~CMRAttachmentUi();

	public: // Interface

		/**
		 * Launches Attachment Fetch UI
		 * Works synchronously
		 * @param aEntry Entry that will updated with attachments
		 * @return System wide error code
		 */
		IMPORT_C TInt LaunchFetchUi( MESMRCalEntry& aEntry );

		/**
		 * Launches Attachment Fetch UI
		 * Works synchronously
		 * @param aEntry Entry which attachments will be listed.
		 * 					Attachments can be added by user also.
		 * @return System wide error code
		 */
		IMPORT_C TInt LaunchViewerUi( MESMRCalEntry& aEntry );

		/**
		 * Add observer
		 * @param aObserver Observer for attachment process
		 */
		IMPORT_C void SetObserver( MMRAttachmentUiObserver& aObserver );

		IMPORT_C TAttachmentOpResult LatestOperationResult();

	public: // From MProgressDialogCallback
		void DialogDismissedL( TInt aButtonId );
		
	public: // From MMGFetchVerifier
	    TBool VerifySelectionL( const MDesCArray* aSelectedFiles );
	
	public: // From MAknFileSelectionObserver
	    TBool OkToExitL( const TDesC& aDriveAndPath, const TEntry& aEntry );

	private: // Implementation
		CMRAttachmentUi();
		void ConstructL();
		void DoLaunchFetchUiL();
		void DoLaunchViewerUiL();
		TMediaFileType ResolveAttachmentTypeL();
		void StartCopyOpL();
		void UpdateEntryL();
		TBool FileDrmProtectedL( RFile& aFile );
		TBool IsDuplicateNameL( const TDesC& aSelectedFile );
		
	private: // Data
		/// Ref: Currently active entry
		MESMRCalEntry* iEntry;
		/// Own: file server sesssion handle
		RFs iFsSession;
		/// Own: Progress bar
		CMRAttachmentProgressInfo* iProgress;
		/// Own: File count for files to be copied
		TInt iFileCount;
		/// Own: Array of files to be copied
		CDesCArrayFlat* iFilesToCopy;
		/// Own: Array of files to be added without copy
		CDesCArrayFlat* iFilesToAdd;
		/// Own: File copy manager
		CMRFileManager* iManager;
		/// Ref: Attachment ui process observer reference
		MMRAttachmentUiObserver* iObserver;
	};
#endif // CMRATTACHMENTUI_H
// End of file
