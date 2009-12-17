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
* Description:  Declaration of the class CFscAddressSelect.
 *
*/


#ifndef CFSCADDRESSSELECT_H
#define CFSCADDRESSSELECT_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>   // TKeyResponse
#include <w32std.h>   // TKeyEvent, TEventCode
#include "MFscDialogEliminator.h"

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class CVPbkFieldTypeSelector;
class CVPbkFieldFilter;
class CFscSelectFieldDlg;
class TFscAddressSelectParams;
class MVPbkContactAttributeManager;

// CLASS DECLARATION

/**
 * Address selection.
 * Responsible for selecting an appropriate contact field
 * from given contact. The selection is based on the
 * given parameters.
 */
class CFscAddressSelect : public CBase, public MFscDialogEliminator
    {
public:
    // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @param aParams   Address select parameters.
     * @return  A new instance of this class.
     */
    static CFscAddressSelect* NewL(
            TFscAddressSelectParams& aParams );

    /**
     * Destructor.
     * Cancels address selection query if active.
     */
    ~CFscAddressSelect();

public: // Interface

    /**
     * Runs an address selection query (if necessary).
     * Note! The caller is responsible for the contact field
     * object returned.
     *
     * @return  NULL if query was cancelled, selected field if
     *          query was accepted. Caller takes the ownership
     *          of the returned field.
     */
    MVPbkStoreContactField* ExecuteLD();

    /**
     * Exit any address selection query. Makes ExecuteLD return
     * which means this object is destroyed after this function returns!
     *
     * @param aAccept   ETrue means the query selection is accepted and
     *                  EFalse that query is canceled.
     */
    void AttemptExitL(TBool aAccept );

    /**
     * Override to handle key events in the list query.
     *
     * @param aKeyEvent     Key event.
     * @param aType         Key event type.
     * @return  EKeyWasConsumed if the key event was consumed,
     *          EKeyWasNotConsumed otherwise.
     */
    virtual TKeyResponse FscControlKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

public: // From MFscDialogEliminator
    void RequestExitL(
            TInt aCommandId );
    void ForceExit();
    void ResetWhenDestroyed(
            MFscDialogEliminator** aSelfPtr );

private: // Implementation
    CFscAddressSelect(
            TFscAddressSelectParams& aParams );
    void ConstructL();
    TBool AddressField(
            const MVPbkStoreContactField& aField ) const;
    void NoAddressesL(
            TFscAddressSelectParams aParams ) const;
    void SelectFieldL();
    void SelectFromApplicableFieldsL();
    TInt IndexOfDefaultFieldL();
    TBool SelectFromDefaultFieldsL();
    MVPbkStoreContactField* FindDefaultFieldLC();
    void SetSelectedFieldL(
            const MVPbkStoreContactField* aField );

private: // Friends
    /// Field selection dialog for this class
    class CSelectFieldDlg;
    friend class CSelectFieldDlg;

private: // Data
    /// Ref: Address select parameters
    TFscAddressSelectParams& iParams;
    /// Own: Field type selector
    CVPbkFieldTypeSelector* iFieldTypeSelector;
    /// Own: Field filter
    CVPbkFieldFilter* iFieldFilter;
    /// Own: Field selection dialog
    CFscSelectFieldDlg* iFieldDlg;
    /// Ref: Set to ETrue when the execution enters destructor
    TBool* iDestroyedPtr;
    /// Ref: The selected field
    MVPbkStoreContactField* iSelectedField;
    /// Ref: Virtual Phonebook attribute manager
    MVPbkContactAttributeManager* iAttributeManager;
    /// Own: No addresses for a name prompt text resource id
    TInt iNoAddressesForNamePromptResource;
    /// Own: No address prompt text resource id
    TInt iNoAddressesPromptResource;
    /// Own: Soft key resource id
    TInt iSoftKeyResource;
    /// Ref: Set to NULL when the execution enters destructor
    MFscDialogEliminator** iSelfPtr;
    };

#endif // CFSCADDRESSSELECT_H
// End of File
