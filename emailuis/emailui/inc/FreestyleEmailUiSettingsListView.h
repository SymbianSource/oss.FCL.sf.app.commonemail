/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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

// SYSTEM INCLUDES
#include <eiklbv.h>

#include "FreestyleEmailUiViewBase.h"

// FORWARD DECLARATIONS
class CFsEmailSettingsList;
class CFSMailClient;
class CFreestyleEmailUiAppUi;
class CAsyncCallBack;

// CLASS DECLARATION
class CFsEmailSettingsListView : public CFsEmailUiViewBase
	{
public:
	// constructors and destructor
	static CFsEmailSettingsListView* NewL( 
			CFSMailClient& aMailClient,
			CFreestyleEmailUiAppUi* aAppUi,
			CAlfControlGroup& aControlGroup );
	
	static CFsEmailSettingsListView* NewLC( 
			CFSMailClient& aMailClient,
			CFreestyleEmailUiAppUi* aAppUi, 
			CAlfControlGroup& aControlGroup );        
	
	virtual ~CFsEmailSettingsListView();

public:
	// from base class CAknView
	TUid Id() const;
	void HandleCommandL( TInt aCommand );

public:
	// set main list index in parent
	void SetSelectedMainListIndex( TInt aIndex );
	
	// get main list index from parent
	TInt GetSelectedMainListIndex()const;

	void StartMailboxAsyncQueryL();

protected:
	// from base class CAknView
    // <cmail> Toolbar
    /*void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );*/
    // </cmail> Toolbar
    void ChildDoDeactivate();                
	void HandleStatusPaneSizeChange();
	void DynInitMenuPaneL(
			TInt aResourceId, 
			CEikMenuPane* aMenuPan );
	
// <cmail> Toolbar    
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
   
// </cmail> Toolbar    
	
private:
	CFsEmailSettingsListView( CAlfControlGroup& aControlGroup,
	        CFreestyleEmailUiAppUi& aAppUi, CFSMailClient& aMailClient );
    void ConstructL();
    
	void SetupStatusPaneL();
	void CleanupStatusPaneL();
	
	static TInt DisplayCreateQuery( TAny* aViewPtr );
	void DisplayCreateMailboxNoteIfNeededL();	
	
private: 
	CFsEmailSettingsList* iFsEmailSettingsList;
	CFSMailClient& iMailClient;
	TBool iMailboxSettings;
	TInt iContainerListIndex;
	// Async callback for create mailbox query      	
    CAsyncCallBack* iAsyncCallback;	
	};

#endif // FSEMAILSETTINGSLISTVIEW_H			
