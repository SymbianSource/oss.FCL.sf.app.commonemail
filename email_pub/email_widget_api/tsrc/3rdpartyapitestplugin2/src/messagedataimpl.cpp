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
* Description:  3rd party api testplugin message data
*
*/

#include <memaildata.h>
#include <memailmessagedata.h>

#include "messagedataimpl.h"

// Hardcoded default values
_LIT( KDefaultSender, "sender@domain.com" );
_LIT( KDefaultSubject, "Test subject" );

// ---------------------------------------------------------
// ---------------------------------------------------------
CMessageDataImpl* CMessageDataImpl::NewL()
    {
    CMessageDataImpl* self = new(ELeave) CMessageDataImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CMessageDataImpl::~CMessageDataImpl()
    {
    delete iSender;
    delete iSubject;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CMessageDataImpl::CMessageDataImpl() : iSender( NULL ), iSubject( NULL ),
    iUnread( EFalse ), iPriority( EmailInterface::ENormal ), iAttachments( EFalse ), iCalendarMsg( EFalse ),
    iReplied( EFalse ), iForwarded( EFalse )
    {
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
void CMessageDataImpl::ConstructL()
    {
    iTimeStamp.HomeTime();
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
const TDesC& CMessageDataImpl::Sender() const
    {
    return KDefaultSender;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
const TDesC& CMessageDataImpl::Subject() const
    {
    return KDefaultSubject;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TTime CMessageDataImpl::TimeStamp() const
    {
    return iTimeStamp;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TBool CMessageDataImpl::Unread() const
    {
    return iUnread;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
EmailInterface::TEmailPriority CMessageDataImpl::Priority() const
    {
    return iPriority;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TBool CMessageDataImpl::Attachments() const
    {
    return iAttachments;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TBool CMessageDataImpl::CalendarMsg() const
    {
    return iCalendarMsg;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TBool CMessageDataImpl::Replied() const
    {
    return iReplied;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TBool CMessageDataImpl::Forwarded() const
    {
    return iForwarded;
    }
