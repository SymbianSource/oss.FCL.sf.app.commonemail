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
* Description: This file defines class CFscFieldProperty.
 *
*/


#ifndef CFSCFIELDPROPERTY_H
#define CFSCFIELDPROPERTY_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2FieldProperty.h>
#include <TPbk2IconId.h>

// FORWARD DECLARATIONS
class TResourceReader;
class MVPbkFieldTypeList;
class CFscFieldOrderingManager;

/**
 * Phonebook 2 field property.
 * Field property for a Phonebook 2 field type.
 * Field property contains data for contact field presentation
 * in the UI.
 */
NONSHARABLE_CLASS(CFscFieldProperty) : public CBase,
public MPbk2FieldProperty
    {
public: // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @param aReader                   Resource reader pointed to a
     *                                  PHONEBOOK2_FIELD_PROPERTY
     *                                  structure.
     * @param aSupportedFieldTypeList   List of supported field types.
     * @param aFieldOrderingManager     Field ordering manager.
     * @return  A new instance of this class.
     */
    static CFscFieldProperty* NewLC(
            TResourceReader& aReader,
            const MVPbkFieldTypeList& aSupportedFieldTypeList,
            CFscFieldOrderingManager& aFieldOrderingManager );

    /**
     * Destructor.
     */
    ~CFscFieldProperty();

public: // Interface

    /**
     * @return ETrue if the property was mapped to the field type
     */
    TBool IsSupported() const;

public: // From MPbk2FieldProperty
    TBool IsSame(
            const MPbk2FieldProperty& aOther ) const;
    const MVPbkFieldType& FieldType() const;
    TPbk2FieldMultiplicity Multiplicity() const;
    TInt MaxLength() const;
    TPbk2FieldEditMode EditMode() const;
    TPbk2FieldDefaultCase DefaultCase() const;
    const TPbk2IconId& IconId() const;
    TPbk2FieldCtrlType CtrlType() const;
    TUint Flags() const;
    TInt OrderingItem() const;
    TInt AddItemOrdering() const;
    const TDesC& AddItemText() const;
    TPbk2FieldLocation Location() const;
    TPbk2FieldGroupId GroupId() const;
    const TDesC& DefaultLabel() const;

private: // Implementation
    CFscFieldProperty();
    void ConstructL(
            TResourceReader& aReader,
            const MVPbkFieldTypeList& aSupportedFieldTypeList,
            CFscFieldOrderingManager& aFieldOrderingManager );

private: // Data
    /// Ref: Field type
    const MVPbkFieldType* iFieldType;
    /// Own: Assorted flags for the field type
    TUint iFlags; // LONG flags
    /// Own: Entry add item label text
    HBufC* iAddItemText; // LTEXT addItemText
    /// Own: Default label for the field
    HBufC* iDefaultLabel; // LTEXT defaultLabel
    /// Own: Maximum length in characters
    TInt16 iMaxLength; // WORD maxLength
    /// Own: Allowed multiplicity (one/many)
    TInt8 iMultiplicity; // BYTE multiplicity
    /// Own: Default editing mode
    TInt8 iEditMode; // BYTE editMode
    /// Own: Default character case
    TInt8 iDefaultCase; // BYTE defaultCase
    /// Own: Index of an icon
    TPbk2IconId iIconId; // STRUCT iconId
    /// Own: Editor UI control type
    TInt8 iCtrlType; // BYTE ctrlType        
    /// Own: Entry item group item ordering
    TInt8 iOrderingItem; // BYTE orderingItem
    /// Own: Entry add item ordering
    TInt8 iAddItemOrdering; // BYTE addItemOrdering
    /// Own; The group id of this field
    TInt8 iGroupId; // BYTE groupId
    /// Own: Entry item location (none/home/work)
    TInt8 iLocation; // BYTE location
    };

#endif // CFSCFIELDPROPERTY_H
// End of File
