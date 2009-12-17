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
* Description:  Implementation of the class CFscContactActionPluginEngine.
*
*/


// INCLUDES
#include "emailtrace.h"
#include <e32std.h>

#include "fsccontactactionserviceuids.hrh"
#include "cfsccontactactionpluginengine.h"
#include "cfscactionutils.h"
#include "mfsccontactaction.h"
#include "cfsccontactactionplugin.h"
#include "tfsccontactactionqueryresult.h"
#include "mfsccontactactionpluginengineobserver.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFscContactActionPluginEngine* CFscContactActionPluginEngine::NewL(
    CVPbkContactManager& aContactManager )
    {
    FUNC_LOG;

    CFscContactActionPluginEngine* self = 
        new ( ELeave ) CFscContactActionPluginEngine( aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFscContactActionPluginEngine::~CFscContactActionPluginEngine()
    {
    FUNC_LOG;
    ReleasePlugins();
    iActionPlugins.Close();
    delete iActionUtils;
    }

// ---------------------------------------------------------------------------
// Detect and load plugins.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::LoadPluginsL()
    {
    FUNC_LOG;
    
    if ( iPluginsLoaded )
        {
        // Realease plugins before reloading
        ReleasePlugins();
        }
        
    RImplInfoPtrArray implArray;
    TUid pluginIfUid = { KFscContactActionPluginIfUid };
    REComSession::ListImplementationsL( pluginIfUid, implArray );
        
    //Create implementations
    CFscContactActionPlugin* plugin = NULL;
    TInt implArrayCount = implArray.Count();
    for ( TInt i = 0; i < implArrayCount; i++ )
        {
        TRAPD( error,
            {
            plugin = CFscContactActionPlugin::NewL( 
                implArray[i]->ImplementationUid(), iPluginParams );
            CleanupStack::PushL( plugin );
            iActionPlugins.AppendL( plugin );    
            CleanupStack::Pop( plugin );
            });
        if ( error )
            {
                     
            // Leave only if no memory. This way unstable plugins can not make 
            // service totaly unusable
            if ( error == KErrNoMemory )
                {
                User::Leave( error );
                }
            }
        plugin = NULL;
        }
        
    implArray.ResetAndDestroy();
    iPluginsLoaded = ETrue;
    
    
    }

// ---------------------------------------------------------------------------
// Release loaded plugins.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::ReleasePlugins()
    {
	FUNC_LOG;
    iActionPlugins.ResetAndDestroy();
    iPluginsLoaded = EFalse;
    REComSession::FinalClose();
    }
    
// ---------------------------------------------------------------------------
// Query state of plugins.
// ---------------------------------------------------------------------------
//
TBool CFscContactActionPluginEngine::PluginsLoaded()
    {
	FUNC_LOG;
    return iPluginsLoaded;
    }    

// ---------------------------------------------------------------------------
// Method for quering actions for contacts and groups.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::QueryActionsL( 
    CFscContactActionList& aActionList, MFscContactSet& aContactSet,
    TBool aStopWhenOneActionFound, TUint64 aActionTypeFlags, 
    TInt aMinPriority, MFscContactActionPluginEngineObserver* aObserver )
    {
	FUNC_LOG;
    switch ( iLastEvent )
        {
        case ECasEventIdle:
            {
            iActionList = &aActionList;
            iContactSet = &aContactSet;
            iStopWhenOneActionFound = aStopWhenOneActionFound;
            iActionTypeFlags = aActionTypeFlags; 
            iMinPriority = aMinPriority;
            iObserver = aObserver;
            iCurrentActionPlugin = 0;
            iCurrentAction = 0;
            //iLastEvent = ECasEventBeforePriorityForContactSet;
            // break; - no break here so we can go further when method 
            // called for 1st time.
            }
        
        case ECasEventBeforePriorityForContactSet:
            {
            TBool asyncStarted = EFalse;
            // Loop plugins
            while ( iCurrentActionPlugin < iActionPlugins.Count() 
                    && !asyncStarted )
                {
                //Loop actions of single plugin
                CFscContactActionPlugin* plugin = 
                    iActionPlugins[ iCurrentActionPlugin ];
                
                const CArrayFix<TUid>* actionList = plugin->ActionList();
                TInt actionCount = 
                    ( actionList != NULL ) ? actionList->Count() : 0;
                
                while ( iCurrentAction < actionCount && !asyncStarted )
                    {
                    TUid uid = ( *actionList )[ iCurrentAction ];
                    TUint64 actionType = plugin->GetActionL( uid ).Type();
                    if ( aActionTypeFlags & actionType )
                        { 
                        if( actionType == KFscAtComSendCalReq )
                            {
                            // skip adding "Send metting request" option
                            ++iCurrentAction;
                            continue;
                            }
                        asyncStarted = ETrue;
                        //Check action's priority for given contact set
                        plugin->PriorityForContactSetL( uid, aContactSet,
                            iContactActionQueryResult.iActionMenuVisibility,
                            iContactActionQueryResult.iOptionsMenuVisibility, 
                            this );
                        }
                    
                    if ( !asyncStarted )
                        {
                        ++iCurrentAction;
                        }
                    }
                
                if ( !asyncStarted )
                    {
                    iCurrentAction = 0;
                    ++iCurrentActionPlugin;
                    }
                }
            
            if ( !asyncStarted )
                {
                // inform observer that method finished
                iObserver->QueryActionsComplete();
                iLastEvent = ECasEventIdle;
                }
            
            break;
            }
            
        case ECasEventAfterPriorityForContactSet:
            {
            // here we have to do the same as in 
            // ECasEventBeforePriorityForContactSet event
            // because GetActionL returns const reference which we can't hold 
            // in class' data.
            CFscContactActionPlugin* plugin = 
                iActionPlugins[ iCurrentActionPlugin ];
            
            const CArrayFix<TUid>* actionList = plugin->ActionList();
            TInt actionCount = 
                ( actionList != NULL ) ? actionList->Count() : 0;
            
            TUid uid = ( *actionList )[ iCurrentAction ];
            const MFscContactAction& action = plugin->GetActionL( uid );
            
            // Add action if given minimum priority is exceeded
            if ( action.Type() != KFscAtComSendCalReq &&
                 iContactActionQueryResult.iPriority >= aMinPriority &&
                 iContactActionQueryResult.iActionMenuVisibility.iVisibility != 
                 TFscContactActionVisibility::EFscActionHidden )
                {
                iContactActionQueryResult.iAction = &action;
                iActionList->AppendL( iContactActionQueryResult );
                }
            
            // if only one action is needed we stop method execution
            if ( iStopWhenOneActionFound && iActionList->Count() )
                {
                iLastEvent = ECasEventIdle;
                iObserver->QueryActionsComplete();
                }
            else
                {
                // get next action
                if ( iCurrentAction < actionCount )
                    {
                    iCurrentAction++;
                    }
                else
                    {
                    // no more actions for current plugin
                    iCurrentAction = 0;
                    iCurrentActionPlugin++;
                    }
                
                // if needed run for next plugin
                if ( iCurrentActionPlugin < iActionPlugins.Count() )
                    {
                    iLastEvent = ECasEventBeforePriorityForContactSet;
                    QueryActionsL( *iActionList, *iContactSet, 
                        iStopWhenOneActionFound, iActionTypeFlags, 
                        iMinPriority, iObserver );
                    }
                else
                    {
                    // inform observer that method finished
                    iLastEvent = ECasEventIdle;
                    iObserver->QueryActionsComplete();
                    }
                }
                        
            break;
            }
            
        case ECasEventCanceledPriorityForContactSet:
            {
            // PriorityforContactSet was cancelled
            iLastEvent = ECasEventIdle;
            iCurrentActionPlugin = 0;
            iCurrentAction = 0;
            iActionList->Reset();
            break;
            }
            
        default:
            {
            // we shouldn't be here
            iObserver->QueryActionsFailed( KErrArgument );
            iLastEvent = ECasEventIdle;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Cancels async method QueryActionsL.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::CancelQueryActions()
    {
	FUNC_LOG;
    TInt err = KErrNone;
    if ( iActionPlugins.Count() > iCurrentActionPlugin )
        {
        iActionPlugins[ iCurrentActionPlugin ]->CancelPriorityForContactSet();
        iLastEvent = ECasEventCanceledPriorityForContactSet;
        TRAP( err, QueryActionsL( *iActionList, *iContactSet, 
                    iStopWhenOneActionFound, iActionTypeFlags, iMinPriority, 
                    iObserver ) );        
        }
    
    if ( err!= KErrNone )
        {
        iObserver->QueryActionsFailed( err );
        iLastEvent = ECasEventIdle;
        }
    }

// ---------------------------------------------------------------------------
// Called when PriorityForContactSetL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::PriorityForContactSetComplete( 
    TInt aPriority )
    {
	FUNC_LOG;
    iContactActionQueryResult.iPriority = aPriority;
    iLastEvent = ECasEventAfterPriorityForContactSet;
    TRAPD(err, QueryActionsL( *iActionList, *iContactSet, 
            iStopWhenOneActionFound, iActionTypeFlags, iMinPriority, 
            iObserver ) );
    if ( err!= KErrNone )
    	{
    	iLastEvent = ECasEventIdle;
    	iObserver->QueryActionsFailed( err );
    	}
    }

// ---------------------------------------------------------------------------
// Called when PriorityForContactSetL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::PriorityForContactSetFailed( 
    TInt aError )
    {
	FUNC_LOG;
    iLastEvent = ECasEventIdle;
    iObserver->QueryActionsFailed( aError );
    }

// ---------------------------------------------------------------------------
// Execute action. Aynchronous method.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::ExecuteL( 
    TUid aActionUid, MFscContactSet& aContactSet,
    MFscContactActionPluginEngineObserver* aObserver )
    {
	FUNC_LOG;
    iObserver = aObserver;
        
    // Loop plugins
    TBool found = EFalse;
    for ( iCurrentActionPlugin = 0; 
          iCurrentActionPlugin < iActionPlugins.Count() && !found; 
          iCurrentActionPlugin++ )
        {
        // Loop actions
        const CArrayFix<TUid>* actionList = 
            iActionPlugins[ iCurrentActionPlugin ]->ActionList();
        
        TInt actionCount = 
            ( actionList != NULL) ? actionList->Count() : 0;
        for ( TInt j = 0; j < actionCount && !found; j++)
            {
            if ( ( *actionList )[j] == aActionUid )
                {
                found = ETrue;
                
                // Action found -> Execute
                iActionPlugins[ iCurrentActionPlugin ]->ExecuteL( 
                    aActionUid, aContactSet, this );
                    
                }
            }// Loop actions
        } // Loop plugins
        
    if ( !found )
        {
        ExecuteFailed( KErrNotFound );
        }
        
    }

// ---------------------------------------------------------------------------
// Cancels async method ExecuteL.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::CancelExecute()
    {
	FUNC_LOG;
    iActionPlugins[ iCurrentActionPlugin ]->CancelExecute();
    }

// ---------------------------------------------------------------------------
// Called when ExecuteL method is complete.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::ExecuteComplete()
    {
	FUNC_LOG;
    iObserver->ExecuteComplete();
    }
       
// ---------------------------------------------------------------------------
// Called when ExecuteL method failed.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::ExecuteFailed( TInt aError )
    {
	FUNC_LOG;
    iObserver->ExecuteFailed( aError );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFscContactActionPluginEngine::CFscContactActionPluginEngine(
    CVPbkContactManager& aContactManager )
    : iContactManager( aContactManager ),
    iPluginParams( NULL ),
    iLastEvent( ECasEventIdle )
    {
	FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFscContactActionPluginEngine::ConstructL()
    {
	FUNC_LOG;
    iActionUtils = CFscActionUtils::NewL( iContactManager );
    iPluginParams = TFscContactActionPluginParams( iActionUtils );
    }   

