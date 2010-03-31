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
* Description: File manager for managing file copy op
*
*/

#ifndef CMRFILEMANAGER_H
#define CMRFILEMANAGER_H

// INCLUDES
#include <e32base.h>
#include <f32file.h> // RFs
#include <s32file.h> // Streams
#include <badesca.h> // CDesCArrayFlat

// FORWARD DECLARATIONS
class MDesC16Array;
// CLASS DECLARATIONS

/**
 * Notification interface for copy operation
 */
class MMRFileManObserver
	{
	public:
		virtual void NotifyStartL( const TDesC& aFilename ) = 0;
		virtual void NotifyProgress( TInt aBytesCopied ) = 0;
		virtual void NotifyEnd() = 0;
		virtual void NotifyError( TInt aError ) = 0;

	protected:
		virtual ~MMRFileManObserver(){}
	};
/**
 * Class for managing async file copy operations
 */
NONSHARABLE_CLASS( CMRFileManager ) : public CActive
	{
	public: // c'tor and d'tor
		static CMRFileManager* NewL( RFs& aFs );

		~CMRFileManager();

	public: // Interface
		void CopyFilesL( MDesC16Array& aFilenameArray );
		void CopyFileL( const TDesC& aSourceFilename, const TDesC& aDestFilename );
		void CopyFileL( RFile& aSource, RFile& aDest );
		void CancelOp();

		void SetObserver( MMRFileManObserver& aObserver );
		MDesC16Array& CopiedFiles() const;

	public: // From CActive
		void RunL();
		TInt RunError( TInt aError );
		void DoCancel();

	private: // Impl
		CMRFileManager( RFs& aFs );
		void ConstructL();
		void AsyncOp();
		void CopyFileL();
		void PrepareNextFileL();

	private: // Data
		enum TState
			{
			ECopyFile,
			ECancelled,
			EDone
			} iState;
			
		RFs& iFs;
		///
		RFileReadStream iReadStream;
		///
		RFileWriteStream iWriteStream;
		/// Own:
		CDesCArrayFlat* iFileArray;
		/// Own:
		CDesCArrayFlat* iOutputFileArray;
		/// Own: current file index
		TInt iFileIndex;
		/// Own:
		HBufC* iCurrentFilename;
		/// Own:
		HBufC* iTempFilename;
		/// Ref:
		MMRFileManObserver* iObserver;
	};

#endif // CMRFILEMANAGER_H
// End of file
