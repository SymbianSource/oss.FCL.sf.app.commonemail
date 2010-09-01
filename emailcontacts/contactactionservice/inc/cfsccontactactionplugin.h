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
* Description:  Declaration of class CFscContactActionPlugin.
*
*/


#ifndef C_FSCCONTACTACTIONPLUGIN_H
#define C_FSCCONTACTACTIONPLUGIN_H

#include <e32base.h>

//<cmail>
#include "fsccontactactionservicedefines.h"
#include "tfsccontactactionpluginparams.h"

// FORWARD DECLARATIONS
class MFscContactAction;
class MFscContactSet;
class MFscContactActionPluginObserver;

/**
 *  Contact Action Plugin API.
 *  Defines an ECom interface for Contact Action Plugins.
 *
 *  @since S60 3.1
 */
class CFscContactActionPlugin : public CBase
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @param aPluginUid Implementation uid of plugin to be created
     * @return New instance of CFscContactActionPlugin
     */
    static inline CFscContactActionPlugin* NewL( 
        TUid aPluginUid,
        TFscContactActionPluginParams& aParams );

    /**
    * Destructor.
    */
    virtual ~CFscContactActionPlugin();
    
public: // Public pure virtual methods

    /**
     * Implementation Uid
     *
     * @return Implementation Uid of the plugin
     */
    virtual TUid Uid() const = 0;

    /**
     * Action List
     *
     * @return a const pointer to a array contaning uids of implemented actions
     */
    virtual const CArrayFix<TUid>* ActionList() const = 0;

    /**
     * Get Action
     *   Method returns a reference to requested action or 
     *   leaves with KErrNotFound error code if action is not found.
     *
     * @param aActionUid Action uid
     * @return Reference to requested action. 
     */
    virtual const MFscContactAction& GetActionL( 
        TUid aActionUid ) const = 0;
    
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
        MFscContactActionPluginObserver* aObserver ) = 0;
    
    /**
     * Cancels async method PriorityForContactSetL.
     */
    virtual void CancelPriorityForContactSet() = 0;
    
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
        MFscContactActionPluginObserver* aObserver ) = 0;
    
    /**
     * Cancels async method ExecuteL.
     */
    virtual void CancelExecute() = 0;
                              
protected: // protected data
    
private: // data

    /** iDtor_ID_Key Instance identifier key. When instance of an
     *               implementation is created by ECOM framework, the
     *               framework will assign UID for it. The UID is used in
     *               destructor to notify framework that this instance is
     *               being destroyed and resources can be released.
     */
    TUid iDtor_ID_Key;

    };

#include "cfsccontactactionplugin.inl"
//</cmail>

#endif // C_FSCCONTACTACTIONPLUGIN_H
