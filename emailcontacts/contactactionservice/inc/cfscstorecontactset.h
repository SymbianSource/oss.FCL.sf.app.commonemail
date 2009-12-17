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
* Description:  Declaration of CFscStoreContactSet.
*
*/


#ifndef C_FSCCONTACTITEMSET_H
#define C_FSCCONTACTITEMSET_H

#include <e32base.h>

#include "cfsccontactset.h"

/**
 *  Contact set.
 *  Implementation of a contact item objects based contact set.
 *
 *  @since S60 3.1
 */
class CFscStoreContactSet: public CFscContactSet
    {

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aVPbkContactManager Contact manager
     * @param aStoreContactList Source contacts
     * @return New instance of CFscStoreContactSet
     */
    static CFscStoreContactSet* NewL( 
        CVPbkContactManager& aVPbkContactManager,
        const RFscStoreContactList& aStoreContactList );
           
    /**
     * Destructor.
     */
    virtual ~CFscStoreContactSet();
   
public: // From base class MFscContactSet
    
    /**
     * Contact count.
     *
     * @return contact count.
     */
    TInt ContactCount() const;
    
    /**
     * Group count.
     *
     * @return group count.
     */
    TInt GroupCount() const;
    
    /**
     * Checks if collection has next contact.
     *
     * @return ETrue if the next contact exists.
     */
    TBool HasNextContact() const;
    
    /**
     * Checks if collection has next group.
     * Just for interface compatibility - always EFalse.
     *
     * @return Just for interface compatibility - always EFalse. 
     */
    TBool HasNextGroup() const;
    
    /**
     * Sets the index to point to the first contact in the list.
     */
    void SetToFirstContact();
    
    /**
     * Sets the index to point to the first group in the list.
     * Just for interface compatibility.
     */
    void SetToFirstGroup();
    
    /**
     * Retrieves store contact.
     */
    void NextContactL( MFscContactSetObserver* aObserver );
    
    /**
      * From MFscContactSet.
      * 
      * @return link for contact.
      */
    MVPbkContactLink* NextContactLinkL();
    
    /**
     * Retrieves store contact for group. Just for interface compatibility.
     * Calling this method for this objects will cause calling observer's
     * mthod NextContactFailed() with KErrArgument as error.
     */
    void NextGroupL( MFscContactSetObserver* aObserver );
    
    /**
      * From MFscContactSet.
      * 
      * @return link for group.
      */
    MVPbkContactLink* NextGroupLinkL();
    
    /**
     * Cancel NextContactL method.
     * Just for interface compatibility.
     */
    void CancelNextContactL();
    
    /**
     * Cancel NextGroupL method.
     * Just for interface compatibility.
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
     * @param aVPbkContactManager Contact manager     
     */
    CFscStoreContactSet( CVPbkContactManager& aVPbkContactManager );

    /**
     * Second phase constructor.
     *
     * @param aStoreContactList Source contacts
     */
    void ConstructL( const RFscStoreContactList& aStoreContactList );

private: // Private members
    
    /**
     * Store contacts list pointer.
     * Not Own.
     */    
    const RFscStoreContactList* iStoreContactList;
    
    /**
     * Current store contact.
     */
    TInt iCurrentStoreContact;
    
    };

#endif // C_FSCCONTACTITEMSET_H
