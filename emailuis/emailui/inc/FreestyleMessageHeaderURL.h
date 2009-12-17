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

#ifndef __CFREESTYLE_MESSAGE_HEADER_URL_H__
#define __CFREESTYLE_MESSAGE_HEADER_URL_H__

#include <e32base.h>

_LIT( KURLSchemeCmail, "cmail" );
_LIT( KURLSchemeSeparator, "://" );
_LIT( KURLTypeFrom, "from" );
_LIT( KURLTypeTo, "to" );
_LIT( KURLTypeCc, "cc" );
_LIT( KURLTypeBcc, "bcc" );
_LIT( KURLTypeAttachment, "attachment" );
_LIT( KURLTypeSeparator, "/" );

class CFreestyleMessageHeaderURL : public CBase
    {
public:
    IMPORT_C static CFreestyleMessageHeaderURL* NewL();
    IMPORT_C static CFreestyleMessageHeaderURL* NewL( TDesC* aScheme, 
                                                      TDesC* aType, 
                                                      TDesC* aItemId );
    virtual ~CFreestyleMessageHeaderURL();
    
    IMPORT_C TDesC* Scheme() const;
    IMPORT_C void SetScheme(TDesC* aScheme);
    
    IMPORT_C TDesC* Type() const;
    IMPORT_C void SetType (TDesC* aType);
    
    IMPORT_C TDesC* ItemId() const;
    IMPORT_C void SetItemId(TDesC* aItemId);
    
    IMPORT_C void InternalizeL(const TDesC& aUrlString);
    IMPORT_C HBufC* ExternalizeL();
    
    IMPORT_C static TBool IsMessageHeaderURL( const TDesC& aUrlString );
    
protected:
    CFreestyleMessageHeaderURL();
    void Reset();
    
private:
    TDesC* iScheme;
    TDesC* iType;
    TDesC* iItemId;
    };

#endif //__CFREESTYLE_MESSAGE_HEADER_URL_H__
