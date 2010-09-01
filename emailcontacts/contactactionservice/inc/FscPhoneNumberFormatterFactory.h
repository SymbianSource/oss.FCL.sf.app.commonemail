/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Phone number formatter factory.
*
*/


#ifndef FSCPHONENUMBERFORMATTERFACTORY_H
#define FSCPHONENUMBERFORMATTERFACTORY_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MFscPhoneNumberFormatter;

// CLASS DECLARATION

/**
 * Phonebook 2 phone number formatter factory.
 *
 * Responsible for creating appropriate phone number formatter.
 */
NONSHARABLE_CLASS(FscPhoneNumberFormatterFactory)
    {
    public: // Interface

        /**
         * Creates and returns phone number formatter implementation.
         *
         * @param aMaxDisplayLength     Maximum length of a phone number on
         *                              the display. The returned phone
         *                              number formatter may not support
         *                              longer phone numbers.
         * @return  Phone number formatter.
         */
        static MFscPhoneNumberFormatter* CreatePhoneNumberFormatterL(
                TInt aMaxDisplayLength );
    };

#endif // FSCPHONENUMBERFORMATTERFACTORY_H

// End of File
