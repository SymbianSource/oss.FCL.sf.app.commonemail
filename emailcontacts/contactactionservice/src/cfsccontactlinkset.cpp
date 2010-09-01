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
* Description:  Implementation of the class CFscContactLinkSet.
*
*/


// INCUDES
#include "emailtrace.h"
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
//<cmail>
#include "mfsccontactlinkiterator.h"
#include "mfsccontactsetobserver.h"
//</cmail>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLinkArray.h>

#include "cfsccontactlinkset.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFscContactLinkSet* CFscContactLinkSet::NewL( 
    CVPbkContactManager& aVPbkContactManager,
    MFscContactLinkIterator* aIterator )
    {
    FUNC_LOG;

    CFscContactLinkSet* self = 
        new ( ELeave ) CFscContactLinkSet( aVPbkContactManager );
    CleanupStack::PushL( self );
    self->ConstructL( aIterator );
    CleanupStack::Pop( self );
        
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFscContactLinkSet::~CFscContactLinkSet()
    {
    FUNC_LOG;
    delete iCurrentContact;
    delete iContactLinkArray;
    delete iVPbkOperationNextContact;
    delete iVPbkOperationNextGroup;
    delete iVPbkOperationGetGroupContact;
    }

// ---------------------------------------------------------------------------
// Contact count.
// ---------------------------------------------------------------------------
//
TInt CFscContactLinkSet::ContactCount() const
    {
    FUNC_LOG;
    return iIterator->ContactCount();
    }

// ---------------------------------------------------------------------------
// Checks if iterator has more contacts.
// ---------------------------------------------------------------------------
//
TBool CFscContactLinkSet::HasNextContact() const
    {
    FUNC_LOG;
    return iIterator->HasNextContact();
    }

// ---------------------------------------------------------------------------
// Sets the iterator to point to the first contact in the list.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::SetToFirstContact()
    {
    FUNC_LOG;
    iIterator->SetToFirstContact();
    }

// ---------------------------------------------------------------------------
// Asynchronous operation for retrieving store contact for contact.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::NextContactL( MFscContactSetObserver* aObserver )
    {
    FUNC_LOG;
    MVPbkContactLink* contactLink = iIterator->NextContactL();
    
    iObserver = aObserver;
    
    // async call - result in VPbkSingleContactOperationComplete()
    iVPbkOperationNextContact = 
            iVPbkContactManager.RetrieveContactL( *contactLink, *this );
    }

// ---------------------------------------------------------------------------
// Used for retrieving link for contact.
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CFscContactLinkSet::NextContactLinkL()
    {
    FUNC_LOG;
    return iIterator->NextContactL();
    }

// ---------------------------------------------------------------------------
// Used for canceling asynchronous NextContactL operation.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::CancelNextContactL()
    {
    FUNC_LOG;
    CancelGetGroupContactL();
    if ( iVPbkOperationNextContact )
        {
        delete iVPbkOperationNextContact;
        iVPbkOperationNextContact = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Group count.
// ---------------------------------------------------------------------------
//
TInt CFscContactLinkSet::GroupCount() const
    {
    FUNC_LOG;
    return iIterator->GroupCount();
    }

// ---------------------------------------------------------------------------
// Checks if iterator has more groups.
// ---------------------------------------------------------------------------
//
TBool CFscContactLinkSet::HasNextGroup() const
    {
    FUNC_LOG;
    return iIterator->HasNextGroup();
    }

// ---------------------------------------------------------------------------
// Sets the iterator to point to the first group in the list.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::SetToFirstGroup()
    {
    FUNC_LOG;
    iIterator->SetToFirstGroup();
    }

// ---------------------------------------------------------------------------
// Asynchronous operation for retrieving store contact for group.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::NextGroupL( MFscContactSetObserver* aObserver )
    {
    FUNC_LOG;
    MVPbkContactLink* groupLink = iIterator->NextGroupL();
        
    iObserver = aObserver;
    
    // async call - result in VPbkSingleContactOperationComplete()
    iVPbkOperationNextGroup = 
            iVPbkContactManager.RetrieveContactL( *groupLink, *this );
    }

// ---------------------------------------------------------------------------
// Used for retrieving link for group.
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CFscContactLinkSet::NextGroupLinkL()
    {
    FUNC_LOG;
    return iIterator->NextGroupL();
    }

// ---------------------------------------------------------------------------
// Used for canceling asynchronous NextGroupL operation.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::CancelNextGroupL()
    {
    FUNC_LOG;
    CancelGetGroupContactL();
    if ( iVPbkOperationNextGroup )
        {
        delete iVPbkOperationNextGroup;
        iVPbkOperationNextGroup = NULL;
        }
    }

// ---------------------------------------------------------------------------
// From MVPbkSingleContactOperationObserver.
// Called when the operation is completed.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    if ( iVPbkOperationNextContact == &aOperation )
        {
        delete iCurrentContact;
        iCurrentContact = aContact;
        delete iVPbkOperationNextContact;
        iVPbkOperationNextContact = NULL;
        iObserver->NextContactComplete( iCurrentContact );
        }
    else if ( iVPbkOperationNextGroup == &aOperation )
        {
        delete iCurrentContact;
        iCurrentContact = aContact;
        delete iVPbkOperationNextGroup;
        iVPbkOperationNextGroup = NULL;
        iObserver->NextGroupComplete( iCurrentContact );
        }
    else if ( iVPbkOperationGetGroupContact == &aOperation )
        {
        delete iVPbkOperationGetGroupContact;
        iVPbkOperationGetGroupContact = NULL;
        
        delete iContactLinkArray;
        iContactLinkArray = NULL;
        
        iObserver->GetGroupContactComplete( aContact );
        }
    else
        {
        //User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// From MVPbkSingleContactOperationObserver.
// Called if the operation fails.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& aOperation, TInt aError )
    {
    FUNC_LOG;
    if ( iVPbkOperationNextContact == &aOperation )
        {
        delete iVPbkOperationNextContact;
        iVPbkOperationNextContact = NULL;
        iObserver->NextContactFailed( aError );
        }
    else if ( iVPbkOperationNextGroup == &aOperation )
        {
        delete iVPbkOperationNextGroup;
        iVPbkOperationNextGroup = NULL;
        iObserver->NextGroupFailed( aError );
        }
    else if ( iVPbkOperationGetGroupContact == &aOperation )
        {
        delete iVPbkOperationGetGroupContact;
        iVPbkOperationGetGroupContact = NULL;
        
        delete iContactLinkArray;
        iContactLinkArray = NULL;
        
        iObserver->GetGroupContactFailed( aError );
        }
    else
        {
        //User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFscContactLinkSet::CFscContactLinkSet( 
    CVPbkContactManager& aVPbkContactManager )
    : CFscContactSet( aVPbkContactManager )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::ConstructL( MFscContactLinkIterator* aIterator )
    {
    FUNC_LOG;
    iIterator = aIterator;        
    }

// ---------------------------------------------------------------------------
// Used for canceling asynchronous GetGroupContactL operation.
// ---------------------------------------------------------------------------
//
void CFscContactLinkSet::CancelGetGroupContactL()
    {
    FUNC_LOG;
    if ( iVPbkOperationGetGroupContact )
        {
        delete iVPbkOperationGetGroupContact;
        iVPbkOperationGetGroupContact = NULL;
        
        delete iContactLinkArray;
        iContactLinkArray = NULL;
        }
    }

