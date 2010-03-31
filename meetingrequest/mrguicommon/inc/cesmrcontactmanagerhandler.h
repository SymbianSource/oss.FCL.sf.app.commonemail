/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class CESMRContactManagerHandler.
*
*/


#ifndef CESMRCONTACTMANAGERHANDLER_H
#define CESMRCONTACTMANAGERHANDLER_H

#include <e32base.h>

#include <mvpbkcontactstoreobserver.h>
#include <mvpbkcontactstorelistobserver.h>

class CVPbkContactManager;

NONSHARABLE_CLASS( MESMRContactManagerObserver )
    {
public:
    virtual void ContactManagerReady ( ) = 0;
    };

NONSHARABLE_CLASS( CESMRContactManagerHandler ) : 
	public CBase,
    public MVPbkContactStoreListObserver
    {
public:
    static CESMRContactManagerHandler* NewL ( );
    ~CESMRContactManagerHandler ( );

    /**
     * Is ready
     * @return ETrue if is else EFalse
     */
    IMPORT_C TBool IsReady ( );
    
    /**
     * Add observer
     * @param aObserver
     */
    IMPORT_C void AddObserverL (MESMRContactManagerObserver* aObserver );
    
    /**
     *  Remove observer
     *  @param aObserver
     */
    IMPORT_C void RemoveObserver (MESMRContactManagerObserver* aObserver );
    
    /**
     * Get contact manager
     */
    IMPORT_C CVPbkContactManager& GetContactManager ( );

private:
    CESMRContactManagerHandler ( );
    void ConstructL ( );

    // from MVPbkContactStoreListObserver
    void OpenComplete ( );
    // from MVPbkContactStoreObserver
    void StoreReady (MVPbkContactStore& aContactStore );
    void StoreUnavailable (MVPbkContactStore& aContactStore, TInt aReason );
    void HandleStoreEventL (MVPbkContactStore& aContactStore,
            TVPbkContactStoreEvent aStoreEvent );

private: //Data
	/// Own:
    TBool iContactManagerReady;
    /// Own:
    CVPbkContactManager* iContactManager;
    /// Own:
    RPointerArray<MESMRContactManagerObserver> iObservers;
    };

#endif
