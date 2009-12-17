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
* Description:  Address selection parameters.
 *
*/


#ifndef TFSCADDRESSSELECTPARAMS_H
#define TFSCADDRESSSELECTPARAMS_H

// INCLUDES
#include <e32base.h>
#include <barsread.h>
#include <VPbkFieldType.hrh>
#include <RVPbkContactFieldDefaultPriorities.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MPbk2ContactNameFormatter;
class CVPbkContactManager;
class CFscFieldPropertyArray;

// CLASS DECLARATION

/**
 * Address selection parameters.
 * Responsible for carrying the parameters to the address select class.
 */
class TFscAddressSelectParams
    {
public:
    // Construction

    /**
     * Constructor.
     *
     * @param aContact            Contact from where to select
     *                            an address.
     * @param aContactManager     Virtual Phonebook contact manager.
     * @param aNameFormatter      Name formatter.
     * @param aFieldPropertyArray Phonebook 2 field property array.
     * @param aReader             Resource reader pointed to
     *                            FSC_ADDRESS_SELECT structure.
     * @param aTitleResId         Resource id for title text.
     *                            Default = 0. If default, the memory
     *                            entry name is used as a title.
     */
    TFscAddressSelectParams(MVPbkStoreContact& aContact,
            const CVPbkContactManager& aContactManager,
            MPbk2ContactNameFormatter& aNameFormatter,
            const CFscFieldPropertyArray& aFieldPropertyArray,
            TResourceReader& aReader, TInt aTitleResId = 0);

public:
    // Interface

    /**
     * Sets the focused field of the contact. If focused field
     * is a non-empty address field it is returned directly by
     * ExecuteLD. Defaults to NULL.
     *
     * @param aFocusedField       The focused field to set.
     */
    void SetFocusedField(const MVPbkStoreContactField* aFocusedField);

    /**
     * Sets the default to be used in address select. The
     * defaults in the array should be in priority order.
     * If the priorities array is not set, no defaults are used.
     * Focused field overrides default address if both are
     * available.
     *
     * @param aDefaultPriorities  An array of default properties in
     *                            priority order.
     */
    void SetDefaultPriorities(
            const RVPbkContactFieldDefaultPriorities& aDefaultPriorities);
    
    /**
     * Set to ETrue to use default address of the contact directly.
     * Focused field overrides default address if both are
     * available. Default value for this parameter is EFalse.
     *
     * @param aUseDefaultDirectly   Defines should the default address
     *                              be used directly.
     */
    void SetUseDefaultDirectly( TBool aUseDefaultDirectly );
    
public:
    // Data
    /// Ref: Contact where to select an address from
    MVPbkStoreContact& iContact;
    /// Ref: Virtual Phonebook contact manager
    const CVPbkContactManager& iContactManager;
    /// Ref: Contact name formatter.
    MPbk2ContactNameFormatter& iNameFormatter;
    /// Ref: Phonebook 2 field property array.
    const CFscFieldPropertyArray& iFieldPropertyArray;
    /// Ref: Resource reader.
    TResourceReader& iResReader;
    /// Own: Title resource id
    TInt iTitleResId;
    /// Ref: Focused field of the contact.
    const MVPbkStoreContactField* iFocusedField;
    /// Ref: Prioritized array of defaults to use.
    const RVPbkContactFieldDefaultPriorities* iDefaultPriorities;
    /// Own: Indicates should the default address be used directly
    TBool iUseDefaultDirectly;
    /// Own: Indicates whether to include contact name in custom prompt.
    ///      Custom prompt refers to iTitleResId.
    TBool iIncludeContactNameInPrompt;
    /// Own: Indicates are the warnings suppressed
    TBool iSuppressWarnings;

private:
    // Friends
    friend class CFscAddressSelect;
    };

#endif // TFSCADDRESSSELECTPARAMS_H
// End of File
