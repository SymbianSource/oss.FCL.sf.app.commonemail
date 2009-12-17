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
* Description:  Definition of the class CPbkxRclActionServiceWrapper.
*
*/


#ifndef CPBKXRCLACTIONSERVICEWRAPPER_H
#define CPBKXRCLACTIONSERVICEWRAPPER_H

#include <e32base.h>
//<cmail>
#include "fsccontactactionservicedefines.h"
#include "mfsccontactactionserviceobserver.h"  
//</cmail>
#include <MVPbkContactStoreObserver.h>

class CFscContactActionService;
class TFscContactActionQueryResult;
class CVPbkContactManager;
class CPbkContactEngine;
class CPbkxRclContactConverter;
class MVPbkContactStore;
class CPbkContactItem;

/**
* Wrapper for contact action service.
*
* Available actions can be queried and actions can be executed.
*
* Save as action is not included in queries, since it is handled
* by ourselves.
*/
class CPbkxRclActionServiceWrapper : 
    public CBase, 
    public MVPbkContactStoreObserver, 
    public MFscContactActionServiceObserver
    {
public: // constructor and destructor

    /**
    * Constructs new object.
    *
    * @return Created object.
    */
    static CPbkxRclActionServiceWrapper* NewL( CPbkContactEngine& aContactEngine );

    /**
    * Destructor.
    */
    virtual ~CPbkxRclActionServiceWrapper();

public: // new methods

    /**
    * Sets contact selector mode on or off.
    *
    * ETrue is for setting mode on, EFalse is for setting mode off.
    *
    * Queried actions depend on the given mode. This method is called only once
    * in the beginning of the execution. After calling this, SetActionMenuMode
    * is called each time when search result view or result info view is set
    * topmost. 
    *
    * @param aContactSelectorMode Contact selector mode.
    */
    void SetContactSelectorMode( TBool aContactSelectorMode );

    /**
    * Sets action menu mode on or off.
    *
    * ETrue is for setting mode on, EFalse is for setting mode off.
    *
    * Queried actions depend on the given mode. This method is called every 
    * time search result view or result info view is set topmost.
    *
    * @param aActionMenuMode Action menu mode.
    */
    void SetActionMenuMode( TBool aActionMenuMode );

    /**
    * Sets contact to contact action service.
    *
    * After setting the contact actions are queried.
    *
    * @param aContactItem Contact item.
    */
    void SetCurrentContactL( CPbkContactItem* aContactItem );
    
    /**
    * Checks whether given action is enabled by contact action service.
    *
    * @param aActionType Action type.
    * @return ETrue if action is enabled, EFalse otherwise.
    */
    TBool IsActionEnabled( const TUint64 aActionType ) const;
    
    /**
    * Executes given action.
    *
    * @param aActionType Action type.
    */
    void ExecuteActionL( const TUint64 aActionType );

    /**
    * Returns contact action service used by this wrapper.
    *
    * Ownership is not transferred.
    *
    * @return Contact action service.
    */
    CFscContactActionService* ActionService();

    /**
    * Searches for a query result of given type.
    *
    * If such is not found, NULL is returned.
    *
    * @param aType Type of result.
    * @return Query result or NULL.
    */
    const TFscContactActionQueryResult* GetResult( const TUint64 aType ) const;

    /**
    * Returns contact converter used by this wrapper.
    *
    * Ownership is not transferred.
    *
    * @return Contact converter.
    */    
    CPbkxRclContactConverter* ContactConverter();
    
    /**
     * Cancels action service requests.
     */
    void CancelQuery();
    
    // from base class MVPbkContactStoreObserver
    void StoreReady( MVPbkContactStore& aContactStore );
    
    void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );
    
    void HandleStoreEventL( MVPbkContactStore& aContactStore, 
            TVPbkContactStoreEvent aStoreEvent );    
    
    // from base class MFscContactActionServiceObserver
    void QueryActionsComplete();
    
    void QueryActionsFailed( TInt aError );
    
    void ExecuteComplete();
    
    void ExecuteFailed( TInt aError );
    
    
private: // constructors

    /**
    * Constructor.
    */
    CPbkxRclActionServiceWrapper( CPbkContactEngine& aContactEngine );

    /**
    * Second-phase constructor.
    */
    void ConstructL();

private: // data
    
    // Contact Manger. Owned. 
    CVPbkContactManager*        iContactManager;
    // Contact Store. Not owned.
    MVPbkContactStore*          iContactStore;
    
    // Contact converter. Owned.
    CPbkxRclContactConverter*   iContactConverter;

    // Contact engine
    CPbkContactEngine&          iContactEngine;
    
    // Contact action service. Owned.
    CFscContactActionService*   iContactActionService;

    // Array to hold converted Contact item. Contains only one contact at time.
    RFscStoreContactList        iConvertedContact;
    
    // For asyncronous operation. Owned.
    CActiveSchedulerWait*       iWait;
    
    // Flag indicating whether contact selector mode is on.
    TBool                       iContactSelectorMode;

    // Flags used in action querying.
    TUint64                     iFlags;

    // Flag indicating whether we have save as contact action.
    TBool                       iSaveAsContactActionExists;

    // Save as contact query result.
    // We need custom implementation in search result view and this 
    // is why it is stored.
    TFscContactActionQueryResult iSaveAsContactResult;
    
    // Error code from Observer
    TInt                        iLastError;
    
    // Query flag
    TBool                       iQueryComplete;
    
    // Action execute flag
    TBool                       iExecuteComplete;
    };

#endif
