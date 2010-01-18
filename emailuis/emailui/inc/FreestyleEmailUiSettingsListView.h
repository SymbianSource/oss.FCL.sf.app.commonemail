/*
* Copyright (c) 2005 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email header file
*
*/

#ifndef FSEMAILSETTINGSLISTVIEW_H
#define FSEMAILSETTINGSLISTVIEW_H

//  SYSTEM INCLUDES
#include <eiklbv.h>

//  INTERNAL INCLUDES
#include "FreestyleEmailUiViewBase.h"

//  FORWARD DECLARATIONS
class CFsEmailSettingsList;
class CFSMailClient;
class CFreestyleEmailUiAppUi;
class CAsyncCallBack;
class CAknStylusPopUpMenu;


// CLASS DECLARATION
class CFsEmailSettingsListView : public CFsEmailUiViewBase
	{
public:	// Constructors and destructor.

	static CFsEmailSettingsListView* NewL( 
			CFSMailClient& aMailClient,
			CFreestyleEmailUiAppUi* aAppUi,
			CAlfControlGroup& aControlGroup );
	
	static CFsEmailSettingsListView* NewLC( 
			CFSMailClient& aMailClient,
			CFreestyleEmailUiAppUi* aAppUi, 
			CAlfControlGroup& aControlGroup );        
	
	virtual ~CFsEmailSettingsListView();


private: // Construction.

	CFsEmailSettingsListView( CAlfControlGroup& aControlGroup,
							  CFreestyleEmailUiAppUi& aAppUi,
							  CFSMailClient& aMailClient );

    void ConstructL();


public: // From base class CAknView.

	/**
	 * @return The UID for this view
	 */
	TUid Id() const;

	/**
	 * Handles commands for this view (override).
	 * @param aCommand The ID of the command to handle.
	 */
	void HandleCommandL( TInt aCommand );


public: // New methods.

	/**
	 * Set the main list index in parent.
	 * @param aIndex The index to set.
	 */
	void SetSelectedMainListIndex( TInt aIndex );

	/**
	 * Returns the main list index from parent.
	 * @return The list index.
	 */
	TInt GetSelectedMainListIndex() const;

	void StartMailboxAsyncQueryL();

	/**
	 * Displays the pop-up menu.
	 * @param aPosition The position of the pop-up menu.
	 */
	void DisplayStylusPopUpMenu( const TPoint& aPosition );
	

protected: // From base class CAknView.

	/**
	 * Handle status pane size change for this view (override).
	 */
	void HandleStatusPaneSizeChange();

	void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPan );
	
  
protected: // From CFsEmailUiViewBase.

	/**
	 *	Handles user actions during activation of the view, 
	 *	such as initializing the content.
	 */
    void ChildDoDeactivate();   

    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
						   TUid aCustomMessageId,
						   const TDesC8& aCustomMessage );


private: // New methods.

	void SetupStatusPaneL();

	/** 
	 * Sets application default title when leaving this view.
	 */
	void CleanupStatusPaneL();
	
	static TInt DisplayCreateQuery( TAny* aViewPtr );

	void DisplayCreateMailboxNoteIfNeededL();	


private: // Data.

	CFsEmailSettingsList* iFsEmailSettingsList;
	CFSMailClient& iMailClient;
	TBool iMailboxSettings;
	TInt iContainerListIndex;

	// Async callback for create mailbox query      	
    CAsyncCallBack* iAsyncCallback;	

    // A long tap detector and a pop-up menu that is invoked by long tap
    // events. Used for e.g. deleting mail boxes.
    CAknStylusPopUpMenu* iStylusPopUpMenu;
	};

#endif // FSEMAILSETTINGSLISTVIEW_H

