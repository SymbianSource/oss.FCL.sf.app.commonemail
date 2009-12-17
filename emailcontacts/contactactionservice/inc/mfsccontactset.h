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
* Description:  Declaration of interface MFscContactSet.
*
*/


#ifndef M_FSCCONTACTSET_H
#define M_FSCCONTACTSET_H

#include <e32base.h>
#include <cntdef.h>

//<cmail>
#include "fsccontactactionservicedefines.h"
//</cmail>

// FORWARD DECLARATIONS
class MFscContactSetObserver;
class MVPbkContactLink;

/**
 *  Contact set.
 *  Defines an interface for contact set.
 *
 *  @since S60 3.1
 */
class MFscContactSet
    {
    
public: //Contact related methods
    
    /**
     * Contact count.
     *
     * @return contact count.
     */
    virtual TInt ContactCount() const = 0;
    
    /**
     * Checks if iterator has more contacts.
     *
     * @return ETrue if the iterator has more contacts.
     */
    virtual TBool HasNextContact() const = 0;
    
    /**
     * Sets the iterator to point to the first contact in the list.
     */
    virtual void SetToFirstContact() = 0;
    
    /**
     * Asynchronous operation for retrieving store contact for contact. 
     *
     * @param aObserver operation observer. 
     */
    virtual void NextContactL( MFscContactSetObserver* aObserver ) = 0;
    
    /**
     * Returns link for contact.
     */
    virtual MVPbkContactLink* NextContactLinkL() = 0;
    
    /**
     * Used for canceling asynchronous NextContactL operation.
     */
    virtual void CancelNextContactL() = 0;

    
public: // Contact group related methods
    
    /**
     * Group count.
     *
     * @return group count.
     */
    virtual TInt GroupCount() const = 0;
    
    /**
     * Checks if iterator has more groups.
     *
     * @return ETrue if the iterator has more groups. 
     */
    virtual TBool HasNextGroup() const = 0;
    
    /**
     * Sets the iterator to point to the first group in the list.
     */
    virtual void SetToFirstGroup() = 0;
    
    /**
     * Asynchronous operation for retrieving store contact for group.
     * 
     * @param aObserver operation observer.
     */
    virtual void NextGroupL( MFscContactSetObserver* aObserver ) = 0;
    
    /**
     * Returns link for group.
     */
    virtual MVPbkContactLink* NextGroupLinkL() = 0;
    
    /**
     * Used for canceling asynchronous NextGroupL operation.
     */
    virtual void CancelNextGroupL() = 0;
    
    /**
     * Contact count of group.
     *
     * @param aStoreContact Group
     * @return group item count
     */
    virtual TInt GroupContactCountL( MVPbkStoreContact& aStoreContact ) = 0;
    
    /**
     * Asynchronous operation for retrieving contact from group.
     *
     * @param aStoreContact group
     * @param aIndex contsct index
     * @param aObserver operation observer
     */
    virtual void GetGroupContactL( MVPbkStoreContact& aStoreContact, 
            TInt aIndex, MFscContactSetObserver* aObserver ) = 0;
    
public:
    
    /**
     * Destructor.
     */
    virtual ~MFscContactSet() {}
    
    };

#endif // M_FSCCONTACTSET_H
