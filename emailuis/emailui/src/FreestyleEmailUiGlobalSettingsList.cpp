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
* Description:  Freestyle Email source file
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <avkon.hrh>
#include <avkon.rsg>
#include <eikmenup.h>
#include <aknappui.h>
#include <eikcmobs.h>
#include <barsread.h>
#include <StringLoader.h>
#include <AknPopupFieldText.h>
#include <eikenv.h>
#include <eikappui.h>
#include <aknViewAppUi.h>
#include <apgcli.h>
#include <FreestyleEmailUi.rsg>

// LOCAL INCLUDES
#include "FreestyleEmailUiGlobalSettingsData.h"
#include "FreestyleEmailUiGlobalSettings.hrh"
#include "FreestyleEmailUiGlobalSettingsListView.h"
#include "FreestyleEmailUiGlobalSettingsList.h"
#include "FreestyleEmailUiAppui.h"
#include "freestyleemailcenrephandler.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiConstants.h"

/**
 * Constructor
 * @param aCommandObserver command observer
 */ 
CFsEmailUiGlobalSettingsList::CFsEmailUiGlobalSettingsList( 
		CFsEmailUiGlobalSettingsData& aSettings, 
		MEikCommandObserver* aCommandObserver )
	: iSettings( aSettings ), iCommandObserver( aCommandObserver )
	{
    FUNC_LOG;
	}

/** 
 * Destructor
 * Destroy any instance variables
 */
CFsEmailUiGlobalSettingsList::~CFsEmailUiGlobalSettingsList()
	{
    FUNC_LOG;
	}

/**
 * Handle system notification that the container's size has changed.
 */
void CFsEmailUiGlobalSettingsList::SizeChanged()
	{
    FUNC_LOG;

	if ( ListBox() ) 
		{
		ListBox()->SetRect( Rect() );
		}

	}

/**
 * Create one setting item at a time, identified by id.
 * CAknSettingItemList calls this method and takes ownership
 * of the returned value.  The CAknSettingItem object owns
 * a reference to the underlying data, which EditItemL() uses
 * to edit and store the value.
 */
CAknSettingItem* CFsEmailUiGlobalSettingsList::CreateSettingItemL( TInt aId )
	{
    FUNC_LOG;

	switch ( aId )
		{
		case EGlobalSettingsTitleDividers:
			{
			CAknBinaryPopupSettingItem* item = new ( ELeave ) 
				CAknBinaryPopupSettingItem( 
					aId,
					iSettings.TitleDividers() );
			return item;
			}/*
		case EGlobalSettingsDownloadNotifications:
			{			
			CAknBinaryPopupSettingItem* item = new ( ELeave ) 
				CAknBinaryPopupSettingItem( 
					aId,
					iSettings.DownloadNotifications() );
			return item;
			}*/
		case EGlobalSettingsWarnBeforeDelete:
		    {         
		    CAknBinaryPopupSettingItem* item = new ( ELeave ) 
		        CAknBinaryPopupSettingItem( 
                    aId,
                    iSettings.WarnBeforeDelete() );
            return item;
            }
		case EGlobalSettingsDownloadHTMLImages:
            {         
            CAknBinaryPopupSettingItem* item = new ( ELeave ) 
                CAknBinaryPopupSettingItem( 
                    aId,
                    iSettings.DownloadHTMLImages() );
            return item;
            }
        case EGlobalSettingsActiveIdle:
            {           
            CAknBigSettingItemBase* item = new ( ELeave )
                CAknBigSettingItemBase( aId );
            TBool hideHsSetting = !IsHsModesAppAvailable(); // handling other HS types to be added later
            item->SetHidden( hideHsSetting );
            return item;
            }
		}
		
	return NULL;
	}
	
/**
 * Edit the setting item identified by the given id and store
 * the changes into the store.
 * @param aIndex the index of the setting item in SettingItemArray()
 * @param aCalledFromMenu true: a menu item invoked editing, thus
 *	always show the edit page and interactively edit the item;
 *	false: change the item in place if possible, else show the edit page
 */
void CFsEmailUiGlobalSettingsList::EditItemL ( TInt aIndex, TBool aCalledFromMenu )
    {
    FUNC_LOG;

    CAknSettingItem* item = ( *SettingItemArray() )[aIndex];
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );	

    item->StoreL();
    SaveSettingValuesL();
    }

/**
 * Handle the "Change" option on the Options menu or on the MSK.
 * The argument tells which one is the case.
 */
void CFsEmailUiGlobalSettingsList::ChangeSelectedItemL( TBool aCalledFromMenu )
	{
    FUNC_LOG;

  	TInt idx = ListBox()->CurrentItemIndex(); 
	if ( idx >= 0 )
		{
		CAknSettingItem* item = ( *SettingItemArray() )[idx];
		    
		if ( item->Identifier() == EGlobalSettingsActiveIdle )
		    {
		    if ( IsHsModesAppAvailable() )
		        {
		        ActivateHsModesEmailViewL();
		        }
		    else
		        {
		        // opening of Email widget settings or settings of non-Eseries
		        // S60 3.2 home screen plugin to be added here later
		        }
		    }
		else
		    {
	        EditItemL( idx, aCalledFromMenu );
		    }
		}
	}

/**
 *	Load the initial contents of the setting items.  By default,
 *	the setting items are populated with the default values from
 * 	the design.  You can override those values here.
 *	
 */
void CFsEmailUiGlobalSettingsList::LoadSettingValuesL()
	{
    FUNC_LOG;

	CFSEmailCRHandler *crHandler = CFSEmailCRHandler::InstanceL();
	
    // Layout setting item has values 1 (1 line) and 0 (2 lines) because no other values are allowed in binary items.
    // CenRep, on the other hand, uses values 1 (1 line) and 2 (2 lines).
	iSettings.SetMessageListLayout( crHandler->MessageListLayout() == 1 ? 1 : 0 );
	
	iSettings.SetBodyPreview( crHandler->BodyPreview() );
	iSettings.SetTitleDividers( crHandler->TitleDividers() );
	//iSettings.SetDownloadNotifications( crHandler->DownloadNotifications() );
	iSettings.SetActiveIdle( crHandler->ActiveIdle() );
	iSettings.SetWarnBeforeDelete( crHandler->WarnBeforeDelete() );
	iSettings.SetDownloadHTMLImages( crHandler->DownloadHTMLImages() );
	}
	
/**
 *	Save the contents of the setting items.  Note, this is called
 *	whenever an item is changed and stored to the model, so it
 *	may be called multiple times or not at all.
 */
void CFsEmailUiGlobalSettingsList::SaveSettingValuesL()
	{
    FUNC_LOG;

	CFSEmailCRHandler *crHandler = CFSEmailCRHandler::InstanceL();
	
	// Layout setting item has values 1 (1 line) and 0 (2 lines) because no other values are allowed in binary items.
	// CenRep, on the other hand, uses values 1 (1 line) and 2 (2 lines).
	crHandler->SetMessageListLayout( iSettings.MessageListLayout() ? 1 : 2 );
	
	crHandler->SetBodyPreview( iSettings.BodyPreview() );
	crHandler->SetTitleDividers( iSettings.TitleDividers() );
	//crHandler->SetDownloadNotifications( iSettings.DownloadNotifications() );
	crHandler->SetActiveIdle( iSettings.ActiveIdle() );
	crHandler->SetWarnBeforeDelete( iSettings.WarnBeforeDelete() );
	crHandler->SetDownloadHTMLImages( iSettings.DownloadHTMLImages() );
	}

/**
 * Get ID of the focused setting list item
 */
TInt CFsEmailUiGlobalSettingsList::FocusedItemId()
    {
    FUNC_LOG;
    TInt id = KErrNotFound;
    
    if ( ListBox() && SettingItemArray() )
        {
        TInt curIdx = ListBox()->CurrentItemIndex();
        CAknSettingItem* item = ( *SettingItemArray() )[curIdx];
        id = item->Identifier();
        }
    
    return id;
    }

/** 
 * Handle global resource changes, such as scalable UI or skin events (override)
 */
void CFsEmailUiGlobalSettingsList::HandleResourceChange( TInt aType )
	{
    FUNC_LOG;

	CAknSettingItemList::HandleResourceChange( aType );
	SetRect( iAvkonViewAppUi->View( GlobalSettingsViewId )->ClientRect() );

	}
				
/** 
 * Handle key event (override)
 * @param aKeyEvent key event
 * @param aType event code
 * @return EKeyWasConsumed if the event was handled, else EKeyWasNotConsumed
 */
TKeyResponse CFsEmailUiGlobalSettingsList::OfferKeyEventL( 
		const TKeyEvent& aKeyEvent, 
		TEventCode aType )
	{
    FUNC_LOG;
  	TKeyResponse retVal = EKeyWasNotConsumed;

    // Check keyboard shortcuts.
    if ( aType == EEventKey )
        {
        CFreestyleEmailUiAppUi* appUi = static_cast<CFreestyleEmailUiAppUi*>(iAvkonViewAppUi);
        TInt shortcutCommand = 
            appUi->ShortcutBinding().CommandForShortcutKey( aKeyEvent,
                                                            CFSEmailUiShortcutBinding::EContextSettings );
        if ( shortcutCommand != KErrNotFound )
            {
            appUi->View( GlobalSettingsViewId )->HandleCommandL( shortcutCommand );
            retVal = EKeyWasConsumed;
            }
        }

    if ( retVal == EKeyWasNotConsumed )
        {
        retVal = CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
        }
    
    // Update MSK label after key has been consumed
    if ( retVal == EKeyWasConsumed )
        {
        CFsEmailUiGlobalSettingsListView* view =
            static_cast<CFsEmailUiGlobalSettingsListView*>( 
                    iAvkonViewAppUi->View(GlobalSettingsViewId) );
        view->SetMskL();
        }
    
	return retVal; 
	}
	
// ---------------------------------------------------------------------------
// Move list focus to top
// ---------------------------------------------------------------------------
//
void CFsEmailUiGlobalSettingsList::GoToTop()
    {
    FUNC_LOG;
    TInt count = ListBox()->Model()->NumberOfItems();
    if ( count )
        {
        ListBox()->SetCurrentItemIndexAndDraw( 0 );
        }
    }
    
// ---------------------------------------------------------------------------
// Move list focus to bottom
// ---------------------------------------------------------------------------
//
void CFsEmailUiGlobalSettingsList::GoToBottom()
    {
    FUNC_LOG;
    TInt count = ListBox()->Model()->NumberOfItems();
    if ( count )
        {
        ListBox()->SetCurrentItemIndexAndDraw( count-1 );
        }
    }

// ---------------------------------------------------------------------------
// Move selector one page up
// ---------------------------------------------------------------------------
//
void CFsEmailUiGlobalSettingsList::PageUp()
    {
    FUNC_LOG;
    TInt itemsOnPage = ListBox()->BottomItemIndex() - ListBox()->TopItemIndex();
    TInt currentIdx = ListBox()->CurrentItemIndex();
    TInt newIdx = Max( 0, currentIdx - itemsOnPage );
    ListBox()->SetCurrentItemIndexAndDraw( newIdx );
    }

// ---------------------------------------------------------------------------
// Move selector one page down
// ---------------------------------------------------------------------------
//
void CFsEmailUiGlobalSettingsList::PageDown()
    {
    FUNC_LOG;
    TInt count = ListBox()->Model()->NumberOfItems();
    TInt itemsOnPage = ListBox()->BottomItemIndex() - ListBox()->TopItemIndex();
    TInt currentIdx = ListBox()->CurrentItemIndex();
    TInt newIdx = Min( count-1, currentIdx + itemsOnPage );
    ListBox()->SetCurrentItemIndexAndDraw( newIdx );
    }

// ---------------------------------------------------------------------------
// Checks if the Homescreen Modes application is available on the device
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiGlobalSettingsList::IsHsModesAppAvailable() const
    {
    FUNC_LOG;
    TBool available = EFalse;
    RApaLsSession apaSession;
    TInt err = apaSession.Connect();
    if ( !err )
        {
        TApaAppInfo appInfo;
        err = apaSession.GetAppInfo( appInfo, KHsModesAppUid );
        apaSession.Close();
        if ( !err )
            {
            available = ETrue;
            }
        }
    return available;
    }

// ---------------------------------------------------------------------------
// Activates the email view of the Homescreen Modes application
// ---------------------------------------------------------------------------
//
void CFsEmailUiGlobalSettingsList::ActivateHsModesEmailViewL()
    {
    FUNC_LOG;
    const TVwsViewId KModesEmailView( KHsModesAppUid, KHsModesPrimaryEmailViewId );
    iAvkonViewAppUi->ActivateViewL( KModesEmailView );
    }
	
