/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CPbkxRemoteContactLookupEnvImpl.
*
*/


#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include "cpbkxremotecontactlookupenvimpl.h"
#include "cpbkxrclsearchengine.h"
#include "cpbkxrclsettingsengine.h"

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY( 
        KPbkxRemoteContactLookupServiceImplImpUid, 
        CPbkxRemoteContactLookupEnvImpl::NewL )
	};

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupEnvImpl::CPbkxRemoteContactLookupEnvImpl()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPbkxRemoteContactLookupEnvImpl::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupEnvImpl* CPbkxRemoteContactLookupEnvImpl::NewL()
    {
    FUNC_LOG;
    CPbkxRemoteContactLookupEnvImpl* self = new (ELeave) CPbkxRemoteContactLookupEnvImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupEnvImpl::~CPbkxRemoteContactLookupEnvImpl()
    {
    FUNC_LOG;
    delete iServiceUi;
    delete iSettingsUi;
    }
    
// ---------------------------------------------------------------------------
// From class MPbkxRemoteContactLookupUi
// ?implementation_description
// ---------------------------------------------------------------------------
//
MPbkxRemoteContactLookupServiceUi* CPbkxRemoteContactLookupEnvImpl::ServiceUiL()
    {
    FUNC_LOG;
    
    if( iServiceUi == NULL )
        {
        iServiceUi = CPbkxRclSearchEngine::NewL();
        }
        
    return static_cast<MPbkxRemoteContactLookupServiceUi*>(iServiceUi);
    }

// ---------------------------------------------------------------------------
// From class MPbkxRemoteContactLookupUi
// ?implementation_description
// ---------------------------------------------------------------------------
//
MPbkxRemoteContactLookupSettingsUi* CPbkxRemoteContactLookupEnvImpl::SettingsUiL()
    {
    FUNC_LOG;
    
    if( iSettingsUi == NULL )
        {
        iSettingsUi = CPbkxRclSettingsEngine::NewL();
        }
    
    return static_cast<MPbkxRemoteContactLookupSettingsUi*>(iSettingsUi);
    }

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
    FUNC_LOG;
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

