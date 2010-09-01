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
* Description:  Field property group.
 *
*/


#ifndef CFSCFIELDPROPERTYGROUP_H
#define CFSCFIELDPROPERTYGROUP_H

// INCLUDES
#include <e32base.h>
#include <TPbk2IconId.h>
#include <Pbk2FieldProperty.hrh>

// FORWARD DECLARATIONS
class TResourceReader;
class MPbk2FieldProperty;

// CLASS DECLARATION

/**
 * Field property group.
 * Field property group for field types.
 */
class CFscFieldPropertyGroup : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @param aReader   Resource reader pointed to a
     *                  PHONEBOOK2_FIELD_GROUP resource.
     * @return  A new instance of this class.
     */
    static CFscFieldPropertyGroup* NewLC(TResourceReader& aReader);

    /**
     * Destructor.
     */
    virtual ~CFscFieldPropertyGroup();

public:
    // Interface

    /**
     * Returns the number of field properties in the group.
     *
     * @return  Number of field properties in the group.
     */
    TInt Count() const;

    /**
     * Returns the field property at given index.
     *
     * @param aIndex    The index of the property.
     * @return  Field property in given index.
     */
    const MPbk2FieldProperty& At(
            TInt aIndex ) const;

    /**
     * Returns the id of this group.
     *
     * @return  Id of the group.
     */
    inline TPbk2FieldGroupId GroupId() const;

    /**
     * Returns the label of this group.
     *
     * @return  The label of this group.
     */
    inline const TDesC& Label() const;

    /**
     * Returns the icon id of this group.
     *
     * @return  Icon id of this group.
     */
    inline const TPbk2IconId& IconId() const;

    /**
     * Returns the add item ordering of this group.
     *
     * @return  Add item ordering of this group.
     */
    inline TInt AddItemOrdering() const;

    /**
     * A private API function to add properties to the group.
     *
     * @param aProperty     A new property to add.
     */
    void AddFieldPropertyL(
            const MPbk2FieldProperty& aProperty );

private: // Implementation
    CFscFieldPropertyGroup();
    void ConstructL(
            TResourceReader& aReader );

private: // Data
    /// Own: The id of the group
    TPbk2FieldGroupId iGroupId;
    /// Own: Label of this group
    HBufC* iLabel;
    /// Own: Icon Id of this group
    TPbk2IconId iIconId;
    /// Own: Add item ordering of this group
    TInt iAddItemOrdering;
    /// Own: Does not own the instances
    RPointerArray<const MPbk2FieldProperty> iProperties;
    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::GroupId
// --------------------------------------------------------------------------
//
inline TPbk2FieldGroupId CFscFieldPropertyGroup::GroupId() const
    {
    return iGroupId;
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::Label
// --------------------------------------------------------------------------
//
inline const TDesC& CFscFieldPropertyGroup::Label() const
    {
    return *iLabel;
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::IconId
// --------------------------------------------------------------------------
//
inline const TPbk2IconId& CFscFieldPropertyGroup::IconId() const
    {
    return iIconId;
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::AddItemOrdering
// --------------------------------------------------------------------------
//
inline TInt CFscFieldPropertyGroup::AddItemOrdering() const
    {
    return iAddItemOrdering;
    }

#endif // CFSCFIELDPROPERTYGROUP_H
// End of File
