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
* Description: This file implements class CIpsSetUiItemsEditText.
*
*/


#include "emailtrace.h"
#include <e32base.h>

#include "ipssetuiitemtext.h"
#include "ipssetutilsconsts.h"
#include "ipssetutilsexception.h"

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::CIpsSetUiItemsEditText()
// ----------------------------------------------------------------------------
CIpsSetUiItemsEditText::CIpsSetUiItemsEditText()
    :
    iUserText( NULL ),
    iItemMaxLength( KIpsSetUiMaxSettingsTextLength )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiItemsEditText::ConstructL()
    {
    FUNC_LOG;
    CIpsSetUiItem::ConstructL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::CIpsSetUiItemsEditText()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemsEditText::~CIpsSetUiItemsEditText()
    {
    FUNC_LOG;
    iUserText.Close();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemsEditText* CIpsSetUiItemsEditText::NewL()
    {
    FUNC_LOG;
    CIpsSetUiItemsEditText* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemsEditText* CIpsSetUiItemsEditText::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiItemsEditText* self =
        new ( ELeave ) CIpsSetUiItemsEditText();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiItemsEditText::operator=()
// ----------------------------------------------------------------------------
//
CIpsSetUiItemsEditText& CIpsSetUiItemsEditText::operator=(
    const CIpsSetUiItemsEditText& aBaseItem )
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
    iUserText.Close();
    iUserText.Create( aBaseItem.iUserText );

    return *this;
    }

// End of file

