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
* Description:  Declaration of CFscContactLinkSet.
*
*/


#ifndef C_FSCCONTACTLINKSET_H
#define C_FSCCONTACTLINKSET_H

#include <e32base.h>

#include "cfsccontactset.h"

/**
 *  Contact link set.
 *  Implementation of a contact link based contact set.
 *
 *  @since S60 3.1
 */
class CFscContactLinkSet 
    : public CFscContactSet
    {

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aVPbkContactManager contact manager.
     * @param aIterator Iterator of collection of MVPbkContactLink objects.
     * @return New instance of CFscContactLinkSet
     */
    static CFscContactLinkSet* NewL(
        CVPbkContactManager& aVPbkContactManager,
        MFscContactLinkIterator* aIterator );
           
    /**
     * Destructor.
     */
    virtual ~CFscContactLinkSet();
    
   
public: // From base class MFscContactSet
    
    /**
     * From MFscContactSet.
     * Contact count.
     *
     * @return contact count.
     */
    TInt ContactCount() const;
    
    /**
     * From MFscContactSet.
     * Checks if iterator has more contacts.
     *
     * @return ETrue if the iterator has more contacts.
     */
    TBool HasNextContact() const;
    
    /**
     * From MFscContactSet.
     * Sets the iterator to point to the first contact in the list.
     */
    void SetToFirstContact();
    
    /**
     * From MFscContactSet.
     * Asynchronous operation for retrieving store contact for contact. 
     *
     * @param aObserver operation observer.
     */
    void NextContactL( MFscContactSetObserver* aObserver );
    
    /**
     * From MFscContactSet.
     * 
     * @return link for contact.
     */
    MVPbkContactLink* NextContactLinkL();
    
    /**
     * From MFscContactSet.
     * Used for canceling asynchronous NextContactL operation.
     */
    void CancelNextContactL();
    
    /**
     * From MFscContactSet.
     * Group count.
     *
     * @return group count.
     */
    TInt GroupCount() const;
    
    /**
     * From MFscContactSet.
     * Checks if iterator has more groups.
     *
     * @return ETrue if the iterator has more groups. 
     */
    TBool HasNextGroup() const;
    
    /**
     * From MFscContactSet.
     * Sets the iterator to point to the first group in the list.
     */
    void SetToFirstGroup();
    
    /**
     * From MFscContactSet.
     * Asynchronous operation for retrieving store contact for group.
     * 
     * @param aObserver operation observer.
     */
    void NextGroupL( MFscContactSetObserver* aObserver );
    
    /**
     * From MFscContactSet.
     * 
     * @return link for group.
     */
    MVPbkContactLink* NextGroupLinkL();
    
    /**
     * From MFscContactSet.
     * Used for canceling asynchronous NextGroupL operation.
     */
    void CancelNextGroupL();
    
    
protected: // From base class MVPbkSingleContactOperationObserver
    
    /**
     * From MVPbkSingleContactOperationObserver.
     * Called when the operation is completed.
     * 
     * @param aOperation The completed operation.
     * @param aContact The contact returned by the operation.
     *                 A client must take the ownership immediately.
     */
    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
    
    /**
     * From MVPbkSingleContactOperationObserver.
     * Called if the operation fails.
     * 
     * @param aOperation The failed operation.
     * @param aError An error code of the failure.
     */
    void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError );
    

private: // Private constructors

    /**
     * Constructor.
     *
     * @param aPbkContactEngine Contact engine     
     */
    CFscContactLinkSet( CVPbkContactManager& aVPbkContactManager );

    /**
     * Second phase constructor.
     *
     * @param aIterator Iterator of collection of MVPbkContactLink objects.
     */
    void ConstructL( MFscContactLinkIterator* aIterator );
    
    /**
     * Used for canceling asynchronous GetGroupContactL operation. 
     */
    void CancelGetGroupContactL();

    
private: // Private members
        
    /**
     * Iterator of collection of MVPbkContactLink objects.
     * Not own.
     */
    MFscContactLinkIterator* iIterator;
    
    /**
     * Virtual Phonebook operation handle - contact retrieval.
     * Own.
     */
    MVPbkContactOperationBase* iVPbkOperationNextContact;
    
    /**
     * Virtual Phonebook operation handle - group retrieval.
     * Own.
     */
    MVPbkContactOperationBase* iVPbkOperationNextGroup;
    
    /**
     * Currently retrieved contact.
     * Own.
     */
    MVPbkStoreContact* iCurrentContact;

    };

#endif // C_FSCCONTACTLINKSET_H
