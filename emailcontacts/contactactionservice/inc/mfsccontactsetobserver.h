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
* Description:  Declaration of interface MFscContactSetObserver.
*
*/


#ifndef M_FSCCONTACTSETOBSERVER_H
#define M_FSCCONTACTSETOBSERVER_H

// FORWARD DECLARATIONS
class MVPbkStoreContact;

/**
 *  Contact set observer.
 *  Defines an interface for contact set observer.
 *
 *  @since S60 3.1
 */
class MFscContactSetObserver
    {
    
public:
    
    /**
     * Called when NextContactL method is complete.
     * 
     * @param aContact retireved store contact for contact. 
     *                 Ownership is NOT transferred to client.
     *                 Object is valid until next call of NextContactL is made.
     */
    virtual void NextContactComplete( MVPbkStoreContact* aContact ) = 0;
    
    /**
     * Called when NextContactL method failed.
     * 
     * @param aError An error code of the failure.
     */
    virtual void NextContactFailed( TInt aError ) = 0;
    
    /**
     * Called when NextGroupL method is complete.
     * 
     * @param aContact retireved store contact for group. 
     *                 Ownership is NOT transferred to client.
     *                 Object is valid until next call of NextGroupL is made.
     */
    virtual void NextGroupComplete( MVPbkStoreContact* aContact ) = 0;
    
    /**
     * Called when NextGroupL method failed.
     * 
     * @param aError An error code of the failure.
     */
    virtual void NextGroupFailed( TInt aError ) = 0;
    
    /**
     * Called when GetGroupContactL method is complete.
     * 
     * @param aContact retireved store contact for contact from group. 
     *                 Ownership is transferred to client.
     */
    virtual void GetGroupContactComplete( MVPbkStoreContact* aContact ) = 0;
    
    /**
     * Called when GetGroupContactL method failed.
     * 
     * @param aError An error code of the failure.
     */
    virtual void GetGroupContactFailed( TInt aError ) = 0;
    
public:
    
    /**
     * Destructor.
     */
    virtual ~MFscContactSetObserver() {}
    
    };

#endif // M_FSCCONTACTSETOBSERVER_H
