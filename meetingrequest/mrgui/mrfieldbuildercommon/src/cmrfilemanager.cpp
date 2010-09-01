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

#include "cmrfilemanager.h"
#include "esmrhelper.h"
#include <f32file.h>

/// LOCAL NAMESPACE
namespace
	{
// File copying block size
const TInt KWriteBlockSize( 1024  );
	}

// ---------------------------------------------------------------------------
// CMRFileManager::NewL
// ---------------------------------------------------------------------------
//
CMRFileManager* CMRFileManager::NewL( RFs& aFs )
    {
    CMRFileManager* self = new ( ELeave ) CMRFileManager( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRFileManager::~CMRFileManager
// ---------------------------------------------------------------------------
//
CMRFileManager::~CMRFileManager()
    {
    delete iFileArray;
    delete iCurrentFilename;
    delete iTempFilename;
    delete iOutputFileArray;
    iReadStream.Close();
    iWriteStream.Close();
    }

// ---------------------------------------------------------------------------
// CMRFileManager::CMRFileManager
// ---------------------------------------------------------------------------
//
CMRFileManager::CMRFileManager( RFs& aFs )
: CActive( CActive::EPriorityStandard ), iFs( aFs )
    {

    }

// ---------------------------------------------------------------------------
// CMRFileManager::ConstructL
// ---------------------------------------------------------------------------
//
void CMRFileManager::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CMRFileManager::CopyFilesL
// ---------------------------------------------------------------------------
//
void CMRFileManager::CopyFilesL( MDesC16Array& aFilenameArray )
    {
    // Take copy of the original filename array
    TInt count( aFilenameArray.MdcaCount() );
    if( iFileArray )
        {
        delete iFileArray;
        iFileArray =  NULL;
        }
    iFileArray = new (ELeave) CDesCArrayFlat( count );
    for( TInt i = 0; i < count; ++i )
        {
        iFileArray->AppendL( aFilenameArray.MdcaPoint( i ) );
        }

    if( iOutputFileArray )
        {
        delete iOutputFileArray;
        iOutputFileArray = NULL;
        }
    iOutputFileArray = new (ELeave) CDesCArrayFlat( count );
    if( iCurrentFilename )
        {
        delete iCurrentFilename;
        iCurrentFilename = NULL;
        }

    PrepareNextFileL();

    iState = ECopyFile;
    AsyncOp();
    }

// ---------------------------------------------------------------------------
// CMRFileManager::CopyFileL
// ---------------------------------------------------------------------------
//
void CMRFileManager::CopyFileL(
        const TDesC& aSourceFilename,
        const TDesC& aDestFilename )
    {
    Cancel();

    iReadStream.Close();
    iWriteStream.Close();
    iReadStream.Open( iFs, aSourceFilename, EFileShareReadersOnly );
    iWriteStream.Open( iFs, aDestFilename, EFileWrite|EFileShareAny );

    if ( iObserver )
        {
        iObserver->NotifyStartL( aSourceFilename );
        }

    iState = ECopyFile;

    AsyncOp();
    }

// ---------------------------------------------------------------------------
// CMRFileManager::CopyFileL
// ---------------------------------------------------------------------------
//
void CMRFileManager::CopyFileL( RFile& aSource, RFile& aDest )
    {
    Cancel();

    iReadStream.Close();
    iWriteStream.Close();
    iReadStream.Attach( aSource );
    iWriteStream.Attach( aDest );

    if ( iObserver )
        {
        TFileName name;

        if ( KErrNone == aSource.Name( name ) )
            {
            iObserver->NotifyStartL( name );
            }
        }

    iState = ECopyFile;
    AsyncOp();
    }

// ---------------------------------------------------------------------------
// CMRFileManager::CancelOp
// ---------------------------------------------------------------------------
//
void CMRFileManager::CancelOp()
    {
    iState = ECancelled;
    }

// ---------------------------------------------------------------------------
// CMRFileManager::PrepareNextFileL
// ---------------------------------------------------------------------------
//
void CMRFileManager::PrepareNextFileL()
    {
    // Take first item as current
    if( iCurrentFilename )
        {
        delete iCurrentFilename;
        iCurrentFilename = NULL;
        }
    iCurrentFilename = (*iFileArray)[ 0 ].AllocL();


    TParse parser;
    parser.Set( *iCurrentFilename , NULL, NULL );
    TFileName attachmentName( parser.NameAndExt() );

    User::LeaveIfError( ESMRHelper::CreateAndAppendPrivateDirToFileName( attachmentName ) );

    iReadStream.Close();
    iWriteStream.Close();
    User::LeaveIfError( iWriteStream.Replace( iFs, attachmentName, EFileWrite|EFileShareAny ) );
    iReadStream.Open( iFs, *iCurrentFilename, EFileRead );
    iOutputFileArray->AppendL( attachmentName );

    delete iTempFilename;
    iTempFilename = NULL;
    iTempFilename = attachmentName.AllocL();

    if( iObserver )
        {
        iObserver->NotifyStartL( *iCurrentFilename );
        }
    }

// ---------------------------------------------------------------------------
// CMRFileManager::SetObserver
// ---------------------------------------------------------------------------
//
void CMRFileManager::SetObserver( MMRFileManObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// ---------------------------------------------------------------------------
// CMRFileManager::RunL
// ---------------------------------------------------------------------------
//
void CMRFileManager::RunL()
    {
    if( iStatus.Int() != KErrNone )
        {
        iReadStream.Close();
        iWriteStream.Close();

        if( iObserver )
            {
            iObserver->NotifyError( iStatus.Int() );
            }
        return;
        }
    switch( iState )
        {
        case ECopyFile:
            {
            CopyFileL();

            if( iObserver )
                {
                iObserver->NotifyProgress( KWriteBlockSize );
                }
            AsyncOp();
            break;
            }
        case ECancelled:
            {
            if( iObserver )
                {
                iReadStream.Close();
                iWriteStream.Close();
                // Delete last because it was cancelled
                if ( iOutputFileArray )
                    {
                    iOutputFileArray->Delete( iOutputFileArray->Count() - 1 );
                    }
                iObserver->NotifyEnd();
                iState = EDone;
                }
            break;
            }
        case EDone:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRFileManager::RunError
// ---------------------------------------------------------------------------
//
TInt CMRFileManager::RunError( TInt aError )
    {
    if ( aError == KErrEof ) // File has been copied fully
        {
        aError = KErrNone;
        if( iFileArray && iFileArray->Count() > 1 )
            {
            iFileArray->Delete( 0 );

            // Start new file copy op
            TRAP( aError,
                    {
                    PrepareNextFileL();
                    AsyncOp();
                    } )
            }
        else
            {
            if( iObserver )
                {
                iReadStream.Close();
                iWriteStream.Close();

                iObserver->NotifyEnd();
                iState = EDone;
                }
            }
        }

    if ( aError )
        {
        iReadStream.Close();
        iWriteStream.Close();

        if( iObserver )
            {
            iObserver->NotifyError( aError );
            iState = EDone;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMRFileManager::DoCancel
// ---------------------------------------------------------------------------
//
void CMRFileManager::DoCancel()
    {
    iReadStream.Close();
    iWriteStream.Close();
    delete iFileArray;
    iFileArray = NULL;
    delete iOutputFileArray;
    iOutputFileArray = NULL;
    iFileIndex = KErrNotFound;
    delete iCurrentFilename;
    iCurrentFilename = NULL;
    delete iTempFilename;
    iTempFilename = NULL;
    }

// ---------------------------------------------------------------------------
// CMRFileManager::AsyncOp
// ---------------------------------------------------------------------------
//
void CMRFileManager::AsyncOp()
    {
    SetActive();
    TRequestStatus *stat = &iStatus;
    User::RequestComplete( stat, KErrNone );
    }

// ---------------------------------------------------------------------------
// CMRFileManager::CopyFileL
// ---------------------------------------------------------------------------
//
void CMRFileManager::CopyFileL()
    {
    iWriteStream.WriteL( iReadStream, KWriteBlockSize );
    }

// ---------------------------------------------------------------------------
// CMRFileManager::CopiedFiles
// ---------------------------------------------------------------------------
//
MDesC16Array& CMRFileManager::CopiedFiles() const
    {
    return *iOutputFileArray;
    }
// End of file

