/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef GLOBALSETTINGSSETTINGITEMLISTVIEW_H
#define GLOBALSETTINGSSETTINGITEMLISTVIEW_H

// SYSTEM INCLUDES
//<cmail>
#include "cfsmailcommon.h" // for email constants
//</cmail>

// LOCAL INCLUDES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiConstants.h"

// FORWARD DECLARATIONS
class CFsEmailUiGlobalSettingsList;
class CFsEmailUiGlobalSettingsData;
class CFreestyleEmailUiAppUi;

// CLASS DECLARATION
class CFsEmailUiGlobalSettingsListView : public CFsEmailUiViewBase
	{
public:
	// constructors and destructor
	static CFsEmailUiGlobalSettingsListView* NewL( CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup );
	static CFsEmailUiGlobalSettingsListView* NewLC( CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup );        
	
	virtual ~CFsEmailUiGlobalSettingsListView();

public: // from base class CAknView
	TUid Id() const;
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
	void HandleCommandL( TInt aCommand );

public: // new methods 
    void SetMskL();

protected: // from base class CAknView
    // <cmail> Toolbar
	/*void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );*/
    // </cmail> Toolbar
    void ChildDoDeactivate();                
	void HandleStatusPaneSizeChange();
	
// <cmail> Toolbar    
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
   
// </cmail> Toolbar    

private: // new methods
    void ConstructL();
	CFsEmailUiGlobalSettingsListView( CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup );
	void SetupStatusPaneL();
	void CleanupStatusPaneL();
	TBool HandleChangeSelectedSettingItemL( TInt aCommand );

private: 
	CFsEmailUiGlobalSettingsList* iGlobalSettingsSettingItemList;
	CFsEmailUiGlobalSettingsData* iSettings;
	};

#endif // GLOBALSETTINGSSETTINGITEMLISTVIEW_H			
