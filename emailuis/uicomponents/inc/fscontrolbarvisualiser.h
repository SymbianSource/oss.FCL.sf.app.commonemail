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
* Description:  Visualiser class for control bar.
*
*/


#ifndef C_FSCONTROLBARVISUALISER_H
#define C_FSCONTROLBARVISUALISER_H

#include <gdi.h>

class CAlfDeckLayout;
class CAlfVisual;
class CAlfLayout;
class CAlfTexture;
class CAlfFrameBrush;
class CAlfImageBrush;
class CAlfGradientBrush;
class CAlfTexture;
class CFsControlBar;
class CFsControlBarModel;


/**
 *  Visualiser class of ControlBar component
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
NONSHARABLE_CLASS( CFsControlBarVisualiser ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aParentControl Parent control.
     * @param aModel Model of control bar.
     */
    static CFsControlBarVisualiser* NewL(
        CFsControlBar& aParentControl,
        CFsControlBarModel& aModel );

    /**
     * Destructor
     */
    virtual ~CFsControlBarVisualiser();

    /**
     * Starts redrawing of the control bar.
     *
     * @param aScreenSizeChanged Screen size changed -> bar's width
     * has to be recalculated.
     */
    void RefreshL( TBool aScreenSizeChanged = EFalse );

    /**
     * Sets bar's background color.
     *
     * @param aColor Background color to be set.
     */
    void SetBackgroundColor( const TRgb& aColor );

    /**
     * Clears bar's background color. Bar is transparent.
     */
    IMPORT_C virtual void ClearBackgroundColor();

    /**
     * Sets bar's image.
     *
     * @param aImage Background image to be set.
     */
    void SetBackgroundImageL( CAlfTexture& aImage );

    /**
     * Clears bar's background image.
     */
    void ClearBackgroundImage();

    /**
     * Update the size of the controlbar layout.
     */
    void UpdateSizeL();

    /**
     * Retrieves layouter of the control bar.
     *
     * @return Control bar's layouter.
     */
    CAlfDeckLayout* Layout();

    /**
     * Sets transition time for selector.
     *
     * @param aTransitionTime Transition time in miliseconds.
     */
    void SetSelectorTransitionTimeL( TInt aTransitionTime );

    /**
     * Set selector image.
     * Gaines the ownership of the brush.
     *
     * @param aSelectorBrush New selector brush.
     * @param aOpacity Opacity of the selector.
     */
    void SetSelectorImageL( CAlfBrush* aSelectorBrush, TReal32 aOpacity );

    /**
     * Hides selector. Invoked when bar is focused no more.
     */
    void HideSelector();

    /**
     * Reorder the visuals. Button visual are build from two separate visual,
     * background and content. Those are arranged so that the background
     * visual are set at back and contents are to front. Selector visual is
     * placed between them.
     */
    void ReorderVisuals();

    /**
     * Enables selector drawing.
     * NOTE: Do not change focus state. Item might be focused altough
     *       selector is not visible.
     *  @param aShow if ETrue selector can be drawn.
     *  @param aFromTouch tells that was method called after touch event.
     */
    void MakeSelectorVisible( TBool aShow, TBool aFromTouch = EFalse );

private:

    /**
     * Constructor.
     *
     * @param aParentControl Parent control.
     * @param aModel Model of control bar.
     */
    CFsControlBarVisualiser( CFsControlBar& aParentControl,
        CFsControlBarModel& aModel );

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Draws selector over selected (focused) button.
     *
     * @param aSelectedButtonIndex Index of the button.
     * @param aFastDraw Updates selector's position without any transition
     * delay.
     */
    void DrawSelectorL( const TInt aSelectedButtonIndex, TBool aFastDraw );

private: // data

    /**
     * Model of control bar.
     * Not own.
     */
    CFsControlBarModel& iModel;

    /**
     * Parent control.
     * Not own.
     */
    CFsControlBar& iParent;

    /**
     * Layout for control bar.
     * Not own.
     */
    CAlfDeckLayout* iBarLayout;

    /**
     * Background layout for control bar.
     * Not own.
     */
    CAlfLayout* iBgLayout;

    /**
     * Background color.
     */
    CAlfGradientBrush* iBgColor;

    /**
     * Brush with the default background image.
     * Owned.
     */
    CAlfFrameBrush* iDefaultBgBrush;

    /**
     * Brush with the background image.
     * Owned.
     */
    CAlfImageBrush* iBgImage;

    /**
     * Visual for image of selector.
     * Not own.
     */
    CAlfVisual* iSelector;

    /**
     * Brush for selector.
     * Owned.
     */
    CAlfBrush* iSelectorBrush;

    /**
     * Selector's opacity.
     */
    TReal32 iSelectorOpacity;

    /**
     * Index of last selected button.
     */
    TInt iLastSelectedButton;

    /**
     * Flag that states if event was received after touch event.
     */
    TBool iTouchPressed;
    };


#endif // C_FSCONTROLBARVISUALISER_H
