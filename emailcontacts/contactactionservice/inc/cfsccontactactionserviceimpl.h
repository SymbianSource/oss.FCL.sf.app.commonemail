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
* Description:  Definition of the class CFscContactActionServiceImpl.
*
*/


#ifndef C_FSCCONTACTACTIONSERVICEIMPL_H
#define C_FSCCONTACTACTIONSERVICEIMPL_H

#include <e32base.h>
#include "cfsccontactactionservice.h"
#include "mfsccontactactionpluginengineobserver.h"

// FORWARD DECLARATIONS
class CFscContactSet;
class CFscContactActionPluginEngine;

/**
 *  Contact Action Service implementation.
 *  Implements functionality of the Contact Action Service.
 *
 *  @lib fsccontactactionservice.lib
 *  @since S60 3.1
 */
class CFscContactActionServiceImpl : 
    public CFscContactActionService,
    public MFscContactActionPluginEngineObserver
    {
    
public: // public constructors and destructor

    /**
     * Two-phased constructor.
     * 
     * @param aParams Pointer construction parameters
     * @return New instance of the component   
     */
    static CFscContactActionServiceImpl* NewL( TAny* aParams );
   
    /**
     * Destructor.
     */
    virtual ~CFscContactActionServiceImpl();
    
public: // From base class CFscContactActionService

    /**
     * From CFscContactActionService.
     * Method for setting current contact set to the service.
     * 
     * @param aIterator iterator to collection of MVPbkContactLink objects.
     *                  Collection can contain links to contacts and group.
     */
    virtual void SetContactSetL( MFscContactLinkIterator* aIterator );
    
    /**
     * From CFscContactActionService.
     * Method for setting current contact set to the service.
     * 
     * @param aStoreContactList collection of MVPbkStoreContact objects.
     *                          Collection can contain ONLY contacts 
     *                          (no groups). 
     */
    virtual void SetContactSetL( 
            const RFscStoreContactList& aStoreContactList );
    
    /**
     * Method for quering available actions
     * Method searches actions for pre-set contact/group set. 
     * Results can be received with QueryResults method.
     * Asynchronous. Data is ready when observer's QueryActionsComplete 
     * method is invoked.
     *
     * @param aObsever operation observer.
     * @param aStopWhenOneActionFound stops query when at least one 
     *                                action found.
     * @param aActionTypeFlags Action type flags which are used to filter
     *                         actions.
     * @param aMinPriority Minimum allowed priority for returned actions.
     */
    void QueryActionsL( 
        MFscContactActionServiceObserver* aObserver,
        TBool aStopWhenOneActionFound = EFalse,
        TUint64 aActionTypeFlags = KFscAtDefault, 
        TInt aMinPriority = 0 );
    
    /**
     * Cancels async method QueryActionsL.
     */
    void CancelQueryActions(); 

    /**
     * Return action query results.
     * Empty list is returned if no queries has been done
     *
     * @return action query result list.
     */
    const CFscContactActionList& QueryResults() const; 
    
    /**
     * Execute action.
     * Asynchronous. Data is ready when observer's ExecuteComplete 
     * mthod is invoked.
     *
     * @param aActionUid uid of action to be executed
     */
    void ExecuteL( TUid aActionUid, 
        MFscContactActionServiceObserver* aObserver );
    
    /**
     * Cancels async method ExecuteL.
     */
    void CancelExecute();
    
protected: // From base class MFscContactActionPluginEngineObserver
    
    /**
     * From MFscContactActionPluginEngineObserver.
     * Called when QueryActionsL method is complete.
     */
    void QueryActionsComplete();
         
    /**
     * From MFscContactActionPluginEngineObserver.
     * Called when QueryActionsL method failed.
     * 
     * @param aError An error code of the failure.
     */
    void QueryActionsFailed( TInt aError );
      
    /**
     * From MFscContactActionPluginEngineObserver.
     * Called when ExecuteL method is complete.
     */
    void ExecuteComplete();
        
    /**
     * From MFscContactActionPluginEngineObserver.
     * Called when ExecuteL method failed.
     * 
     * @param aError An error code of the failure.
     */
    void ExecuteFailed( TInt aError );
     
private: // private contructors

    /**
     * Constructor.
     */
    CFscContactActionServiceImpl(
            const TFscContactActionServiceConstructParameters& aParams );

    /**
     * Second phase constructor.
     */
    void ConstructL();

private: // private methods
    
    /**
     * Clear contacts and groups.
     */
    void ClearContactSet();

private: // data

    /**
     * Instance of plugin engine.
     * Own.
     */
    CFscContactActionPluginEngine* iPluginEngine;
    
    /**
     * Action query result list.
     * Own.
     */
    CFscContactActionList* iActionList;
     
    /**
     * Contact manager.
     */
    CVPbkContactManager& iVPbkContactManager;
    
    /**
     * Current contact set.
     * Own.
     */
    CFscContactSet* iContactSet;
    
    /**
     * Operation Observer.
     */
    MFscContactActionServiceObserver* iObserver;
    
    /**
     * Is action query processed.
     */
    TBool iIsCasUsed;
    };

#endif // C_FSCCONTACTACTIONSERVICEIMPL_H
