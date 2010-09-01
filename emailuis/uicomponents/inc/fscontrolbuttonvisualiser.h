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
* Description:  Visualiser class of the button.
*
*/


#ifndef C_FSCONTROLBUTTONVISUALISER_H
#define C_FSCONTROLBUTTONVISUALISER_H

#include <gdi.h>
//<cmail> SF
#include <alf/alfgc.h>
#include <alf/alftexturemanager.h>
//</cmail>
#include "fscontrolbuttonconst.h"

class CAlfControl;
class CAlfVisual;
class CAlfLayout;
class CAlfBrush;
class CAlfFrameBrush;
class CAlfTextVisual;
class CAlfGridLayout;
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CAlfImageVisual;
class TAlfTextStyle;
class TAlfTimedPoint;
class CAlfTexture;
class CAlfGradientBrush;
class CAlfImageBrush;
class CAlfShadowBorderBrush;
class CFsControlButtonModel;
class CFsControlButtonLayoutData;
class CFsTextStyleManager;
class TCharFormat;

/**
 *  Visualiser class of the button.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
class CFsControlButtonVisualiser : public CBase,
    public MAlfTextureLoadingCompletedObserver
    {

public:

    /**
     * Two-phased constructor..
     */
    static CFsControlButtonVisualiser* NewL(
        CAlfControl& aParent,
        CAlfDeckLayout& aParentLayout,
        CFsTextStyleManager* aTextStyleManager );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CFsControlButtonVisualiser();

    /**
     * Attaches model to visualiser.
     *
     * @param aButtonModel button's model.
     */
    IMPORT_C virtual void InitializeL( CFsControlButtonModel& aButtonModel );

    /**
     * Called when the colors etc should be refreshed
     */
    virtual void UpdateVisualThemeL();
    
    /**
     * Refreshes button on the display.
     */
    IMPORT_C virtual void Refresh();

    /**
     * Refreshes button on the display placing it at specified point.
     *
     * @param aStartAt start point of the buton.
     */
    IMPORT_C virtual void Refresh( TPoint aStartAt );

    /**
     * Retrieves end X coordinate of the button.
     *
     * @return X coordinate of the bottom right point of the button.
     */
    TInt GetBottomRight() const;

    /**
     * Retrieves position of the button.
     *
     * @return start point of the button.
     */
    const TAlfTimedPoint Pos() const;

    /**
     * Retrieves size of the button.
     *
     * @return size of the button.
     */
    const TAlfTimedPoint Size() const;

    /**
     * Sets background image of the button. Ownership of the object is transfered.
     *
     * @param aImage Image brush that is used at background.
     */
    IMPORT_C virtual void SetBackgroundImageL( CAlfImageBrush* aImage );

    /**
     * Sets button's background color.
     *
     * @param aColor background color to be set.
     */
    IMPORT_C virtual void SetBackgroundColor( const TRgb& aColor );

    /**
     * Clears button's background color. Button is transparent.
     */
    IMPORT_C virtual void ClearBackgroundColor();

    /**
     * Clears button's background image.
     */
    IMPORT_C virtual void ClearBackgroundImage();

    /**
     * Sets alignment of element of the button.
     *
     * @param aButtonElem element to be aligned.
     * @param aHAlign horizontal alignement of element.
     * @param aVAlign vertical alignement of element.
     */
    IMPORT_C virtual void SetElemAlignL(
        TFsControlButtonElem aButtonElem,
        TAlfAlignHorizontal aHAlign,
        TAlfAlignVertical aVAlign );

    /**
     * Set new height for the button text.
     * Setting a negative or zero value changes the value back to default.
     *
     * @param aTextHeight Height in pixels.
     */
    IMPORT_C virtual void SetTextHeight( const TInt aTextHeight );

    /**
     * Change the current font.
     *
     * @param aFontSpec Font specification.
     */
    IMPORT_C virtual void SetTextFontL( const TFontSpec& aFontSpec );

    /**
     * Updates element content from the model.
     *
     * @param aButtonElem element to be updated.
     */
    IMPORT_C virtual void UpdateElementL( TFsControlButtonElem aButtonElem );

    /**
     * Retrieves width of the button.
     *
     * @return width of the button.
     */
    TInt Width() const;

    /**
     * Updates size of button.
     */
    IMPORT_C virtual void UpdateButtonSize();

    /**
     * Retrieves the background layout of the button.
     *
     * @return Background layout of the button.
     */
    CAlfLayout* Layout();

    /**
     * Retrieves the content layout of the button.
     *
     * @return Content layout of the button.
     */
    CAlfLayout* ContentLayout();

    /**
     * Enables button.
     */
    IMPORT_C virtual void ShowL();

    /**
     * Disables button.
     */
    IMPORT_C virtual void Hide();

    /**
     * Checks if button is visible or not.
     *
     * @return ETrue if visible, EFalse if hidden.
     */
    TBool IsVisible() const;

    /**
     * Sets parent layout.
     *
     * @param aParentLayout layout of parent control.
     */
    void SetParentLayoutL( CAlfDeckLayout* aParentLayout );

    /**
     * Sets parent control.
     *
     * @param aParentControl parent control.
     */
    void SetParentControlL( CAlfControl* aParentControl );

    /**
     * Assign a new text style manager.
     *
     * @param aTextStyleManager New text style manager.
     */
    void SetTextStyleManager( CFsTextStyleManager* aTextStyleManager );

    /**
     * Update elemets after texture loading is completed.
     *
     * @param aTexture Reference to the texture that has been loaded.
     * @param aTextureId Id of the texture in the texture manager. Can be used
     * to identify the loaded texture, provided that an id was assigned to the
     * texture.
     * @param aErrorCode KErrNone if the load was successful, otherwise one of
     * the system-wide error codes indicating reason why the texture loading
     * failed.
     */
    IMPORT_C virtual void TextureLoadingCompleted(
        CAlfTexture& aTexture, TInt aTextureId, TInt aErrorCode );

    /**
     * Enables focus drawing for item and draws it.
     * NOTE: Do not change focus state. Item might be focused altough
     *       selector is not visible.
     * @param aShow If ETrue visibility of focus is drawn if
     *              EFalse visibility of focus is hidden.
     */
    void MakeFocusVisible( TBool aShow );

protected:

    /**
     * Sets colors and bitmaps for drawing button in "focused" state.
     */
    IMPORT_C virtual void PrepareDrawFocused();

    /**
     * Sets colors and bitmaps for drawing button in "normal
     * (no focus, enabled)" state.
     */
    IMPORT_C virtual void PrepareDrawNormal();

    /**
     * Sets colors and bitmaps for drawing button in "dimmed" state.
     */
    IMPORT_C virtual void PrepareDrawDimmed();

protected:

    /**
     * Constructor.
     *
     * @param aParent parent control.
     * @param aParentLayout layout of parent control.
     * @param aTextStyleManager Text style manager.
     */
    IMPORT_C CFsControlButtonVisualiser(
        CAlfControl& aParent,
        CAlfDeckLayout& aParentLayout,
        CFsTextStyleManager* aTextStyleManager );

    /**
     * Constructor.
     */
    IMPORT_C CFsControlButtonVisualiser();

    /**
     * Second phase constructor.
     */
    IMPORT_C virtual void ConstructL();

    /**
     * Creates needed visuals for specified button type.
     */
    IMPORT_C virtual void CreateButtonVisualsL();

    /**
     * Calculates button size according to content.
     *
     * @return size of button.
     */
    IMPORT_C virtual TInt CalculateButtonSize();

    /**
     * Updates position of button.
     */
    void UpdateButtonPos();

    /**
     * Updates size of columns in grid layouter for button's elements.
     *
     * @param aButtonType type of button.
     */
    IMPORT_C virtual void UpdateElementsSizeL(
        TFsControlButtonType aButtonType );

    /**
     * Sets text styles for specified button type.
     */
    IMPORT_C virtual void UpdateTextStyles();

    /**
     * Sets alignment of specified image of the button.
     *
     * @param aImage image to be aligned.
     * @param aHAlign horizontal alignement of element.
     * @param aVAlign vertical alignement of element.
     */
    IMPORT_C virtual void SetImageAlign( CAlfImageVisual* aImage,
                                         TAlfAlignHorizontal aHAlign,
                                         TAlfAlignVertical aVAlign );

    /**
     * Sets alignment of specified text of the button.
     *
     * @param aText text to be aligned.
     * @param aHAlign horizontal alignement of element.
     * @param aVAlign vertical alignement of element.
     */
    IMPORT_C virtual void SetTextAlign( CAlfTextVisual* aText,
                                        TAlfAlignHorizontal aHAlign,
                                        TAlfAlignVertical aVAlign );

    /**
     * Updates bar layout.
     */
    void UpdateBarLayout();

    /**
     * Resolve the character format from current text visual.
     *
     * @param aCharFormat Place holder for resolved character format.
     * @param aTextVisual Text visual.
     */
    void ResolveCharFormat(
        TCharFormat& aCharFormat,
        CAlfTextVisual* aTextVisual );

protected: // data

    /**
     * Parent control.
     * Not own.
     */
    CAlfControl*    iParent;

    /**
     * Model of the button.
     * Not owned.
     */
    CFsControlButtonModel* iButtonModel;

    /**
     * Layout of the button control.
     * Owned (because of possibility to change visualiser -
     * old one has to be removed from parent and layout tree).
     */
    CAlfLayout* iButtonLayout;

    /**
     * Layout for button content.
     * Not own.
     */
    CAlfLayout* iButtonContentLayout;

    /**
     * Visual for first line of text.
     * Not own.
     */
    CAlfTextVisual* iLabelFirstLine;

    /**
     * Visual for second line of text.
     * Not own.
     */
    CAlfTextVisual* iLabelSecondLine;

    /**
     * Height of the button's text in pixels.
     */
    TInt iTextHeight;

    /**
     * Font specification.
     * Owned.
     */
    TFontSpec* iTextFontSpec;

    /**
     * Flag to determine if the text size has been changed.
     */
    TBool iTextHeightSet;

    /**
     * Visual for the A icon.
     * Not own.
     */
    CAlfImageVisual* iIconA;

    /**
     * Container for icon A.
     * Not own.
     */
    CAlfLayout* iIconAContainer;

    /**
     * Visual for the B icon.
     * Not own.
     */
    CAlfImageVisual* iIconB;

    /**
     * Container for icon B.
     * Not own.
     */
    CAlfLayout* iIconBContainer;

    /**
     * Background color.
     * Owned.
     */
    CAlfGradientBrush* iBgColorBrush;

    /**
     * Brush with background image.
     * Owned.
     */
    CAlfBrush* iBgBrush;

    /**
     * Default brush for background. Owned.
     */
    CAlfFrameBrush* iDefaultBgBrush;

	/**
	 * ETrue if default theme background is used
	 */
    TBool iUseDefaultBackground;
    
    /**
     * Brush for shadowed button borders.
     * Owned.
     */
    CAlfShadowBorderBrush* iShadowBorderBrush;

    /**
     * Layout of the parent control.
     * Not owned.
     */
    CAlfDeckLayout* iParentLayout;

    /**
     * Flag for recalculate positions of icons.
     */
    TBool iUpdateIconsAlign;

    /**
     * First draw.
     */
    TBool iFirstDraw;

    /**
     * Horizontal alignement of A icon.
     */
    TAlfAlignHorizontal iAIconHAlign;

    /**
     * Vertical alignement of A icon.
     */
    TAlfAlignVertical iAIconVAlign;

    /**
     * Horizontal alignement of B icon.
     */
    TAlfAlignHorizontal iBIconHAlign;

    /**
     * Vertical alignement of B icon.
     */
    TAlfAlignVertical iBIconVAlign;

    /**
     * Flag for visible/hidden state.
     */
    TBool iVisible;

    /**
     * Visual for button pane.
     * Not own.
     */
    CAlfVisual* iButtonPane;

    /**
     * Background color.
     */
    TRgb iBackgroundColor;

    /**
     * Text style manager.
     * Not own.
     */
    CFsTextStyleManager* iTextStyleManager;

    /**
     * States that can focus been drawn.
     */
    TBool iDrawFocus;

    /**
     * Show dropdown shadow for buttons  
     */
    TBool iShowShadow;
    };


#endif // C_FSCONTROLBUTTONVISUALISER_H
