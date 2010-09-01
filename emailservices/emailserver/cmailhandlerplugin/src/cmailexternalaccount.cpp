/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmailexternalaccount.h"

// ---------------------------------------------------------
// CMailExternalAccount::NewL
// ---------------------------------------------------------
//
CMailExternalAccount* CMailExternalAccount::NewL(
    const TInt aMailboxId,
    const TInt aPluginId,
    HBufC* aContentId )
    {
    FUNC_LOG;
    CMailExternalAccount* self = new(ELeave) CMailExternalAccount( aMailboxId, aPluginId, aContentId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMailExternalAccount::CMailExternalAccount
// ---------------------------------------------------------
//
CMailExternalAccount::CMailExternalAccount(
    const TInt aMailboxId,
    const TInt aPluginId,
    HBufC* aContentId ) :
    iMailboxId( aMailboxId ),
    iPluginId( aPluginId ),
    iContentId( aContentId )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CMailExternalAccount::~CMailExternalAccount
// ---------------------------------------------------------
//
CMailExternalAccount::~CMailExternalAccount()
    {
    FUNC_LOG;
    delete iContentId;
    }

// ---------------------------------------------------------
// CMailExternalAccount::ConstructL
// ---------------------------------------------------------
//
void CMailExternalAccount::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CMailExternalAccount::MailboxId
// ---------------------------------------------------------
//
TInt CMailExternalAccount::MailboxId()
    {
    return iMailboxId;
    }

// ---------------------------------------------------------
// CMailExternalAccount::PluginId
// ---------------------------------------------------------
//
TInt CMailExternalAccount::PluginId()
    {
    return iPluginId;
    }

// ---------------------------------------------------------
// CMailExternalAccount::ContentId
// ---------------------------------------------------------
//
TDesC& CMailExternalAccount::ContentId()
    {
    return *iContentId;
    }
