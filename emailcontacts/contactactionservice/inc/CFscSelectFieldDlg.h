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
* Description:  Field selection dialog.
 *
*/


#ifndef CFSCSELECTFIELDDLG_H
#define CFSCSELECTFIELDDLG_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class MVPbkStoreContactFieldCollection;
class CFscPresentationContactFieldCollection;
class MFscControlKeyObserver;
class CVPbkContactManager;
class MPbk2FieldPropertyArray;

// CLASS DECLARATION

/**
 * Phonebook 2 field selection dialog.
 */
NONSHARABLE_CLASS(CFscSelectFieldDlg) : public CBase
    {
public: // Construction and destruction

    /**
     * Constructor.
     */
    CFscSelectFieldDlg();

    /**
     * Destructor.
     */
    ~CFscSelectFieldDlg();

public: // Interface

    /**
     * Runs the dialog and returns selected field. Caller takes the
     * ownership of the field. Field is left to the cleanupstack
     * on return. NOTE: this object is deleted when this function
     * returns or leaves.
     *
     * @param aFieldCollection  Array of store contact fields.
     * @param aContactManager   Virtual Phonebook contact manager.
     * @param aFieldProperties  Phonebook 2 field properties.
     * @param aCbaResourceId    Softkey buttons for the control.
     * @param aHeading          Possible heading text for the control.
     * @param aFocusIndex       Field index to focus initially.
     *
     * @return  Returns the selected contact item field or NULL.
     * @see CFscSelectFieldDlg::TPbk2SelectFieldFlags
     */
    MVPbkStoreContactField* ExecuteLD( // <cmail>
            MVPbkStoreContactFieldCollection& aFieldCollection,
            const CVPbkContactManager& aContactManager,
            const MPbk2FieldPropertyArray& aFieldProperties,
            TInt aCbaResourceId,
            const TDesC& aHeading = KNullDesC,
            TInt aFocusIndex = KErrNotFound );

    /**
     * Registers aKeyObserver as a key event observer of this control.
     * Only one observer is supported and setting a new one removes the
     * previous setting.
     *
     * @param aKeyObserver  Phonebook 2 control key observer.
     */
    void SetObserver(
            MFscControlKeyObserver* aKeyObserver );

    /**
     * Attempts to exit this dialog just like a soft button was pressed.
     * @see CAknPopupList::AttemptExitL(TBool).
     */
    void AttemptExitL(
            TBool aAccept );

    /**
     * Sets given pointer to NULL when this dialog is destroyed.
     *
     * @param aSelfPtr      The pointer to use.
     * @precond !aSelfPtr || *aSelfPtr==this
     */
    void ResetWhenDestroyed(
            CFscSelectFieldDlg** aSelfPtr );

private: // Implementation
    void CreateListBoxL(
            const CFscPresentationContactFieldCollection& aFieldCollection,
            const CVPbkContactManager& aContactManager,
            const MPbk2FieldPropertyArray& aFieldProperties,
            TInt aCbaResourceId,
            const TDesC& aHeading,
            TInt aFocusIndex );

private: // Data
    /// Specialized list box class
    class CListBox;
    /// Own: List box inside the popup list
    CListBox* iListBox;
    /// Specialized popup list class
    class CPopupList;
    /// Own: Popup list query
    CPopupList* iPopupList;
    /// Ref: Key event observer
    MFscControlKeyObserver* iKeyObserver;
    /// Ref: Set to ETrue in destructor
    TBool* iDestroyedPtr;
    /// Ref: Set to NULL when this dialog is destroyed.
    CFscSelectFieldDlg** iSelfPtr;
    };

#endif // CFSCSELECTFIELDDLG_H
// End of File
