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
* Description:  Number grouping phone number formatter.
*
*/


#ifndef CFSCNUMBERGROUPINGFORMATTER_H
#define CFSCNUMBERGROUPINGFORMATTER_H

// INCLUDES
#include <e32base.h>
#include "MFscPhoneNumberFormatter.h"

// FORWARD DECLARATIONS
class CPNGNumberGrouping;

// CLASS DECLARATION

/**
 * Phonebook 2 number grouping phone number formatter.
 *
 * Responsible for formatting phone number.
 */
NONSHARABLE_CLASS(CFscNumberGroupingFormatter) :
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
           static CFscNumberGroupingFormatter* NewL(
            TInt aMaxDisplayLength );

        /**
         * Destructor.
         */
        virtual ~CFscNumberGroupingFormatter();

    public: // From MFscPhoneNumberFormatter
        void SetMaxBufferLengthL(
                TInt aMaxLength );
        TPtrC FormatPhoneNumberForDisplay(
                const TDesC& aOriginalPhoneNumber );

    private: // Implementation
        CFscNumberGroupingFormatter();
        void ConstructL(
                TInt aMaxDisplayLength );

    private: // Data
        /// Own: Number grouping object
        CPNGNumberGrouping* iNumberGrouping;
        /// Own: Formatting buffer
        HBufC* iFormatterNumberBuffer;
    };

#endif // CFSCNUMBERGROUPINGFORMATTER_H

// End of File
