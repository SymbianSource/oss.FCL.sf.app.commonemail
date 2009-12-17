/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message header URL
*
*/

#include "FreestyleMessageHeaderURL.h"

EXPORT_C CFreestyleMessageHeaderURL* CFreestyleMessageHeaderURL::NewL()
    {
    CFreestyleMessageHeaderURL* self = new (ELeave) CFreestyleMessageHeaderURL;
    return self;
    }

EXPORT_C CFreestyleMessageHeaderURL* CFreestyleMessageHeaderURL::NewL( TDesC* aScheme, 
                                                                       TDesC* aType,
                                                                       TDesC* aItemId)
    {
    CFreestyleMessageHeaderURL* self = new (ELeave) CFreestyleMessageHeaderURL;
    self->SetScheme( aScheme );
    self->SetType( aType );
    self->SetItemId( aItemId );
    return self;
    }

CFreestyleMessageHeaderURL::~CFreestyleMessageHeaderURL()
    {
    Reset();
    }

EXPORT_C TDesC* CFreestyleMessageHeaderURL::Scheme() const
    {
    return iScheme;
    }

EXPORT_C void CFreestyleMessageHeaderURL::SetScheme(TDesC* aScheme)
    {
    if ( iScheme != aScheme )
        {
        delete iScheme;
        iScheme = NULL;
        }
    iScheme = aScheme;
    }

EXPORT_C TDesC* CFreestyleMessageHeaderURL::Type() const
    {
    return iType;
    }


EXPORT_C void CFreestyleMessageHeaderURL::SetType (TDesC* aType)
    {
    if ( iType != aType )
        {
        delete iType;
        iType = NULL;
        }
    iType = aType;
    }

EXPORT_C TDesC* CFreestyleMessageHeaderURL::ItemId() const
    {
    return iItemId;
    }

EXPORT_C void CFreestyleMessageHeaderURL::SetItemId(TDesC* aItemId)
    {
    if ( iItemId != aItemId )
        {
        delete iItemId;
        iItemId = NULL;
        }
    iItemId = aItemId;
    }

EXPORT_C void CFreestyleMessageHeaderURL::InternalizeL(const TDesC& aUrlString)
    {
    Reset();

    TInt index = aUrlString.Find( KURLSchemeSeparator );
    if ( index == KErrNotFound )
        {
        User::Leave( KErrCorrupt );
        }
    else
        {
        HBufC* Scheme = aUrlString.Left( index ).AllocL();
        SetScheme( Scheme );
        
        TPtrC rest = aUrlString.Mid( index + KURLSchemeSeparator().Length() );
        index = rest.Find( KURLTypeSeparator );
        if ( index == KErrNotFound )
            {
            User::Leave( KErrCorrupt );
            }
        else 
            {
            HBufC* type = rest.Left( index ).AllocL();
            SetType( type );
            HBufC* itemId = rest.Mid( index + KURLTypeSeparator().Length() ).AllocL();
            SetItemId( itemId );
            }
        }    
    }

EXPORT_C HBufC* CFreestyleMessageHeaderURL::ExternalizeL()
    {
    ASSERT( iScheme && iType && iItemId );
    
    TInt len = 0;
    len += iScheme->Length() + KURLSchemeSeparator().Length();
    len += iType->Length() + KURLTypeSeparator().Length();
    len += iItemId->Length();
    
    HBufC* url = HBufC::NewL( len );
    TPtr urlPtr = url->Des();
    urlPtr.Append( *iScheme );
    urlPtr.Append( KURLSchemeSeparator );
    urlPtr.Append( *iType );
    urlPtr.Append( KURLTypeSeparator );
    urlPtr.Append( *iItemId );
    
    return url;
    }

EXPORT_C TBool CFreestyleMessageHeaderURL::IsMessageHeaderURL(const TDesC& aUrlString)
    {
    TInt index = aUrlString.Find( KURLSchemeSeparator );
    if (index == KErrNotFound)
        {
        return EFalse;
        }
    else
        {
        return aUrlString.Left( index ).CompareF( KURLSchemeCmail ) == 0;
        }
    }

CFreestyleMessageHeaderURL::CFreestyleMessageHeaderURL()
    :iScheme( NULL ),
    iType( NULL ),
    iItemId( NULL )
    {    
    }

void CFreestyleMessageHeaderURL::Reset()
    {
    delete iScheme;
    iScheme = NULL;
    delete iType;
    iType = NULL;
    delete iItemId;
    iItemId = NULL;
    }
