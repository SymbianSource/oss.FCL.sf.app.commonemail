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
* Description: 3rd party test plugin email data
*
*/

#include <memaildata.h>

#include "emailtrace.h"
#include "emaildataimpl.h"
#include "mailboxdataimpl.h"

// ---------------------------------------------------------
// ---------------------------------------------------------
CEmailDataImpl* CEmailDataImpl::NewL()
    {
    FUNC_LOG;
    CEmailDataImpl* self = new(ELeave) CEmailDataImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CEmailDataImpl::~CEmailDataImpl()
    {
    for ( TInt i = 0; i < iMailboxes.Count(); i++ )
        {
        delete static_cast<CMailboxDataImpl*>(iMailboxes[i]);
        iMailboxes.Remove(i);
        }
    iMailboxes.ResetAndDestroy();
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CEmailDataImpl::CEmailDataImpl()
    {
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
void CEmailDataImpl::ConstructL()
    {
    CMailboxDataImpl* mailbox = CMailboxDataImpl::NewL();
    iMailboxes.AppendL( mailbox );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
RPointerArray<EmailInterface::MMailboxData>& CEmailDataImpl::MailboxesL()
    {
    FUNC_LOG;
    return iMailboxes;
    }
