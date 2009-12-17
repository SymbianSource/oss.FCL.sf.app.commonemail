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
* Description:  Implementation of the class CPbkxRclSettingsEngine.
*
*/


#include "emailtrace.h"
#include <centralrepository.h>
#include <ecom.h>
#include "cpbkxrclsettingsengine.h"
#include "pbkxrclsettings.hrh"
#include "cpbkxremotecontactlookupprotocoladapter.h"
#include "cpbkxrclsettingitem.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclSettingsEngine::CPbkxRclSettingsEngine
// ---------------------------------------------------------------------------
//
CPbkxRclSettingsEngine::CPbkxRclSettingsEngine()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingsEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclSettingsEngine::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingsEngine::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbkxRclSettingsEngine* CPbkxRclSettingsEngine::NewL()
    {
    FUNC_LOG;
    CPbkxRclSettingsEngine* self = CPbkxRclSettingsEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingsEngine::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CPbkxRclSettingsEngine* CPbkxRclSettingsEngine::NewLC()
    {
    FUNC_LOG;
    CPbkxRclSettingsEngine* self = new( ELeave ) CPbkxRclSettingsEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSettingsEngine::~CPbkxRclSettingsEngine
// ---------------------------------------------------------------------------
//
CPbkxRclSettingsEngine::~CPbkxRclSettingsEngine()
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CPbkxRclSettingsEngine::NewDefaultProtocolAccountSelectorSettingItemL
// ---------------------------------------------------------------------------
//
CAknSettingItem* CPbkxRclSettingsEngine::NewDefaultProtocolAccountSelectorSettingItemL() const
    {
    FUNC_LOG;
    return CPbkxRclSettingItem::NewL();
    }

    

