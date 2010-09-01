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
* Description:  Implementation of the class CFscContactSet.
*
*/


// INCUDES
#include "emailtrace.h"
#include <e32std.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
//<cmail>
#include "mfsccontactsetobserver.h"
//</cmail>

#include "cfsccontactset.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFscContactSet::CFscContactSet( CVPbkContactManager& aVPbkContactManager )
    : iVPbkContactManager( aVPbkContactManager )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFscContactSet::~CFscContactSet()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Contact count of group.
// ---------------------------------------------------------------------------
//    
TInt CFscContactSet::GroupContactCountL( MVPbkStoreContact& aStoreContact )
    {
    FUNC_LOG;
    TInt result = 0;
    
    MVPbkContactGroup* group = aStoreContact.Group();
    
    if ( group )
        {
        MVPbkContactLinkArray* array = group->ItemsContainedLC(); 
        result = array->Count();
        CleanupStack::PopAndDestroy();
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// Asynchronous operation for retrieving contact from group.
// ---------------------------------------------------------------------------
//    
void CFscContactSet::GetGroupContactL( MVPbkStoreContact& aStoreContact, 
    TInt aIndex, MFscContactSetObserver* aObserver )
    {
    FUNC_LOG;
    MVPbkContactGroup* group = aStoreContact.Group();
    
    iObserver = aObserver;
    
    if ( group )
        {
        iContactLinkArray = group->ItemsContainedLC();
        CleanupStack::Pop();
        
        if ( aIndex < iContactLinkArray->Count() )
            {
            const MVPbkContactLink& link = iContactLinkArray->At( aIndex );
            iVPbkOperationGetGroupContact = 
                iVPbkContactManager.RetrieveContactL( link, *this );
            }
        else
            {
            iObserver->GetGroupContactFailed( KErrArgument );
            }
        }
    else
        {
        iObserver->GetGroupContactFailed( KErrArgument );
        }
    }

