/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements classes CIpsSetUiCtrlPageResource, Resource. 
*
*/


#include "emailtrace.h"
#include <cstack.h>                     // CStack

#include "ipssetutilsexception.h"
#include "ipssetuictrlpageresource.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::CIpsSetUiCtrlPageResource()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlPageResource::CIpsSetUiCtrlPageResource()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::~CIpsSetUiCtrlPageResource()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlPageResource::~CIpsSetUiCtrlPageResource()
    {
    FUNC_LOG;
    if ( iPageResources )
        {        
        iPageResources->ResetAndDestroy();
        }
        
    delete iPageResources;
    iPageResources = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlPageResource::ConstructL()
    {
    FUNC_LOG;
    iPageResources = new ( ELeave ) CResourceStackArray( EIpsSetUiLastPage );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlPageResource* CIpsSetUiCtrlPageResource::NewL()
    {
    FUNC_LOG;
    CIpsSetUiCtrlPageResource* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiCtrlPageResource* CIpsSetUiCtrlPageResource::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiCtrlPageResource* self = new ( ELeave ) CIpsSetUiCtrlPageResource();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
    
/******************************************************************************



******************************************************************************/
    
// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::PushResourceL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiCtrlPageResource::PushResourceL(
    const TInt aType,
    const TInt aPageResource )
    {
    FUNC_LOG;
    Stack( aType ).AppendL( aPageResource );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::Resource()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiCtrlPageResource::Resource(
    const TInt aType, 
    const TBool aRemove )
    {
    FUNC_LOG;
    CResourceStack& stack = Stack( aType );
    TInt index = stack.Count() - 1;        
    if ( !aRemove || aRemove && !index )
        {
        return stack[index];
        }
    else
        {
        TInt resource = stack[index];
        stack.Delete( index );
        return resource;        
        }        
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::Count()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiCtrlPageResource::Count( const TInt aType )
    {
    FUNC_LOG;
    return Stack( aType ).Count();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::CreateStackL()
// ----------------------------------------------------------------------------
//
CResourceStack* CIpsSetUiCtrlPageResource::CreateStackL( 
    const TInt aPageResource )
    {
    FUNC_LOG;
    IPS_ASSERT_DEBUG( iPageResources , EArrayNull, EPageResource );
    
    CResourceStack* stack = new ( ELeave ) CResourceStack( EIpsSetUiLastPage );
    CleanupStack::PushL( stack );
    
    iPageResources->AppendL( stack );             
    stack->AppendL( aPageResource ); 
    
    CleanupStack::Pop( stack );
            
    return stack;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::operator[]()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiCtrlPageResource::operator[]( const TInt aType )
    {
    FUNC_LOG;
    return Resource( aType );
    }    
    
/******************************************************************************

    TOOLS

******************************************************************************/
    
    
// ----------------------------------------------------------------------------
// CIpsSetUiCtrlPageResource::Stack()
// ----------------------------------------------------------------------------
//
CResourceStack& CIpsSetUiCtrlPageResource::Stack( const TInt aType )
    {
    FUNC_LOG;
    IPS_ASSERT_DEBUG( iPageResources->Count(), EStackEmpty, EPageResource );
    IPS_ASSERT_DEBUG( aType < iPageResources->Count(), EUnknownType, EPageResource );
    IPS_ASSERT_DEBUG( ( *iPageResources )[aType] , EStackNull, EPageResource );
    IPS_ASSERT_DEBUG( iPageResources , EArrayNull, EPageResource );
    
    return *( *iPageResources )[aType];
    }    
    
// End of File

