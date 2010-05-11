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
 * Description:  MR save attachment command implementation
 *
 */
#include "cmrsaveattachmentcommand.h"

#include <calentry.h>
#include <calattachment.h>
#include <f32file.h>
#include <DocumentHandler.h>
#include <coemain.h>
#include <esmrgui.rsg>
#include <NpdApi.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>

// DEBUG
#include "emailtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRSaveAttachmentCommand::CMRSaveAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRSaveAttachmentCommand::CMRSaveAttachmentCommand(
        CDocumentHandler& aDocHandler ) :
    CMRAttachmentCommand(),
    iDocHandler( aDocHandler )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSaveAttachmentCommand::CMRSaveAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRSaveAttachmentCommand::~CMRSaveAttachmentCommand()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSaveAttachmentCommand::NewL
// ---------------------------------------------------------------------------
//
CMRSaveAttachmentCommand* CMRSaveAttachmentCommand::NewL(
        CDocumentHandler& aDocHandler )
    {
    FUNC_LOG;

    CMRSaveAttachmentCommand* self =
            new( ELeave )CMRSaveAttachmentCommand( aDocHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRSaveAttachmentCommand::ConstructL
// ---------------------------------------------------------------------------
//
void CMRSaveAttachmentCommand::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRSaveAttachmentCommand::ExecuteAttachmentCommandL
// ---------------------------------------------------------------------------
//
void CMRSaveAttachmentCommand::ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex )
    {
    FUNC_LOG;

    CCalAttachmentFile* attachmentFile =
       aEntry.AttachmentL( aAttachmentIndex )->FileAttachment(); // Ownership not gained

    if ( attachmentFile ) // Ignore URI attachment
        {
        RFile file;
        attachmentFile->FetchFileHandleL( file );
        CleanupClosePushL( file );

        TFileName fileName;
        file.FullName( fileName );

        TDataType type;

        // Document handler takes care of the saving process.
        TInt res = iDocHandler.CopyL( file, fileName, type, NULL );

        if ( KUserCancel == res )
            {
            User::Leave( KErrCancel );
            }

        CleanupStack::PopAndDestroy( &file );
        }
    }

// EOF
