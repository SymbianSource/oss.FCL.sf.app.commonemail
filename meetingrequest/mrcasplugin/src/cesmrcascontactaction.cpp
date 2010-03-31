/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CESMRCasContactAction.
 *
*/


#include "emailtrace.h"
#include "cesmrcascontactaction.h"
#include <stringloader.h>
#include <gulicon.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCasContactAction::NewL
// ---------------------------------------------------------------------------
//
CESMRCasContactAction* CESMRCasContactAction::NewL(
        MFscReasonCallback& aReasonCallback,
        TUid aContactActionUid,
        TUint64 aActionType, TInt aActionMenuTextResourceId, CGulIcon* aIcon )
    {
    FUNC_LOG;

    CESMRCasContactAction* self =new ( ELeave ) CESMRCasContactAction(
            aReasonCallback,
            aContactActionUid,
            aActionType,
            aIcon);

    CleanupStack::PushL ( self );
    self->ConstructL (aActionMenuTextResourceId);
    CleanupStack::Pop ( self );


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::~CESMRCasContactAction
// ---------------------------------------------------------------------------
//
CESMRCasContactAction::~CESMRCasContactAction( )
    {
    FUNC_LOG;

    delete iIcon;
    delete iActionMenuText;

    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::Uid
// ---------------------------------------------------------------------------
//
TUid CESMRCasContactAction::Uid( ) const
    {
    FUNC_LOG;

    return iContactActionUid;
    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::Type
// ---------------------------------------------------------------------------
//
TUint64 CESMRCasContactAction::Type( ) const
    {
    FUNC_LOG;

    return iActionType;
    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::Icon
// ---------------------------------------------------------------------------
//
const CGulIcon* CESMRCasContactAction::Icon( ) const
    {
    FUNC_LOG;

    return iIcon;
    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::ActionMenuText
// ---------------------------------------------------------------------------
//
const TDesC& CESMRCasContactAction::ActionMenuText( ) const
    {
    FUNC_LOG;

    return *iActionMenuText;
    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::ReasonL
// ---------------------------------------------------------------------------
//
void CESMRCasContactAction::ReasonL(TInt aReasonId, HBufC*& aReason ) const
    {
    FUNC_LOG;

    iReasonCallback.GetReasonL( iContactActionUid, aReasonId, aReason );

    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::CESMRCasContactAction
// ---------------------------------------------------------------------------
//
CESMRCasContactAction::CESMRCasContactAction(
        MFscReasonCallback& aReasonCallback,
        TUid aContactActionUid,
        TUint64 aActionType,
        CGulIcon* aIcon )
:   iContactActionUid(aContactActionUid),
    iActionType(aActionType),
    iIcon(aIcon),
    iReasonCallback(aReasonCallback)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCasContactAction::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCasContactAction::ConstructL(TInt aActionMenuTextResourceId)
    {
    FUNC_LOG;

    iActionMenuText = StringLoader::LoadL ( aActionMenuTextResourceId );


    }

// ======== GLOBAL FUNCTIONS ========

