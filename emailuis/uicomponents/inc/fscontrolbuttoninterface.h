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
* Description:  Interface for FsControlButton class
*
*/


#ifndef M_FSCONTROLBUTTONINTERFACE_H
#define M_FSCONTROLBUTTONINTERFACE_H

//<cmail> SF
#include <alf/alfgc.h>
#include <alf/alftimedvalue.h>
//</cmail>
#include "fscontrolbuttonconst.h"

class CAlfTexture;
class CAlfImageBrush;
class CFsControlButtonVisualiser;
class MFsTriggeredComponent;
class CFsControlButton;

/**
 *  Interface for FsControlButton class.
 *
 *  Interface for FsControlButton class which is available for the component
 *  user. No direct access to FsControlButton methods is available.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
NONSHARABLE_CLASS( MFsControlButtonInterface )
    {

public:

    /** Specifies the text row. */
    enum TFsButtonContent
        {
        EFsButtonFirstLine,
        EFsButtonSecondLine
        };

    /**  Auto size modes for button. */
    enum TFsAutoSizeMode
        {
        EFsManual,
        EFsFitToContent,
        EFsFitToParent,
        EFsLayout
        };

    /**
     * Sets first line of text in button.
     *
     * @param aLabel text to be set.
     * @param aContent Specifies the text row.
     */
    virtual void SetTextL(
        const TDesC& aLabel,
        TFsButtonContent aContent = EFsButtonFirstLine ) = 0;

    /**
     * Retrieves id of the button.
     *
     * @return id of the button.
     */
    virtual TInt Id() const = 0;

    /**
     * Sets icon in the button.
     *
     * @param aIcon icon to be added.
     * @param aWhich specifies which icon is to be added, A or B.
     */
    virtual void SetIconL(
        CAlfTexture& aIcon,
        TFsControlButtonElem aWhich = ECBElemIconA ) = 0;

    /**
     * Retrieves button's visualiser. Ownership of the new visualiser is
     * gained.
     *
     * @param aVisualiser New visualiser to be used to draw button.
     */
    virtual void SetVisualiserL(
        CFsControlButtonVisualiser* aVisualiser ) = 0;

    /**
     * Sets component to button which will be triggered when button
     * is pressed.
     *
     * @param aComponent component to be set.
     */
    virtual void SetTriggeredComponent(
        MFsTriggeredComponent& aComponent ) = 0;

    /**
     * Clears triggered component. No more events to triggered
     * component are sent.
     */
    virtual void ClearTriggeredComponent() = 0;

    // <cmail> Platform layout changes
    /**
     * Sets position of the button (top left point).
     *
     * @param aTlPoint coordinates of top left point.
     */
    virtual void SetPos( const TPoint& aTlPoint ) = 0;
    // </cmail> Platform layout changes

    /**
     * Sets alignement of element.
     *
     * @param aButtonElem element to be aligned.
     * @param aHAlign horizontal alignement of element.
     * @param aVAlign vertical alignement of element.
     */
    virtual void SetElemAlignL( TFsControlButtonElem aButtonElem,
        TAlfAlignHorizontal aHAlign, TAlfAlignVertical aVAlign ) = 0;

    /**
     * Sets width of the button.
     *
     * @param aWidth width of the button.
     */
    virtual void SetWidth( TInt aWidth ) = 0;

    // <cmail> Platform layout changes
    /**
     * Sets size of the button.
     *
     * @param aSize size of the button.
     */
    virtual void SetSize( const TSize& aSize ) = 0;
    // </cmail> Platform layout changes

    /**
     * Set auto size mode for button.
     * Defines how the buttons size is changed.
     *
     * @param aAutoSizeMode new mode.
     */
    virtual void SetAutoSizeMode( TFsAutoSizeMode aAutoSizeMode ) = 0;

    /**
     * Sets dimm state of the button.
     *
     * @param aDimmed dimm state of the the button.
     */
    virtual void SetDimmed( TBool aDimmed = ETrue ) = 0;

    /**
     * Checks if button is dimmed.
     *
     * @return ETrue if dimmed, EFalse otherwise.
     */
    virtual TBool IsDimmed() const = 0;

    /**
     * Shows (enables) button.
     */
    virtual void ShowButtonL() = 0;

    /**
     * Hides (disables) button.
     */
    virtual void HideButton() = 0;

    /**
     * Checks if button is visible (enabled).
     *
     * @return ETrue if enabled, EFalse otherwise.
     */
    virtual TBool IsVisible() const = 0;

    /**
     * Sets button's background image. Ownership of the object is transfered.
     *
     * @param aImage Background image brush.
     */
    virtual void SetBackgroundImageL( CAlfImageBrush* aImage ) = 0;

    /**
     * Sets button's background color.
     *
     * @param aColor color of background.
     */
    virtual void SetBackgroundColor( const TRgb& aColor ) = 0;

    /**
     * Clears button's background color. Button is transparent.
     */
    virtual void ClearBackgroundColor() = 0;

    /**
     * Clears button's background image.
     */
    virtual void ClearBackgroundImage() = 0;

    /**
     * Checks if button has focus.
     *
     * @return ETrue if focused, EFalse otherwise.
     */
    virtual TBool IsFocused() const = 0;

    /**
     * Retrieves position of button.
     *
     * @return  position of button.
     */
    virtual const TAlfTimedPoint Pos() const = 0;

    /**
     * Retrieves size of button.
     *
     * @return size of button.
     */
    virtual const TAlfTimedPoint Size() const = 0;

    /**
     * Retrieves button's text.
     *
     * @param aContent Specifies the text row.
     * @return Text of the button.
     */
    virtual TPtrC Text(
        TFsButtonContent aContent = EFsButtonFirstLine ) const = 0;

    /**
     * Set new height for the button text.
     *
     * @param aTextHeight Height in pixels.
     */
    virtual void SetTextHeight( const TInt aTextHeight ) = 0;

    /**
     * Set button's text color when it's not focused or dimmed.
     *
     * @param aColor New color.
     */
    virtual void SetNormalTextColor( const TRgb& aColor ) = 0;

    /**
     * Set button's text color when it's focused.
     *
     * @param aColor New color.
     */
    virtual void SetFocusedTextColor( const TRgb& aColor ) = 0;

    /**
     * Set button's text color when it's dimmed.
     *
     * @param aColor New color.
     */
    virtual void SetDimmedTextColor( const TRgb& aColor ) = 0;

    /**
     * Change the current font.
     *
     * @param aFont Font specification.
     */
    virtual void SetTextFontL( const TFontSpec& aFont ) = 0;

    /**
     * Retrieves button's type.
     *
     * @return aImage background image.
     */
    virtual TFsControlButtonType ControlButtonType() const = 0;

// <cmail> Touch
    virtual CAlfControl* AsAlfControl() = 0;
// </cmail>
    /**
     * Destructor.
     */
    virtual ~MFsControlButtonInterface() {};

    };


#endif // M_FSCONTROLBUTTONINTERFACE_H
