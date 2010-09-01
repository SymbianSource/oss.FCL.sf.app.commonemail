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
* Description:  Sort order field mapper.
 *
*/


#ifndef TFSCSORTORDERFIELDMAPPER_H
#define TFSCSORTORDERFIELDMAPPER_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class MVPbkBaseContactFieldCollection;
class MVPbkBaseContactField;

// CLASS DECLARATION

/**
 * Sort order field mapper.
 * Maps fields in a contact to the same order than in the sort order.
 */
class TFscSortOrderFieldMapper
    {
public:
    // Construction

    /**
     * Constructor.
     */
    TFscSortOrderFieldMapper();

public:
    // Interface

    /**
     * Sets the current sort order that the fields are mapped against.
     *
     * @param aSortOrder    New sort order.
     */
    void SetSortOrder(const MVPbkFieldTypeList& aSortOrder);

    /**
     * Sets the contact fields that are mapped.
     *
     * @param aContactFields    Fields to be mapped to sort order.
     */
    void SetContactFields(
            const MVPbkBaseContactFieldCollection& aContactFields);

    /**
     * Returns the number of mapped fields.
     *
     * @return  Number of mapped fields.
     */
    TInt FieldCount() const;

    /**
     * Returns a field in a contact that is of the same type than
     * the field at given index in the sort order.
     *
     * @param aIndex    Index of the field type in the sort order.
     * @return  Field that is of the same type than the field type
     *          at given index in the sort order.
     */
    const MVPbkBaseContactField* FieldAt(
            TInt aIndex ) const;

private: // Data
    /// Ref: Current sort order
    const MVPbkFieldTypeList* iSortOrder;
    /// Ref: Fields that are mapped to sort order
    const MVPbkBaseContactFieldCollection* iContactFields;
    };

#endif // TFSCSORTORDERFIELDMAPPER_H
// End of File
