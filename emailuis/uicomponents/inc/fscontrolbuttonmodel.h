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
* Description:  Model class for control button.
*
*/


#ifndef C_FSCONTROLBUTTONMODEL_H
#define C_FSCONTROLBUTTONMODEL_H
//<cmail> SF
#include <alf/alftexture.h>
//</cmail>
#include <e32base.h>

#include "fscontrolbuttonconst.h"
#include "fscontrolbuttoninterface.h"

class CAlfLayout;

/**
 *  Model class of ControlButton component
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib ?library
 */
NONSHARABLE_CLASS( CFsControlButtonModel ) : public CBase
    {

public:

    /**  Defines button states */
    enum TFsButtonStateType
        {
        EButtonNormal,
        EButtonFocused,
        EButtonDimmed,
        EButtonBackground
        };

    /**
     * Two-phased constructor.
     */
    static CFsControlButtonModel* NewL( TInt aId, TFsControlButtonType aType,
        const TRect& aStartPoint, CAlfLayout& aParentLayout );

    /**
     * Destructor
     */
    virtual ~CFsControlButtonModel();

    /**
     * Sets id of the button.
     *
     * @param aId id to be set.
     */
    void SetId( TInt aId );

    /**
     * Retrieves button's id.
     *
     * @return button's id.
     */
    TInt Id();

    /**
     * Sets type of the button.
     *
     * @param aType type to be set.
     */
    void SetType( TFsControlButtonType aType );

    /**
     * Retrieves button's type.
     *
     * @return button's type.
     */
    TFsControlButtonType Type();

    /**
     * Sets first line of text of the button.
     *
     * @param aLabel text to be set.
     * @param aContent Specifies the text element.
     */
    void SetTextL(
        const TDesC& aLabel,
        MFsControlButtonInterface::TFsButtonContent aContent );

    /**
     * Change the color of defined target.
     *
     * @param aColor New color.
     * @param aType Target type.
     */
    void SetTextColor( const TRgb& aColor, TFsButtonStateType aType );

    /**
     * Retrieves the requested target color.
     *
     * @param aType Target type.
     * @return Color of requested target.
     */
    const TRgb& TextColor( TFsButtonStateType aType ) const;

    /**
     * Retrieves first line of button's text.
     *
     * @param aContent Specifies the text element.
     * @return button's text.
     */
    TPtrC Text( MFsControlButtonInterface::TFsButtonContent aContent );

    /**
     * Horizontal padding for text in button.
     *
     * @param aType Type of the button.
     * @return Horizontal padding.
     */
    TInt LabelHPadding( TFsControlButtonType aType ) const;

    /**
     * Vertical padding for text in button.
     *
     * @param aType Type of the button.
     * @return Vertical padding.
     */
    TInt LabelVPadding( TFsControlButtonType aType ) const;

    /**
     * Sets width of the button.
     *
     * @param aWidth width to be set.
     */
    void SetWidth( TInt aWidth );

    /**
     * Set size for the button.
     *
     * @param aSize New size of the button.
     */
    void SetSize( TSize aSize );

    /**
     * Retrieves size of the button.
     *
     * @return size of the button.
     */
    const TSize& Size() const;

    /**
     * Set auto size mode for button.
     * Defines how the buttons size is changed.
     *
     * @param aAutoSizeMode new mode.
     */
    void SetAutoSizeMode(
        MFsControlButtonInterface::TFsAutoSizeMode aAutoSizeMode );

    /**
     * Resolve the current mode.
     *
     * @return Current auto size mode.
     */
    MFsControlButtonInterface::TFsAutoSizeMode AutoSizeMode();

    /**
     * Sets focus to control button.
     *
     * @param aState state of focus.
     */
    void SetFocus( TBool aState = ETrue );

    /**
     * Checks if button has focus.
     *
     * @return ETrue if focused, EFalse otherwise.
     */
    TBool IsFocused();

    /**
     * Sets dimmed state of control button.
     *
     * @param aDimmed state of control button.
     */
    void SetDimmed( TBool aDimmed = ETrue );

    /**
     * Checks if button is dimmed (read only).
     *
     * @return ETrue if enabled, EFalse otherwise.
     */
    TBool IsDimmed();

    /**
     * Sets icon in the button.
     *
     * @param aIcon icon to be added.
     * @param aElemType specifies which icon is to be added, A or B.
     */
    void SetIconL( CAlfTexture& aIcon,
        TFsControlButtonElem aElemType = ECBElemIconA );

    /**
     * Retrieves specified icon image.
     *
     * @param aElemType icon to be retrieved.
     * @return icon image.
     */
    CAlfTexture* Icon( TFsControlButtonElem aElemType );

    // <cmail> Platform layout changes
    /**
     * Sets position of the button.
     *
     * @param aTlPoint top left point of the button.
     * @param aClearFlag ETrue to remove autopositioning flag from the button,
     * EFalse otherwise.
     */
    void SetPos( const TPoint& aTlPoint, TBool aClearFlag = ETrue );
    // </cmail> Platform layout changes

    /**
     * Retrieves top left point of the button.
     *
     * @return top left point of the button.
     */
    TPoint TopLeftPoint();

    /**
     * Checks if current set button type contains specified icon type.
     *
     * @param aWhich icon to be checked.
     * @return ETrue if contains, WFalse otherwise.
     */
    TBool ContainsElement( TFsControlButtonElem aWhich );

    /**
     * Checks if size of button was changed.
     *
     * @return ETrue if width or height changed, EFalse otherwise.
     */
    TBool ButtonSizeChanged();

    /**
     * Checks if position of button was changed.
     *
     * @return ETrue if position changed, EFalse otherwise.
     */
    TBool ButtonPosChanged();

    /**
     * Checks if button should be automatically positioned.
     *
     * @return ETrue if button should be positioned automatically
     *         EFalse if position was set to fixed value.
     */
    TBool AutoPosition();

    /**
     * Resolve the button's size from layout.
     *
     * @return Button's layout defined size.
     */
    TSize GetLayoutSize();

    /**
     * Resolve the button's size from layout.
     *
     * @param aParentRect Size and position of the parent rect.
     * @return Button's layout defined size.
     */
    TSize GetLayoutSize( TRect& aParentRect );

    /**
     * Get the layout position on controlbar.
     *
     * @return Layout position on controlbar.
     */
    TInt GetLayoutPos();

    /**
     * Set the layout position index on controlbar.
     *
     * @param aLayoutPos New layout position on controlbar.
     */
    void SetParentIndex( TInt aLayoutPos );

    /**
     * Resolve if layout data is used to define the size of the button.
     *
     * @return ETrue if layout data is used, otherwise EFalse.
     */
    TBool IsLayoutSize();

    /**
     * Refresh button's position.
     *
     * @param aParentSize Size of the button's parent.
     */
    void RefreshButtonPosition( const TSize& aParentSize );

private:

    CFsControlButtonModel( TInt aId, TFsControlButtonType aType,
        const TRect& aStartPoint, CAlfLayout& aParentLayout );

    /**
    * Constructs and initializes button's model.
    */
    void ConstructL();

private: // data

    /**
     * Button's type.
     */
    TFsControlButtonType iType;

    /**
     * Button's id value.
     */
    TInt iId;

    /**
     * Focus flag for the button.
     */
    TBool iFocused;

    /**
     * Texture of A icon of the button.
     * Not own.
     */
    CAlfTexture* iIconA;

    /**
     * Texture of B icon of the button.
     * Not own.
     */
    CAlfTexture* iIconB;

    /**
     * First line of text
     * Own.
     */
    HBufC* iFirstTextLine;

    /**
     * Second line of text
     * Own.
     */
    HBufC* iSecondTextLine;

    /**
     * Text horizontal padding for one text line item.
     */
    TInt iLabelHPadding1;

    /**
     * Text horizontal padding for two text lines item.
     */
    TInt iLabelHPadding2;

    /**
     * Text vertical padding for one text line item.
     */
    TInt iLabelVPadding1;

    /**
     * Text vertical padding for two text lines item.
     */
    TInt iLabelVPadding2;

    /**
     * Normal button text color.
     */
    TRgb iColorNormal;

    /**
     * Focused button text color.
     */
    TRgb iColorFocused;

    /**
     * Dimmed button text color.
     */
    TRgb iColorDimmed;

    /**
     * Button's background color.
     */
    TRgb iColorBackground;

    /**
     * Start point of the button (TopLeft)
     */
    TPoint iStartPoint;

    /**
     * Flag for dimmed state.
     */
    TBool iDimmed;

    /**
     * Flag for size changed.
     */
    TBool iSizeChanged;

    /**
     * Is button auto posiotioned or has fixed position.
     */
    TBool iAutoPosition;

    /**
     * Flag for position changed.
     */
    TBool iPosChanged;

    /**
     * Size of the item.
     */
    TSize iSize;

    /**
     * Current autosize mode info.
     */
    MFsControlButtonInterface::TFsAutoSizeMode iAutoSizeMode;

    /**
     * Reference to button's layout.
     */
    CAlfLayout& iParentLayout;

    /**
     * Position on controlbar.
     */
    TInt iLayoutPos;

    /**
     * Bit to define if layout data used to define buttons size.
     */
    TBool iUseLayoutData;

    /**
     * Button's manually set position. Used to determine mirrored position.
     */
    TPoint iManualSetPos;

    };


#endif // C_FSCONTROLBUTTONMODEL_H
