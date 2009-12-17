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
* Description:  Main class for Control Button component.
*
*/


#ifndef C_FSCONTROLBUTTON_H
#define C_FSCONTROLBUTTON_H
//<cmail> SF
#include <alf/alfcontrol.h>
#include <alf/alftexturemanager.h>
#include <alf/alfgc.h>
//</cmail>
#include "fscontrolbuttoninterface.h"

class CAlfDeckLayout;
class CAlfTexture;
class CFsControlButtonModel;
class MFsControlButtonObserver;
class CFsControlButtonVisualiser;
class CFsTextStyleManager;

/**
 *  Main class for ControlButton component
 *  Control class of ControlButton component.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
  */
NONSHARABLE_CLASS( CFsControlButton ) :
    public CAlfControl,
    public MFsControlButtonInterface
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aEnv huitk environment instance.
     * @param aParent parent control.
     * @param aId unique id of the button.
     * @param aType Button type.
     * @param aStartPoint Starting point of the button.
     * @param aParentLayout layout of the button.
     * @param aTextStyleManager Text style manager.
     */
    static CFsControlButton* NewL(
        CAlfEnv& aEnv,
        MFsControlButtonObserver& aParent,
        TInt aId,
        TFsControlButtonType aType,
        TRect& aStartPoint,
        CAlfDeckLayout& aParentLayout,
        CFsTextStyleManager* aTextStyleManager );

    /**
     * Destructor.
     */
    virtual ~CFsControlButton();


    /**
     * Retrieves button's visualiser.
     *
     * @return visualiser of the button.
     */
    CFsControlButtonVisualiser* Visualiser();

    /**
     * Set the layout position index on controlbar.
     *
     * @param aLayoutPos New layout position on controlbar.
     */
    void SetParentIndex( TInt aLayoutPos );

    /**
     * Sets focus to control button.
     *
     * @param aState state of focus.
     */
    void SetFocus( TBool aState = ETrue );

    /**
     * Refresh the button's position.
     */
    void RefreshButtonPosition( TSize& aParentSize );

// from base class CAlfControl

    virtual TBool OfferEventL( const TAlfEvent& aEvent );

// from base class MFsControlButtonInterface

    /**
     * From MFsControlButtonInterface.
     * Sets first line of text in button.
     *
     * @param aLabel text to be set.
     * @param aContent Specifies the text row.
     */
    virtual void SetTextL( const TDesC& aLabel, TFsButtonContent aContent );

    /**
     * From MFsControlButtonInterface.
     * Sets visualiser for the button. Ownership of the new visualiser is
     * gained.
     *
     * @param aVisualiser New visualiser to be used to draw button.
     */
    virtual void SetVisualiserL( CFsControlButtonVisualiser* aVisualiser );

    /**
     * From MFsControlButtonInterface.
     * Retrieves id of the button.
     *
     * @return id of the button.
     */
    virtual TInt Id() const;

    /**
     * From MFsControlButtonInterface.
     * Sets icon in the button.
     *
     * @param aIcon icon to be added.
     * @param aWhich specifies which icon is to be added, A or B.
     */
    virtual void SetIconL(
        CAlfTexture& aIcon,
        TFsControlButtonElem aWhich = ECBElemIconA );

    /**
     * From MFsControlButtonInterface.
     * Sets width of the button.
     *
     * @param aWidth width of the button.
     */
    virtual void SetWidth( TInt aWidth );

    // <cmail> Platform layout changes
    /**
     * @see MFsControlButtonInterface::SetSize
     */
    virtual void SetSize( const TSize& aSize );
    // </cmail> Platform layout changes

    /**
     * From MFsControlButtonInterface.
     * Set auto size mode for button.
     * Defines how the buttons size is changed.
     *
     * @param aAutoSizeMode new mode.
     */
    virtual void SetAutoSizeMode(
        MFsControlButtonInterface::TFsAutoSizeMode aAutoSizeMode );

    /**
     * From MFsControlButtonInterface.
     * Checks if button has focus.
     *
     * @return ETrue if focused, EFalse otherwise.
     */
    virtual TBool IsFocused() const;

    // <cmail> Platform layout changes
    /**
     * From MFsControlButtonInterface.
     * Sets position of the button (top left point).
     *
     * @param aTlPoint coordinates of top left point.
     */
    virtual void SetPos( const TPoint& aTlPoint );
    // </cmail> Platform layout changes

    /**
     * From MFsControlButtonInterface.
     * Retrieves position of button.
     *
     * @return  position of button.
     */
    virtual const TAlfTimedPoint Pos() const;

    /**
     * From MFsControlButtonInterface.
     * Retrieves size of button.
     *
     * @return size of button.
     */
    virtual const TAlfTimedPoint Size() const;

    /**
     * From MFsControlButtonInterface.
     * Sets component to button which will be triggered when button
     * is pressed.
     *
     * @param aComponent component to be set.
     */
    virtual void SetTriggeredComponent( MFsTriggeredComponent& aComponent );

    /**
     * From MFsControlButtonInterface.
     * Clears triggered component. No more events to triggered
     * component are sent.
     */
    virtual void ClearTriggeredComponent();

    /**
     * From MFsControlButtonInterface.
     * Sets alignment of element.
     *
     * @param aButtonElem element to be aligned.
     * @param aHAlign horizontal alignment of element.
     * @param aVAlign vertical alignment of element.
     */
    virtual void SetElemAlignL( TFsControlButtonElem aButtonElem,
                       TAlfAlignHorizontal aHAlign,
                       TAlfAlignVertical aVAlign );

    /**
     * From MFsControlButtonInterface.
     * Sets dimm state of the button.
     *
     * @param aDimmed dimm state of the the button.
     */
    virtual void SetDimmed( TBool aDimmed = ETrue );

    /**
     * From MFsControlButtonInterface.
     * Checks dimm state of the button.
     *
     * @return ETrue if dimmed, EFalse otherwise.
     */
    virtual TBool IsDimmed() const;

    /**
     * From MFsControlButtonInterface.
     * Shows (enables) button.
     */
    virtual void ShowButtonL();

    /**
     * From MFsControlButtonInterface.
     * Hides (disables) button.
     */
    virtual void HideButton();

    /**
     * From MFsControlButtonInterface.
     * Checks if button is visible (enabled).
     *
     * @return ETrue if enabled, EFalse otherwise.
     */
    virtual TBool IsVisible() const;

    /**
     * From MFsControlButtonInterface.
     * Sets bar's background image. Ownership of the object is transfered.
     *
     * @param aImage Background image brush.
     */
    virtual void SetBackgroundImageL( CAlfImageBrush* aImage );

    /**
     * From MFsControlButtonInterface.
     * Sets button's background color.
     *
     * @param aColor color of background.
     */
    virtual void SetBackgroundColor( const TRgb& aColor );

    /**
     * From MFsControlButtonInterface.
     * Clears button's background color. Button is transparent.
     */
    virtual void ClearBackgroundColor();

    /**
     * From MFsControlButtonInterface.
     * Clears button's background image.
     */
    virtual void ClearBackgroundImage();

    /**
     * From MFsControlButtonInterface.
     * Retrieves text from control button.
     *
     * @param aContent Specifies the text row.
     * @return text on control button.
     */
    virtual TPtrC Text( TFsButtonContent aContent ) const;

    /**
     * From MFsControlButtonInterface.
     * Set new height for the button text.
     *
     * @param aTextHeight Height in pixels.
     */
    virtual void SetTextHeight( const TInt aTextHeight );

    /**
     * From MFsControlButtonInterface.
     * Change the current font.
     *
     * @param aFont Font specification.
     */
    virtual void SetTextFontL( const TFontSpec& aFont );

    /**
     * From MFsControlButtonInterface.
     * Set button's text color when it's not focused or dimmed.
     *
     * @param aColor New color.
     */
    virtual void SetNormalTextColor( const TRgb& aColor );

    /**
     * From MFsControlButtonInterface.
     * Set button's text color when it's focused.
     *
     * @param aColor New color.
     */
    virtual void SetFocusedTextColor( const TRgb& aColor );

    /**
     * From MFsControlButtonInterface.
     * Set button's text color when it's dimmed.
     *
     * @param aColor New color.
     */
    virtual void SetDimmedTextColor( const TRgb& aColor );

    /**
     * From MFsControlButtonInterface.
     * Retrieves control button type.
     *
     * @return control button type.
     */
    virtual TFsControlButtonType ControlButtonType() const;

// <cmail> Touch
    virtual CAlfControl* AsAlfControl();
// </cmail>

    /**
     * Enables focus drawing for item and draws it.
     * NOTE: Do not change focus state. Item might be focused altough
     *       selector is not visible.
     * @param aShow If ETrue visibility of focus is drawn if
     *              EFalse visibility of focus is hidden.
     */
    void MakeFocusVisible( TBool aShow );

private:

    /**
     * Constructor.
     *
     * @param aParent parent control.
     * @param aParentLayout If given, has the parent layout.
     */
    CFsControlButton( MFsControlButtonObserver& aParent,
        CAlfDeckLayout& aParentLayout );

    /**
     * Constructs and initializes button control.
     *
     * @param aEnv Environment.
     * @param aId unique id of the button.
     * @param aType type of the button.
     * @param aStartPoint Starting point of the button.
     * @param aTextStyleManager Text style manager.
     */
    void ConstructL(
        CAlfEnv& aEnv,
        TInt aId,
        TFsControlButtonType aType,
        TRect& aStartPoint,
        CFsTextStyleManager* aTextStyleManager );

    /**
     * Starts to listen unhandled pointer events.
     *
     * @param aGrab ETrue adds pointer event observer if EFalse removes it.
     */
    void GrabPointerEvents( TBool aGrab );

private: // data

    /**
     * Parent of button.
     * Not own.
     */
    MFsControlButtonObserver& iParent;

    /**
     * Button's model.
     * Own.
     */
    CFsControlButtonModel* iModel;

    /**
     * Button's visualiser.
     * Own.
     */
    CFsControlButtonVisualiser* iVisualiser;

    /**
     * Component triggered by button.
     * Not own.
     */
    MFsTriggeredComponent* iTriggeredComponent;

    /**
     * Flag for visibility state changed from hidden to visible.
     */
    TBool iWasHidden;

    /**
     * Parent layout.
     */
    CAlfDeckLayout& iParentLayout;

    /**
     * Flag that states if event was received after touch event.
     */
    TBool iTouchPressed;

    };

#endif // C_FSCONTROLBUTTON_H
