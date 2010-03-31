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
 * Description:  MR save and open attachment command implementation
 *
 */
#include "cmrsaveandopenattachmentcommand.h"

#include <calentry.h>
#include <calattachment.h>
#include <f32file.h>
#include <documenthandler.h>
#include <coemain.h>
#include <esmrgui.rsg>
#include <npdapi.h>
#include <aknnotewrappers.h>
#include <stringloader.h>

// DEBUG
#include "emailtrace.h"


namespace { // codescanner::namespace

// Notepad data type
_LIT8( KNotePadTextDataType, "text/plain" );

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRSaveAndOpenAttachmentCommand::CMRSaveAndOpenAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRSaveAndOpenAttachmentCommand::CMRSaveAndOpenAttachmentCommand(
        CDocumentHandler& aDocHandler )
    : CMRAttachmentCommand(),
      iDocHandler( aDocHandler )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSaveAndOpenAttachmentCommand::CMRSaveAndOpenAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRSaveAndOpenAttachmentCommand::~CMRSaveAndOpenAttachmentCommand()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSaveAndOpenAttachmentCommand::NewL
// ---------------------------------------------------------------------------
//
CMRSaveAndOpenAttachmentCommand* CMRSaveAndOpenAttachmentCommand::NewL(
        CDocumentHandler& aDocHandler )
    {
    FUNC_LOG;

    CMRSaveAndOpenAttachmentCommand* self =
            new( ELeave )CMRSaveAndOpenAttachmentCommand( aDocHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRSaveAndOpenAttachmentCommand::ConstructL
// ---------------------------------------------------------------------------
//
void CMRSaveAndOpenAttachmentCommand::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSaveAndOpenAttachmentCommand::ExecuteAttachmentCommandL
// ---------------------------------------------------------------------------
//
void CMRSaveAndOpenAttachmentCommand::ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex )
    {
    FUNC_LOG;

    CCoeEnv* coeEnv = CCoeEnv::Static();
    ASSERT( coeEnv );

    TDataType type;

    // Ownership not gained
    CCalAttachmentFile* attachmentFile =
       aEntry.AttachmentL( aAttachmentIndex )->FileAttachment();

    RFile file;
    attachmentFile->FetchFileHandleL( file );
    CleanupClosePushL( file );

    TFileName fileName;
    file.FullName( fileName );

    // Document handler takes care of the saving process. Return value does
    // not require any action
    iDocHandler.CopyL( file, fileName, type, NULL );

    TFileName copiedFileName;
    iDocHandler.GetPath(copiedFileName);

    RFs& fsSession = coeEnv->FsSession();
    User::LeaveIfError( fsSession.ShareProtected() );

    RFile copiedFile;
    TInt err = copiedFile.Open(
                    fsSession,
                    copiedFileName,
                    EFileRead | EFileShareReadersOnly);
    User::LeaveIfError( err );

    CleanupClosePushL( copiedFile );

    TDataType datatype(
            aEntry.AttachmentL( aAttachmentIndex )->MimeType() );

    if( datatype == KNotePadTextDataType() )
        {
        // Notepad will try to open text/plain type data
        err = CNotepadApi::ExecFileViewerL(
                copiedFile,
                NULL,
               ETrue,
               EFalse,
               KCharacterSetIdentifierIso88591 );
        }
    else
        {
        // Doc handler will try to open other than text files
        TRAP( err, iDocHandler.OpenFileEmbeddedL( copiedFile, datatype ) );
        }

    CleanupStack::PopAndDestroy( &copiedFile );
    CleanupStack::PopAndDestroy( &file );

    if( err != KErrNone )
        {
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
        CleanupStack::PushL( note );
        HBufC* buf = StringLoader::LoadLC(
                R_MEET_REQ_INFO_CANNOT_OPEN_ATTACHMENT );
        note->ExecuteLD( *buf );
        CleanupStack::PopAndDestroy( buf );
        CleanupStack::Pop( note );
        }
    }

// EOF
