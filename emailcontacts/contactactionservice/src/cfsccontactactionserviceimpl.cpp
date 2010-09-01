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
* Description:  Implementation of the class CFscContactActionServiceImpl.
*
*/


// INCUDES
#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include "mfsccontactactionserviceobserver.h"

#include "cfsccontactactionserviceimpl.h"
#include "fsccontactactionserviceuids.hrh"
#include "cfsccontactactionpluginengine.h"
#include "cfsccontactactionplugin.h"
#include "cfsccontactset.h"
#include "cfscstorecontactset.h"
#include "cfsccontactlinkset.h"


const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KFscContactActionServiceImplImpUid, 
                                CFscContactActionServiceImpl::NewL )
    };
    
// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFscContactActionServiceImpl* CFscContactActionServiceImpl::NewL(
    TAny* aParams )
    {
    FUNC_LOG;

    TFscContactActionServiceConstructParameters* params = reinterpret_cast< 
            TFscContactActionServiceConstructParameters* >( aParams );
    
    CFscContactActionServiceImpl* self = 
        new ( ELeave ) CFscContactActionServiceImpl( *params );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFscContactActionServiceImpl::~CFscContactActionServiceImpl()
    {
    FUNC_LOG;
    delete iPluginEngine;
    delete iActionList;
    ClearContactSet();
    }

// ---------------------------------------------------------------------------
// Method for setting current contact set to the service.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::SetContactSetL( 
    MFscContactLinkIterator* aIterator )
    {
    FUNC_LOG;
    
    ClearContactSet();
    
    iContactSet = CFscContactLinkSet::NewL( iVPbkContactManager, aIterator );
    
    }

// ---------------------------------------------------------------------------
// Method for setting current contact set to the service.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::SetContactSetL( 
    const RFscStoreContactList& aStoreContactList )
    {
    FUNC_LOG;
    
    ClearContactSet();
    
    iContactSet = CFscStoreContactSet::NewL( iVPbkContactManager, 
            aStoreContactList );
    
    }

// ---------------------------------------------------------------------------
// Method for quering available actions.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::QueryActionsL(
    MFscContactActionServiceObserver* aObserver,
    TBool aStopWhenOneActionFound, TUint64 aActionTypeFlags, 
    TInt aMinPriority )
    {
    FUNC_LOG;
    iObserver = aObserver;

    
    if ( !iIsCasUsed )
        {
        iIsCasUsed = ETrue;
        
        if ( !iPluginEngine->PluginsLoaded() )
            {
            iPluginEngine->LoadPluginsL();
            }
        iActionList->Reset();
    
        iPluginEngine->QueryActionsL( *iActionList, *iContactSet, 
            aStopWhenOneActionFound, aActionTypeFlags, aMinPriority, this );
        }
    else
        {
        aObserver->QueryActionsFailed( KErrInUse );
        }
    
    }

// ---------------------------------------------------------------------------
// Cancels async method QueryActionsL.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::CancelQueryActions()
    {
    FUNC_LOG;
    if ( iIsCasUsed )
        {
        iPluginEngine->CancelQueryActions();
        iIsCasUsed = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Called when QueryActionsL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::QueryActionsComplete()
    {
    FUNC_LOG;
    iIsCasUsed = EFalse;
    iObserver->QueryActionsComplete();
    }
 
// ---------------------------------------------------------------------------
// Called when QueryActionsL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::QueryActionsFailed( TInt aError )
    {
    FUNC_LOG;
    iIsCasUsed = EFalse;
    iObserver->QueryActionsFailed( aError );
    }

// ---------------------------------------------------------------------------
// Return action query results.
// ---------------------------------------------------------------------------
//
const CFscContactActionList& 
    CFscContactActionServiceImpl::QueryResults() const
    {
    return *iActionList;
    }

// ---------------------------------------------------------------------------
// Execute action.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::ExecuteL( TUid aActionUid, 
    MFscContactActionServiceObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    
    if ( !iIsCasUsed )
        {
        iIsCasUsed = ETrue;
    
        if ( !iPluginEngine->PluginsLoaded() )
            {
            iPluginEngine->LoadPluginsL();
            }
    
        iPluginEngine->ExecuteL( aActionUid, *iContactSet, this );
        }
    else
        {
        aObserver->ExecuteFailed( KErrInUse );
        }
    
    }

// ---------------------------------------------------------------------------
// Cancels async method ExecuteL.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::CancelExecute()
    {
    FUNC_LOG;
    if ( iIsCasUsed )
        {
        iPluginEngine->CancelExecute();
        iIsCasUsed = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Called when ExecuteL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::ExecuteComplete()
    {
    FUNC_LOG;
    iIsCasUsed = EFalse;
    iObserver->ExecuteComplete();
    }
   
// ---------------------------------------------------------------------------
// Called when ExecuteL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::ExecuteFailed( TInt aError )
    {
    FUNC_LOG;
    iIsCasUsed = EFalse;
    iObserver->ExecuteFailed( aError );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFscContactActionServiceImpl::CFscContactActionServiceImpl( 
    const TFscContactActionServiceConstructParameters& aParams )
    : CFscContactActionService(), 
      iVPbkContactManager( aParams.iVPbkContactManager ),
      iIsCasUsed( EFalse )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::ConstructL()
    {
    FUNC_LOG;
    
    iPluginEngine = 
        CFscContactActionPluginEngine::NewL( iVPbkContactManager );
    
    iActionList = new ( ELeave ) CFscContactActionList( 5 );
    } 
   
// ---------------------------------------------------------------------------
// Clear contacts and groups
// ---------------------------------------------------------------------------
//
void CFscContactActionServiceImpl::ClearContactSet()
    {
    FUNC_LOG;
    delete iContactSet;
    iContactSet = NULL;
    }      

    
// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
        sizeof( TImplementationProxy );
    return ImplementationTable;
    }

