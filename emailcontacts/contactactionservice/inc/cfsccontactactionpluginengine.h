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
* Description:  Definition of the class CFscContactActionPluginEngine.
*
*/


#ifndef C_FSCCONTACTACTIONPLUGINENGINE_H
#define C_FSCCONTACTACTIONPLUGINENGINE_H

#include <e32base.h>
//<cmail>
#include "fsccontactactionservicedefines.h"
#include "mfsccontactactionpluginobserver.h"
#include "tfsccontactactionpluginparams.h"
//</cmail>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CFscContactActionPlugin;
class CFscActionUtils;
class MFscContactSet;
class MFscContactActionPluginEngineObserver;

/**
 *  Contact Action Service's plugin engine.
 *  The engine implements service's plugin handling
 *
 *  @lib fsccontactactionservice.lib
 *  @since S60 3.1
 */
class CFscContactActionPluginEngine : public CBase,
    public MFscContactActionPluginObserver
    {
    
    /** the states of operations */
    enum TCasPluginEngineLastEvent 
        {
        ECasEventIdle,
        ECasEventBeforePriorityForContactSet,
        ECasEventAfterPriorityForContactSet,
        ECasEventCanceledPriorityForContactSet
        };
    
public:

    /**
     * Two-phased constructor.
     *
     * @param aContactManager Contact manager
     * @return Constructed object
     */
    static CFscContactActionPluginEngine* NewL(
            CVPbkContactManager& aContactManager );
   
    /**
     * Destructor.
     */
    virtual ~CFscContactActionPluginEngine();
    
public: // New methods

    /**
     * Detect and load plugins
     */
    void LoadPluginsL();

    /**
     * Release loaded plugins
     */
    void ReleasePlugins();
    
    /**
     * Query state of plugins
     *
     * @return ETrue if plugins are loaded
     */
    TBool PluginsLoaded();

    /**
     * Method for quering actions for contacts and groups
     *   Method searches actions for contacts and groups and returns found 
     *   actions in aActionList.
     *
     * @param aActionList A reference to action list. Found actions are 
     *                    appended to the list.
     * @param aContactSet Target contact set
     * @param aStopWhenOneActionFound stops query when at least one 
     *                                action found.
     * @param aActionTypeFlags Action type flags which are used to filter
     *                         actions.
     * @param aMinPriority Min allowed priority of returned actions.
     * @param aObserver operation observer.
     */
    void QueryActionsL( CFscContactActionList& aActionList, 
            MFscContactSet& aContactSet,
            TBool aStopWhenOneActionFound,
            TUint64 aActionTypeFlags, 
            TInt aMinPriority,
            MFscContactActionPluginEngineObserver* aObserver );
    
    /**
     * Cancels async method QueryActionsL.
     */
    void CancelQueryActions();
    
    /**
     * Execute action. Aynchronous method.
     *
     * @param aActionUid uid of the action to be executed.
     * @param aContactSet Target contact set.
     * @param aObserver operation observer.
     */
    void ExecuteL( 
        TUid aActionUid,
        MFscContactSet& aContactSet,
        MFscContactActionPluginEngineObserver* aObserver );
    
    /**
     * Cancels async method ExecuteL.
     */
    void CancelExecute();
    
public: // From base class MFscContactActionPluginObserver
    
    /**
     * From MFscContactActionPluginObserver.
     * Called when PriorityForContactSetL method is complete.
     * 
     * @param aPriority Retrieved priority.
     */
    void PriorityForContactSetComplete( TInt aPriority );
        
    /**
     * From MFscContactActionPluginObserver.
     * Called when PriorityForContactSetL method failed.
     * 
     * @param aError An error code of the failure.
     */
    void PriorityForContactSetFailed( TInt aError );
    
    /**
     * From MFscContactActionPluginObserver.
     * Called when ExecuteL method is complete.
     */
    virtual void ExecuteComplete();
           
    /**
     * From MFscContactActionPluginObserver.
     * Called when ExecuteL method failed.
     * 
     * @param aError An error code of the failure.
     */
    virtual void ExecuteFailed( TInt aError );
                                 
private:

    /**
     * Constructor.
     *
     * @param aContactManager Contact manager
     */
    CFscContactActionPluginEngine( CVPbkContactManager& aContactManager );

    /**
     * Second phase constructor.
     */
    void ConstructL();

private: // data

    /**
     * Plug-ins loaded flag.
     */
    TBool iPluginsLoaded;
    
    /**
     * Action plug-in array.
     */
    RPointerArray< CFscContactActionPlugin > iActionPlugins;
        
    /**
     * Virtual phonebook contact manager
     */
    CVPbkContactManager& iContactManager;
    
    /**
     * Action Utils.
     * Own.
     */
    CFscActionUtils* iActionUtils;
    
    /**
     * Plugin params
     */
    TFscContactActionPluginParams iPluginParams;
    
    /**
     * Last event that took place - for cooperation with async methods.
     */
    TCasPluginEngineLastEvent iLastEvent;
    
    /**
     * Operation observer.
     * Not own.
     */
    MFscContactActionPluginEngineObserver* iObserver;
    
private: // data for async call of QueryActionsL
    
    /**
     * Action list. Found actions are appended to the list.
     */
    CFscContactActionList* iActionList;
    
    /**
     * Target contact set for which actions are queried.
     */
    MFscContactSet* iContactSet;
    
    /**
     * Flag for stopping query when at least one action found.
     */
    TBool iStopWhenOneActionFound;
    
    /**
     * Action type flags which are used to filter actions.
     */
    TUint64 iActionTypeFlags;
    
    /**
     * Min allowed priority of returned actions.
     */
    TInt iMinPriority;
    
    /**
     * Currently processed plugin in QueryActionsL.
     */
    TInt iCurrentActionPlugin;
    
    /**
     * Currently processed action in QueryActionsL.
     */
    TInt iCurrentAction;
    
    /**
     * Result of QueryActionsL.
     */
    TFscContactActionQueryResult iContactActionQueryResult;

    };

#endif // C_FSCCONTACTACTIONPLUGINENGINE_H
