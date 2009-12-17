/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  FreestyleEmailUi main UI grid definition.
*
*/



#ifndef __FREESTYLEEMAILUI_MAINGRIDUIVISUALISER_H__
#define __FREESTYLEEMAILUI_MAINGRIDUIVISUALISER_H__

// SYSTEM INCLUDE FILES
//<cmail> SF
#include <alf/alfcontrol.h>
//</cmail>
// for AIW handling
#include <AiwServiceHandler.h>
//<cmail>
#include <alf/alfdecklayout.h>
#include <alf/alfgridlayout.h>
#include <alf/alfmappingfunctions.h>
#include <alf/alfbitmapprovider.h>
//</cmail>

// INTERNAL INCLUDE FILES
#include "FreestyleEmailUiViewBase.h"
#include "FreestyleEmailUiLauncherGridModel.h"
#include "FreestyleEmailUiAppui.h"

#include "FSEmailLauncherItem.h"
#include "freestyleemailuimailboxdeleter.h"

// FORWARD DECLARATIONS
// <cmail> 
class CBrushAnimation;
// </cmail> 
class CFsAlfScrollbarLayout;

class TFSLauncherGridMailboxStatus
    {
    public:
        TInt iMailboxCount;
        TInt iMailboxesOnline;
        TInt iMailboxesOffline;
        TInt iMailboxesSyncing;
    };


// Grid visualiser
class CFSEmailUiLauncherGridVisualiser : public CFsEmailUiViewBase,
                     				   	 public MAlfBitmapProvider,
                     				   	 public MFSEmailLauncherItemObserver,
                     				   	 public MEikScrollBarObserver,
                     				   	 public MFSEmailUiMailboxDeleteObserver
    {
public:
    enum TDirection
        {
        EDirectionRight=0,
        EDirectionLeft,
        EDirectionUp,
        EDirectionDown,
        EDirectionReset, 
        EDirectionNone,
        // <cmail> 
        EDirectionTouch 
        // </cmail> 
        };

    static CFSEmailUiLauncherGridVisualiser* NewL(CAlfEnv& aEnv, 
   									  CFSEmailUiLauncherGrid* aControl, 
    								  CFreestyleEmailUiAppUi* aAppUi,
    								  CAlfControlGroup& aControlGroup, 
    								  TInt aColumns, 
    								  TInt aRows);

    static CFSEmailUiLauncherGridVisualiser* NewLC(CAlfEnv& aEnv,
    									 CFSEmailUiLauncherGrid* aControl, 
    								   CFreestyleEmailUiAppUi* aAppUi,
    								   CAlfControlGroup& aControlGroup, 
    								   TInt aColumns, 
    								   TInt aRows);

    void AddMenuItemL(TUid aUid);
	void SelectL();	
	// Menu pane init and command handling
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void HandleCommandL(TInt aCommand);

    TBool OfferEventL(const TAlfEvent& aEvent);
    // <cmail> 
    /**
      * Handler for touch related events
      * @param aEvent, event type
      * @return ETrue if event was used
      */
    TBool HandlePointerEventL(const TAlfEvent& aEvent);
    
    /**
      * Function for searching right item when pointer event occurs
      * @param aEvent, event type
      * @return KErrnone if item was not found, else index to founded item
      */
    TInt FindPointedItem( const TAlfEvent& aEvent );
    
    /**
      * Function for setting focus to item 
      * @param aId, id of item where focus is going to be changed
      */
    void SetFocusedItemL( TInt aId );
    
    void HandleRowMovement( TDirection aDir, TInt aSelected );
    // </cmail> 
    TUid Id() const;			   
    // <cmail> Toolbar
    /*
    void DoActivateL(const TVwsViewId& aPrevViewId,
	                 TUid aCustomMessageId,
	                 const TDesC8& aCustomMessage);
     */
	void ChildDoDeactivate(); 
  
    void VisualLayoutUpdatedL();

	// Dynamic switch handling
	void HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType );
	void HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType aType );
	
	// React to changes in grid contents
	void HandleContentChangeL();
    
    void LaunchWizardL();
    void GoToInboxL( TFSMailMsgId& mailboxId, 
                     TFSMailMsgId& mailboxInboxId );

	// From MFSEmailLauncherItemObserver
	void RefreshLauncherViewL();
    
	TBool UiOperationLaunched();

	// Modifies the value of iWizardWaitnoteShown
    void SetWizardWaitnoteShown( TBool aWaitnoteShown );
    
	// Bitmap provider for grid ecom plugins icons 
	void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap);

    // Set refresh needed flag
	void SetRefreshNeeded();
	// <cmail> 
    // Handling of foreground events
	void HandleForegroundEventL();
	
    /**
     * From MEikScrollBarObserver
     * 
     * Callback method for scroll bar events
     *
     * Scroll bar observer should implement this method to get scroll bar events.
     *
     * @since S60 0.9
     * @param aScrollBar A pointer to scrollbar which created the event
     * @param aEventType The event occured on the scroll bar
     */
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
    // </cmail> 
	
	// From base class CFsEmailUiViewBase
	void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;
	
    /**
     * Called when flip state changes.
	 */
	void FlipStateChangedL( TBool aKeyboardFlipOpen );

// <cmail> Toolbar    
private: // from
    
    /**
     * @see CFsEmailUiViewBase::ChildDoActivateL
     */
    void ChildDoActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );  
// </cmail> Toolbar 

private: // from MFSEmailUiMailboxDeleteObserver
    
    void MailboxDeletionComplete();
    
    
private: // methods
    CFSEmailUiLauncherGridVisualiser(CAlfEnv& aEnv, 
    					 CFSEmailUiLauncherGrid* aControl,
    					 CFreestyleEmailUiAppUi* aAppUi,
    					 CAlfControlGroup& aControlGroup);
    void ConstructL(TInt aColumns, TInt aRows);
    virtual ~CFSEmailUiLauncherGridVisualiser();
	void CreateModelL();
    void MoveSelection( TDirection aDir );
    void MoveSelectorToCurrentItem( TDirection aDir );
    void Select();
    void DisplayProductInfoL();
	void RescaleIconsL();
    void SetRingWrapLimits();
	void AddItemToModelL( CFSEmailLauncherItem* aItem,  TInt aPluginArrayIndex );

	void ConstructScrollbarL( CAlfLayout* aParent );
    void UpdateScrollBarRangeL();
    CFSMailBox* ShowMailboxSelectionQueryL( const RPointerArray<CFSMailBox>& aMailBoxes );

    // Check status of all configured mailboxes
    TFSLauncherGridMailboxStatus CheckMailboxStatusL();

    void DoFirstStartL();

    /**
     * Funciton should decide if item focus should still be drawn dispite of
     * releasing button.
     */
    void HandleButtonReleaseEvent();

    /**
     * Reduces item's icon size.
     */
    void ResizeItemIcon( TBool aReduce );
	
    /**
     * Handles the focus removal of CFSEmailUiLauncherGridVisualiser
     * @param aShow Indicates if focus should become visible or removed
     */
	void HandleTimerFocusStateChange( TBool aShow );

private: // data types

    struct TItemVisualData
        {
        CAlfAnchorLayout* iBase;
        CAlfImageVisual* iImage;
        CAlfTextVisual* iText;
        };

	struct TLevel
        {
	    CAlfGridLayout* iGridLayout;
		RArray<TItemVisualData> iItemVisualData;
		TInt iSelected;
        TInt iParent; 
		TPoint iParentPos;
        RArray<TMenuItem> iItems; 
        };      

	struct TPluginIdIconIdPair
		{
		TInt iPluginArrayIndex;
		TInt iIconId;
		};
	
    class TRingMovementFuncMappingDataProvider: public MAlfTableMappingFunctionDataProvider
        {
    public:
        inline TRingMovementFuncMappingDataProvider() : iStart(0), iEnd(0) { }
        // From MAlfTableMappingFunctionDataProvider    
        TReal32 MapValue( TReal32 aValue, TInt aMode ) const;
        inline void SetStart( TReal32 aStart ) { iStart = aStart; }
        inline void SetEnd( TReal32 aEnd ) { iEnd = aEnd; }
    private:
        TReal32 iStart;
        TReal32 iEnd;
        };

    /**
     * Structure for passing button down events
     */
    struct TItemIdInButtonDownEvent
        {
        TInt iItemId;
        TBool iLaunchSelection;
        };

private: // methods
    void ConstructRingL();

    void CreateCaptionForApplicationL(TUid aUid, TDes& aCaption, TBool aShortCaption);
	void FocusItem(TBool aHasFocus, TInt aItem);
    void ScrollToRow(TInt row);
	void PopulateL(TLevel& aLevel);
	void UpdateLauncherItemListL();
	void SetDefaultStatusPaneTextL();

    void DetachSelectorMappingFunctions();
    void UpdateFocusVisibility();

private: // data
	CAlfEnv& iEnv;
	CAlfControl* iControl;
	TLevel iCurrentLevel;
    CFSEmailUiLauncherGridModel* iModel;
    TInt iVisibleRows;
    TInt iVisibleColumns;
    TInt iRowCount;
    TInt iFirstVisibleRow;
    TReal iRowHeight;
    TReal iColumnWidth;
    // Selector layout. Not own.
    CAlfLayout* iSelector;
    // Mapping function for selector horizontal position. Own.
    CAlfTableMappingFunction* iRingMovementXFunc;
    // Mapping function for selector vertical position. Own.
    CAlfTableMappingFunction* iRingMovementYFunc;
    TRingMovementFuncMappingDataProvider iRingMovementXFuncMappingDataProvider;
    TRingMovementFuncMappingDataProvider iRingMovementYFuncMappingDataProvider;

	TBool iStartupAnimation;
    CAlfAnchorLayout* iWaterMarkBackgroundImageLayout;
	TInt iStartupEffectStyle;
    TInt iNewEmailCount;
    CAlfTextVisual* iNewMailTextVisual;
  	CAlfImageVisual* iNewMailBgVisual;
 	CAlfAnchorLayout* iTopTextLayout; 
  	CAlfTextVisual* iTimeTitle;
  	CAlfImageVisual *iSelectorImageVisual;
  	RPointerArray<CAlfTexture> iIconArray;
  	RArray<TInt> iMailboxRequestIds;
  	TBool iSynchronizingMailbox;
  	TBool iMailboxOnline;
  	RPointerArray<CFSEmailLauncherItem> iLauncherItems;
  	RArray<TUid> iLauncherItemUids;

    CAlfDeckLayout* iParentLayout;
    CAlfAnchorLayout* iWidgetLayout;
    // <cmail> 
    CAknDoubleSpanScrollBar* iScrollbar;
    TAknDoubleSpanScrollBarModel iScrollbarModel;
    // </cmail> 
	CAlfImageVisual* iBgImageVisual;
    
    CAiwServiceHandler* iAiwSHandler;
	
	/**
	 * ETrue if "Creating mailbox" wait note is shown. 
	 * Then the key presses are ignored.
	 */
	TBool iWizardWaitnoteShown;

	/**
	 * ETrue indicates that the grid contents and layout must be updated next time when the
	 * grid is activated.
	 */
	TBool iRefreshNeeded;
	
	// Array holding pairs of plugin id's vs icon id's
	RArray<TPluginIdIconIdPair> iPluginIdIconIdPairs;
   
    // Plugin texture count, also serves as index when adding items to array
    // and if for bitmap provider
	TInt iPluginTextureId;
	
	// Boolean value indicating that construction has completed.
	TBool iConstructionCompleted;
	
	// To indicate whether grid has started, for ODS, IAD and 
	// "create mailbox query" interoperability
	TBool iFirstStartComplete;

	// Boolean value to prevent wizard/help crash when double-clicking.
	TBool iDoubleClickLock;

	// Item id that was seleceted in button down event
	TItemIdInButtonDownEvent iItemIdInButtonDownEvent;

	// Boolean value - true is user has launched any operation from UI.
    TBool iUiOperationLaunched;

    // Mailbox deleter - handles deletion of one or more mailboxes.
    CFSEmailUiMailboxDeleter* iMailboxDeleter;
    };

#endif // __FREESTYLEEMAILUI_MAINGRIDUIVISUALISER_H__
