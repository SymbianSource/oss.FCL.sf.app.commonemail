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
* Description:  Definition of the class CFscCallPluginImpl.
*
*/


#ifndef C_FSCCALLPLUGINIMPL_H
#define C_FSCCALLPLUGINIMPL_H

//<cmail>
#include "cfsccontactactionplugin.h"
//</cmail>
#include "mfscreasoncallback.h"
#include "mfsccontactset.h"
#include "mfsccontactsetobserver.h"

// FORWARD DECLARATIONS
class CFscContactAction;
class CAiwServiceHandler;
class MFscContactAction;
class MFscContactActionPluginObserver;
class MAiwNotifyCallback;

// CONSTANTS DECLARATIONS
const TInt KMaxLengthOfNumber = 100;

/**
 *  Call plugin implementation.
 *  Contains implementation of Call actions.
 *
 *  @lib fsccallplugin.lib
 *  @since S60 3.1
 */
class CFscCallPluginImpl : public CFscContactActionPlugin,
                           public MFscReasonCallback,
                           public MFscContactSetObserver,
                           public MAiwNotifyCallback
    {
    
    /**  
     * States of operations
     */
    enum TActionPluginLastEvent 
        {
        EActionEventIdle,
        EActionEventCanExecuteLaunched,
        EActionEventCanExecuteFinished,
        EActionEventContactRetrieve,
        EActionEventGroupRetrieve,
        EActionEventExecuteLaunched,
        EActionEventCanceled
        };
    
public:

    /**
     * Two-phased constructor.
     *
     * @param aParams contact action plugin parameters
     * @return New instance of CFscCallPluginImpl
     */
    static CFscCallPluginImpl* NewL( TAny* aParams );
   
    /**
     * Destructor.
     */
    virtual ~CFscCallPluginImpl();
    
public: // From CFscContactActionPlugin

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
        TUid aActionUid ) const;
            
    /**
     * Method for quering action's priority and visibility in current context with given contacts
     *
     * @param aActionUid Action uid
     * @param aContactSet Target contact set
     * @param aGroupList List containing target groups and their contacts
     * @param aActionMenuVisibility Action's visibility in action menu. 
     *   Visibility information is updated by the method.
     * @param aOptionsMenuVisibility Action's visibility in options menu
     *   Visibility information is updated by the method.
     * @return Priority of the action
     */
    virtual void PriorityForContactSetL( 
        TUid aActionUid,
        MFscContactSet& aContactSet,
        TFscContactActionVisibility& aActionMenuVisibility,
        TFscContactActionVisibility& aOptionsMenuVisibility,
        MFscContactActionPluginObserver* aObserver );
        
    /**
     * Execute action. Asynchronous method.
     *
     * @param aActionUid Action uid of the action to be executed.
     * @param aContactSet Target contact set.
     * @param aObserver operation observer.
     */
    virtual void ExecuteL( 
        TUid aActionUid,
        MFscContactSet& aContactSet,
        MFscContactActionPluginObserver* aObserver );
    
public: // From MFscReasonCallback

    /**
     * @see MFscReasonCallback::GetReasonL
     */
    virtual void GetReasonL(
        TUid aActionUid,
        TInt aReasonId,
        HBufC*& aReason ) const;
    
public: //From base class MFscContactSetObserver
    
    /**
     * @see MFscContactSetObserver::NextContactComplete
     */
    virtual void NextContactComplete( MVPbkStoreContact* aContact );
    
    /**
     * @see MFscContactSetObserver::NextContactFailed
     */
    virtual void NextContactFailed( TInt aError );
    
    /**
     * @see MFscContactSetObserver::NextGroupComplete
     */
    virtual void NextGroupComplete( MVPbkStoreContact* aContact );
    
    /**
     * @see MFscContactSetObserver::NextGroupFailed
     */
    virtual void NextGroupFailed( TInt aError );
    
    /**
     * @see MFscContactSetObserver::GetGroupContactComplete
     */
    virtual void GetGroupContactComplete( MVPbkStoreContact* aContact );
    
    /**
     * @see MFscContactSetObserver::GetGroupContactFailed
     */
    virtual void GetGroupContactFailed( TInt aError );
    
    /**
     * @see CFscContactActionPlugin::CancelPriorityForContactSet
     */
    virtual void CancelPriorityForContactSet();
     
    /**
     * @see CFscContactActionPlugin::CancelExecute
     */
    virtual void CancelExecute();
    
public: //from MAiwNotifyCallback
	
	/**
	 * @see MAiwNotifyCallback :: HandleNotifyL
	 */
	
	virtual TInt HandleNotifyL(
        TInt aCmdId,
        TInt aEventId,
        CAiwGenericParamList& aEventParamList,
        const CAiwGenericParamList& aInParamList);

private: // methods

    /**
     * Makes call using Dial AIW API
     *
     * @param aNumber phone number to make a call
     * @param aCallType specifies voice, video or voip
     */      
    void MakeAiwCallL( const TDesC& aNumber, CAiwDialData::TCallType aCallType );  
    
    /**
     * Helper method to check if action can be executed with 
     * given contacts in current context
     *
     * @param aActionId Action Id
     * @param aContactSet contains contact list and group lists
     * @return KErrNone if action can be executed,
     *  otherwise system wide error code
     */
    void CanExecuteL(
        TUid aActionUid,
        MFscContactSet& aContactSet );

    /**
     * Updates action icons
     *
     * @param aActionId Action Id
     */
    void UpdateActionIconL( TUid aActionUid );
    
private: // constructors

    /**
     * Constructor.
     *
     * @param aParams contact action plugin parameters
     */
    CFscCallPluginImpl(
        const TFscContactActionPluginParams& aParams );

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
        
private: // data

    /**
     * Action plugin parameters
     */
    TFscContactActionPluginParams iParams;
    
    /**
     * Resource handle
     */
    TInt iResourceHandle;

    /**
     * Own: Contact action for Gsm call
     */      
    CFscContactAction* iContactCallGsmAction;

    /**
     * Own: Contact action for video call
     */    
    CFscContactAction* iContactCallVideoAction;
  
    /**
     * Own: Contact action for conf number call
     */    
    CFscContactAction* iContactCallConfNumAction;

    /**
     * Own: Contact action for voip call
     */    
    CFscContactAction* iContactCallVoipAction;

    /**
     * Own: Contact action array
     */    
    CArrayFixFlat<TUid>* iActionList;

    /**
     * Own: AIW service handler
     */
    CAiwServiceHandler* iAiwServiceHandler;
    
    //-----------------------------------
  
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
     * Is ExecuteL or PriorityForContactSetL launched
     */
    TBool iIsExecute;
    
    /**
     * Variable used to check if preferred call is a voice or internet call
     * (only when VOIP is set as 'default' in Contacts application and
     * user selects 'Voice call' from Options or Action Menu)
     */
    TBool iPreferredCallValueChanged;
    
    };

#endif // C_FSCCALLPLUGINIMPL_H
