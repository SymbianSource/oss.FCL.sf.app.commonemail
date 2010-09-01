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
* Description:  Implements class with user data and container for setting 
*                 items.
*
*/


#include "emailtrace.h"
#include <e32base.h>

#include "ipssetutilsconsts.h"
#include "ipssetutilsexception.h"
#include "ipssetuiitemextlink.h"


// ----------------------------------------------------------------------------
// CIpsSetUiItemLinkExt::CIpsSetUiItemLinkExt()
// ----------------------------------------------------------------------------
CIpsSetUiItemLinkExt::CIpsSetUiItemLinkExt()
    :
    iUserValue( KErrNotFound )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLinkExt::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiItemLinkExt::ConstructL()
    {
    FUNC_LOG;
    CIpsSetUiItemLink::ConstructL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLinkExt::CIpsSetUiItemLinkExt()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemLinkExt::~CIpsSetUiItemLinkExt()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLinkExt::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemLinkExt* CIpsSetUiItemLinkExt::NewL()
    {
    FUNC_LOG;
    CIpsSetUiItemLinkExt* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemLinkExt* CIpsSetUiItemLinkExt::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiItemLinkExt* self =
        new ( ELeave ) CIpsSetUiItemLinkExt();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemLinkExt::operator=()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemLinkExt& CIpsSetUiItemLinkExt::operator=(
    const CIpsSetUiItemLinkExt& aBaseItem )
    {
    FUNC_LOG;
    IPS_ASSERT_DEBUG( iItemLabel, ENullPointer, EItemText );
    IPS_ASSERT_DEBUG( aBaseItem.iItemLabel, ENullPointerInParam, EItemText );
    IPS_ASSERT_DEBUG( iUserText, ENullPointer, EItemText );
    IPS_ASSERT_DEBUG( aBaseItem.iUserText, ENullPointerInParam, EItemText );

    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;
    iItemLinkArray = aBaseItem.iItemLinkArray;
    iItemSettingText->Copy( *aBaseItem.iItemSettingText );
    iItemAppereance = aBaseItem.iItemAppereance;
    iUserValue = aBaseItem.Value();

    return *this;
    }

// End of file

