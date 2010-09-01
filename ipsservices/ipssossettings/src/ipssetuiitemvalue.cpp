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
* Description: This file implements class CIpsSetUiItemValue.
*
*/



#include "emailtrace.h"
#include "ipssetuiitemvalue.h"
#include "ipssetutilsconsts.h"
#include "ipssetutilsexception.h"


// ----------------------------------------------------------------------------
// CIpsSetUiItemValue::CIpsSetUiItemValue()
// ----------------------------------------------------------------------------
CIpsSetUiItemValue::CIpsSetUiItemValue()
    :
    iUserValue( KErrNotFound ),
    iItemMaxLength( KIpsSetUiMaxSettingsTextLength ),
    iItemSettingText( 0 )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemValue::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiItemValue::ConstructL()
    {
    FUNC_LOG;
    CIpsSetUiItem::ConstructL();

    iItemSettingText = new ( ELeave ) TIpsSetUtilsTextPlain();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemValue::CIpsSetUiItemValue()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemValue::~CIpsSetUiItemValue()
    {
    FUNC_LOG;
    delete iItemSettingText;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemValue::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemValue* CIpsSetUiItemValue::NewL()
    {
    FUNC_LOG;
    CIpsSetUiItemValue* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemValue* CIpsSetUiItemValue::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiItemValue* self =
        new ( ELeave ) CIpsSetUiItemValue();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemValue::operator=()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemValue& CIpsSetUiItemValue::operator=(
    const CIpsSetUiItemValue& aBaseItem )
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
    iItemMaxLength = aBaseItem.iItemMaxLength;
    iItemSettingText->Copy( *aBaseItem.iItemSettingText );
    iUserValue = aBaseItem.Value();

    return *this;
    }

// End of file

