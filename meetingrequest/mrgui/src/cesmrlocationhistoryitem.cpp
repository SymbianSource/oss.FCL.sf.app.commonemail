/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Location history item class implementation
*
*/


#include "emailtrace.h"
#include "cesmrlocationhistoryitem.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::NewL
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryItem* CESMRLocationHistoryItem::NewL(
        const TUint aId,
        const TDesC& aAddress, 
        const TDesC& aUrl )
    {
    FUNC_LOG;
    CESMRLocationHistoryItem* object = CESMRLocationHistoryItem::NewLC( aId, aAddress, aUrl );
    CleanupStack::Pop( object );
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::NewLC
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryItem* CESMRLocationHistoryItem::NewLC(
        const TUint aId,
        const TDesC& aAddress, 
        const TDesC& aUrl )
    {
    FUNC_LOG;
    CESMRLocationHistoryItem* object = new (ELeave) CESMRLocationHistoryItem( aId );
    CleanupStack::PushL( object );
    object->ConstructL( aAddress, aUrl );
    return object;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::~CESMRLocationHistoryItem
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryItem::~CESMRLocationHistoryItem()
    {
    FUNC_LOG;
    delete iAddress;
    delete iUrl;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::::SetAddressL
// -----------------------------------------------------------------------------
//
void CESMRLocationHistoryItem::SetAddressL( const TDesC& aAddress )
    {
    FUNC_LOG;
    HBufC* temp = aAddress.AllocL();
    delete iAddress;
    iAddress = temp;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::SetUrlL
// -----------------------------------------------------------------------------
//
void CESMRLocationHistoryItem::SetUrlL( const TDesC& aUrl )
    {
    FUNC_LOG;
    HBufC* temp = aUrl.AllocL();
    delete iUrl;
    iUrl = temp;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::Address
// -----------------------------------------------------------------------------
//
const TDesC& CESMRLocationHistoryItem::Address() const
    {
    FUNC_LOG;
    return *iAddress;
    }
    
// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::Url
// -----------------------------------------------------------------------------
//
const TDesC& CESMRLocationHistoryItem::Url() const
    {
    FUNC_LOG;
    return *iUrl;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::Id
// -----------------------------------------------------------------------------
//
TUint CESMRLocationHistoryItem::Id() const
    {
    FUNC_LOG;
    return iId;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::CESMRLocationHistoryItem
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryItem::CESMRLocationHistoryItem( const TUint aId ) :
    iId( aId )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CESMRLocationHistoryItem::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRLocationHistoryItem::ConstructL( const TDesC& aAddress, 
                                           const TDesC& aUrl )
    {
    FUNC_LOG;
    iAddress = aAddress.AllocL();
    iUrl = aUrl.AllocL();
    }

// EOF

