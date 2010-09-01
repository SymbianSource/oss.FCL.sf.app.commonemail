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
* Description:  Default phone number formatter.
*
*/


#ifndef CFSCDEFAULTPHONENUMBERFORMATTER_H
#define CFSCDEFAULTPHONENUMBERFORMATTER_H

// INCLUDES
#include <e32base.h>
#include "MFscPhoneNumberFormatter.h"

// CLASS DECLARATION

/**
 * Default phone number formatter.
 *
 * Responsible for formatting phone number.
 */
NONSHARABLE_CLASS(CFscDefaultPhoneNumberFormatter) :
        public CBase,
        public MFscPhoneNumberFormatter
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aMaxDisplayLength     Maximum phone number display length.
         * @return  A new instance of this class.
         */
        static CFscDefaultPhoneNumberFormatter* NewL(
                TInt aMaxDisplayLength );

        /**
         * Destructor
         */
        virtual ~CFscDefaultPhoneNumberFormatter();

    public: // From MFscPhoneNumberFormatter
        void SetMaxBufferLengthL(
                TInt aMaxLength );
        TPtrC FormatPhoneNumberForDisplay(
                const TDesC& aOriginalPhoneNumber );

    private: // Implementation
        CFscDefaultPhoneNumberFormatter();
        void ConstructL(
            TInt aMaxDisplayLength );

    private: // Data
        /// Own: Formatting buffer
        HBufC* iFormatterNumberBuffer;
    };

#endif // CFSCDEFAULTPHONENUMBERFORMATTER_H

// End of File
