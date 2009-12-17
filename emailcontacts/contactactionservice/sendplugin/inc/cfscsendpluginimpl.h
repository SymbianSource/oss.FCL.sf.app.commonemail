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
* Description:  Definition of the class CFscSendPluginImpl.
*
*/


#ifndef CFSCSENDPLUGINIMPL_H
#define CFSCSENDPLUGINIMPL_H

//<cmail>
#include "cfsccontactactionplugin.h"
#include "mfscreasoncallback.h"
#include "mfsccontactsetobserver.h"
//</cmail>

// FORWARD DECLARATIONS
class CSendUi;
class CFscAttachmentFile;
class CMessageData;
class MFscContactActionPluginObserver;

/**
 *  Send plugin implementation.
 *  Contains implementation of send actions.
 *
 *  @lib fscsendplugin.lib
 *  @since S60 3.1
 */
class CFscSendPluginImpl : public CFscContactActionPlugin,
                            public MFscReasonCallback,
                            public MFscContactSetObserver
    {
    
    /**  
     * States of operations
     */
    enum TActionPluginLastEvent 
        {
        EActionEventIdle,
        EActionEventCanExecuteLaunched,
        EActionEventCanExecuteProcessContact,
        EActionEventCanExecuteProcessGroup,
        EActionEventCanExecuteProcessGroupMember,
        EActionEventCanExecuteFinished,
        EActionEventCanExecuteContactRetrieve,
        EActionEventCanExecuteGroupRetrieve,
        EActionEventContactAvailableLaunched,
        EActionEventContactAvailableProcess,
        EActionEventContactAvailableFinished,
        EActionEventContactAvailableContactRetrieve,
        EActionEventExecuteContactRetrieve,
        EActionEventExecuteGroupRetrieve,
        EActionEventExecuteProcessContact,
        EActionEventExecuteProcessGroup,
        EActionEventExecuteProcessGroupMember,
        EActionEventExecuteFinished,
        EActionEventCanceled
        };
    
    
public:

    /**
     * Two-phased constructor.
     *
     * @param aParams contact action plugin parameters
     * @return New instance of CFscSendPluginImpl
     */
    static CFscSendPluginImpl* NewL( TAny* aParams );
   
    /**
     * Destructor.
     */
    virtual ~CFscSendPluginImpl();
    
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
        TUid aActionUid ) const ;
            
    /**
     * @see CFscContactActionPlugin::PriorityForContactSetL
     */
    virtual void PriorityForContactSetL( 
            TUid aActionUid,
            MFscContactSet& aContactSet,
            TFscContactActionVisibility& aActionMenuVisibility,
            TFscContactActionVisibility& aOptionsMenuVisibility,
            MFscContactActionPluginObserver* aObserver );
        
    /**
     * @see CFscContactActionPlugin::ExecuteL
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
    
private: // methods

    /**
     * Sends unified/audio message using SEND UI API
     *
     * @param aActionUid Uid of action to be performed
     * @param aMessageData list of addresses
     */      
    void SendToSelectedMembersL(
        TUid aActionUid,
        const CMessageData* aAddressList );

    /**
     * Returns true if aContactSet contains atleast one contact
     *
     * @param aContactSet contact set to process
     */
    void ContactAvailableL( MFscContactSet& aContactSet );

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
    
    /**
     * Add contact
     *
     * @param aActionId Action Id
     * @param aAddressList list of addresses
     * @param aContact Contact data
     */
    TInt AddContactL( 
        TUid aActionUid,
        CMessageData& aAddressList,
        MVPbkStoreContact& aContact );
    
    /**
     * Add postcard contact
     *
     * @param aAddressList list of contacts
     * @param aContact Contact data
     */
    void AddPostcardContactL( 
        CMessageData& aAddressList,
        const MVPbkStoreContact& aContact );
    
    /**
     * Write data to postcard attachment file
     *
     * @param aContact Contact data
     * @param aAttachmentFile attachment file
     */
    void WritePostcardAttachmentContentL(
        const MVPbkStoreContact& aContact, 
        CFscAttachmentFile& aAttachmentFile );
    
private: // constructors

    /**
     * Constructor.
     *
     * @param aParams contact action plugin parameters
     */
    CFscSendPluginImpl( 
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
     * Checks action priority and informs observer
     * 
     * @param aActionUid action UID
     */
    void CheckPriority( TUid aActionUid );
    
    /**
     * Move Group iterator to specified index
     * 
     * @param aContactSet - set of contacts
     * @param aIndex - specified index
     * @param aError - result
     */
    void MoveGroupToIndexL( MFscContactSet& aContactSet,
            TInt aIndex, TInt& aError );
            
    /**
     * Checks if contact is member of selected groups 
     */
    TBool IsAnyGroupMemberL( MVPbkStoreContact& aContact );
    
    /**
     * Check maximum recipients value
     */
    TInt MaxMsgRecipientsL();
    
    
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
     * Own: SMS and MMS action
     */      
    CFscContactAction* iContactMsgAction;

    /**
     * Own: Audio message action
     */    
    CFscContactAction* iContactAudioMsgAction;

    /**
     * Own: Send Postcard action
     */    
    CFscContactAction* iContactPostcardAction;

    /**
     * Own: Send Email action
     */    
    CFscContactAction* iContactEmailAction;

    /**
     * Own: Contact action array
     */    
    CArrayFixFlat<TUid>* iActionList;
    
    /**
     * CFscAttachmentFile
     */
    CFscAttachmentFile* iAttachmentFile;
    
    /**
     * Action Uid
     */
    TUid iActionUid;
    
    /**
     * Can display action
     */
    TInt iCanDisplay;
    
    /**
     * Is contact available
     */
    TBool iIsContactAvailable;
    
    //TBool is
    
    /**
     * Retrieved store contact
     */
    MVPbkStoreContact* iRetrievedStoreContact;
    
    /**
     * Retrieved store contact
     */
    MVPbkStoreContact* iRetrievedStoreGroup;
    
    /**
     * Retrieved group member - own
     */
    MVPbkStoreContact* iRetrievedGroupMember;
    
    /*
     * Count of current group
     */
    TInt iGroupMembersCount;
    
    /*
     * Current group member index
     */
    TInt iGroupMemberIndex;
    
    /**
     * Action menu visbility
     */
    TFscContactActionVisibility* iActionMenuVisibility;
  
    /**
     * Options menu visbility
     */
    TFscContactActionVisibility* iOptionsMenuVisibility;
    
    /**
     * Contact set
     */
    MFscContactSet* iContactSet;
    
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
     * Is ExecuteL or PriorityForContactSetL launched
     */
    TBool iIsExecute;
    
    /**
     * Number type
     */
    TContactNumberType iNumberType;
    
    /**
     * List of recipients addresses
     */
    CMessageData* iAddressList;
    
    /**
     * Missing contacts counter
     */
    TInt iMissingCount;
    
    /**
     * Last proccessed group in interator
     */
    MVPbkContactLink* iLastGroupLink;
 
    /**
     * Group iterator index
     */
    TInt iGroupIteratorPosition;
    
    /**
     * Maximum message recipients
     */
    TInt iMaxRecipients;
    
    /**
     * Recipients counter
     */ 
    TInt iRecipientsCounter;
    };

#endif // CFSCSENDPLUGINIMPL_H
