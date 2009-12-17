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
* Description: This file implements class CEmailInterfaceFactoryImpl.
*
*/

#include "emailinterfacefactoryimpl.h"
#include "emailcontent.h"
#include "CFSMailClient.h"
#include "emailclientapiimpldefs.h"
#include "emailclientapiimpl.h"
#include "emailaddress.h"
#include "emailmessagesearch.h"

_LIT( KEmailImplPanic, "Email client API" );

// ---------------------------------------------------------------------------
// Email client API panic wrapper
// ---------------------------------------------------------------------------
//
void Panic( TEmailImplPanic aPanic )
    {
    User::Panic( KEmailImplPanic(), aPanic );
    }
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::NewL
// ---------------------------------------------------------------------------
//
CEmailInterfaceFactoryImpl* CEmailInterfaceFactoryImpl::NewL()
    {
    CEmailInterfaceFactoryImpl* self = new (ELeave) CEmailInterfaceFactoryImpl();
    return self;
    }
    
// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::~CEmailInterfaceFactoryImpl
// ---------------------------------------------------------------------------
//
CEmailInterfaceFactoryImpl::~CEmailInterfaceFactoryImpl()
    {
    }

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::CEmailInterfaceFactoryImpl
// ---------------------------------------------------------------------------
//
CEmailInterfaceFactoryImpl::CEmailInterfaceFactoryImpl() : 
    CEmailInterfaceFactory()
    {
    }
    
// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::InterfaceL
// ---------------------------------------------------------------------------
//
MEmailInterface* CEmailInterfaceFactoryImpl::InterfaceL( const TInt aInterfaceId )
    {
    MEmailInterface* interface = NULL;
    switch ( aInterfaceId )
        {
        case KEmailClientApiInterface:
            interface = CEmailClientApi::NewL();
            break;
        case KEmailIFUidTextContent:
            //interface = CEmailTextContent::NewL();
            break;
        case KEmailIFUidAddress:
            interface = CEmailAddress::NewL( MEmailAddress::EUndefined, EClientOwns );
        default:
            break;
        }
    if ( !interface )
        {
        User::Leave( KErrNotSupported );
        }
    return interface;
    }

// End of file.
