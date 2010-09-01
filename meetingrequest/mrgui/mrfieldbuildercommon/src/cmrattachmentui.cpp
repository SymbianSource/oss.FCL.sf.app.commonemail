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

#include "cmrattachmentui.h"
#include "cmrattachmentprogressinfo.h"
#include "mesmrcalentry.h"
#include "cmrfilemanager.h"
#include "cmrgrid.h"
#include "cesmrglobalnote.h"
#include "esmrhelper.h"

#include <MGFetch.h>
#include <aknlistquerydialog.h>
#include <sysutil.h>
#include <calattachment.h>
#include <NpdApi.h>
#include <AknCommonDialogsDynMem.h>
#include <apparc.h>
#include <esmrgui.rsg>
#include <apgcli.h>
#include <apmrec.h>
#include <CommonContentPolicy.h>
#include <ct/rcpointerarray.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>

#include "emailtrace.h"

namespace // codescanner::namespace
	{
	_LIT8( KUnknownDatatype, "unknown");
	
	const TInt KTempTextFileNameLength( 20 );
	_LIT( KTextFilenameExtension, ".txt" );
	
    /**
     * Check if the character is illegal for the filename
     */
	TBool IsIllegalChar(const TUint aChar)
	    {
	    return (
	        aChar == '*'  ||
	        aChar == '\\' ||
	        aChar == '<'  ||
	        aChar == '>'  ||
	        aChar == ':'  ||
	        aChar == '.'  ||
	        aChar == '"'  ||
	        aChar == '/'  ||
	        aChar == '|'  ||
	        aChar == '?'  ||
	        aChar == CEditableText::EParagraphDelimiter  ||
	        aChar == CEditableText::ELineBreak  ||
	        aChar <  ' ' );
	    }

	/**
	 * Get the file name by making use of the content
	 */
	void GetFileNameFromBuffer( TFileName& aFileName,
	            const TDesC& aBuffer, TInt aMaxLength, const TDesC* aExt /*= NULL*/ )
	    {
	    if(aExt!=NULL)
	        {
	        aMaxLength -= aExt->Length();
	        }

	    TInt len = aBuffer.Length();
	    TInt max = Min( len, aMaxLength );
	    aFileName.Zero();

	    TInt cc = 0;
	    TUint ch;
	    TUint ch1 = 0;
	    TBool spaces = EFalse;
	    for( TInt i = 0; i < len && cc < max; i++ )
	        {
	        ch = aBuffer[i];

	        // ignore spaces from beginning of the buffer until first
	        // non-space is encountered.
	        if( !spaces && ch != ' ' )
	            {
	            spaces = ETrue;
	            }

	        if(i>0)
	            {
	            ch1 = aBuffer[i-1];
	            }

	        // strip illegal chars away.
	        // checks also if previous and current chars are '.'
	        if( spaces && !IsIllegalChar(ch) )
	            {
	            if( !( i > 0 && ch == '.' && ch1 == '.' ) )
	                {
	                aFileName.Append( ch );
	                cc++;
	                }
	            }
	        }

	    aFileName.Trim();

	    // If filename is empty at this point, do not append extension either.
	    // Instead, empty filename is returned so that caller can use whatever
	    // default name
	    if( aFileName.Length() > 0 && aExt != NULL )
	        {
	        aFileName.Append(*aExt);
	        }
	    }
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C CMRAttachmentUi* CMRAttachmentUi::NewL()
	{
    FUNC_LOG;
    
	CMRAttachmentUi* self = new ( ELeave ) CMRAttachmentUi();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMRAttachmentUi::LaunchFetchUi( MESMRCalEntry& aEntry )
	{
    FUNC_LOG;
    
	iEntry = &aEntry;

	TInt error( KErrNone );
	TRAP( error, DoLaunchFetchUiL() );
	return error;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMRAttachmentUi::LaunchViewerUi( MESMRCalEntry& aEntry )
	{
    FUNC_LOG;
    
	iEntry = &aEntry;
	TInt error( KErrNone );
	TRAP( error, DoLaunchViewerUiL() );
	return error;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C CMRAttachmentUi::~CMRAttachmentUi()
	{
    FUNC_LOG;
    
	delete iFilesToCopy;
	delete iFilesToAdd;
	delete iManager;
	delete iProgress;
	iFsSession.Close();
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C void CMRAttachmentUi::SetObserver( MMRAttachmentUiObserver& aObserver )
	{
    FUNC_LOG;
    
	iObserver = &aObserver;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
EXPORT_C TAttachmentOpResult CMRAttachmentUi::LatestOperationResult()
	{
    FUNC_LOG;
    
	TAttachmentOpResult result;
	result.iFiles = &(iManager->CopiedFiles());
	return result;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
CMRAttachmentUi::CMRAttachmentUi()
	{
    FUNC_LOG;
    
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentUi::ConstructL()
	{
    FUNC_LOG;
    
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentUi::DoLaunchFetchUiL()
	{
    FUNC_LOG;
    
	TMediaFileType type = ResolveAttachmentTypeL();
	
	if ( ENoMediaFile != type )
	    {
        // Create array of descriptors for the selected files
        iFilesToCopy = new ( ELeave ) CDesCArrayFlat( 5 );
        iFilesToAdd = new ( ELeave ) CDesCArrayFlat( 1 );
        TBool retval( EFalse );
        HBufC* buf = NULL;
        // Connect session
        User::LeaveIfError( iFsSession.Connect() );
        User::LeaveIfError( iFsSession.ShareProtected() );
    
        if( type == EPresentationsFile )
            {
            buf = CNotepadApi::FetchMemoL();
            if( buf )
                {
                CleanupStack::PushL(buf);
    
                TFileName filename;
                TInt max = KTempTextFileNameLength;
    
                // first try to create filename from memo text.
                GetFileNameFromBuffer( filename, *buf, max, &KTextFilenameExtension );
    
                TFileName tempPath;
                User::LeaveIfError( ESMRHelper::CreateAndAppendPrivateDirToFileName( tempPath ) );
                tempPath.Append( filename );
    
                // check the file name for validity and possible name duplicates.
                TInt err = CApaApplication::GenerateFileName( iFsSession, tempPath );
    
                if( err == KErrNone )
                    {
                    // write buffer to text file
                    CPlainText* text = CPlainText::NewL();
                    CleanupStack::PushL( text );
                    text->InsertL( 0, *buf );
                    text->ExportAsTextL( tempPath, CPlainText::EOrganiseByParagraph, 0 );
                    CleanupStack::PopAndDestroy( text );
                    iFilesToAdd->AppendL( tempPath );
                    }
    
                CleanupStack::PopAndDestroy( buf );
                }
            }
        else if( type == EAnyMediaFile )
            {
            TInt supportedTypes = AknCommonDialogsDynMem::EMemoryTypePhone |
                                  AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage |
                                  AknCommonDialogsDynMem::EMemoryTypeMMCExternal;
    
            TFileName filename;
            retval = AknCommonDialogsDynMem::RunSelectDlgLD( supportedTypes,
                                                       filename,
                                                       R_MR_ATTACHMENT_MEMORY_SELECTION_DIALOG,
                                                       /*aFilter*/NULL,
                                                       /*aVerifier*/this );
            if( retval )
                {
                iFilesToCopy->AppendL( filename );
                }
            }
        else
            {
            // Open the dialog. 
             retval = MGFetch::RunL(
                *iFilesToCopy, // When dialog is closed, fileArray contains selected files
                type, // Displays only media files of type aMediaType
                ETrue,		// single or multiple file selection
                this		// Pointer to class implementing MMGFetchVerifier;
                        // when user has selected file(s),
                        // MMGFetchVerifier::VerifySelectionL is called.
                );
            }
    
        TInt index(0);
        while ( index < iFilesToCopy->MdcaCount() )
            {
            TPtrC filename( iFilesToCopy->MdcaPoint(index) );
            if ( IsDuplicateNameL( filename ) )
                {
                iFilesToCopy->Delete( index );
                
                //though attachment is aleady attached to this entry, show the info note for this info.
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote(ETrue);
                HBufC* cannotAttach = StringLoader::LoadLC( 
                        R_QTN_MEET_REQ_INFO_ALREADY_ATTACHED );
                note->ExecuteLD( *cannotAttach );
                CleanupStack::PopAndDestroy( cannotAttach );                
                }
            else 
                {
                ++index;
                }
            }
        
        // Start copy
        if( iFilesToCopy->MdcaCount() > 0 )
            {
            // Count files to be copied
            iFileCount = iFilesToCopy->Count();
            if( iFileCount > 0 )
                {
                StartCopyOpL();
                }
            }
        // Add directly to CCalAttachment, no copy needed
        else if( iFilesToAdd->MdcaCount() > 0 )
            {
            UpdateEntryL();
            }
	    }
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentUi::StartCopyOpL()
	{
    FUNC_LOG;
    
	// Fetch the first filename from the array
	TInt size( 0 );
	for( TInt i = 0; i < iFileCount; ++i )
		{
		const TDesC& originalPath = (*iFilesToCopy)[i];
		TEntry entry;
		User::LeaveIfError( iFsSession.Entry( originalPath,entry ) );
		// Count total size for the progressbar
		size += entry.iSize;
		}

	if( !iManager )
		{
		iManager = CMRFileManager::NewL(iFsSession);
		}
	if( !iProgress )
		{
		iProgress = CMRAttachmentProgressInfo::NewL( *iManager, *this );
		}
	// Give total size to progress bar
	// Set progressbar as a observer to copy operation
	iProgress->StartProgressNoteL( size );

    // Check if there is space available
    if( !SysUtil::FFSSpaceBelowCriticalLevelL( &iFsSession, size ) )
    	{
		iManager->SetObserver( *iProgress );
		iManager->CopyFilesL( *iFilesToCopy );
    	}
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentUi::UpdateEntryL()
	{
    FUNC_LOG;
    
	MDesC16Array* fileArray = NULL;
	if( iFilesToAdd->MdcaCount() > 0  )
		{
		fileArray = iFilesToAdd;
		}
	else
		{
		fileArray = &iManager->CopiedFiles();
		}

	TInt fileCount( fileArray->MdcaCount() );

	for( TInt i = 0; i < fileCount; ++i )
		{
		const TDesC& fileName( fileArray->MdcaPoint(i) );
		// When copy done add attachment File handles to calentry
		RFile fileHandle;
		User::LeaveIfError( fileHandle.Open( 
		        iFsSession, fileName, EFileWrite ) );
		CleanupClosePushL( fileHandle );
		// create attachment and add to calendar entry
		CCalAttachment* attachment = CCalAttachment::NewFileL( fileHandle );
		
        RApaLsSession apaSession;
        TDataRecognitionResult dataType;
        User::LeaveIfError(apaSession.Connect());
        apaSession.RecognizeData(fileHandle, dataType);
        apaSession.Close();
        
        TPtrC8 contentType( dataType.iDataType.Des8() );
        
        if ( contentType.Length() )
            {
            attachment->SetMimeTypeL( contentType );
            }
        else
            {
            attachment->SetMimeTypeL( KUnknownDatatype );
            }
		
		CleanupStack::PopAndDestroy( &fileHandle );
		CleanupStack::PushL( attachment );
		// set attachment properties
		const TDesC& filenameOriginal = iFilesToCopy->MdcaPoint( i );
		TFileName label;
		TParse parser;
		parser.Set( filenameOriginal , NULL, NULL );
		label.Append( parser.NameAndExt() );
		attachment->SetLabelL( label );
			
		
		
		iEntry->Entry().AddAttachmentL( *attachment ); // calEntry takes ownership
		CleanupStack::Pop( attachment );
		}

	if( iObserver )
		{
		iObserver->AttachmentOperationCompleted( *iFilesToCopy );
		}
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentUi::DoLaunchViewerUiL()
	{
    FUNC_LOG;
    
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
TMediaFileType CMRAttachmentUi::ResolveAttachmentTypeL()
	{
    FUNC_LOG;
    
	TMediaFileType type = ENoMediaFile;
	TInt selectedOption( 0 );
    
	if ( CMRGrid::ExecuteL( selectedOption ) )
        {
        switch( selectedOption )
            {
            case 0:
                type = EImageFile;
                break;
            case 1:
                type = EVideoFile;
                break;
            case 2:
                type = EMusicFile;
                break;
            case 3:
                type = EPresentationsFile;
                break;
            case 4:
                type = EAnyMediaFile;
                break;
    
            default:
                break;
            }
        }
	
	return type;
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentUi::DialogDismissedL( TInt /*aButtonId*/ )
	{
    FUNC_LOG;
    
	// Called whent the copy process has ended
	UpdateEntryL();
	}

// ---------------------------------------------------------------------------
// CMRAttachmentUi::VerifySelectionL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentUi::VerifySelectionL( const MDesCArray* aSelectedFiles )
    {
    FUNC_LOG;
    
    TInt fileCount = aSelectedFiles->MdcaCount();
    TBool isProtected = EFalse;
    for( TInt i = 0; i < fileCount; ++i )
        {
        const TDesC& originalPath = aSelectedFiles->MdcaPoint( i );
        RFile selected;
        User::LeaveIfError( selected.Open( 
                iFsSession, originalPath, EFileRead ) );
        CleanupClosePushL( selected );
        isProtected =  FileDrmProtectedL( selected );
        CleanupStack::PopAndDestroy();
        if ( isProtected )
            {
            break;
            }
        }
    // Protected file returns verification fail and 
    // unprotected file returns verification ok
    return !isProtected;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentUi::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentUi::OkToExitL( 
        const TDesC& aDriveAndPath, 
        const TEntry& aEntry )
    {
    FUNC_LOG;
    
    TFileName path;
    path = aDriveAndPath;
    path.Append( aEntry.iName );
    
    RFile selected;
    User::LeaveIfError( selected.Open( iFsSession, path, EFileRead ) );
    CleanupClosePushL( selected );
    TBool isProtected = FileDrmProtectedL( selected );
    CleanupStack::PopAndDestroy();

    // Protected file returns not ok to exit and 
    // unprotected file returns ok to exit
    return !isProtected;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentUi::FileDrmProtectedL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentUi::FileDrmProtectedL( RFile& aFile )
    {
    FUNC_LOG;
    
    TBool isProtected( EFalse );
    CCommonContentPolicy* ccp = CCommonContentPolicy::NewLC();      
    isProtected = ccp->IsClosedFileL( aFile );
    if ( isProtected )
            {
            CESMRGlobalNote::ExecuteL
                                ( CESMRGlobalNote::EESMRAlarmAlreadyPassed );
            }
    CleanupStack::PopAndDestroy( ccp );
    return isProtected;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentUi::IsDuplicateNameL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentUi::IsDuplicateNameL(
        const TDesC& aSelectedFile )
    {               
    FUNC_LOG;
    
    CCalEntry& entry( iEntry->Entry() );
    
    TInt attachmentCount = entry.AttachmentCountL();
    
    TBool matchNotFound( ETrue );
    if( attachmentCount )
        {
        TParsePtrC fileNameParser(aSelectedFile);
        TPtrC parsedFileName = fileNameParser.NameAndExt();

        for(TInt index = 0; index < attachmentCount && matchNotFound; ++index)
            {     
            CCalAttachment* attachment = entry.AttachmentL(index);
            matchNotFound = parsedFileName.Compare( attachment->Label() ); 
            }
        }
    
    return !matchNotFound; 
    }

// End of file

