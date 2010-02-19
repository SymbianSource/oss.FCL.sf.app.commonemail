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

#ifndef __CFREESTYLE_MESSAGE_HEADER_URL_FACTORY_H__
#define __CFREESTYLE_MESSAGE_HEADER_URL_FACTORY_H__

#include <e32base.h>
#include "FreestyleMessageHeaderURL.h"

class CFSMailAddress;

class FreestyleMessageHeaderURLFactory
    {
public:
    enum TEmailAddressType
        {
        EEmailAddressTypeFrom,
        EEmailAddressTypeTo,
        EEmailAddressTypeCc,
        EEmailAddressTypeBcc
        };
public:
    IMPORT_C static CFreestyleMessageHeaderURL* CreateEmailAddressUrlL( TEmailAddressType aEmailType, const CFSMailAddress& aEmailAddress );
    IMPORT_C static CFreestyleMessageHeaderURL* CreateAttachmentUrlL( const TDesC& aAttachmentItemId );
    IMPORT_C static CFreestyleMessageHeaderURL* CreateEmailSubjectUrlL( const TDesC& aSubject );
    };

#endif //__CFREESTYLE_MESSAGE_HEADER_URL_FACTORY_H__

