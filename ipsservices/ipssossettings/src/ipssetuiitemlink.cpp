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
* Description:  Implements class with container to setting items.
*
*/


#include "emailtrace.h"
#include <e32base.h>
#include "ipssetutilsexception.h"
#include "ipssetuiitemlink.h"
#include "ipssetutilsconsts.h"
#include "ipssetuibasearray.h"


// ----------------------------------------------------------------------------
// CIpsSetUiItemLink::CIpsSetUiItemLink()
// ----------------------------------------------------------------------------
CIpsSetUiItemLink::CIpsSetUiItemLink()
    :
    iItemAppereance( EIpsSetUiUndefined ),
    iItemLinkArray( NULL ),
    iItemSettingText( NULL )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiItemLink::ConstructL()
    {
    FUNC_LOG;
    CIpsSetUiItem::ConstructL();

    iItemSettingText = new ( ELeave ) TIpsSetUtilsTextPlain();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLink::CIpsSetUiItemLink()
// ----------------------------------------------------------------------------
CIpsSetUiItemLink::~CIpsSetUiItemLink()
    {
    FUNC_LOG;
    // Check if subarray exist
    if ( iItemLinkArray )
        {
        // Check all items in array and delete any subarrays from list
        for ( TInt index = iItemLinkArray->Count()-1; index >= 0; index-- )
            {
            // Only certain type of items can contain subarrays
            CIpsSetUiItem* base = ( *iItemLinkArray )[index];

            delete base;
            base = NULL;
            }
        }

    delete iItemLinkArray;
    iItemLinkArray = NULL;
    delete iItemSettingText;
    iItemSettingText = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLink::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemLink* CIpsSetUiItemLink::NewL()
    {
    FUNC_LOG;
    CIpsSetUiItemLink* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLink::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemLink* CIpsSetUiItemLink::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiItemLink* self =
        new ( ELeave ) CIpsSetUiItemLink();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLink::operator=()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemLink& CIpsSetUiItemLink::operator=(
    const CIpsSetUiItemLink& aBaseItem )
    {
    FUNC_LOG;
    IPS_ASSERT_DEBUG( iItemLabel, EItemText, ENullPointer );
    IPS_ASSERT_DEBUG( aBaseItem.iItemLabel, EItemText, ENullPointer );
    IPS_ASSERT_DEBUG( iItemSettingText, EItemText, ENullPointerInParam );
    IPS_ASSERT_DEBUG( aBaseItem.iItemSettingText, EItemText, ENullPointerInParam );

    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;
    iItemLinkArray = aBaseItem.iItemLinkArray;
    iItemSettingText->Copy( *aBaseItem.iItemSettingText );
    iItemAppereance = aBaseItem.iItemAppereance;

    return *this;
    }

// End of file

