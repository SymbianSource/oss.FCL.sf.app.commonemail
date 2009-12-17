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
* Description:  Implementation of the class PbkxRclUtils.
*
*/


#include "emailtrace.h"
#include <centralrepository.h>
#include <CPbkContactEngine.h>
#include <CPbkFieldsInfo.h>
#include <CPbkFieldInfo.h>
#include <AknUtils.h>

#include "pbkxrclutils.h"
#include "pbkxrclsettings.hrh"
#include "pbkxrclengineconstants.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// PbkxRclUtils::FieldTextL
// ---------------------------------------------------------------------------
//
HBufC* PbkxRclUtils::FieldTextL( const CPbkContactItem* aItem, TPbkFieldId aId )
    {
    FUNC_LOG;

    TPbkContactItemField* field = aItem->FindField( aId );
    if ( field != NULL )
        {
        return PbkxRclUtils::FieldTextL( *field );
        }
    return KNullDesC().AllocL();
    }

// ---------------------------------------------------------------------------
// PbkxRclUtils::FieldTextL
// ---------------------------------------------------------------------------
//
HBufC* PbkxRclUtils::FieldTextL( TPbkContactItemField& aField )
    {
    FUNC_LOG;
    if ( aField.StorageType() == KStorageTypeText )
        {
        TPtrC text = aField.Text();
        HBufC* textBuffer = HBufC::NewL( text.Length() );
        textBuffer->Des() = text;
        return textBuffer;
        }
    else if ( aField.StorageType() == KStorageTypeDateTime )
        {
        HBufC* textBuffer = HBufC::NewL( 50 );
        TPtr des = textBuffer->Des();
        TTime time = aField.Time();
        time.FormatL( des, _L("%D%M%Y%/0%1%/1%2%/2%3%/3") );
        return textBuffer;
        } 
    else
        {
        return KNullDesC().AllocL();
        }
    }

// ---------------------------------------------------------------------------
// PbkxRclUtils::FieldText
// ---------------------------------------------------------------------------
//
TPtrC PbkxRclUtils::FieldText( const CContactCard* aCard, TFieldType aFieldType )
    {
    FUNC_LOG;
    CContactItemFieldSet& fields = aCard->CardFields();
    
    TInt index = fields.Find( aFieldType );
    if ( index != KErrNotFound )
        {
        const CContactItemField& field = fields[index];
        if ( field.StorageType() == KStorageTypeText )
            {
            return field.TextStorage()->Text();
            }
        }
    TPtrC empty;
    return empty;
    }

// ---------------------------------------------------------------------------
// PbkxRclUtils::DefaultProtocolAccountIdL
// ---------------------------------------------------------------------------
//
TPbkxRemoteContactLookupProtocolAccountId PbkxRclUtils::DefaultProtocolAccountIdL()
    {
    FUNC_LOG;
    CRepository *cr = CRepository::NewLC( TUid::Uid( KPbkxRclCrUid ) );
       
    TInt protocolUid( 0 );
    User::LeaveIfError( cr->Get( KPbkxRclProtocolUidKey, protocolUid ) );
       
    TInt accountId( 0 );
    User::LeaveIfError( cr->Get( KPbkxRclAccountUidKey, accountId ) );
       
    CleanupStack::PopAndDestroy( cr );

    TPbkxRemoteContactLookupProtocolAccountId account(
        TUid::Uid( protocolUid ), 
        accountId );
        
    return account;
    }

// ---------------------------------------------------------------------------
// PbkxRclUtils::StoreDefaultProtocolAccountIdL
// ---------------------------------------------------------------------------
//
void PbkxRclUtils::StoreDefaultProtocolAccountIdL(
    TPbkxRemoteContactLookupProtocolAccountId aId )
    {
    FUNC_LOG;
    CRepository* cr = CRepository::NewLC( TUid::Uid( KPbkxRclCrUid ) );
    TInt uidValue = aId.iProtocolUid.iUid;
    TInt accountValue = ( TInt )aId.iAccountId;
    
    User::LeaveIfError( cr->Set( KPbkxRclProtocolUidKey, uidValue ) );
    User::LeaveIfError( cr->Set( KPbkxRclAccountUidKey, accountValue ) );
    
    CleanupStack::PopAndDestroy( cr );
    }

// ---------------------------------------------------------------------------
// PbkxRclUtils::IsAlphaString
// ---------------------------------------------------------------------------
//
TBool PbkxRclUtils::HasAlphaCharsInString( const TDesC& aText )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < aText.Length(); i++ )
        {
        TChar c( aText[i] );
        if ( c.IsAlpha() )
            {
            return ETrue;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// PbkxRclUtils::CreateContactItemL
// ---------------------------------------------------------------------------
//
CPbkContactItem* PbkxRclUtils::CreateContactItemL(
    CContactCard* aCard,
    CPbkContactEngine& aEngine )
    {
    FUNC_LOG;
    CContactCard* copy = CContactCard::NewLC( aCard );

    CPbkContactItem* contactItem = CPbkContactItem::NewL(
        copy,
        aEngine.FieldsInfo(),
        aEngine.ContactNameFormat() );
    
    CleanupStack::Pop( copy );

    return contactItem;
    }

