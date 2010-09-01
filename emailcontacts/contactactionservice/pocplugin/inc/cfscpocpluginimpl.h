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
* Description:  Definition of the class CFscPocPluginImpl.
*
*/


#ifndef C_FSCPOCPLUGINIMPL_H
#define C_FSCPOCPLUGINIMPL_H

//<cmail>
#include "cfsccontactactionplugin.h"
#include "mfscreasoncallback.h"
#include <MVPbkContactObserver.h>
#include "mfsccontactsetobserver.h"
#include "FscActionPluginUtils.h"
//</cmail>

// FORWARD DECLARATIONS
class CFscContactAction;
class CAiwServiceHandler;
class MFscContactActionPluginObserver;
    
/**
 *  PoC ( PTT over Cellular ) plugin implementation.
 *  Contains implementation of PoC plugin.
 *
 *  @lib fscpocplugin.lib
 *  @since S60 3.1
 */
class CFscPocPluginImpl : 
    public CFscContactActionPlugin,
    public MFscReasonCallback,
    public MFscContactSetObserver
    {
    
public:

    /**
     * Two-phased constructor.
     *
     * @param aParams contact action plugin parameters
     * @return New instance of CFscPocPluginImpl
     */
    static CFscPocPluginImpl* NewL( TAny* aParams );
   
    /**
     * Destructor.
     */
    virtual ~CFscPocPluginImpl();
    
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
     * @see CFscContactActionPlugin::PriorityForContactSetL
     */
    virtual void PriorityForContactSetL( 
        TUid aActionUid,
        MFscContactSet& aContactSet,
        TFscContactActionVisibility& aActionMenuVisibility,
        TFscContactActionVisibility& aOptionsMenuVisibility,
        MFscContactActionPluginObserver* aObserver );
    
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
        MFscContactActionPluginObserver* aObserver );
    
    /**
     * @see CFscContactActionPlugin::CancelExecute
     */
    virtual void CancelExecute();

public: // From MFscReasonCallback

    /**
     * @see MFscReasonCallback::GetReasonL
     */
    virtual void GetReasonL(
        TUid aActionUid,
        TInt aReasonId,
        HBufC*& aReason ) const;
    
private:

    /**
     * Makes call using Dial AIW API
     */      
    void MakeAiwCallL();

    /**
     * Helper method to check if action can be executed with 
     * given contacts in current context
     *
     * @param aActionId Action Id
     * @param aContactSet     
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
     * Resets data
     */
    void ResetData();

private: // constructors

    /**
     * Constructor.
     *
     * @param aParams contact action plugin parameters
     */
    CFscPocPluginImpl( const TFscContactActionPluginParams& aParams );

    /**
     * Second phase constructor.
     */
    void ConstructL();
        
    /**
     * Process retrieved contact.
     */
    void ProcessContactL( MVPbkStoreContact* aContact );
    
    /**
     * Process retrieved group.
     */
    void ProcessGroupL( MVPbkStoreContact* aContact );
    
    /**
     * Process retrieved group contact.
     */
    void ProcessGroupContactL( MVPbkStoreContact* aContact );

    
    
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
     * Own: Contact action for ptt call
     */      
    CFscContactAction* iContactPttAction;

    /**
     * Own: Contact action array
     */    
    CArrayFixFlat<TUid>* iActionList;

    /**
     * Own: AIW service handler
     */
    CAiwServiceHandler* iAiwServiceHandler;
    
    /**
     * Plug-in observer
     */
    MFscContactActionPluginObserver* iPluginObserver;
    
    /**
     * Phone numbers array
     */
    CDesCArray* iSelectedPhNumArray;
    
    /**
     * Counter for contacts without POC number
     */
    TInt iMissingCount;
    
    /**
     * Indicates if plugin is running in execution mode
     */
    TBool iExecute;
    
    /**
     * Identifier of last event
     */
    TActionPluginLastEvent iLastEvent;
    
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
     * Action Uid
     */
    TUid iActionUid;
    
    /**
     * Index of Store Contact In the group
     */
    TInt iSCInGroup;
    
    /**
     * Store contact group pointer
     */
    MVPbkStoreContact* iGroup;
    
    /**
     * Indicates if there is at least one PTT address in contact set
     */
    TBool iIsNmbAvailable;
    
    /**
     * Index of group
     */
    TInt iGroupIndex;
    };

#endif // C_FSCPOCPLUGINIMPL_H
