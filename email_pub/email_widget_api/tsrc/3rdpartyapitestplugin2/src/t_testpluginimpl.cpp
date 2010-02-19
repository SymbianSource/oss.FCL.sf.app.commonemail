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
* Description:  3rd party api testplugin
*
*/

#include <memailobserverlistener.h>
#include <memaildata.h>

#include "emailtrace.h"
#include "t_testpluginimpl.h"
#include "emaildataimpl.h"
#include "mailboxdataimpl.h"
#include "messagedataimpl.h"

// ---------------------------------------------------------
// ---------------------------------------------------------
EmailInterface::CEmailObserverPlugin* CEmailObserverPluginImpl::NewL(
    EmailInterface::MEmailObserverListener* aListener )
    {
    FUNC_LOG;
    CEmailObserverPluginImpl* self = new(ELeave) CEmailObserverPluginImpl( aListener );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CEmailObserverPluginImpl::~CEmailObserverPluginImpl()
    {
    delete iData;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CEmailObserverPluginImpl::CEmailObserverPluginImpl( EmailInterface::MEmailObserverListener* aListener ) :
    iListener( aListener ), iData( NULL )
    {
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
void CEmailObserverPluginImpl::ConstructL()
    {
    iData = CEmailDataImpl::NewL();
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
EmailInterface::MEmailData& CEmailObserverPluginImpl::EmailDataL()
    {
    FUNC_LOG;
    return *( static_cast<EmailInterface::MEmailData*>( iData ) );
    }
