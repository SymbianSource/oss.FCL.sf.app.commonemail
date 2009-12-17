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
* Description:  Implementation of the class CFscContactAction.
*
*/


#include "emailtrace.h"
#include <StringLoader.h>
#include <gulicon.h>

#include "cfsccontactaction.h"
#include "mfscreasoncallback.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscContactAction::NewL
// ---------------------------------------------------------------------------
//
CFscContactAction* CFscContactAction::NewL(
    MFscReasonCallback& aReasonCallback,
    TUid aContactActionUid,
    TUint64 aActionType,
    TInt aActionMenuTextResourceId,
    CGulIcon* aIcon )
    {
    FUNC_LOG;
    CFscContactAction* self = 
        new ( ELeave ) CFscContactAction(
            aReasonCallback,
            aContactActionUid,
            aActionType,
            aIcon );
                                                                
    CleanupStack::PushL( self );

    self->ConstructL( aActionMenuTextResourceId );
    CleanupStack::Pop( self );
        
    return self;
    }

// ---------------------------------------------------------------------------
// CFscContactAction::~CFscContactAction
// ---------------------------------------------------------------------------
//
CFscContactAction::~CFscContactAction()
    {
    FUNC_LOG;
    delete iIcon;
    delete iActionMenuText;
    }
    
// ---------------------------------------------------------------------------
// CFscContactAction::Uid
// ---------------------------------------------------------------------------
//
TUid CFscContactAction::Uid() const 
    {
    FUNC_LOG;
    return iContactActionUid;
    }

// ---------------------------------------------------------------------------
// CFscContactAction::Type
// ---------------------------------------------------------------------------
//
TUint64 CFscContactAction::Type() const
    {
    FUNC_LOG;
    return iActionType;
    }

// ---------------------------------------------------------------------------
// CFscContactAction::Icon
// ---------------------------------------------------------------------------
//
const CGulIcon* CFscContactAction::Icon() const
    {
    FUNC_LOG;
    return iIcon;
    }

// ---------------------------------------------------------------------------
// CFscContactAction::ActionMenuText
// ---------------------------------------------------------------------------
//
const TDesC& CFscContactAction::ActionMenuText() const
    {
    FUNC_LOG;
    return *iActionMenuText;
    }

// ---------------------------------------------------------------------------
// CFscContactAction::ReasonL
// ---------------------------------------------------------------------------
//
void CFscContactAction::ReasonL( TInt aReasonId, HBufC*& aReason ) const
    {
    FUNC_LOG;
    iReasonCallback.GetReasonL( iContactActionUid, aReasonId, aReason );
    }

// ---------------------------------------------------------------------------
// CFscContactAction::SetIcon
// ---------------------------------------------------------------------------
//
void CFscContactAction::SetIcon( CGulIcon* aIcon )
    {
    FUNC_LOG;
    delete iIcon;
    iIcon = NULL;
    iIcon = aIcon;
    }
    
// ---------------------------------------------------------------------------
// CFscContactAction::CFscContactAction
// ---------------------------------------------------------------------------
//
CFscContactAction::CFscContactAction(
    MFscReasonCallback& aReasonCallback,
    TUid aContactActionUid,
    TUint64 aActionType,
    CGulIcon* aIcon )
    : iContactActionUid( aContactActionUid ),
    iActionType( aActionType ),
    iIcon( aIcon ),
    iReasonCallback( aReasonCallback )
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CFscContactAction::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactAction::ConstructL( TInt aActionMenuTextResourceId )
    {
    FUNC_LOG;
    iActionMenuText = StringLoader::LoadL( aActionMenuTextResourceId );
    }
    
// ======== GLOBAL FUNCTIONS ========

