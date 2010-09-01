/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines class MESMRFieldStorage.
*
*/


#ifndef MESMRFIELDSTORAGE_H
#define MESMRFIELDSTORAGE_H

#include <e32std.h>
#include "esmrdef.h" // TESMREntryFieldId

class MESMRCalEntry;
class CESMRField;
class CESMRPolicy;

/**
 * Pure virtual class to be inherited by field storage classes.
 */
class MESMRFieldStorage
    {
public:
    virtual ~MESMRFieldStorage(){}
public:
    /**
     * Returns the count for all the fields in the storage
     *
     * @return TInt - field count
     */
    virtual TInt Count() const = 0;

    /**
     * Returns field for given index.
     *
     * @param aInd - index of the wanted field.
     * @return CESMRField pointer
     */
    virtual CESMRField* Field( TInt aInd ) const = 0;

    /**
     * Returns field for given field id.
     * 
     * @param aId field id
     * @return CESMRField poiter or NULL. 
     */
    virtual CESMRField* FieldById( TESMREntryFieldId aId ) const = 0;
    
    /**
     * Called in the construction phase to set needed
     * entry informations to fields.
     *
     * @param aEntry - contains needed information.
     */
    virtual void InternalizeL( MESMRCalEntry& aEntry ) = 0;

    /**
     * Called when editing the entry is done. Information from edited fields
     * is collected to aEntry.
     *
     * @param aEntry - entry where all the edited information is stored
     */
    virtual void ExternalizeL( MESMRCalEntry& aEntry ) = 0;

    /**
     * Validates the content of fields
     *
     * @param aId Field id where focus is set if validation fails
     * @param aForceValidate ETrue if force validation is used
     * @return TInt
     */
    virtual TInt Validate( 
    		TESMREntryFieldId& aId, 
    		TBool aForceValidate = EFalse ) = 0;
    
    /**
     * Updates storage on policy change.
     * 
     * @param aNewPolicy the new policy to adjust the storage.
     * @param aEntry the entry to handle.
     */
    virtual void ChangePolicyL( const CESMRPolicy& aNewPolicy,
                                MESMRCalEntry& aEntry ) = 0;
    };

#endif
