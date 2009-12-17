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
* Description:  Message header URL Factory
*
*/

#include "FreestyleMessageHeaderURLFactory.h"
#include "CFSMailAddress.h"

EXPORT_C CFreestyleMessageHeaderURL* FreestyleMessageHeaderURLFactory::CreateEmailAddressUrlL( TEmailAddressType aEmailType, 
        const CFSMailAddress& aEmailAddress )
    {
    HBufC* email = aEmailAddress.GetEmailAddress().AllocLC();
    HBufC* scheme = KURLSchemeCmail().AllocLC();
    HBufC* type = NULL;
    switch ( aEmailType )
        {
        case FreestyleMessageHeaderURLFactory::EEmailAddressTypeFrom:
            type = KURLTypeFrom().AllocLC();
            break;
            
        case FreestyleMessageHeaderURLFactory::EEmailAddressTypeTo:
            type = KURLTypeTo().AllocLC();
            break;
            
        case FreestyleMessageHeaderURLFactory::EEmailAddressTypeCc:
            type = KURLTypeCc().AllocLC();
            break;
            
        case FreestyleMessageHeaderURLFactory::EEmailAddressTypeBcc:
            type = KURLTypeBcc().AllocLC();
            break;
            
        default:
            User::Leave( KErrNotSupported );
        }
    
    CFreestyleMessageHeaderURL* url = CFreestyleMessageHeaderURL::NewL( scheme, type, email );
    
    CleanupStack::Pop( type );
    CleanupStack::Pop( scheme );
    CleanupStack::Pop( email );
    return url;
    }

EXPORT_C CFreestyleMessageHeaderURL* FreestyleMessageHeaderURLFactory::CreateAttachmentUrlL( const TDesC& aAttachmentItemId )
    {
    HBufC* scheme = KURLSchemeCmail().AllocLC();
    HBufC* type = KURLTypeAttachment().AllocLC();
    HBufC* itemId = aAttachmentItemId.AllocLC();
    CFreestyleMessageHeaderURL *url = CFreestyleMessageHeaderURL::NewL( scheme, type, itemId );
    CleanupStack::Pop( itemId );
    CleanupStack::Pop( type );
    CleanupStack::Pop( scheme );
    
    return url;
    }


