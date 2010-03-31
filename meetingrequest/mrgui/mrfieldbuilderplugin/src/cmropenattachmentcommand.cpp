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
 * Description:  MR attahcment field implementation
 *
 */
#include "cmropenattachmentcommand.h"

#include <calentry.h>
#include <calattachment.h>
#include <f32file.h>
#include <esmrgui.rsg>
#include <npdapi.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <documenthandler.h>

// DEBUG
#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

// Notepad data type
_LIT8( KNotePadTextDataType, "text/plain" );

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMROpenAttachmentCommand::CMROpenAttachmentCommand
// ---------------------------------------------------------------------------
//
CMROpenAttachmentCommand::CMROpenAttachmentCommand(
        CDocumentHandler& aDocHandler )
    : iDocHandler( aDocHandler )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CMROpenAttachmentCommand::CMROpenAttachmentCommand
// ---------------------------------------------------------------------------
//
CMROpenAttachmentCommand::~CMROpenAttachmentCommand()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMROpenAttachmentCommand::NewL
// ---------------------------------------------------------------------------
//
CMROpenAttachmentCommand* CMROpenAttachmentCommand::NewL(
        CDocumentHandler& aDocHandler )
    {
    FUNC_LOG;
    
    CMROpenAttachmentCommand* self =
            new (ELeave) CMROpenAttachmentCommand( aDocHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMROpenAttachmentCommand::ConstructL
// ---------------------------------------------------------------------------
//
void CMROpenAttachmentCommand::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMROpenAttachmentCommand::ExecuteAttachmentCommandL
// ---------------------------------------------------------------------------
//
void CMROpenAttachmentCommand::ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex )
    {
    FUNC_LOG;
    
    // Ownership not gained
    CCalAttachmentFile* attachmentFile =
            aEntry.AttachmentL( aAttachmentIndex )->FileAttachment(); 

    RFile file;
    attachmentFile->FetchFileHandleL( file );
    CleanupClosePushL( file );
    
    TDataType datatype( 
            aEntry.AttachmentL( aAttachmentIndex )->MimeType() );
    
    TInt err( KErrNone );
    if( datatype == KNotePadTextDataType() )
        {
        // Notepad will try to open text/plain type data
        err = CNotepadApi::ExecFileViewerL( 
                file,
                NULL,
               ETrue,
               EFalse,
               KCharacterSetIdentifierIso88591 );
        }
    else
        {
        // Doc handler will try to open other than text files
        TRAP( err, iDocHandler.OpenFileEmbeddedL( file, datatype ) );
        }
    
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
