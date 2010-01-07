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
* Description:  Model class for control bar.
*
*/


#ifndef C_FSCONTROLBARMODEL_H
#define C_FSCONTROLBARMODEL_H

#include <e32base.h>
#include <e32cmn.h>
#include <gdi.h>

class CFsControlButton;
class CHuiTexture;

/**
 *  Model class of ControlBar component
 *
 *  @code
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
NONSHARABLE_CLASS( CFsControlBarModel ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CFsControlBarModel* NewL();

    /**
     * Two-phased constructor.
     */
    static CFsControlBarModel* NewLC();

    /**
     * Destructor
     */
    virtual ~CFsControlBarModel();

    /**
     * Adds button to model.
     *
     * @param aButton Button to be added.
     */
    void AddButtonL( CFsControlButton& aButton );

    /**
     * Retrieves button with specified index from model.
     *
     * @param aIndex Index of the button to be retrieved.
     * @return retrieved button
     */
    CFsControlButton* ButtonByIndex( TInt aIndex );

    /**
     * Retrieves button with specified id from model.
     *
     * @param aId Id of button to be retrieved.
     * @return retrieved button
     */
    CFsControlButton* ButtonById( TInt aId );

    /**
     * Gets actual number of buttons in model.
     *
     * @return number of buttons.
     */
    TInt Count() const;

    /**
     * Removes button with specified index from model.
     *
     * @param aIndex Index of button to be deleted
     */
    void RemoveButtonByIndex( TInt aIndex );

    /**
     * Removes button with specified id from model.
     *
     * @param aId Id of button to be deleted.
     * @return KErrNotFound If button's id doesn't exist, otherwise KErrNone.
     */
    TInt RemoveButtonById( TInt aId );

    /**
     * Checks if button with specified id is in the model.
     *
     * @param aId Id to be checked.
     * @return ETrue if yes, EFalse otherwise.
     */
    TBool ExistsButtonWithId( TInt aId );

    /**
     * Return index of the button with specified id.
     *
     * @param aButtonId Button's id.
     * @return Button's index in collection.
     */
    TInt IndexById( TInt aButtonId );

    /**
     * Retrieves button next to the button with specified id.
     *
     * @param aButtonId Current button's id.
     * @return Next button after current.
     */
    CFsControlButton* NextButton( TInt aButtonId );

    /**
     * Retrieves button previous to the button with specified id.
     *
     * @param aButtonId Current button's id.
     * @return Previous button after current.
     */
    CFsControlButton* PrevButton( TInt aButtonId );

    /**
     * Sets focus state for control bar.
     *
     * @param aState State to be set.
     */
    void SetFocus( TBool aState = ETrue );

    /**
     * Checks if focus.
     *
     * @return Current focus state.
     */
    TBool IsFocused() const;

    /**
     * Generates id for new button.
     *
     * @return Unique id for new button.
     */
    TInt GenerateButtonId();

    /**
     * Gets background color for control bar.
     *
     * @return Controlbar background color.
     */
    const TRgb& BarBgColor() const;

    /**
     * Gets transition time for moving selector.
     *
     * @return Transition time in miliseconds.
     */
    TInt SelectorTransitionTime() const;

    /**
     * Sets transition time for moving selector.
     *
     * @param aTransitionTime Transition time in miliseconds.
     */
    void SetSelectorTransitionTime( TInt aTransitionTime );

    /**
     * Refresh the buttons' positions.
     */
    void UpdateButtonsPositions();

    /**
     * Sets bar's width.
     *
     * @param aWidth New bar's width.
     */
    void SetWidth( TInt aWidth );

    /**
     * Sets bar's height.
     *
     * @param aHeight New bar's height.
     */
    void SetHeight( TInt aHeight );

    /**
     * Set bar's rect
     */
    void SetRect( const TRect& aRect );
    
    /**
     * Get size of the controlbar.
     *
     * @return Size of the controlbar.
     */
    TSize Size() const;

    /**
     * Get top-left position
     */
    TPoint Pos() const;
    
private:

    /**
     * Default constructor.
     */
    CFsControlBarModel();

    /**
     * Constructs and initializes bar model.
     */
    void ConstructL();

private: // data

    /**
     * Collection of button's objects.
     * Own all button objects.
     */
    RPointerArray<CFsControlButton> iButtons;

    /**
     * Focus state.
     */
    TBool iFocused;

    /**
     * Controlbar's background color.
     */
    TRgb iBgColor;

    /**
     * Delay for selector moves.
     */
    TInt iSelectorTransitionTime;

    /**
     * Size of the bar.
     */
    TSize iSize;

    /**
     * Width default flag.
     */
    TBool iWidthUseDefault;

    /**
     * Height default flag.
     */
    TBool iHeightUseDefault;

    /**
     * Top-left position 
     */
    TPoint iTl;
    };


#endif // C_FSCONTROLBARMODEL_H
