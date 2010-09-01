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
* Description:  Implementation of the class CFscContactActionMenuModelImpl.
*
*/


// INCUDES
//<cmail>
#include "emailtrace.h"
#include "cfsccontactactionservice.h"
#include "mfsccontactaction.h"
//</cmail>

#include "cfsccontactactionmenumodelimpl.h"
#include "cfsccontactactionmenuitemimpl.h"
#include "cfsccontactactionmenucasitemimpl.h"
#include "fsccontactactionmenudefines.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuModelImpl* CFscContactActionMenuModelImpl::NewL(
    CFscContactActionService& aService )
    {
    FUNC_LOG;

    CFscContactActionMenuModelImpl* self = 
        new (ELeave) CFscContactActionMenuModelImpl( aService );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::~CFscContactActionMenuModelImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuModelImpl::~CFscContactActionMenuModelImpl()
    {
    FUNC_LOG;
    iMenuItems.ResetAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::NewMenuItemL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItem* CFscContactActionMenuModelImpl::NewMenuItemL( 
    const TDesC& aMenuText, const CGulIcon* aIcon, TInt aPriority, 
    TBool aStatic, TUid aUid )
    {
    FUNC_LOG;
    
    CFscContactActionMenuItemImpl* newItem = 
        CFscContactActionMenuItemImpl::NewL(
            aMenuText, aIcon, aPriority, aStatic, aUid );   
    
    return newItem;
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::AddItemL
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuModelImpl::AddItemL( 
    CFscContactActionMenuItem* aItem )
    {
    FUNC_LOG;

    // Check that item uid is unique    
    TBool found = EFalse;
    TInt itemCount = iMenuItems.Count();
    for ( TInt i = 0; i < itemCount; i++ )
        {
        if ( iMenuItems[i]->ImplementationUid() == aItem->ImplementationUid() )
            {
            found = ETrue;
            break;
            }
        }
    
    if ( found )
        {
        User::Leave( KErrAlreadyExists );
        }

    TLinearOrder<CFscContactActionMenuItem> order( PriorityCompare );
    iMenuItems.InsertInOrderAllowRepeatsL( aItem, order );
    
    TInt position = iMenuItems.Find( aItem );   
    return position;
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::CasQueryResults
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuModelImpl::AddPreQueriedCasItemsL()
    {
    FUNC_LOG;
    
    const CFscContactActionList& results = Service()->QueryResults();
    
    // Remove old non-static items
    RemoveAll();
    
    // Add actions to the menu
    for ( TInt i = 0; i < results.Count(); i++ )
        {
        CFscContactActionMenuItem* newItem = 
            CFscContactActionMenuCasItemImpl::NewLC( results[i] );
        TLinearOrder<CFscContactActionMenuItem> order( PriorityCompare );
        iMenuItems.InsertInOrderAllowRepeatsL( newItem, order );
        CleanupStack::Pop( newItem );
        }
    
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::RemoveItemL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuModelImpl::RemoveItemL( TInt aIndex )
    {
    FUNC_LOG;
    
    if ( aIndex < 0 || aIndex >= iMenuItems.Count() )
        {
        User::Leave( KErrArgument );
        }
    delete iMenuItems[aIndex];
    iMenuItems.Remove( aIndex );    
 
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::RemoveAll
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuModelImpl::RemoveAll( TBool aStaticAlso )
    {
    FUNC_LOG;
    
    if ( aStaticAlso )
        {
        iMenuItems.ResetAndDestroy();
        }
    else
        {
        for ( TInt i = iMenuItems.Count() - 1; i >= 0; i-- )
            {
            // CAS items are always deleted even if client had changed 
            // them to statics
            CFscContactActionMenuCasItemImpl* temp = 
                dynamic_cast<CFscContactActionMenuCasItemImpl*>( 
                    iMenuItems[i] );
            if ( !iMenuItems[i]->Static() || 
                 temp != NULL )
                {
                delete iMenuItems[i];  
                iMenuItems.Remove( i ); 
                }
            }
        }
    
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::ItemL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItem& CFscContactActionMenuModelImpl::ItemL( TInt aIndex )
    {
    FUNC_LOG;
    
    if ( aIndex < 0 || aIndex >= iMenuItems.Count() )
        {
        User::Leave( KErrArgument );
        }
        
    return *iMenuItems[aIndex];
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::ItemCount
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuModelImpl::ItemCount()
    {
    FUNC_LOG;
    TInt itemCount = iMenuItems.Count();
    return itemCount;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::VisibleItemL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItem& CFscContactActionMenuModelImpl::VisibleItemL( 
    TInt aIndex )
    {
    FUNC_LOG;
    
    CFscContactActionMenuItem* foundItem = NULL;
    TInt itemCount = iMenuItems.Count();
    TInt visibleIndex = KErrNotFound;
    for ( TInt i = 0; i < itemCount; i++ )
        {
        CFscContactActionMenuItem& item = *iMenuItems[i];
        if ( IsVisible( item ) )
            {
            visibleIndex++;
            }
        if ( visibleIndex == aIndex )
            {
            foundItem = iMenuItems[i];
            break;
            }    
        }  
    
    if ( foundItem == NULL )
        {
        User::Leave( KErrArgument );
        }
        
    return *foundItem;
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::VisibleItemCount
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuModelImpl::VisibleItemCount()
    {
    FUNC_LOG;
    TInt itemCount = iMenuItems.Count();
    TInt visibleCount = 0;
    for ( TInt i = 0; i < itemCount; i++ )
        {
        CFscContactActionMenuItem& item = *iMenuItems[i];
        if ( IsVisible( item ) )
            {
            visibleCount++;
            }
        } 
    return visibleCount;
    }  
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::ServiceL
// ---------------------------------------------------------------------------
//
CFscContactActionService* CFscContactActionMenuModelImpl::Service()
    {
    FUNC_LOG;
    
    return &iCas;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::CFscContactActionMenuModelImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuModelImpl::CFscContactActionMenuModelImpl( 
    CFscContactActionService& aService ) 
    : iCas( aService )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuModelImpl::ConstructL()
    {
    FUNC_LOG;
    }   

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::SortMenuItems
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuModelImpl::SortMenuItems()
    {
    FUNC_LOG;
    TLinearOrder<CFscContactActionMenuItem> order( PriorityCompare );
    iMenuItems.Sort( order );
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::IsVisible
// ---------------------------------------------------------------------------
//
TBool CFscContactActionMenuModelImpl::IsVisible( 
    const CFscContactActionMenuItem& aItem )
    {
    FUNC_LOG;
    TBool result = ( aItem.Icon() != NULL &&
        !aItem.Dimmed() && !aItem.Hidden() );
    return result;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuModelImpl::PriorityCompare
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuModelImpl::PriorityCompare( 
    const CFscContactActionMenuItem& aItem1,
    const CFscContactActionMenuItem& aItem2 )
    {
    FUNC_LOG;
    TInt result = aItem2.Priority() - aItem1.Priority();
    return result;
    }

