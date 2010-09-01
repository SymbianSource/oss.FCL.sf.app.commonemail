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


#ifndef GLOBALSETTINGSSETTINGITEMLIST_H
#define GLOBALSETTINGSSETTINGITEMLIST_H

// SYSTEM INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class MEikCommandObserver;
class CFsEmailUiGlobalSettingsData;


// CLASS DECLARATION
class CFsEmailUiGlobalSettingsList : public CAknSettingItemList
	{
public: // constructors and destructor

	CFsEmailUiGlobalSettingsList( 
			CFsEmailUiGlobalSettingsData& aSettings, 
			MEikCommandObserver* aCommandObserver );
	virtual ~CFsEmailUiGlobalSettingsList();

public:

	// from CCoeControl
	void HandleResourceChange( TInt aType );

	// overrides of CAknSettingItemList
	CAknSettingItem* CreateSettingItemL( TInt aId );
	void EditItemL( TInt aIndex, TBool aCalledFromMenu );
	TKeyResponse OfferKeyEventL( 
			const TKeyEvent& aKeyEvent, 
			TEventCode aType );

public:
	// utility function for menu
	void ChangeSelectedItemL( TBool aCalledFromMenu );

	void LoadSettingValuesL();
	void SaveSettingValuesL();

	TInt FocusedItemId();
	
	void GoToTop();
	void GoToBottom();
	
	void PageUp();
	void PageDown();
			
private:
	// override of CAknSettingItemList
	void SizeChanged();

	// new methdos
	TBool IsHsModesAppAvailable() const;
	void ActivateHsModesEmailViewL();
	
private:
	// current settings values
	CFsEmailUiGlobalSettingsData& iSettings;
	MEikCommandObserver* iCommandObserver;

	};
#endif // GLOBALSETTINGSSETTINGITEMLIST_H
