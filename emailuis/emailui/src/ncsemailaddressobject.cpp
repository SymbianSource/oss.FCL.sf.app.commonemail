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
* Description:  Implementation of the MNcsEmailAddressObject
*
*/


// INCLUDES
#include "emailtrace.h"
#include "ncsemailaddressobject.h"
#include "ncsconstants.h"


//  CONSTRUCTION AND DESTRUCTION

CNcsEmailAddressObject* CNcsEmailAddressObject::NewL( TBool aUserAdded )
    {
    FUNC_LOG;
    CNcsEmailAddressObject* self = new( ELeave ) CNcsEmailAddressObject( aUserAdded );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcsEmailAddressObject* CNcsEmailAddressObject::NewL( 
    const CNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    CNcsEmailAddressObject* self = new( ELeave ) CNcsEmailAddressObject( aAddress );
    CleanupStack::PushL( self );
    self->ConstructL( aAddress );
    CleanupStack::Pop( self );
    return self;
    }

CNcsEmailAddressObject* CNcsEmailAddressObject::NewL( 
    const TDesC& aDisplayName, 
    const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    CNcsEmailAddressObject* self = new( ELeave ) CNcsEmailAddressObject( ETrue );
    CleanupStack::PushL( self );
    self->ConstructL( aDisplayName, aEmailAddress );
    CleanupStack::Pop( self );
    return self;
    }

CNcsEmailAddressObject::~CNcsEmailAddressObject()
    {
    FUNC_LOG;
    delete iEmailAddress;
    delete iDisplayName;
    delete iFormattedAddress;
    } 

CNcsEmailAddressObject::CNcsEmailAddressObject( TBool aUserAdded )
	: iDirtyFlag( ETrue ), 
	  iUserAdded( aUserAdded ), 
	  iDisplayFull( EFalse )
    {
    FUNC_LOG;
    }

CNcsEmailAddressObject::CNcsEmailAddressObject( const CNcsEmailAddressObject& aAddress )
    : iDirtyFlag( ETrue ), 
      iUserAdded( aAddress.IsUserAdded() ), 
      iDisplayFull( aAddress.DisplayFull() )
    {
    FUNC_LOG;
    }

void CNcsEmailAddressObject::ConstructL()
    {
    FUNC_LOG;
    iEmailAddress = HBufC::NewL( 0 );
    iDisplayName = HBufC::NewL( 0 );
    iFormattedAddress = HBufC::NewL( 0 );
    }

void CNcsEmailAddressObject::ConstructL(
    const CNcsEmailAddressObject& aAddress )
    {
    FUNC_LOG;
    ConstructL( aAddress.DisplayName(), aAddress.EmailAddress() );
    }

void CNcsEmailAddressObject::ConstructL(
    const TDesC& aDisplayName, 
    const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    iEmailAddress = aEmailAddress.AllocL();
    TPtr ptr = iEmailAddress->Des();
	ptr.Trim();
	
    iDisplayName = aDisplayName.AllocL();
    
    iFormattedAddress = HBufC::NewL( 0 );
    }


// METHODS

void CNcsEmailAddressObject::SetEmailAddressL( const TDesC& aEmailAddress )
	{
    FUNC_LOG;
	HBufC* buf = aEmailAddress.AllocL();
	delete iEmailAddress;
	iEmailAddress = buf;
	
	TPtr ptr = iEmailAddress->Des();
	ptr.Trim();
	
	iDirtyFlag = ETrue;
	}

void CNcsEmailAddressObject::SetDisplayNameL( 
    const TDesC& aGivenName,
    const TDesC& aFamilyName )
    {
    FUNC_LOG;
	TInt len = aGivenName.Length() + aFamilyName.Length() + KLastNameFirstNameSeparator().Length();
	delete iDisplayName;
	iDisplayName = NULL;
	iDisplayName = HBufC::NewL( len );
	TPtr ptr = iDisplayName->Des();
	ptr.Append( aFamilyName );
	ptr.Append( KLastNameFirstNameSeparator );
	ptr.Append( aGivenName );
	
	iDirtyFlag = ETrue;
    }

void CNcsEmailAddressObject::SetDisplayNameL( const TDesC& aName )
	{
    FUNC_LOG;
	HBufC* buf = aName.AllocL();
	delete iDisplayName;
	iDisplayName = buf;
	
	TPtr ptr = iEmailAddress->Des();
	ptr.Trim();	
	
	iDirtyFlag = ETrue;
	}

const TDesC& CNcsEmailAddressObject::FormattedAddressL()
	{
    FUNC_LOG;
	if( iDirtyFlag )
		{
		GenerateFormattedAddressL();
		iDirtyFlag = EFalse;
		}
		
	return *iFormattedAddress;
	}

void CNcsEmailAddressObject::GenerateFormattedAddressL()
	{
    FUNC_LOG;
	TInt length = 0;
	HBufC* buf = NULL;
	
	TInt dispNameLength = iDisplayName->Length();
	TInt emailLength = iEmailAddress->Length();
	
	if( dispNameLength > 0 )
		{
                               // format is "Name Lastname <name.lastnamemiumaudot.com>"
        length = dispNameLength + emailLength + 
            KSpace().Length() + 
            KEmailAddressDecorationHead().Length() +
            KEmailAddressDecorationTail().Length() +
            KEmailAddressSeparator().Length();
	
        buf = HBufC::NewL( length );
	TPtr ptr = buf->Des();
	
		ptr.Append( *iDisplayName );
        ptr.Append( KSpace );
        ptr.Append( KEmailAddressDecorationHead );
	ptr.Append( *iEmailAddress );
        ptr.Append( KEmailAddressDecorationTail );
        ptr.Append( KEmailAddressSeparator );
		}
	else
		{
	    buf = iEmailAddress->AllocL();
		}
		
	delete iFormattedAddress;
	iFormattedAddress = buf;
	}

// End of file

