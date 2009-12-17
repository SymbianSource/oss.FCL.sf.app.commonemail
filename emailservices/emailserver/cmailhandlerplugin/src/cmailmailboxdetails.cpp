/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CMailMailboxDetails.
*
*/


#include "emailtrace.h"
#include "cmailmessagedetails.h"
#include "cmailmailboxdetails.h"

// ---------------------------------------------------------
// CMailMailboxDetails::NewL
// ---------------------------------------------------------
//
CMailMailboxDetails* CMailMailboxDetails::NewL(
    const TFSMailMsgId aMailboxId,
    const TDesC& aMailboxName )
    {
    FUNC_LOG;
    CMailMailboxDetails* self = new(ELeave) CMailMailboxDetails( aMailboxId );
    CleanupStack::PushL( self );
    self->ConstructL( aMailboxName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMailMailboxDetails::CMailboxDetails
// ---------------------------------------------------------
//
CMailMailboxDetails::CMailMailboxDetails( const TFSMailMsgId aMailboxId ) :
    iMailboxId( aMailboxId )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CMailMailboxDetails::~CMailMailboxDetails
// ---------------------------------------------------------
//
CMailMailboxDetails::~CMailMailboxDetails()
    {
    FUNC_LOG;
    delete iMailboxName;
    delete iWidgetInstance;    
    iMessageDetailsArray.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CMailMailboxDetails::SetWidgetInstance
// ---------------------------------------------------------
//
void CMailMailboxDetails::SetWidgetInstance( const TDesC& aWidgetInstance )
    {
    FUNC_LOG;
    iWidgetInstance = aWidgetInstance.Alloc();
    }

// ---------------------------------------------------------
// CMailMailboxDetails::ConstructL
// ---------------------------------------------------------
//
void CMailMailboxDetails::ConstructL( const TDesC& aMailboxName )
    {
    FUNC_LOG;
    iMailboxName = aMailboxName.Alloc();
    }

