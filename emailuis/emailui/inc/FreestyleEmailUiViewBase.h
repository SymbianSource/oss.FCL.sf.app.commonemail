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
* Description:  Declaration for the common base class for all FsEmailUi views.
*                The class is not intended for instantation.
*
*/


#ifndef __FREESTYLEEMAILUI_VIEWBASE_H__
#define __FREESTYLEEMAILUI_VIEWBASE_H__

#include <aknview.h>
//<cmail>
#include "CFSMailCommon.h"
//</cmail>
// <cmail> Toolbar
#include "akntoolbarobserver.h"
// </cmail> Toolbar

// FORWARD DECLARATIONS
class CAlfControlGroup;
class CFreestyleEmailUiAppUi;
class CAlfVisual;

class CFsEmailUiViewBase: public CAknView, public MAknToolbarObserver
    {
public: // types
    enum TDynamicSwitchType
        {
        ESkinChanged,
        EScreenLayoutChanged,
        EOther
        };

public: // from CAknView
    // dummy implementation for the base class abstact functions are needed to be able to define a constructor
    TUid Id() const;


// <cmail> Toolbar
private:

    virtual void DoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );
// </cmail> Toolbar

    /**
    * Inherited classes should NOT override this. The should override ChildDoDeactivate() instead.
    */
    void DoDeactivate();

public: // new functions
    /**
    * Returns a handle to the control group assosiated to this view, as given in the class constructor.
    */
    CAlfControlGroup& ControlGroup();

    /**
     * Handles application foreground events. When applcation is brought to
     * foreground, calls the virtual HandleForegroundEventL() method, which
     * can be implemented by subclasses.
     */
    void HandleAppForegroundEventL( TBool aForeground );

    /**
    * Inherited classes should override this to react to skin or layout changes
    * happening while the view is active.
    */
    virtual void HandleDynamicVariantSwitchL( TDynamicSwitchType aType );

    /**
    * Inherited classes should override this to react to skin or layout changes
    * which happen when the view is not active.
    */
    virtual void HandleDynamicVariantSwitchOnBackgroundL( TDynamicSwitchType aType );

    /**
    * Mail list type views should override these to enable next/previous message
    * functionality in the mail viewer which has been launched from this list view.
    */
    virtual TBool IsNextMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId, TFSMailMsgId& aFoundNextMsgFolder ) const;
    virtual TBool IsPreviousMsgAvailable( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId, TFSMailMsgId& aFoundPrevMsgFolder ) const;
    virtual TInt MoveToNextMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId );
    virtual TInt MoveToPreviousMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId );
    virtual TInt MoveToPreviousMsgAfterDeleteL( TFSMailMsgId aFoundPreviousMsgId );
    /**
     * Do the transition effect for the view if effects are enabled.
     */
    void DoTransition( TBool aDirectionOut );

    /**
     * Prepares application exit. This is called by AppUi during cleanup before AppUi's destructor
     */
	virtual void PrepareExitL();

	/**
	 * Called when flip state changes.
	 */
	virtual void FlipStateChangedL( TBool aKeyboardFlipOpen );

	/**
	* This method is called from the appui when the focus state is changed.
	* Each inherited view should implement own method for focus handling
	* @param aVisible Indicates if focus should become visible or removed
	*/
	virtual void FocusVisibilityChange( TBool aVisible );

protected: // methods

    /**
     * Inherited classes may override this to react to FsEmail application
     * being brougt to foreground while that particular view is active.
     */
    virtual void HandleForegroundEventL();

    /**
    * Inherited classes should override this instead of DoDeactivate()
    * to do the view deactivation
    */
    virtual void ChildDoDeactivate() = 0;

    /**
     * Make control group of the view visible and set to accept input.
     */
    virtual void ActivateControlGroup( TInt aDelay = 0 );

    /**
     * Make control group of the view invisible and set to ignore input.
     */
    virtual void DeactivateControlGroup( TInt aDelay = 0 );

    /**
    * Inheriting Alfred views can override these functions, if they need to do
    * some special tricks for transition effects
    */
    virtual void DoTransitionEffect( TBool aDirectionOut );

    /**
    * Inheriting Alfred views should override this and return their parent
    * layout(s) in array given as parameter. Default implementation
    * does nothing, so then the array is empty.
    */
    virtual void GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const;

    /**
     * Utility function to fade in/out given visual
     */
    void FadeVisual( CAlfVisual* aVisual,
                     TBool aFadeDirectionOut,
                     TBool aImmediate = EFalse );

    /**
    * Used to indicate that the next out transition effect should be long
    * (some long opening view is to be opened next)
    */
    void SetNextTransitionOutLong( TBool aLongTransitionOut );

    TBool IsNextTransitionOutLong();

    /**
     * Derived classes should call this always when the view is entered with forward navigation.
     * This is required to make returning from the view work correctly in case of external activation.
     * It doesn't hurt to call this also from views which cannot be externally activated.
     */
    void ViewEntered( const TVwsViewId& aPrevViewId );

    /**
     * Derived classes should call this to do backwards navigation e.g. when user
     * presses "Back" button. This returns to the previous view, and in case the view
     * was activated externally, takes FsEmail to background and brings the previous
     * application to foreground.
     */
    virtual void NavigateBackL();

    /**
     * Derived classes can call this utility function to set the middle soft key
     * label and ID from the given resource.
     */
    void ChangeMskCommandL( TInt aLabelResourceId );

// <cmail> Toolbar
protected: // toobar changes

    /**
     * @see CAknView::DoActivateL
     */
    virtual void ChildDoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage ) = 0;

    /**
     * Return view's toolbar resource id.
     */
    virtual TInt ToolbarResourceId() const;

    /**
     * Called when whole toolbar needs to be updated. Creates and sets new
     * toolbar by calling ToolbarResourceId() method.
     */
    void UpdateToolbarL();

    /**
     * Get array of item IDs that need to be dimmed before toolbar is set for
     * view.
     * @param aResourceId Resource ID of the toolbar.
     * @param aDimmedItems Array of items that will be dimmed.
     */
    virtual void GetInitiallyDimmedItemsL( const TInt aResourceId,
            RArray<TInt>& aDimmedItems ) const;

    /**
     * Replaces single toolbar item with item from resource file. Control
     * must be of AVKON_BUTTON type.
     * @param aReaplceId Id of command to be replaced.
     * @param aResourceId Resource Id of the control that will replace existing
     * item.
     * @param aIndex Index in array for new item. Default value is zero.
     */
    void ReplaceToolbarItemL( const TInt aReplaceId, const TInt aResourceId,
            const TInt aIndex = 0 );

    /**
     * Removes single toolbar item from toolbar.
     */
    void RemoveToolbarItem( const TInt aRemoveId );

    /**
     * Hides toolbar.
     */
    void HideToolbar();

    /**
     * Show toolbar.
     */
    void ShowToolbar();

    /**
     * Dim/undim toolbar item.
     */
    void SetToolbarItemDimmed( const TInt aCommandId, const TBool aDimmed );

    /**
     * Returns ETrue if view has toolbar.
     */
    virtual TBool HasToolbar() const;

protected: // Single click changes
    /**
     * Return current state of the focus visibility
     * @return ETrue if the focus should be shown
     */
    virtual TBool IsFocusShown();

	/**
	 * Set status bar layout. By default layout is R_AVKON_STATUS_PANE_LAYOUT_USUAL.
	 * Inherited classes can override this and set desired status bar layout.
	 */
	virtual void SetStatusBarLayout();

protected: // from MAknToolbarObserver

    /**
     * @see MAknToolbarObserver::OfferToolbarEventL
     */
    void OfferToolbarEventL( TInt aCommand );

// </cmail> Toolbar

protected: // construction

    CFsEmailUiViewBase( CAlfControlGroup& aControlGroup, CFreestyleEmailUiAppUi& aAppUi );

    /**
     * Return view active status. ETrue if the view is currently activated, EFalse otherwise.
     */
    TBool IsViewActive() const;

private:

    /**
     * Sets view's active status. Private to protect calling from inherited classes.
     */
    void SetViewActive( const TBool aActive );

protected: // data

    CAlfControlGroup& iControlGroup;
    CFreestyleEmailUiAppUi& iAppUi;

    // Status of the keyboard flip
    TBool iKeyboardFlipOpen;

    // Is focus visible
    TBool iFocusVisible;

private: // data

    TUid iPreviousAppUid;
    // ID of the mailbox which was active before this view was activated from an external application
    TFSMailMsgId iActiveMailboxBeforeExternalActivation;
    // Flag to indicate if next fade out effect should be longer than normally
    TBool iIsNextTransitionEffectOutLong;
    // Flag to indicate that Email app should be sent to background when the current view is deactivated
    TBool iSendToBackgroundOnDeactivation;
    // View's active status
    TBool iViewActive;
    TBool iWasActiveControlGroup;
    };

#endif // __FREESTYLEEMAILUI_VIEWBASE_H__
