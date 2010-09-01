/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgDeleteLocal.
*
*/


// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgDeleteLocal::CIpsPlgDeleteLocal
// ----------------------------------------------------------------------------
// 
CIpsPlgDeleteLocal::CIpsPlgDeleteLocal(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus)
    :
    CImCacheManager(aMsvSession, aObserverRequestStatus)
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteLocal::ConstructL
// ----------------------------------------------------------------------------
// 
void CIpsPlgDeleteLocal::ConstructL(
    CMsvEntrySelection& aMessageSelection )
    {
    FUNC_LOG;
    CImCacheManager::ConstructL();
    iMessageSelection = aMessageSelection.CopyL();
    StartL( *iMessageSelection, iObserverRequestStatus );
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteLocal::NewL
// ----------------------------------------------------------------------------
// 
CIpsPlgDeleteLocal* CIpsPlgDeleteLocal::NewL(
    CMsvEntrySelection& aMessageSelection,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus)
    {
    FUNC_LOG;
    CIpsPlgDeleteLocal* self =
        new ( ELeave ) CIpsPlgDeleteLocal(
        aMsvSession,
        aObserverRequestStatus);

    CleanupStack::PushL( self );
    self->ConstructL( aMessageSelection );
    CleanupStack::Pop( self );

    return self;
    };

// ----------------------------------------------------------------------------
// CIpsPlgDeleteLocal::~CIpsPlgDeleteLocal
// ----------------------------------------------------------------------------
// 
CIpsPlgDeleteLocal::~CIpsPlgDeleteLocal()
    {
    FUNC_LOG;
    delete iMessageSelection;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteLocal::Filter
// ----------------------------------------------------------------------------
// 
TBool CIpsPlgDeleteLocal::Filter() const
    {
    FUNC_LOG;
    if ( iMessageSelection->Find( iCurrentEntry->Entry().Id() )
        > KErrNotFound )
        {
        return ETrue;
        }
    return EFalse;
    }

//  End of File


