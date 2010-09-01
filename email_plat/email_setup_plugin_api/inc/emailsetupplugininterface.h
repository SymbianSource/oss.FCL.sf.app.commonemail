/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: ECom interface definition for EmailSetupPlugin
*     		   Interface to be implemented by different email protocols.
*			   SettingWizard can initiate account creation using these interfaces 
*
*/


#ifndef _EMAIL_SETUP_PLUGIN_INTERFACE__
#define _EMAIL_SETUP_PLUGIN_INTERFACE__

#include <e32base.h>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS
class TEmailSetupPluginParam;
class CGulIcon;

 //Interface UID for EmailSetupPlugin
const TUid KEmailSetupPluginIFUid = {0x2001FCFA};

//Strings to identify plugins
_LIT8(KPopImapPluginImplDefaultData,"email");


/**
 * Interface class for handling callback when request is completed.
 * One instance of this interface will be sent with the Execute request.
 * The callback method of this interface should be called when the client finished
 * the execution.
 */
class MEmailSetupPluginObserver
    {
    public:    
    
        /**
         * Identifies status of the plugin at the moment of completing execution
         */
        enum TPluginEndResultStatus
            {
            EUndefined,
            ECompleted,
            ECanceled,
            ERejected,
            ESuspended, 
            EFailed
            };
    
    public:

        /** 
         * Client should call this method to notify about request completion. 
         * @param aData - e-mail account data (optional, can be empty)
         *              - data is xml format
         * @param aStatus - the state in what the plugin ended should be communicated to the caller
         *                 - can have one of the TPluginEndResultStatus enum values
         */
        virtual void HandleExecuteFinishedL( 
                                const TDesC8& aData, 
                                const TPluginEndResultStatus aStatus ) = 0;

    };



/**
 * ECom interface class for handling Email settings
 *
 * Plugins are always called inside an Avkon application (CAknViewAppUi).
 * It is recommended that the plugin implementation 
 * is using dialog architecture for handling UI issues. 
 * 
 */
class CEmailSetupPluginInterface : public CBase
    {
    public:
        /**
         * Identifies the status of the plugin
         */
        enum TPluginStatus
            {
            ENonUsable, 
            EOkHidden, //plugin can be used, but should not be shown in a menu list
            EOkShow   //plugin can be used and should be shown in a menu
            };  
            
        /**
         * Two-phased contructor
         *
         * @param aImplementationUid ImplementationUid
         * @param aParams Application parameters
         * @return Instance of CEmailSetupPluginInterface
         */
        static CEmailSetupPluginInterface* NewL(     
                            const TUid& aImplementationUid,
                            TEmailSetupPluginParam& aParams ); 
		/**
         * Two-phased contructor
         *
         * @param aMatchString - if the default data is known by the framework,
		 *              		 it can be used for instantiating the plugin.
		 * (See examples of known id_strings in the constant definition section)
         * @param aParams Application parameters
         * @return Instance of CEmailSetupPluginInterface
         */
		static CEmailSetupPluginInterface* NewL(
			    const TDesC8& aMatchString, 
			    TEmailSetupPluginParam& aParams );
        /**
         * Destructor.
         */
        virtual ~CEmailSetupPluginInterface();

        /**
         * Method for querying the status of the plugin.
         * Framework calles this method to decide is it OK to show in a list or 
         * use this plugin.
         * @return - status of the plugin (TPluginStatus enum values)
         */
        virtual TPluginStatus GetPluginStatus() = 0;  
        
		/**
         * Method for querying if maximum number of account is reached.
         * Framework calles this method before it starts to use this plugin 
         * and shows a message to the user that no more accunts can be created.
         * @return - ETrue if no more accounts can be created with the plugin
         *         - EFalse otherwise
         */
		virtual TBool IsMaxAccountNrReached() = 0;
        /**
         * Method to invoke plugin execution.
         * 
         * Caller needs to be notified of completion and status via 
         * MEmailSetupPluginObserver::HandleExecuteFinishedL()
         * @param aData - available e-mail account data in XML format
         *              - data is xml format 
         * @param aObserver - observer for handling callback when operation is finished
         */
        virtual void ExecuteL( const TDesC8& aData, MEmailSetupPluginObserver& aObserver ) = 0;
        
        /**
         * Flow initializer method. ( Can have empty implementaion.)
         * Framework will call this method generally before calling ExecuteL().
         * Plugin instances will not be deleted between two ExecuteL() calls  
         * during the same session. 
         * (Justification: in some rare cases plugin needs to continue from the 
         * point it was left during the previous execution, 
         * in that case this method will not be called.)
         */
        virtual void InitializeFlowL() = 0;
        
        /**
         * Returns icon to be used in list view.
         *
         * @return icon
         */
        virtual CGulIcon* IconLC() = 0; 
        
        /**
         * Return plug-in localized resource, list text
         *     
         * @return Localized buffer
         */
        virtual HBufC* ListTextLC() = 0;  

        /**
         * Return plug-in localized resource, description 
         *     
         * @return Localized buffer
         */
        virtual HBufC* DescriptionLC() = 0;  
	

    private:
    
        // Unique instance identifier key
        TUid iDtor_ID_Key;
    };

#include "emailsetupplugininterface.inl"

#endif // _EMAIL_SETUP_PLUGIN_INTERFACE__
