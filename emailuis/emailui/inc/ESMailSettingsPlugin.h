/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares plugin settings interface.
*
*/


#ifndef _ESMAILSESETTINGSPLUGIN_H_
#define _ESMAILSESETTINGSPLUGIN_H_

// SYSTEM INCLUDES
#include <aknview.h>

/**
 * 	In addition to the pure virtuals below, the plug-in implementers must implement
 *	following methods of CAknView
 *		
 *	 -DoActivate()
 *	  * The EMail UI will pass the account id object (provided by the FW) and the subview id
 *	    packaged to a package buffer in the custom message. The implementations should save the
 *	    aPrevViewId to be able to reactivate the EMail UI settings view when the user wants to
 *	    return from the subview. The custom message id contains enumeration 
 * 		TESEmailSettingsPluginCustomMessageId
 *   -DoDeactivate()
 *	 -Id()
 *	  * The plug-ins should return their implementation UID as the view's id to ensure
 *	  	unique ids. The EMail UI will also use this id to map the accounts to correct settings
 *	  	plug-ins with the help of FWs Settings Info API (which returns the settings UI plug-in
 *	  	implementation UID for an account).
 * 
 */

// INTERFACE DECLARATION
class CESMailSettingsPlugin : public CAknView
{
	public:
		/**
		 * Creates instance of a CESMailSettingsPlugin
		 * 
		 * @param aImplementationUid protocol plugin Id reveiced from Freestyle Framework
		 * @return CESMailSettingsPlugin pointer
		 */
		inline static CESMailSettingsPlugin* NewL( const TUid& aImplementationUid );
		
		/**
		 * Destructor
		 */
		inline virtual ~CESMailSettingsPlugin();
		
	public:
		/**
		 * Returns the number of subviews visible under the mail settings. The subviews will 
		 * be presented in the settings UI in the same order as their ids (0, 1, 2, 3, ...).
		 */
		virtual TInt MailSettingsSubviewCount() = 0;
		
		/** 
		 * Returns the caption for a mail settings subview. 		 
		 * 
		 * @param aAccountId Freestyle email account information
		 * @param aSubviewIndex index of a settings subview
		 * @param aLongCaption if ETrue long version of caption is returned
		 * @return TPtrC to caption

		 */
		virtual TPtrC MailSettingsSubviewCaption( 
				TFSMailMsgId aAccountId, 
				TInt aSubviewIndex, 
				TBool aLongCaption ) = 0;
		
		/**
		 * Returns ETrue if the protocol can handle syncing of PIM data (calendar, contacts, etc...), EFalse if not.
		 * Returning ETrue will make the protocol visible in the Personal Information Management / PIM Source radio selection
		 */
		virtual TBool CanHandlePIMSync() = 0;
		
		/**
		 * Returns the localized protocol name that is shown in the 
		 * Personal Information Management / PIM Source radio selection
		 */
		virtual TPtrC LocalizedProtocolName() = 0;
		
		/**
		 * Returns ETrue if the protocol is selected for PIM sync, EFalse if not. The information is used
		 * to show the correct protocol name in the settings UI
		 */
		virtual TBool IsSelectedForPIMSync() = 0;
		
		/**
		 * Selects or disables the protocol for PIM sync.
		 */
		virtual void SelectForPIMSync( TBool aSelectForSync ) = 0;

		/**
		 * Returns the number of subviews visible under the PIM settings. The subviews will 
		 * be presented in the settings UI in the same order as their ids (0, 1, 2, 3, ...).
		 */
		virtual TInt PIMSettingsSubviewCount() = 0;
		
		/**
		 * Returns the caption for a PIM settings subview. 
		 * 
		 * @param aAccountId Freestyle email account information
		 * @param aSubviewIndex index of a settings subview
		 * @param aLongCaption if ETrue long version of caption is returned
		 * @return TPtrC to caption
		 */
		virtual TPtrC PIMSettingsSubviewCaption( 
				TFSMailMsgId aAccountId, 
				TInt aSubviewIndex, 
				TBool aLongCaption ) = 0;
		
		// DATA TYPES
	public:
		/**
		 * Container for the data needed to activate 
		 * a subview in a setings UI plug-in.
		 * This class gets passed in the custom message 
		 * part of the view activation message. 
		 */
		class TSubViewActivationData
			{
			public:
                            //<cmail> Initialize variables
			    TSubViewActivationData():
			    iPimAccount(EFalse), iLaunchedOutsideFSEmail(EFalse), iLaunchFolderSettings(EFalse)
			    {
			    //intentionally left empty
			    };
                            //</cmail>
				TFSMailMsgId iAccount;
				TInt iSubviewId;
				TBool iPimAccount;
				TBool iLaunchedOutsideFSEmail;
				TBool iLaunchFolderSettings; //<cmail>
			};
			
		enum TESEmailSettingsPluginCustomMessageId
			{
			EActivateMailSettingsSubview,
			EActivatePIMSettingsSubview
			};
	private:
		/**
		 * Unique instance identifier key (needed for ECom)
		 */
		TUid iDtor_ID_Key;
};

#include "ESMailSettingsPlugin.inl"

#endif /*_ESMAILSESETTINGSPLUGIN_H_*/
