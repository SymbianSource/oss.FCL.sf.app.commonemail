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
* Description:  Implementation of the MNcsEmailAddressObject
*
*/


//  CLASS HEADER
#include "emailtrace.h"
#include "cesmrncsemailaddressobject.h"

// Unnamed namespace for local definitions
namespace{

// Extra space needed when generating display name and address
const TInt KExtraSpace = 3;
_LIT(KEmptySpace, " ");
_LIT(KDotAndEmptySpace, ", ");

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::NewL
// ---------------------------------------------------------------------------
//
CESMRNcsEmailAddressObject* CESMRNcsEmailAddressObject::NewL( TBool aUserAdded )
    {
    FUNC_LOG;
    CESMRNcsEmailAddressObject* self = new( ELeave ) CESMRNcsEmailAddressObject( aUserAdded );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::NewL
// ---------------------------------------------------------------------------
//
CESMRNcsEmailAddressObject* CESMRNcsEmailAddressObject::NewL(
    const CESMRNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    CESMRNcsEmailAddressObject* self = new( ELeave ) CESMRNcsEmailAddressObject( ETrue );
    CleanupStack::PushL( self );
    self->ConstructL( aAddress );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::NewL
// ---------------------------------------------------------------------------
//
CESMRNcsEmailAddressObject* CESMRNcsEmailAddressObject::NewL(
    const TDesC& aDisplayName,
    const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    CESMRNcsEmailAddressObject* self = new( ELeave ) CESMRNcsEmailAddressObject( ETrue );
    CleanupStack::PushL( self );
    self->ConstructL( aDisplayName, aEmailAddress );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::~CESMRNcsEmailAddressObject
// ---------------------------------------------------------------------------
//
CESMRNcsEmailAddressObject::~CESMRNcsEmailAddressObject()
    {
    FUNC_LOG;
    delete iEmailAddress;
    delete iDisplayName;
    delete iFormattedAddress;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::CESMRNcsEmailAddressObject
// ---------------------------------------------------------------------------
//
CESMRNcsEmailAddressObject::CESMRNcsEmailAddressObject( TBool aUserAdded )
    : iDirtyFlag( ETrue ), iUserAdded( aUserAdded)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::NewL
// ---------------------------------------------------------------------------
//
void CESMRNcsEmailAddressObject::ConstructL()
    {
    FUNC_LOG;
    iEmailAddress = HBufC::NewL( 0 );
    iDisplayName = HBufC::NewL( 0 );
    iFormattedAddress = HBufC::NewL( 0 );
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRNcsEmailAddressObject::ConstructL(
    const CESMRNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    ConstructL( aAddress.DisplayName(), aAddress.EmailAddress() );
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRNcsEmailAddressObject::ConstructL(
    const TDesC& aDisplayName,
    const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    iEmailAddress = aEmailAddress.AllocL();
    iDisplayName = aDisplayName.AllocL();
    iFormattedAddress = HBufC::NewL( 0 );
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::SetEmailAddressL
// ---------------------------------------------------------------------------
//
void CESMRNcsEmailAddressObject::SetEmailAddressL( const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    HBufC* buf = aEmailAddress.AllocL();
    delete iEmailAddress;
    iEmailAddress = buf;

    TPtr ptr = iEmailAddress->Des();
    ptr.Trim();

    iDirtyFlag = ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::SetDisplayNameL
// ---------------------------------------------------------------------------
//
void CESMRNcsEmailAddressObject::SetDisplayNameL(
    const TDesC& aGivenName,
    const TDesC& aFamilyName )
    {
    FUNC_LOG;
    TInt len = aGivenName.Length() + aFamilyName.Length() + KExtraSpace;
    delete iDisplayName;
    iDisplayName = NULL;
    iDisplayName = HBufC::NewL( len );
    TPtr ptr = iDisplayName->Des();
    ptr.Append( aFamilyName );
    ptr.Append( KDotAndEmptySpace );
    ptr.Append( aGivenName );

    iDirtyFlag = ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::SetDisplayNameL
// ---------------------------------------------------------------------------
//
void CESMRNcsEmailAddressObject::SetDisplayNameL( const TDesC& aName )
    {
    FUNC_LOG;
    HBufC* buf = aName.AllocL();
    delete iDisplayName;
    iDisplayName = buf;

    TPtr ptr = iEmailAddress->Des();
    ptr.Trim();

    iDirtyFlag = ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::FormattedAddressL
// ---------------------------------------------------------------------------
//
const TDesC& CESMRNcsEmailAddressObject::FormattedAddressL()
    {
    FUNC_LOG;
    if( iDirtyFlag )
        {
        GenerateFormattedAddressL(); //codescanner::leave
        iDirtyFlag = EFalse;
        }

    return *iFormattedAddress;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::IsIdnaCompliantAddress
// ---------------------------------------------------------------------------
//
TBool CESMRNcsEmailAddressObject::IsIdnaCompliantAddress()
    {
    FUNC_LOG;
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::GenerateFormattedAddressL
// ---------------------------------------------------------------------------
//
void CESMRNcsEmailAddressObject::GenerateFormattedAddressL()
    {
    FUNC_LOG;
    TInt length = 0;

    TInt dispNameLength = iDisplayName->Length();
    TInt emailLength = iEmailAddress->Length();

    if( dispNameLength > 0 )
        {
        length += dispNameLength;
        length += KExtraSpace; // for the delimeters ->
                               // format is "Forname Surname <Forname.Surname(at)dot.com>"
        }

    length += emailLength;

    HBufC* buf = HBufC::NewLC( length );
    TPtr ptr = buf->Des();

    if( dispNameLength > 0 )
        {
        ptr.Append( *iDisplayName );
        ptr.Append( KEmptySpace ); // add one white space
        _LIT(KLeft, "<");
        ptr.Append( KLeft );
        }

    ptr.Append( *iEmailAddress );

    if( dispNameLength > 0 )
        {
        _LIT(KRight, ">");
        ptr.Append( KRight );
        }

    delete iFormattedAddress;
    iFormattedAddress = buf;

    CleanupStack::Pop( buf );
    }

// End of file

