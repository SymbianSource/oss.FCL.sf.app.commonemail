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
 * Description:  MR remove attachment command implementation
 *
 */
#include "cmrremoveattachmentcommand.h"

#include <calentry.h>
#include <calattachment.h>

// DEBUG
#include "emailtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRRemoveAttachmentCommand::CMRRemoveAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRRemoveAttachmentCommand::CMRRemoveAttachmentCommand() :
    CMRAttachmentCommand()
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CMRRemoveAttachmentCommand::CMRRemoveAttachmentCommand
// ---------------------------------------------------------------------------
//
CMRRemoveAttachmentCommand::~CMRRemoveAttachmentCommand()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRRemoveAttachmentCommand::NewL
// ---------------------------------------------------------------------------
//
CMRRemoveAttachmentCommand* CMRRemoveAttachmentCommand::NewL()
    {
    FUNC_LOG;
    
    CMRRemoveAttachmentCommand* self = new( ELeave )CMRRemoveAttachmentCommand;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRRemoveAttachmentCommand::ConstructL
// ---------------------------------------------------------------------------
//
void CMRRemoveAttachmentCommand::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRRemoveAttachmentCommand::ExecuteAttachmentCommandL
// ---------------------------------------------------------------------------
//
void CMRRemoveAttachmentCommand::ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex )
    {
    FUNC_LOG;
    aEntry.DeleteAttachmentL( *( aEntry.AttachmentL( aAttachmentIndex ) ) );
    }

// EOF
