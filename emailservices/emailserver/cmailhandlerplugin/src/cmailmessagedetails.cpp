/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  helper classes (data containers)
*
*/


//<cmail>
#include "emailtrace.h"
#include "CFSMailBox.h"
#include "CFSMailFolder.h"
//</cmail>

#include "cmailmessagedetails.h"
#include "cmailmailboxdetails.h"

// ---------------------------------------------------------
// CMailMessageDetails::NewL
// ---------------------------------------------------------
//
CMailMessageDetails* CMailMessageDetails::NewL( 
    TFSMailMsgId aMsgId, 
    CMailMailboxDetails* aMailboxDetails, 
    const TDesC& aSender, 
    const TDesC& aSubject, 
    const TTime aTimeStamp )
    {
    FUNC_LOG;
    CMailMessageDetails* self = new(ELeave) CMailMessageDetails(
        aMsgId, aMailboxDetails, aTimeStamp );
    CleanupStack::PushL( self );
    self->ConstructL( aSender, aSubject );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMailMessageDetails::CMailMessageDetails
// ---------------------------------------------------------
//
CMailMessageDetails::CMailMessageDetails( 
    TFSMailMsgId aMsgId, CMailMailboxDetails* aMailboxDetails, const TTime aTimeStamp ) :
    iMsgId( aMsgId ),
    iMailboxDetails( aMailboxDetails ),
    iTimeStamp( aTimeStamp )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------
// CMailMessageDetails::~CMailMessageDetails
// ---------------------------------------------------------
//
CMailMessageDetails::~CMailMessageDetails()
    {
    FUNC_LOG;
    // iMailboxDetails not owned
    delete iSender;
    delete iSubject;
    }

// ---------------------------------------------------------
// CMailMessageDetails::ConstructL
// ---------------------------------------------------------
//
void CMailMessageDetails::ConstructL( const TDesC& aSender, const TDesC& aSubject )
    {
    FUNC_LOG;
    iSender = aSender.AllocL();
    iSubject = aSubject.AllocL();
    }

