/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for single text link
*
*/

#ifndef CESMRRICHTEXTLINK_H
#define CESMRRICHTEXTLINK_H

#include <cntitem.h>

/**
 * Class for single text link object
 */
NONSHARABLE_CLASS( CESMRRichTextLink ): public CBase
    {
public:
    // Enumerations for the higlighted item type
    enum TType
        {
        ETypeEmail,
        ETypePhoneNumber,
        ETypeURL,
        ETypeAttachment,
        ETypeShowAll,
        ETypeLocationUrl
        };

    // Enumerations for the action trigger key
    enum TTriggerKey
        {
        ETriggerKeyRight,
        ETriggerKeyOk
        };

    /**
     * Creates new rich text link instance.
     *
     * @param aStartPos   - Start position of the link
     * @param aLength     - Length of the link
     * @param aValue      - Actual value of the link (e.g. email address)
     * @param aType       - Type of the link
     * @param aTriggerKey - Key that triggers the action
     * @return New rich text link object
     */
    IMPORT_C static CESMRRichTextLink* NewL(
            TInt aStartPos,
            TInt aLength,
            const TDesC& aValue,
            TType aType,
            TTriggerKey aTriggerKey );

    // Destructor
    IMPORT_C ~CESMRRichTextLink( );

    /**
     * Getter for start position
     *
     * @return Start postion of the link
     */
    IMPORT_C TInt StartPos( ) const;

    /**
     * Getter for link length
     *
     * @return Length of the link
     */
    IMPORT_C TInt Length( ) const;

    /**
     * Getter for link actual value
     *
     * @return Value of the link
     */
    IMPORT_C TDesC& Value( ) const;

    /**
     * Getter for link type
     *
     * @return Type of the link
     */
    IMPORT_C TType Type( ) const;

    /**
     * Getter for trigger key
     *
     * @return Key that triggers the event
     */
    IMPORT_C TTriggerKey TriggerKey( ) const;

    /**
     * Setter for start position
     *
     * @param aPos New start position of the link
     */
    IMPORT_C void SetStartPos( TInt aPos );

private: // Implementation
    CESMRRichTextLink(
            TInt aStartPos,
            TInt aLength,
            TType aType,
            TTriggerKey aTriggerKey );
    void ConstructL(const TDesC& aValue );

private:
	/// Own:
    TInt iStartPos;
    /// Own:
	TInt iLength;
    /// Own:
    HBufC* iValue;
    /// Own:
    TType iType;
    /// Own:
    TTriggerKey iTriggerKey;
    };

#endif
