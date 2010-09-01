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
* Description:  Interface to be used by field listbox model for analyzing
 *                contact field instances. 
 *
*/


#ifndef MFSCFIELDANALYZER_H
#define MFSCFIELDANALYZER_H

//  FORWARD DECLARATIONS
class MVPbkStoreContactField;
class CFscPresentationContactField;

// CLASS DECLARATION

/**
 * @internal Only Phonebook 2 internal use supported!
 */
class MFscFieldAnalyzer
    {
public:
    // Interface
    /**
     * Analyses if the field has a speed dial definition.
     *
     * @param aField    The field to analyze.
     * @return ETrue if field has speed dial definition, otherwise EFalse.
     */
    virtual TBool
            HasSpeedDialL(const MVPbkStoreContactField& aField) const = 0;

    /**
     * Analyses if the field has a voice tag.
     *
     * @param aField    The field to analyze.
     * @return ETrue if field has voice tag definition, otherwise EFalse.
     */
    virtual TBool HasVoiceTagL(const MVPbkStoreContactField& aField) const = 0;

    /**
     * Analyses is the field hidden, ie. should it not be shown in UI.
     *
     * @param aField    The field to analyze.
     * @return ETrue if field is hidden, EFalse otherwise.
     */
    virtual TBool
            IsHiddenField(const CFscPresentationContactField& aField) const = 0;

    };

#endif // MFSCFIELDANALYZER_H
// End of File
