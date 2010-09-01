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
* Description:  Implementation of the class CFscContactActionMenuItemImpl.
*
*/


// INCUDES
#include "emailtrace.h"
#include "cfsccontactactionmenuitemimpl.h"
#include "fsccontactactionmenudefines.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItemImpl* CFscContactActionMenuItemImpl::NewL()
    {
    FUNC_LOG;

    CFscContactActionMenuItemImpl* self = 
        CFscContactActionMenuItemImpl::NewLC();
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItemImpl* CFscContactActionMenuItemImpl::NewLC()
    {
    FUNC_LOG;

    CFscContactActionMenuItemImpl* self = 
        new (ELeave) CFscContactActionMenuItemImpl( 
            NULL, 0, EFalse, KFscDefaultItemUid );
    CleanupStack::PushL( self );
    self->ConstructL( KNullDesC() );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItemImpl* CFscContactActionMenuItemImpl::NewL( 
    const TDesC& aMenuText,
    const CGulIcon* aIcon,
    TInt aPriority,
    TBool aStatic,
    TUid aUid )
    {
    FUNC_LOG;

    CFscContactActionMenuItemImpl* self = 
        CFscContactActionMenuItemImpl::NewLC( 
            aMenuText, aIcon, aPriority, aStatic, aUid );
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::NewL
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItemImpl* CFscContactActionMenuItemImpl::NewLC( 
    const TDesC& aMenuText,
    const CGulIcon* aIcon,
    TInt aPriority,
    TBool aStatic,
    TUid aUid )
    {
    FUNC_LOG;

    CFscContactActionMenuItemImpl* self = 
        new (ELeave) CFscContactActionMenuItemImpl( 
            aIcon, aPriority, aStatic, aUid );
    CleanupStack::PushL( self );
    self->ConstructL(aMenuText);
        
    return self;
    }
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::~CFscContactActionMenuItemImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItemImpl::~CFscContactActionMenuItemImpl()
    {
    FUNC_LOG;
    delete iMenuText;
    delete iDimmReason;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetMenuTextL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetMenuTextL( const TDesC& aMenuText )
    {
    FUNC_LOG;
    HBufC* temp = aMenuText.AllocL();
    delete iMenuText;
    iMenuText = temp;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::MenuText
// ---------------------------------------------------------------------------
//
const TDesC& CFscContactActionMenuItemImpl::MenuText() const
    {
    FUNC_LOG;
    return *iMenuText;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetIcon
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetIcon( const CGulIcon* aIcon )
    {
    FUNC_LOG;
    ASSERT( aIcon != NULL );
    iIcon = aIcon;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::Icon
// ---------------------------------------------------------------------------
//
const CGulIcon* CFscContactActionMenuItemImpl::Icon() const
    {
    FUNC_LOG;
    return iIcon;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetPriority
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetPriority( TInt aPriority )
    {
    FUNC_LOG;
    iPriority = aPriority;
    }

// ---------------------------------------------------------------------------
// TInt CFscContactActionMenuItemImpl::Priority
// ---------------------------------------------------------------------------
//
TInt CFscContactActionMenuItemImpl::Priority() const
    {
    FUNC_LOG;
    return iPriority;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetImplementationUid
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetImplementationUid( TUid aUid )
    {
    FUNC_LOG;
    iImplementationUid = aUid;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::ImplementationUid
// ---------------------------------------------------------------------------
//
TUid CFscContactActionMenuItemImpl::ImplementationUid() const
    {
    FUNC_LOG;
    return iImplementationUid;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetDimmed
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetDimmed( TBool aDimmed )
    {
    FUNC_LOG;
    iDimmed = aDimmed;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::Dimmed
// ---------------------------------------------------------------------------
//
TBool CFscContactActionMenuItemImpl::Dimmed() const
    {
    FUNC_LOG;
    return iDimmed;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetDimmReasonL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetDimmReasonL( const TDesC& aDimmReason )
    {
    FUNC_LOG;
    HBufC* temp = aDimmReason.AllocL();
    delete iDimmReason;
    iDimmReason = temp;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::DimmReason
// ---------------------------------------------------------------------------
//
const TDesC& CFscContactActionMenuItemImpl::DimmReason() const
    {
    FUNC_LOG;
    return *iDimmReason;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetHidden
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetHidden( TBool aHidden )
    {
    FUNC_LOG;
    iHidden = aHidden;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::Hidden
// ---------------------------------------------------------------------------
//
TBool CFscContactActionMenuItemImpl::Hidden() const
    {
    FUNC_LOG;
    return iHidden;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::SetStatic
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::SetStatic( TBool aStatic )
    {
    FUNC_LOG;
    iStatic = aStatic;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::Static
// ---------------------------------------------------------------------------
//
TBool CFscContactActionMenuItemImpl::Static() const
    {
    FUNC_LOG;
    return iStatic;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::CFscContactActionMenuItemImpl
// ---------------------------------------------------------------------------
//
CFscContactActionMenuItemImpl::CFscContactActionMenuItemImpl(
    const CGulIcon* aIcon, TInt aPriority, TBool aStatic, TUid aUid ) 
    : iIcon(aIcon), iPriority(aPriority), iImplementationUid(aUid), 
      iDimmed(EFalse), iHidden(EFalse), iStatic(aStatic)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuItemImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuItemImpl::ConstructL( 
    const TDesC& aMenuText )
    {
    FUNC_LOG;
    iMenuText = aMenuText.AllocL();
    iDimmReason = KNullDesC().AllocL();
    }   

