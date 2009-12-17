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
* Description:  Decleares class which check user input.
*
*/


#ifndef IPSSETUIAPPROVER_H
#define IPSSETUIAPPROVER_H


#include "ipssetuinotes.h"

class CIpsSetUi;
class CIpsSetUiItem;

/**
 *  Class to verify that user input is valid
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiApprover : public CBase
    {
public: // Constructors and destructor

    /**
     * Destructor
     */
    ~CIpsSetUiApprover();

    /**
     * 2-phase contructor
     *
     * @return Approver object with client ownership.
     */
    static CIpsSetUiApprover* NewL();

    /**
     * 2-phase contructor
     *
     * @return Approver object with client ownership.
     */
    static CIpsSetUiApprover* NewLC();

public: // New functions

    /**
     * Evaluates new text.
     *
     * @param aBaseItem Item which stores the text.
     * @param aNewText New text.
     */
    TIpsSetUiEventResult EvaluateText(
        const CIpsSetUiItem& aBaseItem,
        TDes& aNewText );

    /**
     * Evaluates new value.
     *
     * @param aBaseItem Item which stores the value.
     * @param aNewValue New value.
     */
    TIpsSetUiEventResult EvaluateValue(
        const CIpsSetUiItem& aBaseItem,
        TInt& aNewValue );

private: // Constructors

    /**
     * Constructor.
     */
    CIpsSetUiApprover();

    /**
     * 2nd phase of construction.
     */
    void ConstructL();

private:  // New functions

    // COMMON ITEM VALIDATION

    /**
     * Validates the item.
     *
     * @param aBaseItem Item which stores the text or value.
     * @param aNewText New text.
     * @param aNewValue New value.
     */
    TIpsSetUiNoteErrors ValidateType(
        const CIpsSetUiItem& aBaseItem,
        TDes& aNewText,
        TInt& aNewValue );

    /**
     * Evaluates the item.
     *
     * @param aBaseItem Item which stores the text or value.
     * @param aNewText New text.
     * @param aNewValue New value.
     */
    TIpsSetUiNoteErrors EvaluateSettingItems(
        const CIpsSetUiItem& aBaseItem,
        TDes& aNewText,
        TInt& aNewValue );

    /**
     * Evaluates the item.
     *
     * @param aBaseItem Item which stores the text or value.
     * @param aNewText New text.
     * @param aNewValue New value.
     */
    TIpsSetUiEventResult EvaluateItem(
        const CIpsSetUiItem& aBaseItem,
        TDes& aNewText,
        TInt& aNewValue );

    /**
     * Checks if the item is filled.
     *
     * @param aBaseItem Item which stores the value or text.
     * @param aLength Length of the item.
     */
    TBool IsItemFilled(
        const CIpsSetUiItem& aBaseItem,
        const TInt aLength );

    /**
     * Validate the text is valid.
     *
     * @param aBaseItem Item which stores the value or text.
     * @param aNewText User input text.
     */
    TIpsSetUiNoteErrors ValidateText(
        const CIpsSetUiItem& aBaseItem,
        const TDesC& aNewText );

    /**
     * Validates the value is correct.
     *
     * @param aBaseItem Item which stores the value or text.
     * @param aNewValue User input value.
     */
    TIpsSetUiNoteErrors ValidateValue(
        const CIpsSetUiItem& aBaseItem,
        TInt& aNewValue );

    /**
     * Validates new radiobutton value.
     *
     * @param aNewValue New checked button.
     */
    TIpsSetUiNoteErrors ValidateRadioButtons(
        const TInt& aNewValue );

    // SPESIFIC ITEM VALIDATION

    /**
     *
     * @param aBaseItem Item which stores the value or text.
     */
    TIpsSetUiNoteErrors EvaluateServerAddress(
        const CIpsSetUiItem& aBaseItem,
        const TDesC& aNewText );

    /**
     *
     * @param aBaseItem Item which stores the value or text.
     */
    TIpsSetUiNoteErrors EvaluateEmailAddress(
        const CIpsSetUiItem& aBaseItem,
        const TDesC& aNewText );

private:    // Data

    /**
     * Class to handle notes in UI
     * Owned.
     */
    CIpsSetUiNotes*     iNoteUi;
    };

#endif // IPSSETUIAPPROVER_H

// End of File
