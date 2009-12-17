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
* Description:  Main class for Control Bar component.
*
*/


#ifndef C_FSCONTROLBAR_H
#define C_FSCONTROLBAR_H

//<cmail> SF
#include <alf/alfcontrol.h>
#include <alf/alfenv.h>
//</cmail>

#include "fscontrolbuttonobserver.h"
#include "fscontrolbarobserver.h"
#include "fscontrolbuttonconst.h"

class MFsControlButtonInterface;
class CFsControlButton;
class CFsControlBarModel;
class CFsControlBarVisualiser;
class CAlfTexture;
class CAlfBrush;
class CFsTextStyleManager;

/**  default value for focusing control bar  */
const TInt KFocusFirstFocusableButton = -1;

/**  value for current focused button - no button is focused  */
const TInt KNoButtonFocused = -1;


/**
 *  Main class for ControlBar component
 *  Control class of ControlBar component.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
NONSHARABLE_CLASS( CFsControlBar ) :
    public CAlfControl,
    public MFsControlButtonObserver,
    public MAlfActionObserver
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aEnv huitk environment instance.
     */
    IMPORT_C static CFsControlBar* NewL( CAlfEnv& aEnv );

    /**
     * Two-phased constructor.
     *
     * @param aEnv huitk environment instance.
     */
    IMPORT_C static CFsControlBar* NewLC( CAlfEnv& aEnv );

    /**
     * Destructor
     */
    virtual ~CFsControlBar();

    /**
     * Adds button with specified id and layout to control bar.
     *
     * @param aId unique id of button.
     * @param aType type of button.
     */
    IMPORT_C void AddButtonL( TInt aId, TFsControlButtonType aType );

    /**
     * Adds button with specified layout to control bar and return id.
     *
     * @param aType type of button.
     * @return unique id of button.
     */
    IMPORT_C TInt AddButtonL( TFsControlButtonType aType );

    /**
     * Removes button with specified button (and all data it contains)
     * from control bar
     *
     * @param aId id of button to be removed.
     * @return KErrNotFound if button's id doesn't exist, otherwise KErrNone.
     */
    IMPORT_C TInt RemoveButtonL( TInt aId );

    /**
     * Retrieves button with specified id from control bar.
     *
     * @param aId id of button to be retrieved.
     * @return button with specified id.
     */
    IMPORT_C MFsControlButtonInterface* ButtonById( TInt aId );

    /**
     * Change focused state.
     * When focus is gained it is set to the first focusable button.
     *
     * @param aFocused ETrue to set the focus to the first button otherwise
     * EFalse to lose the focus.
     */
    IMPORT_C void SetFocusL( TBool aFocused = ETrue );

    /**
     * Sets focus to specified button.
     *
     * @param aButtonId id of button to set focus on.
     */
    IMPORT_C void SetFocusByIdL( TInt aButtonId );

    /**
     * Gets focused button or NULL if not focused.
     *
     * @return focused button or NULL if bar not focused.
     */
    IMPORT_C MFsControlButtonInterface* GetFocusedButton() const;

    /**
     * Sets height of the bar in pixels.
     *
     * @param aHeight new height of the bar.
     */
    IMPORT_C void SetHeightL( TInt aHeight );

    /**
     * Retrieves height of the bar in pixels.
     *
     * @return height of the bar in pixels.
     */
    IMPORT_C TInt Height() const;

    /**
     * Sets width of the bar in pixels.
     *
     * @param aWidth new width of the bar.
     */
    IMPORT_C void SetWidthL( TInt aWidth );

    /**
     * Retrieves width of the bar in pixels.
     *
     * @return width of the bar in pixels.
     */
    IMPORT_C TInt Width() const;

    /**
     * Sets controlbar's background color.
     *
     * @param aColor color of background.
     */
    IMPORT_C void SetBackgroundColor( const TRgb& aColor );

    /**
     * Clear controlbar's background color. It becomes transparent.
     */
    IMPORT_C void ClearBackgroundColor();

    /**
     * Sets bar's background image.
     *
     * @param aImage background image.
     */
    IMPORT_C void SetBackgroundImageL( CAlfTexture& aImage );

    /**
     * Clears bar's background image.
     */
    IMPORT_C void ClearBackgroundImage();

    /**
     * Adds additional observer for the bar events.
     *
     * @param aObserver observer of bar's events.
     */
    IMPORT_C void AddObserverL( MFsControlBarObserver& aObserver );

    /**
     * Removes specified observer from the bar events.
     *
     * @param aObserver observer to be removed.
     */
    IMPORT_C void RemoveObserver( MFsControlBarObserver& aObserver );

    /**
     * Sets transition time for selector.
     *
     * @param aTransitionTime transition time in miliseconds.
     */
    IMPORT_C void SetSelectorTransitionTimeL( TInt aTransitionTime );

    /**
     * Sets selector's image and opacity.
     * Ownership of the brush is gained.
     *
     * @param aSelectorBrush New selector brush.
     * @param aOpacity Selector's opacity.
     */
    IMPORT_C void SetSelectorImageL(
        CAlfBrush* aSelectorBrush, TReal32 aOpacity = 1.0 );

    /**
     * Retrieves amount of space in pixels from specified point to the end
     * of the screen.
     *
     * @return Controlbar visual.
     */
    IMPORT_C CAlfVisual* Visual();

    /**
     * Resolve the area reserved for requested layout slot.
     *
     * @param aIndex Index of requested layout slot. Indexing starts from one.
     * @return Size and position of requested layout item.
     */
    static TRect GetLayoutRect( TInt aIndex );

    // <cmail> Touch
    /**
     * Disable or enable touch input
     */
    IMPORT_C void EnableTouch(TBool aEnabled);
    // </cmail>

    /**
     * Hides or shows selector.
     * NOTE: Do not change focus.
     *
     * @param aShow if ETrue selector is shown.
     * @param aFromTouch if method is called after touch event.
     */
    IMPORT_C void MakeSelectorVisible( TBool aShow, TBool aFromTouch = EFalse );

// from base class MAlfActionObserver

    /**
     * From MAlfActionObserver.
     * Called by the scheduler when an action command is executed.
     *
     * @param aActionCommand The command that is being executed.
     */
    virtual void HandleActionL( const TAlfActionCommand& aActionCommand );

protected:

// from base class CAlfControl

    /**
     * From CAlfControl.
     * Notifies the owner that the layout of a visual has been recalculated.
     * Called only when the EAlfVisualFlagLayoutUpdateNotification flag has
     * been set for the visual.
     *
     * @param aVisual  Visual that has been laid out.
     */
    virtual void VisualLayoutUpdated( CAlfVisual& aVisual );

    /**
     * From CAlfControl.
     * Called when an input event is being offered to the control.
     *
     * @param aEvent Event to be handled.
     * @return <code>ETrue</code>, if the event was handled. Otherwise
     * <code>EFalse</code>.
     */
    virtual TBool OfferEventL( const TAlfEvent& aEvent );

// from base class MFsControlButtonObserver

    /**
     * From MFsControlButtonObserver.
     * Invoked when one of button from control bar is pressed.
     *
     * @param aEvent event type from button.
     * @param aButtonId pressed button id.
     */
    virtual TBool HandleButtonEvent(
        TFsControlButtonEvent aEvent, TInt aButtonId );

private:

    /**
     * Constructor.
     *
     * @param aEnv huitk environment instance.
     * @param aParent parent control to which events will be sent.
     */
    CFsControlBar();

    /**
     * Constructs and initializes bar control.
     */
    void ConstructL( CAlfEnv& aEnv );

    /**
     * Sends event to observers.
     *
     * @param aEvent event sent to observers
     * @param aData additional data sent with event
     */
    void NotifyObservers(
        MFsControlBarObserver::TFsControlBarEvent aEvent,
        TInt aData = -1 );

    /**
     * Changes focus to another button or looses focus from control bar.
     *
     * @param aButton pointer to button which schould be focused.
     * If NULL focus is lost by the control bar.
     */
    void ChangeFocusL( CFsControlButton* aButton );

    /**
     * Sets needed variables when bar loses focus.
     *
     * @param aLooseFocusEvent cause of focus lost.
     */
    void LooseFocus(
        MFsControlBarObserver::TFsControlBarEvent aLooseFocusEvent );

private: // data

    /**
     * Control bar's model
     * Own.
     */
    CFsControlBarModel* iModel;

    /**
     * Control bar's visualiser
     * Own.
     */
    CFsControlBarVisualiser* iVisualiser;

    /**
     * Selected button index.
     */
    TInt iSelectedButton;

    /**
     * Collection of additional Observers.
     */
    RPointerArray< MFsControlBarObserver > iObservers;

    /**
     * Last screen width.
     */
    TInt iLastScreenWidth;

    /**
     * Text style manager.
     * Own.
     */
    CFsTextStyleManager* iTextStyleManager;

    // <cmail> Touch
    /**
     * Control touch
     */
    TBool iTouchEnabled;
    TBool iTouchWasEnabled;
    // </cmail>


    };


#endif // C_FSCONTROLBAR_H
