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
* Description:  3rd party api testplugin app launcher data
*
*/

#include "applaunchdataimpl.h"

// ---------------------------------------------------------
// ---------------------------------------------------------
CAppLaunchDataImpl* CAppLaunchDataImpl::NewL( TUid aAppUid, TUid aViewUid )
    {
    CAppLaunchDataImpl* self = new(ELeave) CAppLaunchDataImpl( aAppUid, aViewUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CAppLaunchDataImpl::~CAppLaunchDataImpl()
    {
    delete iCustomMessage;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CAppLaunchDataImpl::CAppLaunchDataImpl( TUid aAppUid, TUid aViewUid ) : 
    iAppUid( aAppUid ), iViewUid( aViewUid ), iCustMsgUid( TUid() ), iCustomMessage( NULL )
    {
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
void CAppLaunchDataImpl::ConstructL()
    {
    iCustomMessage = HBufC8::NewL(1);
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TUid CAppLaunchDataImpl::ApplicationUid() const
    {
    return iAppUid;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TUid CAppLaunchDataImpl::ViewId() const
    {
    return iViewUid;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TUid CAppLaunchDataImpl::CustomMessageId() const
    {
    return iCustMsgUid;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
const TDesC8& CAppLaunchDataImpl::CustomMessage() const
    {
    return *iCustomMessage;
    }
