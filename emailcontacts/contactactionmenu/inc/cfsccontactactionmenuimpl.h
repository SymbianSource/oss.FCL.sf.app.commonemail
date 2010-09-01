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
* Description:  Definition of the class CFscContactActionMenuImpl.
*
*/


#ifndef C_FSCCONTACTACTIONMENUIMPL_H
#define C_FSCCONTACTACTIONMENUIMPL_H

#include <e32base.h>
#include <coemain.h>
#include <AknPopupFader.h> 
#include <eikcmobs.h>
//<cmail>
#include "cfsccontactactionmenu.h"
#include "mfsccontactactionserviceobserver.h"
//</cmail>

#include "mfsccontactactionmenulistobserver.h"

// FORWARD DECLARATIONS
class CFscContactActionService;
class CFscContactActionMenuModelImpl;
class CFscContactActionMenuList;
class CActiveSchedulerWait;
class CEikButtonGroupContainer;
class CTextResolver;
class CEikonEnv;
class CFscContactActionMenuItem;
    
/**
 *  Contact Action Menu implementation.
 *  Contains API that implements all the functionality of the Contact Action
 *  Menu.
 *
 *  @lib fsccontactactionmenu.lib
 *  @since S60 3.1
 */
class CFscContactActionMenuImpl 
    : public CFscContactActionMenu, 
      public MFscContactActionMenuListObserver,
      public MAknFadedComponent,
      public MCoeForegroundObserver,
      public MEikCommandObserver,
      public MFscContactActionServiceObserver
    {
    
public: // Public constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aParams Pointer construction parameters
     * @return New instance of the component       
     */
    static CFscContactActionMenuImpl* NewL( TAny* aParams );
   
    /**
     * Destructor.
     */
    virtual ~CFscContactActionMenuImpl();  
    
public: // From CFscContactActionMenu

    /**
     * Execute Action Menu
     *   ExecuteL opens Action Menu with current item set. Method is
     *   synchronous and it will return when Action Menu is closed.
     *
     * @param aPosition Vertical position where menu is to be openned
     * @param aIndex Index of focused/highlighted item when menu is opened
     * @return Execution result
     */
    TFscActionMenuResult ExecuteL( 
        TFscContactActionMenuPosition aPosition = EFscCenter, TInt aIndex = 0, MFsActionMenuPositionGiver* aPositionGiver = 0 );

    /**
     * Dismiss menu
     *   Menu gets closed if it was visible when the method was called. 
     *   Otherwise method call does not do anything
     *
     * @param aSlide If ETrue menu slides away from the screen. 
     *   Otherwise it disappears instantly
     */
    void Dismiss( TBool aSlide );  

    /**
     * Return a reference to Action Menu's model
     * 
     * @return Reference to menu model
     */
    MFscContactActionMenuModel& Model();
  
    /**
     * Return index of selected item
     * 
     * @return index of selected item
     */
    TInt FocusedItemIndex();
    
    /**
     * Return selected menu item object.
     * 
     * @return selected menu item object.
     */
    CFscContactActionMenuItem& FocusedItem();

    /**
     * This method is called if the menu was opened from meeting request.
     * 
     * @param aOpenedFromMR ETrue, if opened from meeting request.
     */
    void SetOpenedFromMR( TBool aOpenedFromMR );
    
public: // From MFscContactActionMenuListObserver
    
    /**
     * Handle action menu list event
     *
     * @param aEvent Event code
     * @param aError Error code used with error event
     */
    void HandleListEvent( TFscContactActionMenuListEvent aEvent,
                          TInt aError = KErrNone );   
    
    /**
     * Check if executed action is completed.
     */
    TBool IsOperationCompleted();
    
public: // From MAknFadedComponent

    /**
     * Count Faded Components
     *
     * @return count
     */
    TInt CountFadedComponents();

    /**
     * Get Faded Component
     *
     * @param component index
     * @return component
     */
    CCoeControl* FadedComponent( TInt aIndex );     
     
public: // From MCoeForegroundObserver
    
    /** 
     * Handles the application coming to the foreground. 
     */
    void HandleGainingForeground();
    
    /** 
     * Handles the application going into the background. 
     */
    void HandleLosingForeground();
     
public: // From MEikCommandObserver

    /**
     * ProcessCommandL
     *
     * @param aCommandId Command Id
     */
    void ProcessCommandL( TInt aCommandId );
    
protected: // From base class MFscContactActionServiceObserver
    
    /**
     * From MFscContactActionServiceObserver.
     * Called when QueryActionsL method is complete.
     */
    void QueryActionsComplete();
        
    /**
     * From MFscContactActionServiceObserver.
     * Called when QueryActionsL method failed.
     * 
     * @param aError An error code of the failure.
     */
    void QueryActionsFailed( TInt aError );
    
    /**
     * From MFscContactActionServiceObserver.
     * Called when ExecuteL method is complete.
     */
    void ExecuteComplete();
       
    /**
     * From MFscContactActionServiceObserver.
     * Called when ExecuteL method failed.
     * 
     * @param aError An error code of the failure.
     */
    void ExecuteFailed( TInt aError );
    

private: // Private constructors

    /**
     * Constructor.
     *
     * @param aParams Construction parameters
     */
    CFscContactActionMenuImpl( 
        const TFscContactActionMenuConstructParameters& aParams );

    /**
     * Second phase constructor.
     *
     * @param aParams Construction parameters
     */
    void ConstructL(
        const TFscContactActionMenuConstructParameters& aParams );

private: // Private methods

    /**
     * Execute Action Menu
     *   ExecuteL opens Action Menu with current item set. Method is
     *   synchronous and it will return when Action Menu is closed.
     *
     * @param aPosition Vertical position where menu is to be openned
     * @param aIndex Index of focused/highlighted item when menu is opened
     */
    void DoExecuteL( 
        TFscContactActionMenuPosition aPosition,
        TInt aIndex );

    /**
     * Dismiss Action Menu
     */
    void DoDismiss();
        
    /**
     * Start menu execution wait
     */
    void StartWait();

    /**
     * Stop menu execution wait
     */
    void StopWait();
    
    /**
     * Show menu's cba
     */
    void ShowMenuCbaL();
        
    /**
     * Hide menu's cba
     */
    void HideMenuCba();
    
    /**
     * Handle item click
     */
     void HandleItemClick();
    
    /**
     * Update fade colors
     */
     void UpdateFadeColors();

private: // data

    /**
     * Menu model
     *  Owned
     */
    CFscContactActionMenuModelImpl*    iModel;

    /**
     * Menu's UI list
     *  Owned
     */
    CFscContactActionMenuList*         iList;
    
    /**
     * Menu's current mode
     */
    TFscContactActionMenuMode          iMode;
    
    /**
     * ExecuteL result code
     */
    TFscActionMenuResult               iMenuResult;
 
    /**
     * Selected item index
     */
    TInt                                iSelectedItemIndex;
    
    /**
     * Error status while action menu is executed
     */
    TInt                                iError;
    
    /*
     * Instant exit flag
     */
    TBool                               iExit;
    
    /**
     * Button group container for menu's softkeys
     *  Owned
     */
    CEikButtonGroupContainer*           iButtonGroupContainer;    
    
    /**
     * Wait loop for ExecuteL
     *  Owned
     */
    CActiveSchedulerWait*               iWait;
    
    /**
     * Popup fader for dialog style fading effect
     */
    TAknPopupFader                      iFader;
    
    /**
     * Text resolver for resolving error texts
     *  Owned
     */
    CTextResolver*                      iTextResolver;
    
    /**
     * Operation observer.
     * Not own.
     */
    MFscContactActionServiceObserver* iObserver;
    
    /**
     * Is action execution finished?
     */
    TBool isExecuteCompleted;

    /**
     * Is action executing.
     */
    TBool iIsActionExecuting;
    
    /**
     * Dummy item returned in case of error.
     */
    CFscContactActionMenuItem* iDummyMenuItem;

    TBool iOpenedFromMR;
    };

#endif // C_FSCCONTACTACTIONMENUIMPL_H

