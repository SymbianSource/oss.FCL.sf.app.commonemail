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
* Description:  Definition of the class CPbkxRclSearchEngine.
*
*/


#include "emailtrace.h"
#include <coemain.h>
//<cmail>
#include "cfsccontactactionservice.h"
#include "mfsccontactaction.h"
#include "fscactionpluginactionuids.h"

#include "tfsccontactactionqueryresult.h"
//</cmail>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>

#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkStoreContact.h>  // for RFscStoreContactList items

#include "cpbkxrclactionservicewrapper.h"
#include "pbkxrclengineconstants.h"
#include "cpbkxrclcontactconverter.h"


// Contact databases
_LIT(KRclDefaultCntDbURI, "cntdb://c:contacts.cdb");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclActionServiceWrapper* CPbkxRclActionServiceWrapper::NewL( 
    CPbkContactEngine& aContactEngine )
    {
    FUNC_LOG;
    CPbkxRclActionServiceWrapper* wrapper = new ( ELeave ) 
        CPbkxRclActionServiceWrapper( aContactEngine );
    CleanupStack::PushL( wrapper );
    wrapper->ConstructL();
    CleanupStack::Pop( wrapper );
    return wrapper;
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::CPbkxRclActionServiceWrapper
// ---------------------------------------------------------------------------
//
CPbkxRclActionServiceWrapper::CPbkxRclActionServiceWrapper( CPbkContactEngine& 
    aContactEngine ) : CBase(), iContactEngine( aContactEngine ) 
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::~CPbkxRclActionServiceWrapper
// ---------------------------------------------------------------------------
//
CPbkxRclActionServiceWrapper::~CPbkxRclActionServiceWrapper()
    {
    FUNC_LOG;
    
    iConvertedContact.ResetAndDestroy(); 
    
    if( iContactManager )
        {
        if ( iContactStore )
            {
            iContactStore->Close(*this);
            }
        } 
    delete iContactManager;
    delete iContactActionService;
    delete iContactConverter;    
    delete iWait;  
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::ConstructL()
    {
    FUNC_LOG;
    // Create Contact Manager
    CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();    
    uriList->AppendL( TVPbkContactStoreUriPtr( KRclDefaultCntDbURI ) );  
    iContactManager = CVPbkContactManager::NewL( *uriList, &(CCoeEnv::Static()->FsSession()) );
    CleanupStack::PopAndDestroy( uriList );
    
    // Get store list
    MVPbkContactStoreList& defaultstore = iContactManager->ContactStoresL();
    
    // Open contact store
    iContactStore = &defaultstore.At( 0 );
    iContactStore->OpenL( *this );   
    
    iContactActionService = CFscContactActionService::NewL( *iContactManager );
    
    iContactConverter = CPbkxRclContactConverter::NewL( 
            iContactEngine, *iContactManager, *iContactStore );
    
    iWait = new ( ELeave ) CActiveSchedulerWait();

    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::SetContactSelectorMode
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::SetContactSelectorMode( 
    TBool aContactSelectorMode )
    {
    FUNC_LOG;
    iContactSelectorMode = aContactSelectorMode;
    if ( aContactSelectorMode )
        {
        // in contact selector mode the flags are static

        // all actions except save as is queried 
        iFlags = KFscAtMan & ( ~KFscAtManSaveAs );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::SetActionMenuMode
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::SetActionMenuMode(
    TBool aActionMenuMode )
    {
    FUNC_LOG;
    // if not in contact selector mode, flags depend on whether
    // we are showing the action menu or not
    if ( !iContactSelectorMode )
        {
        if ( aActionMenuMode )
            {
            // in action menu mode poc and save as are disabled
            iFlags = KFscAtAll & ( ~KFscAtComCallPoc ) & ( ~KFscAtManSaveAs );
            }
        else
            {
            // when not in action menu mode, all actions but save as contact 
            // are enabled
            iFlags = KFscAtAll & ( ~KFscAtManSaveAs );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::SetCurrentContactL
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::SetCurrentContactL( CPbkContactItem* aContactItem )
    {
    FUNC_LOG;

    iConvertedContact.ResetAndDestroy(); // clean before new use
    
    // Convert contact to MVPbkStoreContact object, only one item in 
    if ( aContactItem )
        {
        iContactConverter->ConvertContactL( *aContactItem, iConvertedContact );
        }

    iContactActionService->SetContactSetL( iConvertedContact );

    iSaveAsContactActionExists = EFalse;

    iQueryComplete = EFalse;
    // first query for save as contact
    iContactActionService->QueryActionsL( this, EFalse, KFscAtManSaveAs );

    // Wait async operation callback. If operation is completed before returning method call
    // don't start waiting
    if (!iQueryComplete)
        {
        iWait->Start();
        }
    
    const CFscContactActionList& actions = iContactActionService->QueryResults();

    // look for save as contact
    for ( TInt i = 0; i < actions.Count(); i++ )
        {
        const TFscContactActionQueryResult& result = actions[i];
        if ( result.iAction->Uid() == KFscActionUidSaveAsContact )
            {
            iSaveAsContactResult = TFscContactActionQueryResult(
                result.iAction, 
                result.iPriority,
                result.iActionMenuVisibility,
                result.iOptionsMenuVisibility );
            
            iSaveAsContactActionExists = ETrue;
            break;
            }
        }
    
    // Wait async operation callback. If operation is completed before returning method call
    // don't start waiting
    iQueryComplete = EFalse;
    iContactActionService->QueryActionsL( this, EFalse, iFlags );
    if (!iQueryComplete)
        {
        iWait->Start();
        } 
        
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::IsActionEnabled
// ---------------------------------------------------------------------------
//
TBool CPbkxRclActionServiceWrapper::IsActionEnabled( 
    const TUint64 aActionType ) const
    {
    FUNC_LOG;
    TBool enabled = EFalse;
    
    const TFscContactActionQueryResult* result = GetResult( aActionType );
    
    if ( result != NULL )
        {
        enabled = ( result->iOptionsMenuVisibility.iVisibility == 
        TFscContactActionVisibility::EFscActionVisible && 
                    result->iPriority >= 0 );
        }

    return enabled;
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::ExecuteActionL
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::ExecuteActionL( const TUint64 aActionType )
    {
    FUNC_LOG;
    const TFscContactActionQueryResult* result = GetResult( aActionType );
   
    if ( result == NULL )
        {
        User::Leave( KErrNotFound );
        }
    iExecuteComplete = EFalse;
    iContactActionService->ExecuteL( result->iAction->Uid(), this );
    // Wait async operation callback. If operation is completed before returning method call
    // don't start waiting
    if (!iExecuteComplete)
        {
        iWait->Start();
        }     
   
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::ActionService
// ---------------------------------------------------------------------------
//
CFscContactActionService* CPbkxRclActionServiceWrapper::ActionService()
    {
    FUNC_LOG;
    return iContactActionService;
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::GetResult
// ---------------------------------------------------------------------------
//
const TFscContactActionQueryResult* CPbkxRclActionServiceWrapper::GetResult( 
    const TUint64 aType ) const
    {
    FUNC_LOG;
    if ( iSaveAsContactActionExists && aType == iSaveAsContactResult.iAction->Type() )
        {
        return &iSaveAsContactResult;
        }
    else
        {
        const CFscContactActionList& actions = iContactActionService->QueryResults();
        
        for ( TInt i = 0; i < actions.Count(); i++ )
            {
            const TFscContactActionQueryResult& queryResult = actions[i];
            if ( queryResult.iAction->Type() == aType )
                {
                return &queryResult;
                }
            }
        }
    return NULL;
    }
// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::ContactConverter
// ---------------------------------------------------------------------------
//
CPbkxRclContactConverter* CPbkxRclActionServiceWrapper::ContactConverter()
    {
    FUNC_LOG;
    return iContactConverter;
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::StoreReady
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::StoreReady( 
        MVPbkContactStore& /* aContactStore */ )
    {
    FUNC_LOG;
    iLastError = KErrNone;
    }
// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::StoreUnavailable()
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::StoreUnavailable( 
        MVPbkContactStore& /* aContactStore */, 
        TInt aReason )
    {
    FUNC_LOG;
    iLastError = aReason;
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::HandleStoreEventL()
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::HandleStoreEventL( 
        MVPbkContactStore& /* aContactStore */, 
        TVPbkContactStoreEvent /* aStoreEvent */ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::QueryActionsComplete()
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::QueryActionsComplete()
    {
    FUNC_LOG;
    iLastError = KErrNone;
    iQueryComplete = ETrue;
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::QueryActionsFailed()
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::QueryActionsFailed( TInt aError )
    {
    FUNC_LOG;
    iLastError = aError;
    iQueryComplete = ETrue;
    if (iWait->IsStarted())
        {    
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::ExecuteComplete()
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::ExecuteComplete()
    {
    FUNC_LOG;
    iLastError = KErrNone;
    iExecuteComplete = ETrue;
    if (iWait->IsStarted())
        {    
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::ExecuteFailed()
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::ExecuteFailed( TInt aError )
    {
    FUNC_LOG;
    iLastError = aError;
    iExecuteComplete = ETrue;
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclActionServiceWrapper::ExecuteFailed()
// ---------------------------------------------------------------------------
//
void CPbkxRclActionServiceWrapper::CancelQuery()
	{
    FUNC_LOG;
	iContactActionService->CancelQueryActions();
	if (iWait->IsStarted())
		{
		iWait->AsyncStop();
		}
	}
