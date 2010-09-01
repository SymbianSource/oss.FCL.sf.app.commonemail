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
* Description:  Decleares signature class.
*
*/


#ifndef IPSSETDATASIGNATURE_H
#define IPSSETDATASIGNATURE_H


class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;

/**
 *  Class to store the signature and its format.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetDataSignature : public CBase
    {
public:

    /**
     * 2-phase construction.
     *
     * @return Signature object.
     */
    static CIpsSetDataSignature* NewL();

    /**
     * Destructor.
     */
    virtual ~CIpsSetDataSignature();

    /**
     * @param aSignature Signature to be copied.
     */
    void CopyL( const CIpsSetDataSignature& aSignature );

    /**
     * Initializes the object.
     */
    void CreateEmptyRichTextL();

private:

    /**
     * 2nd phase of construction.
     *
     * @leave For what ever random reason.
     */
    void ConstructL();

    /**
     * Constructor.
     */
    CIpsSetDataSignature();

    /**
     * Another constructor.
     *
     * @param aRichText Text for initialization.
     * @param aParaFormat Formatting, part I
     * @param aCharFormat Formatting, part II
     */
    CIpsSetDataSignature(
        CRichText* aRichText,
        CParaFormatLayer* aParaFormat,
        CCharFormatLayer* aCharFormat );

public:

    /**
     * Stores the text in rich text format
     * Pwns.
     */
    CRichText* iRichText;

    /**
     * Some layer
     * Pwns.
     */
    CParaFormatLayer* iParaFormatLayer;

    /**
     * Some layer for characters.
     * Pwns.
     */
    CCharFormatLayer* iCharFormatLayer;
    };

#endif /* IPSSETDATASIGNATURE_H */

// End of File
