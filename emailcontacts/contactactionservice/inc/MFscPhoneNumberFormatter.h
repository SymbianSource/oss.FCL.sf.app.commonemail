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
* Description:  Phone number formatter interface.
 *
*/


#ifndef MFSCPHONENUMBERFORMATTER_H
#define MFSCPHONENUMBERFORMATTER_H

// CLASS DECLARATION

/**
 * Phone number formatter interface.
 */
class MFscPhoneNumberFormatter
    {
public:
    // Interface

    /**
     * Virtual destructor.
     */
    virtual ~MFscPhoneNumberFormatter()
        {
        }

    /**
     * Sets formatting buffer capacity.
     *
     * @param aMaxLength    Length of the phone number to be formatted.
     */
    virtual void SetMaxBufferLengthL(TInt aMaxLength) = 0;

    /**
     * Formats phone number for display. The formatting is done on
     * best-effort basis. If there is insufficient memory to perform the
     * formatting the original phone number may be returned.
     *
     * @param aOriginalPhoneNumber      The phone number to format.
     * @return  The formatted phone number. The return value is
     *          valid until this function is called again.
     */
    virtual TPtrC FormatPhoneNumberForDisplay(
            const TDesC& aOriginalPhoneNumber) = 0;
    };

#endif // MFSCPHONENUMBERFORMATTER_H
// End of File
