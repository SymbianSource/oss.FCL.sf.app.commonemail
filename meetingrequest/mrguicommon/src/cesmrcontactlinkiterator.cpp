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
* Description:  Implementation of class CESMRContactLinkIterator.
*
*/


#include "emailtrace.h"
#include "cesmrcontactlinkiterator.h"
#include <mvpbkcontactlinkarray.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::NewL( )
// ---------------------------------------------------------------------------
//
CESMRContactLinkIterator* CESMRContactLinkIterator::NewL( )
    {
    FUNC_LOG;
    CESMRContactLinkIterator* self = new (ELeave) CESMRContactLinkIterator();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::~CESMRContactLinkIterator( )
// ---------------------------------------------------------------------------
//
CESMRContactLinkIterator::~CESMRContactLinkIterator( )
    {
    FUNC_LOG;
    delete iContactLinkArray;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::SetContactLinkArray
// ---------------------------------------------------------------------------
//
void CESMRContactLinkIterator::SetContactLinkArray(
        MVPbkContactLinkArray* aContactLinkArray )
    {
    FUNC_LOG;
    delete iContactLinkArray;
    iContactLinkArray = aContactLinkArray;
    iContactLinkArrayIndex = 0;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::ContactCount
// ---------------------------------------------------------------------------
//
TInt CESMRContactLinkIterator::ContactCount() const
    {
    FUNC_LOG;
    TInt count( KErrNotFound ); 
    if ( iContactLinkArray )
    	{
    	count =  iContactLinkArray->Count();
    	}    
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::HasNextContact
// ---------------------------------------------------------------------------
//
TBool CESMRContactLinkIterator::HasNextContact() const
    {
    FUNC_LOG;
    return iContactLinkArrayIndex < ContactCount();
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::NextContactL
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CESMRContactLinkIterator::NextContactL()
    {
    FUNC_LOG;
    MVPbkContactLink* link = NULL;
    if ( iContactLinkArray )
    	{
    	link = (MVPbkContactLink*) 
					&(iContactLinkArray->At( iContactLinkArrayIndex++ ));
    	}
    
    return link;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::SetToFirstContact( )
// ---------------------------------------------------------------------------
//
void CESMRContactLinkIterator::SetToFirstContact( )
    {
    FUNC_LOG;
    iContactLinkArrayIndex = 0;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::GroupCount( )
// ---------------------------------------------------------------------------
//
TInt CESMRContactLinkIterator::GroupCount( ) const
    {
    FUNC_LOG;
    return 0;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::HasNextGroup( )
// ---------------------------------------------------------------------------
//
TBool CESMRContactLinkIterator::HasNextGroup( ) const
    {
    FUNC_LOG;
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::NextGroupL( )
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CESMRContactLinkIterator::NextGroupL( )
    {
    FUNC_LOG;
    return NULL;
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::SetToFirstGroup( )
// ---------------------------------------------------------------------------
//
void CESMRContactLinkIterator::SetToFirstGroup( )
    {
    FUNC_LOG;
    //Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactLinkIterator::CESMRContactLinkIterator( )
// ---------------------------------------------------------------------------
//
CESMRContactLinkIterator::CESMRContactLinkIterator( )
    {
    FUNC_LOG;
    //Do nothing
    }

// End of file

