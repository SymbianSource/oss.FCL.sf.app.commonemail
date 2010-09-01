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
* Description: This file implements class CIpsSetUiItem.
*
*/


#include "emailtrace.h"
#include <e32base.h>

#include "ipssetuiitembase.h"
#include "ipssetutilsconsts.h"
#include "ipssetutilsflags.h"

// ----------------------------------------------------------------------------
// CIpsSetUiItem::CIpsSetUiItem()
// ----------------------------------------------------------------------------
CIpsSetUiItem::CIpsSetUiItem()
    :
    iItemType( EIpsSetUiUndefined ),
    iItemLabel( NULL ),
    iItemId( TUid::Uid( 0 ) ),
    iItemResourceId( 0 )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItem::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiItem::ConstructL()
    {
    FUNC_LOG;
    iItemLabel = new ( ELeave ) TIpsSetUtilsTextPlain();
    }



// ----------------------------------------------------------------------------
// CIpsSetUiItem::~CIpsSetUiItem()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem::~CIpsSetUiItem()
    {
    FUNC_LOG;
    delete iItemLabel;
    iItemLabel = NULL;
    }


// ----------------------------------------------------------------------------
// CIpsSetUiItem::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiItem::NewL()
    {
    FUNC_LOG;
    CIpsSetUiItem* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------------------------------
// CIpsSetUiItem::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiItem::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiItem* self = new ( ELeave ) CIpsSetUiItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItem::operator=()
// ----------------------------------------------------------------------------
//
CIpsSetUiItem& CIpsSetUiItem::operator=(
    const CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;

    return *this;
    }

// End of file

