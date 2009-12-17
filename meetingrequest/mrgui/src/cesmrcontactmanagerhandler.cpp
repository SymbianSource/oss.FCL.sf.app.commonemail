/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CESMRContactManagerHandler.
*
*/


#include "emailtrace.h"
#include "cesmrcontactmanagerhandler.h"

#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreList.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::CESMRContactManagerHandler
// ---------------------------------------------------------------------------
//
CESMRContactManagerHandler::CESMRContactManagerHandler()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::NewL
// ---------------------------------------------------------------------------
//
CESMRContactManagerHandler* CESMRContactManagerHandler::NewL()
    {
    FUNC_LOG;
    CESMRContactManagerHandler* self = new (ELeave) CESMRContactManagerHandler();
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop (self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRContactManagerHandler::ConstructL ( )
    {
    FUNC_LOG;
    CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();
    uriList->AppendL(VPbkContactStoreUris::DefaultCntDbUri() );
    iContactManager = CVPbkContactManager::NewL( *uriList );
    CleanupStack::PopAndDestroy(uriList);

    MVPbkContactStoreList& stores = iContactManager->ContactStoresL( );
    stores.OpenAllL( *this );
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::~CESMRContactManagerHandler
// ---------------------------------------------------------------------------
//
CESMRContactManagerHandler::~CESMRContactManagerHandler()
    {
    FUNC_LOG;
    delete iContactManager;
    iObservers.Reset();
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::IsReady
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRContactManagerHandler::IsReady()
    {
    FUNC_LOG;
    return iContactManagerReady;
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::AddObserverL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactManagerHandler::AddObserverL(
        MESMRContactManagerObserver* aObserver )
    {
    FUNC_LOG;
    if ( iObservers.Find(aObserver) == KErrNotFound )
        {
        iObservers.AppendL(aObserver);
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::RemoveObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactManagerHandler::RemoveObserver(
        MESMRContactManagerObserver* aObserver )
    {
    FUNC_LOG;
    TInt index = iObservers.Find(aObserver );
    if ( index != KErrNotFound )
        {
        iObservers.Remove(index );
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::GetContactManager
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactManager& CESMRContactManagerHandler::GetContactManager()
    {
    FUNC_LOG;
    return *iContactManager;
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::OpenComplete
// ---------------------------------------------------------------------------
//
void CESMRContactManagerHandler::OpenComplete ( )
    {
    FUNC_LOG;
    iContactManagerReady = ETrue;
    const TInt count(iObservers.Count());
    for (TInt i=count-1; i>=0; --i )
        {
        iObservers[i]->ContactManagerReady( );
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::StoreReady
// ---------------------------------------------------------------------------
//
void CESMRContactManagerHandler::StoreReady(
		MVPbkContactStore& /*aContactStore*/ )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::StoreUnavailable
// ---------------------------------------------------------------------------
//
void CESMRContactManagerHandler::StoreUnavailable (
        MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactManagerHandler::HandleStoreEventL
// ---------------------------------------------------------------------------
//
void CESMRContactManagerHandler::HandleStoreEventL (
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    FUNC_LOG;
    // Do nothing
    }

// End of file

