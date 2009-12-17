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
* Description:  Implementation of the class CFscStoreContactSet.
*
*/


// INCUDES
#include "emailtrace.h"
#include <e32std.h>
//<cmail>
#include "mfsccontactsetobserver.h"
//</cmail>
#include <MVPbkStoreContact.h>

#include "cfscstorecontactset.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFscStoreContactSet* CFscStoreContactSet::NewL( 
    CVPbkContactManager& aVPbkContactManager,
    const RFscStoreContactList& aStoreContactList )
    {
    FUNC_LOG;

    CFscStoreContactSet* self = 
        new ( ELeave ) CFscStoreContactSet( aVPbkContactManager );
    CleanupStack::PushL( self );
    self->ConstructL( aStoreContactList );
    CleanupStack::Pop( self );
        
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFscStoreContactSet::~CFscStoreContactSet()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Contact count.
// ---------------------------------------------------------------------------
//
TInt CFscStoreContactSet::ContactCount() const
    {
    FUNC_LOG;
    if ( iStoreContactList->Count() )
        {
        MVPbkStoreContact* store = (*iStoreContactList)[0];
        if ( !store->Group() )
            {
            return iStoreContactList->Count();
            }
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// Group count.
// ---------------------------------------------------------------------------
//
TInt CFscStoreContactSet::GroupCount() const
    {
    FUNC_LOG;
    if ( iStoreContactList->Count() )
        {
        MVPbkStoreContact* store = (*iStoreContactList)[0];
        if ( store->Group() )
            {
            return iStoreContactList->Count();
            }
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// Checks if collection has next contact.
// ---------------------------------------------------------------------------
//
TBool CFscStoreContactSet::HasNextContact() const
    {
    FUNC_LOG;
    TBool result = EFalse;
    
    if ( iCurrentStoreContact < iStoreContactList->Count() )
        {
        if ( !(*iStoreContactList)[0]->Group() )
            {
            result = ETrue;
            }
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// Checks if collection has next group.
// Just for interface compatibility - always EFalse.
// ---------------------------------------------------------------------------
//
TBool CFscStoreContactSet::HasNextGroup() const
    {
    FUNC_LOG;
    TBool result = EFalse;
    
    if ( iCurrentStoreContact < iStoreContactList->Count() )
        {
        if ( (*iStoreContactList)[0]->Group() )
            {
            result = ETrue;
            }
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// Sets the index to point to the first contact in the list.
// ---------------------------------------------------------------------------
//
void CFscStoreContactSet::SetToFirstContact()
    {
    FUNC_LOG;
    iCurrentStoreContact = 0;
    }

// ---------------------------------------------------------------------------
// Sets the index to point to the first group in the list.
// Just for interface compatibility.
// ---------------------------------------------------------------------------
//
void CFscStoreContactSet::SetToFirstGroup()
    {
    FUNC_LOG;
    iCurrentStoreContact = 0;
    }

// ---------------------------------------------------------------------------
// Retrieves store contact.
// ---------------------------------------------------------------------------
//
void CFscStoreContactSet::NextContactL( 
    MFscContactSetObserver* aObserver )
    {
    FUNC_LOG;
    if ( HasNextContact() )
        {
        aObserver->NextContactComplete( 
                (*iStoreContactList)[ iCurrentStoreContact++ ] );
        }
    else
        {
        aObserver->NextContactFailed( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// Used for retrieving link for contact.
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CFscStoreContactSet::NextContactLinkL()
    {
    FUNC_LOG;
    return NULL;
    }

// ---------------------------------------------------------------------------
// Retrieves store contact for group.
// Just for interface compatibility.
// ---------------------------------------------------------------------------
//
void CFscStoreContactSet::NextGroupL( 
    MFscContactSetObserver* aObserver )
    {
    FUNC_LOG;
    if ( HasNextGroup() )
        {
        aObserver->NextGroupComplete( 
                (*iStoreContactList)[ iCurrentStoreContact++ ] );
        }
    else
        {
        aObserver->NextGroupFailed( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// Used for retrieving link for group.
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CFscStoreContactSet::NextGroupLinkL()
    {
    FUNC_LOG;
    return NULL;
    }

// ---------------------------------------------------------------------------
// Cancel NextContactL method.
// Just for interface compatibility.
// ---------------------------------------------------------------------------
//
void CFscStoreContactSet::CancelNextContactL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Cancel NextGroupL method.
// Just for interface compatibility.
// ---------------------------------------------------------------------------
//
void CFscStoreContactSet::CancelNextGroupL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Called when the operation is completed.
// Just for interface compatibility.
// ---------------------------------------------------------------------------
//    
void CFscStoreContactSet::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& /*aOperation*/, 
    MVPbkStoreContact* /*aContact*/ )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// Called if the operation fails.
// Just for interface compatibility.
// ---------------------------------------------------------------------------
//    
void CFscStoreContactSet::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFscStoreContactSet::CFscStoreContactSet( 
    CVPbkContactManager& aVPbkContactManager )
    : CFscContactSet( aVPbkContactManager )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFscStoreContactSet::ConstructL( 
    const RFscStoreContactList& aStoreContactList )
    {
    FUNC_LOG;
    iStoreContactList = &aStoreContactList;
    }     
    
// ======== GLOBAL FUNCTIONS ========

