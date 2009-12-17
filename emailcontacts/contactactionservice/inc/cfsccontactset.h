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
* Description:  Declaration of CFscContactSet.
*
*/


#ifndef C_FSCCONTACTSET_H
#define C_FSCCONTACTSET_H

#include <e32base.h>
//<cmail>
#include "mfsccontactset.h"
//</cmail>
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkContactLinkArray;


/**
 *  Contact set.
 *  Implementation of a contact set.
 *
 *  @since S60 3.1
 */
class CFscContactSet : 
    public CBase, 
    public MFscContactSet, 
    public MVPbkSingleContactOperationObserver
    {

public: // Constructors and destructor

    /**
     * Constructor.
     *
     * @param aVPbkContactManager Contact manager     
     */
    CFscContactSet( CVPbkContactManager& aVPbkContactManager );
           
    /**
     * Destructor.
     */
    virtual ~CFscContactSet();
   
public: // From base class MFscContactSet

    /**
     * Contact count of group.
     *
     * @param aStoreContact Group
     * @return group item count
     */
    TInt GroupContactCountL( MVPbkStoreContact& aStoreContact );
    
    /**
     * Asynchronous operation for retrieving contact from group.
     *
     * @param aStoreContact group
     * @param aIndex contsct index
     * @param aObserver operation observer
     */
    void GetGroupContactL( MVPbkStoreContact& aStoreContact, 
            TInt aIndex, MFscContactSetObserver* aObserver );

protected: // Private members
    
    /**
     * Virtual Phonebook contact manager.
     * Not own.
     */
    CVPbkContactManager& iVPbkContactManager;
    
    /**
     * Asynchronous operation observer.
     * Not own.
     */
    MFscContactSetObserver* iObserver;
    
    /**
     * Virtual Phonebook operation handle - group contact retrieval.
     * Own.
     */
    MVPbkContactOperationBase* iVPbkOperationGetGroupContact;
    
    /**
     * Contact link array needed by for handling async GetGroupContactL.
     */
    MVPbkContactLinkArray* iContactLinkArray;
    
    };

#endif // C_FSCCONTACTSET_H
