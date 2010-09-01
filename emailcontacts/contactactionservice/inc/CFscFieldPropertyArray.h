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
* Description:  Field property array for field types.
 *
*/


#ifndef CFSCFIELDPROPERTYARRAY_H
#define CFSCFIELDPROPERTYARRAY_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2FieldPropertyArray.h>

// FORWARD DECLARATIONS
class TResourceReader;
class MVPbkFieldTypeList;
class CFscFieldProperty;
class RFs;
class CFscFieldOrderingManager;

// CLASS DECLARATION

/**
 * Field property array for Phonebook 2 field types.
 */
class CFscFieldPropertyArray : public CBase, public MPbk2FieldPropertyArray
    {
public:
    // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @param aSupportedFieldTypeList   A list of field types.
     *                                  If this is the master fieldtype
     *                                  list then all the properties
     *                                  are loaded, otherwise only those
     *                                  properties that have the field
     *                                  type in the list are loaded.
     * @return  A new instance of this class.
     */
    static CFscFieldPropertyArray* NewL(
            const MVPbkFieldTypeList& aSupportedFieldTypeList);

    /**
     * Creates a new instance of this class.
     *
     * @param aSupportedFieldTypeList   A list of field types.
     *                                  If this is the master fieldtype
     *                                  list then all the properties
     *                                  are loaded, otherwise only those
     *                                  properties that have the field
     *                                  type in the list are loaded.
     * @param aReader                   Resource reader pointed to
     *                                  PHONEBOOK2_FIELD_PROPERTY_ARRAY
     *                                  resource.
     * @return  A new instance of this class.
     */
     static CFscFieldPropertyArray* NewL(
            const MVPbkFieldTypeList& aSupportedFieldTypeList,
            TResourceReader& aReader);

    /**
     * Destructor.
     */
    ~CFscFieldPropertyArray();

public: // From MPbk2FieldPropertyArray
    const MPbk2FieldProperty* FindProperty(
            const MVPbkFieldType& aFieldType ) const;
    TInt Count() const;
    const MPbk2FieldProperty& At(
            TInt aIndex ) const;

private: // Implementation
    CFscFieldPropertyArray();
    void ConstructL(
            const MVPbkFieldTypeList& aSupportedFieldTypeList);
    void ReadFieldPropertiesL(
            TResourceReader& aReader,
            const MVPbkFieldTypeList& aSupportedFieldTypeList,
            CFscFieldOrderingManager& aOrderingManager );
    CFscFieldOrderingManager* CreateFieldOrderingsLC(void);

private: // Data
    /// Own: Field properties
    RPointerArray<CFscFieldProperty> iFieldProperties;
    /// Own: drive letter
    TFileName iLetterDrive;
    };

#endif // CFSCFIELDPROPERTYARRAY_H
// End of File
