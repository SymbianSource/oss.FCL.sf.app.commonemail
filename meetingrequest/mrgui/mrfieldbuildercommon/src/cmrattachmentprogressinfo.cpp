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

#include "cmrattachmentprogressinfo.h"

#include <esmrgui.rsg>
#include <eikprogi.h>
#include <AknWaitDialog.h> 
#include <StringLoader.h>

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

const TInt KOneKiloByte( 1024 );
const TInt KOneMegaByte( 1048576 ); // (1024^2 or 2^20):

}//namespace

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::NewL
// ---------------------------------------------------------------------------
//
CMRAttachmentProgressInfo* CMRAttachmentProgressInfo::NewL(
		CMRFileManager& aFileMan, MProgressDialogCallback& aObserver )
	{
	CMRAttachmentProgressInfo* self =
			new (ELeave) CMRAttachmentProgressInfo( aFileMan, aObserver );
	return self;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::CMRAttachmentProgressInfo
// ---------------------------------------------------------------------------
//
CMRAttachmentProgressInfo::CMRAttachmentProgressInfo(
		CMRFileManager& aFileMan, MProgressDialogCallback& aObserver  )
: iFileMan( aFileMan ), iObserver( aObserver )
	{
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::~CMRAttachmentProgressInfo
// ---------------------------------------------------------------------------
//
CMRAttachmentProgressInfo::~CMRAttachmentProgressInfo()
	{
	if( iProgressDialog )
		{
		// We cannot do anything for an error at this point
		TRAP_IGNORE( iProgressDialog->ProcessFinishedL() ); 
		}
	delete iAsynCallBack;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::StopL
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::StopL()
	{
	iProgressDialog->ProcessFinishedL();
	}
 
// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::StartProgressNoteL
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::StartProgressNoteL(TInt aFinalValue)	
	{
	iAttachmentSize = aFinalValue;
	
	iProgressDialog = new (ELeave) CAknProgressDialog(
			(REINTERPRET_CAST(CEikDialog**, &iProgressDialog)),
			ETrue);
	iProgressDialog->PrepareLC(R_ATTACHMENT_PROGRESS_NOTE);
	iProgressInfo = iProgressDialog->GetProgressInfoL();
	iProgressDialog->SetCallback(this);
	iProgressDialog->RunLD();
	iProgressInfo->SetFinalValue(aFinalValue);
	}
 
// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::DialogDismissedL( TInt aButtonId )
	{	
	iProgressDialog = NULL;
	iProgressInfo = NULL;
	if (aButtonId == EAknSoftkeyCancel)
		{
		  // cancel any process in here
		iFileMan.CancelOp();
		}

	// Notify client asynchronously
	if( iAsynCallBack )
		{
		delete iAsynCallBack;
		iAsynCallBack = NULL;
		}
	TCallBack callback( NotifyObserverCallback, this );
	iAsynCallBack = new (ELeave) CAsyncCallBack(
	        callback,
	        CActive::EPriorityStandard );

	// The observer will not be notified if this is not called
	iAsynCallBack->CallBack();
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::NotifyStart
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::NotifyStartL( const TDesC& aFilename )
	{
	TParse parser;
	parser.Set( aFilename , NULL, NULL );
	const TDesC& filename = parser.NameAndExt();
	
    HBufC* stringholder;

    CDesCArrayFlat* attachmentStrings = 
        new(ELeave)CDesCArrayFlat( 1 );
    CleanupStack::PushL( attachmentStrings );
        
    CArrayFixFlat<TInt>* integers =
        new (ELeave) CArrayFixFlat<TInt>(1);
    CleanupStack::PushL( integers );
    
	if ( iAttachmentSize >= KOneMegaByte )
        {
        attachmentStrings->AppendL( filename ); 

        integers->AppendL( iAttachmentSize / KOneMegaByte );
        
        stringholder = StringLoader::LoadLC(
                R_MEET_REQ_NOTE_ATTACHMENT_PROGRESS_MEGABYTE,
                *attachmentStrings,
                *integers );
        }
    else
        {
        attachmentStrings->AppendL( filename ); 

        integers->AppendL( iAttachmentSize / KOneKiloByte );

        stringholder = StringLoader::LoadLC(
                R_MEET_REQ_NOTE_ATTACHMENT_PROGRESS_KILOBYTE,
                *attachmentStrings,
                *integers );
        }

    iProgressDialog->SetTextProcessing( ETrue );
    iProgressDialog->SetTextL( *stringholder );

    CleanupStack::PopAndDestroy( stringholder );
    CleanupStack::PopAndDestroy( integers );
    CleanupStack::PopAndDestroy( attachmentStrings );

	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::NotifyProgress
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::NotifyProgress( TInt aBytesCopied )
	{
	iBytesTransferred += aBytesCopied;
	if(iProgressInfo)
		{
		iProgressInfo->SetAndDraw(iBytesTransferred);
		}
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::NotifyEnd
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::NotifyEnd()
	{
	if( iProgressDialog )
		{
		// We can't do anything if this leaves
		TRAP_IGNORE( iProgressDialog->ProcessFinishedL() );
		}
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::NotifyObserverCallback
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentProgressInfo::NotifyObserverCallback( TAny* aThis )
	{
	(static_cast<CMRAttachmentProgressInfo*>( aThis ))->NotifyObserver();
	return 0;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::NotifyObserver
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::NotifyObserver()
	{
	// We don't do anything with observer's leaves
	TRAP_IGNORE( iObserver.DialogDismissedL( 0 ) );
	}

// ---------------------------------------------------------------------------
// CMRAttachmentProgressInfo::NotifyError
// ---------------------------------------------------------------------------
//
void CMRAttachmentProgressInfo::NotifyError( TInt /*aError*/ )
	{
	NotifyEnd();
	}
// End of file
