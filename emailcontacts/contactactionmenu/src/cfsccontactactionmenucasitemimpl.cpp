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
* Description:  Implementation of the class CFscContactActionMenuCasItemImpl.
*
*/


// INCUDES
//<cmail>
#include "emailtrace.h"
#include "mfsccontactaction.h"
#include "tfsccontactactionvisibility.h"
//</cmail>

#include "cfsccontactactionmenucasitemimpl.h"
#include "fsccontactactionmenudefines.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuCasItemImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuCasItemImpl* CFscContactActionMenuCasItemImpl::NewL(
    const TFscContactActionQueryResult& aItem )
    {
    FUNC_LOG;

    CFscContactActionMenuCasItemImpl* self = 
        CFscContactActionMenuCasItemImpl::NewLC( aItem );
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuCasItemImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuCasItemImpl* CFscContactActionMenuCasItemImpl::NewLC(
    const TFscContactActionQueryResult& aItem )
    {
    FUNC_LOG;

    CFscContactActionMenuCasItemImpl* self = 
        new (ELeave) CFscContactActionMenuCasItemImpl( aItem );
    CleanupStack::PushL( self );
    self->ConstructL( aItem );
        
    return self;
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuCasItemImpl::~CFscContactActionMenuCasItemImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuCasItemImpl::~CFscContactActionMenuCasItemImpl()
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuCasItemImpl::CFscContactActionMenuCasItemImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuCasItemImpl::CFscContactActionMenuCasItemImpl(
    const TFscContactActionQueryResult& aItem ) 
    : CFscContactActionMenuItemImpl( aItem.iAction->Icon(), 
        aItem.iPriority, EFalse, aItem.iAction->Uid() )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuCasItemImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuCasItemImpl::ConstructL(
    const TFscContactActionQueryResult& aItem )
    {
    FUNC_LOG;
    
    CFscContactActionMenuItemImpl::ConstructL( 
        aItem.iAction->ActionMenuText() );
    
    switch ( aItem.iActionMenuVisibility.iVisibility )
        {
        case TFscContactActionVisibility::EFscActionHidden:
            {
            SetHidden( ETrue );
            break;
            }
        case TFscContactActionVisibility::EFscActionDimmed:
            {
            SetDimmed( ETrue );
            
            HBufC* reason;
            aItem.iAction->ReasonL( 
                aItem.iActionMenuVisibility.iReasonId, reason );
            CleanupStack::PushL( reason );
            SetDimmReasonL( *reason );
            CleanupStack::PopAndDestroy( reason );
            break;
            }
        default:
            {
            //Do nothing
            break;
            }
        }
    
    }

