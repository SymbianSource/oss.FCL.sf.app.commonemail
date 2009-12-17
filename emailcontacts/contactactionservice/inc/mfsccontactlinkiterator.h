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
* Description:  Declaration of interface MFscContactLinkIterator.
*
*/


#ifndef M_FSCCONTACTLINKITERATOR_H
#define M_FSCCONTACTLINKITERATOR_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;

/**
 *  Contact link iterator.
 *  Defines an interface for contact link iterator.
 *
 *  @since S60 3.1
 */
class MFscContactLinkIterator
    {
   
public: // Contact related methods.
    
    /**
     * Returns contacts count.
     * 
     * @return contact count 
     */
    virtual TInt ContactCount() const = 0;
    
    /**
     * Checks if iterator has more contacts links.
     * 
     * @return ETrue if the iterator has more contacts.
     */
    virtual TBool HasNextContact() const = 0;
    
    /**
     * Operation for retrieving contact link.
     * Gets current contact link and moves iterator to next one. 
     */
    virtual MVPbkContactLink* NextContactL() = 0;
    
    /**
     * Sets the iterator to point to the first contact link in the list.
     */
    virtual void SetToFirstContact() = 0;
    
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
     * Operation for retrieving contact link for group.
     * 
     */
    virtual MVPbkContactLink* NextGroupL() = 0;
    
    /**
     * Sets the iterator to point to the first group in the list.
     */
    virtual void SetToFirstGroup() = 0;

public: 

    /**
     * Destructor.
     */
    virtual ~MFscContactLinkIterator() {}
    
    };

#endif // M_FSCCONTACTLINKITERATOR_H
