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
* Description:  Email plugin for s60 general settings
*
*/


#ifndef __GSEMAILSETTINGS_VIEW_H__
#define __GSEMAILSETTINGS_VIEW_H__

#include <gsbaseview.h>
#include <ConeResLoader.h>

//Forward Declarations
class CGSEmailSettingsContainer;
class CAsyncCallBack;

// Constants
// <cmail> S60 UID update
const TUid KGSEmailSettingsPluginUid = { 0x2001F40C };
// </cmail> S60 UID update

class CGSEmailSettingsView: public CGSPluginInterface
	{
	public: // Constructors and destructor

	    /**
	     * Symbian OS two-phased constructor
	     * @return 
	     */
	    static CGSEmailSettingsView* NewL();

	    /**
	     * Symbian OS two-phased constructor
	     * @return 
	     */
	    static CGSEmailSettingsView* NewLC();

	    /**
	     * Destructor.
	     */
	    ~CGSEmailSettingsView();       


	public: // from CGSPluginInterface
	    /**
	     * Method for checking, if plugin should be visible and used in GS FW.
	     *
	     * On default plugin is visible. Overwrite this function to enable or
	     * disable your plugin dynamically.
	     *
	     * @return ETrue if plugin should be visible in GS.
	     * @return EFalse if plugin should not be visible in GS.
	     */
	    TBool Visible() const;

	    /**
	     * Method for getting caption of this plugin. This should be the
	     * localized name of the settings view to be shown in parent view.
	     * @param aCaption pointer to Caption variable
	     */
	    void GetCaptionL( TDes& aCaption ) const;

	    /**
	     * Method for getting the type of this GS plugin. We return EGSItemTypeSingleLargeDialog
	     * to tell General settings that this is non-view-type plugin, and HandleSelection() should
	     * be called when our item is selected from the listbox.
	     */
	    TGSListboxItemTypes ItemType();

	    /**
	     * General Settings calls this method when Email settings item is selected by the user.
	     */
	    void HandleSelection( const TGSSelectionTypes aSelectionType );

	    /**
	     * Gets the icon to be shown in the General Settings list on the Email settings item.
	     */
	    CGulIcon* CreateIconL( const TUid aIconType );

	    
	public: // from CAknView  

	    /**
	     * @return Returns id of the view.
	     */       
	    TUid Id() const;

	    /** An empty implementation must be provided here even though this is not a view type GS plugin */
	    void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage );

	    /** An empty implementation must be provided here even though this is not a view type GS plugin */
        void DoDeactivate();
		
        
	protected: // constructors

	    /**
	     * C++ default constructor.
	     */
	    IMPORT_C CGSEmailSettingsView();

	    /**
	     * Symbian OS default constructor.
	     */
	    void ConstructL();

	   
    private: // data
	    
	    // Caption text shown in general settings application
	    HBufC* iCaptionText;
	};
	


#endif // __MYSETTINGSFRMWRK_VIEW_H__
