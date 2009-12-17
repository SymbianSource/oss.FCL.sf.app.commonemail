/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsSetUiApprover.
*
*/



#include "emailtrace.h"
#include "ipssetutils.h"                // IpsSetUtils
#include "ipssetuiitembase.h"           // CIpsSetUiItem
#include "ipssetutils.h"
#include "ipssetutilspageids.hrh"
#include "ipssetutilsconsts.h"

#include "ipssetuiapprover.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::CIpsSetUiApprover()
// ----------------------------------------------------------------------------
CIpsSetUiApprover::CIpsSetUiApprover()
    :
    iNoteUi( NULL )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiApprover::ConstructL()
    {
    FUNC_LOG;
    iNoteUi = CIpsSetUiNotes::NewL();
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::~CIpsSetUiApprover()
// ----------------------------------------------------------------------------
//
CIpsSetUiApprover::~CIpsSetUiApprover()
    {
    FUNC_LOG;
    delete iNoteUi;
    iNoteUi = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiApprover* CIpsSetUiApprover::NewL()
    {
    FUNC_LOG;
    CIpsSetUiApprover* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiApprover* CIpsSetUiApprover::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiApprover* self = new ( ELeave ) CIpsSetUiApprover();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

/******************************************************************************

    Common item validating

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::ValidateType()
// ----------------------------------------------------------------------------
//
TIpsSetUiNoteErrors CIpsSetUiApprover::ValidateType(
    const CIpsSetUiItem& aBaseItem,
    TDes& aNewText,
    TInt& aNewValue )
    {
    FUNC_LOG;
    // Handle the item, based on the type
    switch ( aBaseItem.Type() )
        {
        // Text editor
        case EIpsSetUiItemText:
            return ValidateText( aBaseItem, aNewText );

        // Number editor
        case EIpsSetUiCheckBoxArray:
        case EIpsSetUiItemValue:
            return ValidateValue( aBaseItem, aNewValue );

        case EIpsSetUiRadioButtonArray:
            return ValidateRadioButtons( aNewValue );

        // Type is not recognized or not supported
        default:
            break;
        }

    return EIpsSetUiNoError;
    }

/******************************************************************************

    Special item validating

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::EvaluateServerAddress()
// ----------------------------------------------------------------------------
//
TIpsSetUiNoteErrors CIpsSetUiApprover::EvaluateServerAddress(
    const CIpsSetUiItem& aBaseItem,
    const TDesC& aNewText )
    {
    FUNC_LOG;
    TIpsSetUiNoteErrors result = ValidateText( aBaseItem, aNewText );

    // Check that dot isn't in start of the server address
    if ( result == EIpsSetUiNoError )
        {
        TBool ok = EFalse;
        TRAPD( error, ok = IpsSetUtils::IsValidDomainL( aNewText ) );

        result = ( error == KErrNone ) && ok ? 
            EIpsSetUiNoError : EIpsSetUiItemInvalid;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::EvaluateEmailAddress()
// ----------------------------------------------------------------------------
//
TIpsSetUiNoteErrors CIpsSetUiApprover::EvaluateEmailAddress(
    const CIpsSetUiItem& aBaseItem,
    const TDesC& aNewText )
    {
    FUNC_LOG;
    // First make basic test
    TIpsSetUiNoteErrors result = ValidateText( aBaseItem, aNewText );

    // If text check was successful and the address is given, validate the
    // email address
    if ( result == EIpsSetUiNoError && aNewText.Length() )
        {
        // Trap any errors to prevent leaving
        TBool ok = EFalse;
        TRAPD( error, ok =
            IpsSetUtils::IsValidEmailAddressL( aNewText ) );

        result = ( ok && error == KErrNone ) ? 
            EIpsSetUiNoError : EIpsSetUiItemInvalid;
        }

    return result;
    }

/******************************************************************************

    Brand new validating code

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::EvaluateSettingItems()
// ----------------------------------------------------------------------------
//
TIpsSetUiNoteErrors CIpsSetUiApprover::EvaluateSettingItems(
    const CIpsSetUiItem& aBaseItem,
    TDes& aNewText,
    TInt& aNewValue )
    {
    FUNC_LOG;
    // Incoming settings
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIpsSetUiIncomingMailServer:
        case EIpsSetUiOutgoingMailServer:
            return EvaluateServerAddress( aBaseItem, aNewText );

        case EIpsSetUiMailboxReplyToAddress:
        case EIpsSetUiMailboxEmailAddress:
            return EvaluateEmailAddress( aBaseItem, aNewText );

        default:
            break;
        }

    return ValidateType( aBaseItem, aNewText, aNewValue );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::EvaluateItem()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiApprover::EvaluateItem(
    const CIpsSetUiItem& aBaseItem,
    TDes& aNewText,
    TInt& aNewValue )
    {
    FUNC_LOG;
    TIpsSetUiNoteErrors error = 
        EvaluateSettingItems( aBaseItem, aNewText, aNewValue );

    return iNoteUi->ShowDialog( aBaseItem, error, aNewText );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::EvaluateText()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiApprover::EvaluateText(
    const CIpsSetUiItem& aBaseItem,
    TDes& aNewText )
    {
    FUNC_LOG;
    TInt nothing = 0;
    return EvaluateItem( aBaseItem, aNewText, nothing );
    }

 // ----------------------------------------------------------------------------
// CIpsSetUiApprover::EvaluateValue()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiApprover::EvaluateValue(
    const CIpsSetUiItem& aBaseItem,
    TInt& aNewValue )
    {
    FUNC_LOG;
    TIpsSetUtilsTextPlain nothing;
    return EvaluateItem( aBaseItem, nothing, aNewValue );
    }

/******************************************************************************

    Common code

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::IsItemFilled()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiApprover::IsItemFilled(
    const CIpsSetUiItem& aBaseItem,
    const TInt aLength )
    {
    FUNC_LOG;
    // At first, check if the item can be left empty
    TBool ok =
        !( aBaseItem.iItemFlags.Flag( KIpsSetUiFlagMustFill ) ||
           aBaseItem.iItemFlags.Flag( KIpsSetUiFlagForceMustFill ) );

    // The text field is considered filled, when:
    // A: Item has NO must fill -flags on
    // B: Item has must fill -flags on and length is higher than 0
    return ok || ( !ok && aLength > 0 );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::ValidateRadioButton()
// ----------------------------------------------------------------------------
//
TIpsSetUiNoteErrors CIpsSetUiApprover::ValidateRadioButtons(
    const TInt& aNewValue )
    {
    FUNC_LOG;
    // Make sure item is checked
    return ( aNewValue != KErrNotFound ) ? EIpsSetUiNoError : EIpsSetUiItemEmpty;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::ValidateValue()
// ----------------------------------------------------------------------------
//
TIpsSetUiNoteErrors CIpsSetUiApprover::ValidateValue(
    const CIpsSetUiItem& aBaseItem,
    TInt& aNewValue )
    {
    FUNC_LOG;
    // This check makes sure, that the provided value is larger than 0
    return IsItemFilled( aBaseItem, aNewValue ) ?
        EIpsSetUiNoError : EIpsSetUiItemInvalid;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiApprover::ValidateTextL()
// ----------------------------------------------------------------------------
//
TIpsSetUiNoteErrors CIpsSetUiApprover::ValidateText(
    const CIpsSetUiItem& aBaseItem,
    const TDesC& aNewText )
    {
    FUNC_LOG;
    return IsItemFilled( aBaseItem, aNewText.Length() ) ?
        EIpsSetUiNoError : EIpsSetUiItemEmpty;
    }

//  End of File

