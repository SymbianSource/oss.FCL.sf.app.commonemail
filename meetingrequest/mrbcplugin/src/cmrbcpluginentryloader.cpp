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
* Description:  MBUtils ECOM implementation
*
*/

#include "cmrbcpluginentryloader.h"

#include <calenservices.h>
#include <calencontext.h>
#include <calentryview.h>
#include <caleninstanceid.h>

#include "emailtrace.h"

namespace { //codescanner::namespace

#ifdef _DEBUG

_LIT( KMRBCPluginEntryLoader, "MRBCPluginEntryLoader" );

enum TMRBCPluginEntryLoaderPanic
    {
    EEntryNotFetched
    };

void Panic( TMRBCPluginEntryLoaderPanic aPanic )
    {
    User::Panic( KMRBCPluginEntryLoader, aPanic );
    }

#endif // _DEBUG

} // namespace

// -----------------------------------------------------------------------------
// CMRBCPluginEntryLoader::CMRBCPluginEntryLoader
// Static constructor for this class
// -----------------------------------------------------------------------------
//
CMRBCPluginEntryLoader::CMRBCPluginEntryLoader( MCalenServices& aServices )
:   iServices( aServices )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginEntryLoader::~CMRBCPluginEntryLoader
// Static constructor for this class
// -----------------------------------------------------------------------------
//
CMRBCPluginEntryLoader::~CMRBCPluginEntryLoader()
    {
    FUNC_LOG;
    delete iEntry;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginEntryLoader::ConstructL
// Static constructor for this class
// -----------------------------------------------------------------------------
//
void CMRBCPluginEntryLoader::ConstructL()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginEntryLoader::NewL
// Static constructor for this class
// -----------------------------------------------------------------------------
//
CMRBCPluginEntryLoader* CMRBCPluginEntryLoader::NewL( 
        MCalenServices& aServices )
    {
    FUNC_LOG;
    
    CMRBCPluginEntryLoader* self = new (ELeave) CMRBCPluginEntryLoader( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMRBCPluginEntryLoader::UpdateEntryFromDatabaseL
// Static constructor for this class
// -----------------------------------------------------------------------------
//
void CMRBCPluginEntryLoader::UpdateEntryFromDatabaseL()
    {
    FUNC_LOG;
    
    if ( iEntry )
        {        
        delete iEntry;
        iEntry = NULL;
        }
    
    MCalenContext& context = iServices.Context();
    TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
       
    iEntry = iServices.EntryViewL( 
            context.InstanceId().iColId )->FetchL( instanceId );   
    }
    
CCalEntry& CMRBCPluginEntryLoader::Entry()
    {
    FUNC_LOG;
    
    __ASSERT_DEBUG( iEntry, Panic(EEntryNotFetched) );
    
    return *iEntry;
    }
    
// EOF
