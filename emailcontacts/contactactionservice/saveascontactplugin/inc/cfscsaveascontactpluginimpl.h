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
* Description:  Definition of the class CFscSaveAsContactPluginImpl.
 *
*/


#ifndef CFSCSAVEASCONTACTPLUGINIMPL_H
#define CFSCSAVEASCONTACTPLUGINIMPL_H

//<cmail>
#include "cfsccontactactionplugin.h"
//</cmail>
#include <MVPbkContactObserver.h>
#include <MVPbkContactStoreObserver.h>
#include "mfscreasoncallback.h"
#include "mfsccontactsetobserver.h"
//<cmail>
#include "FscActionPluginUtils.h"
//</cmail>

// FORWARD DECLARATIONS
class CFscContactAction;
class CAknWaitDialog;

/**
 *  SaveAsContact plugin implementation.
 *  Contains implementation of Save as contact action plugin.
 *
 *  @lib fscsaveascontactplugin.lib
 *  @since S60 3.1
 */
class CFscSaveAsContactPluginImpl : public CFscContactActionPlugin,
									public MFscReasonCallback, 
									public MFscContactSetObserver,
									public MVPbkContactObserver,
									public MVPbkContactStoreObserver
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aParams contact action plugin parameters
     * @return New instance of CFscSaveAsContactPluginImpl
     */
    static CFscSaveAsContactPluginImpl* NewL(
            TAny* aParams);

    /**
     * Destructor.
     */
    virtual ~CFscSaveAsContactPluginImpl();

public:
    // From CFscContactActionPlugin

    /**
     * @see CFscContactActionPlugin::Uid
     */
    virtual TUid Uid() const;

    /**
     * @see CFscContactActionPlugin::ActionList
     */
    virtual const CArrayFix<TUid>* ActionList() const;

    /**
     * @see CFscContactActionPlugin::GetActionL
     */
    virtual const MFscContactAction& GetActionL(
            TUid aActionUid) const;

    /**
     * @see CFscContactActionPlugin::PriorityForContactSetL
     */
    virtual void PriorityForContactSetL(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            TFscContactActionVisibility& aActionMenuVisibility,
            TFscContactActionVisibility& aOptionsMenuVisibility,
            MFscContactActionPluginObserver* aObserver);

    /**
     * @see CFscContactActionPlugin::CancelPriorityForContactSet
     */
    virtual void CancelPriorityForContactSet();

    /**
     * @see CFscContactActionPlugin::ExecuteL
     */
    virtual void ExecuteL(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            MFscContactActionPluginObserver* aObserver);

    /**
     * @see CFscContactActionPlugin::CancelExecute
     */
    virtual void CancelExecute();
public:
    // From MFscReasonCallback

    /**
     * @see MFscReasonCallback::GetReasonL
     */
    virtual void GetReasonL(
            TUid aActionUid,
            TInt aReasonId,
            HBufC*& aReason) const;

public:
    // From MFscContactSetObserver
    /**
     * @see MFscContactSetObserver::NextContactComplete
     */
    virtual void NextContactComplete(
            MVPbkStoreContact* aContact);

    /**
     * @see MFscContactSetObserver::NextContactFailed
     */
    virtual void NextContactFailed(
            TInt aError);

    /**
     * @see MFscContactSetObserver::NextGroupComplete
     */
    virtual void NextGroupComplete(
            MVPbkStoreContact* aContact);

    /**
     * @see MFscContactSetObserver::NextGroupFailed
     *
     */
    virtual void NextGroupFailed(
            TInt aError);

    /**
     * @see MFscContactSetObserver::GetGroupContactComplete
     */
    virtual void GetGroupContactComplete(
            MVPbkStoreContact* aContact);

    /**
     * @see MFscContactSetObserver::GetGroupContactFailed
     */
    virtual void GetGroupContactFailed(
            TInt aError);
public:
	
	//From MVPbkContactStoreObserver
	
	/**
	 * @see MVPbkContactStoreObserver::StoreReady
	 */
     void StoreReady(MVPbkContactStore& aContactStore);

     /**
	  * @see MVPbkContactStoreObserver::StoreUnavailable
	  */
     void StoreUnavailable(MVPbkContactStore& aContactStore, 
             TInt aReason);

     /**
	  * @see MVPbkContactStoreObserver::StoreUnavailable
	  */
     void HandleStoreEventL(
             MVPbkContactStore& aContactStore, 
             TVPbkContactStoreEvent aStoreEvent);
     
private:
    // methods

    /**
     * Saves contact to default Pbk2 store
     *
     * @param aContactStore to be saved into contact database
     */
    void SaveToContactDatabaseL(
            MVPbkStoreContact* aStoreContact);

    /**
     * Prepares store for update
     */
    void PrepareStoreL();
    
    /**
     * Checks if store is valid for saving contacts from RCL
     */
    // <cmail>
    TBool IsValidStoreL();
    // </cmail>

    /**
     * Helper method to check if action can be executed with 
     * given contacts in current context
     *
     * @param aActionId Action Id
     * @param aContactSet contains contact list and group lists
     */
    void CanExecuteL(
            TUid aActionUid,
            MFscContactSet& aContactSet);

    /**
     * Updates action icon
     *
     * @param aActionId Action Id
     */
    void UpdateActionIconL(
            TUid aActionUid);

    /**
     * @see MVPbkContactObserver::ContactOperationCompleted
     * 
     */

    virtual void ContactOperationCompleted(
            TContactOpResult aResult);

    /**
     * @see MVPbkContactObserver::ContactOperationFailed
     * 
     */

    virtual void ContactOperationFailed(
            TContactOp aOpCode,
            TInt aErrorCode,
            TBool aErrorNotified);
private:

    /**
     * Constructor.
     *
     * @param aParams contact action plugin parameters
     */
    CFscSaveAsContactPluginImpl(
            const TFscContactActionPluginParams& aParams);

    /**
     * Second phase constructor.
     */
    void ConstructL();
    
    /**
     * Restores members related to state machine
     */
    void ResetData();
    
    /**
     * Resumes plugin processing
     * Resumes ExecuteL or PriorityForContactSetL execution
     * 
     * @param aActionUid action UID
     * @param aContactSet set of contacts
     * @param aIsExecute is ExecuteL or PriorityForContactSetL launched
     */
    void ResumeAsync(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            TBool aIsExecute );
    
    /**
     * Dismisses wait note
     */
    void DismissWaitNote();

private:
    // data

    /**
     * Action plugin parameters
     */
    TFscContactActionPluginParams iParams;

    /**
     * Resource handle
     */
    TInt iResourceHandle;

    /**
     * Own: save as contact action
     */
    CFscContactAction* iContactSaveAsContactAction;

    /**
     * Own: Contact action array
     */
    CArrayFixFlat<TUid>* iActionList;

    // For async implementation
    
    /**
     * Action Uid
     */
    TUid iActionUid;
    
    /**
     * Contact set
     */
    MFscContactSet* iContactSet;
    
    /**
     * Action menu visbility
     */
    TFscContactActionVisibility* iActionMenuVisibility;
    
    /**
     * Options menu visbility
     */
    TFscContactActionVisibility* iOptionsMenuVisibility;
    
    /**
     * Plug-in observer
     */
    MFscContactActionPluginObserver* iPluginObserver;
    
    /**
     * Current state of state machine 
     */
    TActionPluginLastEvent iLastEvent;
    
    /**
     * Action priority
     */
    TInt iActionPriority;
    
    /**
     * Can display action
     */
    TInt iCanDisplay;

    /**
     * Retrieved store contact
     */
    MVPbkStoreContact* iRetrievedStoreContact;

    /**
     * New contact which will be saved to the phone memory
     */
    MVPbkStoreContact* iNewStoreContact;
    
    /**
     * Is ExecuteL or PriorityForContactSetL launched
     */
    TBool iIsExecute;
    
    /**
     * Waiting dialog launched when contact is being saved to the database
     */
    CAknWaitDialog* iWaitDialog;
    
    /**
     * Ref: Target store
     */
	MVPbkContactStore* iTargetStore;
    };

#endif // CFSCSAVEASCONTACTPLUGINIMPL_H
