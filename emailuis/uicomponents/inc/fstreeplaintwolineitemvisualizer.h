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
* Description:  A visualizer for data items with plain text.
*
*/


#ifndef C_FSTREEPLAINTWOLINEITEMVISUALIZER_H
#define C_FSTREEPLAINTWOLINEITEMVISUALIZER_H

#include <e32base.h>
#include "fstreeplainonelineitemvisualizer.h"

class CFsTreePlainTwoLineItemData;
class TFsMarqueeClet;


/**
 * CFsTreePlainTwoLineItemVisualizer is a visualizer for data items with plain
 * text.
 *
 * Thic class is a concrete implementation for the MFsTreeItemVisualizer 
 * interface. It provides visualization for data items with plain text.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( CFsTreePlainTwoLineItemVisualizer ):
    public CFsTreePlainOneLineItemVisualizer 
    {
public:

    /**
     * Two-phased constructor
     * 
     * @param aOwnerControl Owner control.
     */
    IMPORT_C static CFsTreePlainTwoLineItemVisualizer* NewL( 
                                                 CAlfControl& aOwnerControl );
    
    /** 
    * C++ destructor
    */
    virtual ~CFsTreePlainTwoLineItemVisualizer();    
    
public:

    /**
     * Returns the type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const;

    /**
     * Sets horizontal alignment of the displayed plain text.
     * 
     * For AH layout EAlfAlignHLeft is internally converted to EAlfAlignHRight
     * and EAlfAlignHRight is converted to EAlfAlignHLeft.
     * 
     * EAlfAlignHLocale (default setting) - alignment set according to text directionality
     * 
     * @param aAlign Horizontal text alignment type.
     */
    IMPORT_C virtual void SetSecondaryTextAlign( 
            const TAlfAlignHorizontal aAlign );

    /**
     * Turn on/off displaying preview pane (3rd line).
     * 
     * @param aPreviewOn ETrue, preview pane is displayed. EFalse, preview 
     *                   pane is hidden. 
     */
    virtual void SetPreviewPaneOn( TBool aPreviewOn );
    
    /**
     * Retrurns infromation whether preview pane is turned on.
     * 
     * @return ETrue if preview pane (3rd line) is turned on.
     */
    virtual TBool IsPreviewPaneOn () const;
    
    /**
     * Sets the size of the item visualization area when it has preview pane
     * turned on.
     * 
     * @param aSize Size in pixels of the item when it has preview pane 
     *              turned on.
     */
    virtual void SetPreviewPaneEnabledSize ( const TSize aSize );
    
    /**     
     * Returns the size of an item when it has preview pane turned on.
     * 
     * @return Size in pixels of the item when has preview pane turned on.
     */
    virtual TSize PreviewPaneEnabledSize() const;

    /**
     * Function sets wether flag icon should be visible or hidden.
     * By default the icon is shown (if it is set).
     * 
     * @param aFlagVisible ETrue if flag icon should be visible.
     */
    virtual void SetFlagIconVisible( TBool aFlagVisible );

    /**
     * Function returns information if flag icon is visible or hidden.
     * 
     * @return ETrue if flag icon is visible.
     */
    virtual TBool IsFlagIconVisible( ) const;

    /**
     * The function sets whether time visual should be visible when item is 
     * not focused (in one line layout).
     * 
     * @param ETrue if time visual should be visible.
     */
    virtual void SetIsTimeVisibleInFirstLineWhenNotFocused( TBool aIsVisible );
    
    /**
     * The function returns whether time visual is visible when item is not 
     * focused (in one line layout).
     * 
     * @return ETrue if time visual is visible.
     */
    virtual TBool IsTimeVisibleInFirstLineWhenNotFocused( ) const;
    
    
// from base class MFsTreeItemVisualizer
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns the size of an item in a normal (not extended) state.
     * 
     * @return Size in pixels of an item when it is not extended.
     */
    virtual TSize Size() const;
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns the size of an item in expanded state.
     * 
     * @return Size in pixels of the item when it is in expanded state.
     */
    virtual TSize ExtendedSize() const;
    
    /**
     * From MFsTreeItemVisualizer.
     * A call to this function means that the item is requested to draw itself
     * within specified parent layout. Data to be visualized is passed as an
     * argument. 
     * 
     * Parameter descriptions will be added when the final function prototype
     * is ready.
     */
    virtual void ShowL( CAlfLayout& aParentLayout,
                        const TUint aTimeout = 0 );

    /**
    * A call to this function means that the item is requested to update its
    * visual content.
    */
    virtual void UpdateL( const MFsTreeItemData& aData,
                          TBool aFocused,
                          const TUint aLevel,
                          CAlfTexture*& aMarkOnIcon,
                          CAlfTexture*& aMarkOffIcon,                          
                          CAlfTexture*& aMenuIcon,
                          const TUint aTimeout = 0,
                          TBool aUpdateData = ETrue);
    
    /**
     * From MFsTreeItemVisualizer.
     * A call to this function means that the item goes out of the visible 
     * items scope. The visualizer should destroy all its visuals to save 
     * memory.
     */
    virtual void Hide( const TInt aTimeout = 0 );
    
    /**
     * From MFsTreeItemVisualizer.
     * This method marquees the text in tree item if it is too long.
     * 
     * @param aMarqueeType Type of marquee.
     * @param aMarqueeSpeed Speed of marquee in pixels per second.
     * @param aMarqueeStartDelay Marquee's start delay in miliseconds.
     * @param aMarqueeCycleStartDelay Marquee cycle's start delay.
     * @param aMarqueeRepetitions Number of marquee's cycles.
     */
    virtual void MarqueeL(const TFsTextMarqueeType aMarqueeType,
                          const TUint aMarqueeSpeed,
                          const TInt aMarqueeStartDelay,
                          const TInt aMarqueeCycleStartDelay,
                          const TInt aMarqueeRepetitions);   
    
    /**
     * 
     */
    virtual TBool OfferEventL(const TAlfEvent& aEvent);

    
    virtual void SetKeyWordsToHighlight(const RPointerArray<TDesC>& aKeyWords);
protected:

    /**
     * C++ constructor
     * 
     * @param aOwnerControl Owner control.
     */
    CFsTreePlainTwoLineItemVisualizer( CAlfControl& aOwnerControl );
    
    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
private:
    void HighlightKeyWords(TDesC& aText);
    void UpdateLayout(
            const CFsTreePlainTwoLineItemData* aData, 
            const TInt aIndentation );

private: // member data
    
    /**
     * Text visual.
     * Own.
     */
    CAlfTextVisual* iSecondaryTextVisual;
    
    /**
     * Secondary viewport layout. For text marquee.
     * Own.
     */
    CAlfViewportLayout* iSecViewport;    
    
    /*
     * Horizontal text alignment in visual.
     */
    TAlfAlignHorizontal iSecondaryTextAlign;
    
    /**
     * Preview pane is on/off.
     */
    TBool iPreviewPaneOn;
    
    /**
     * Flag icon shown/hidden.
     */
    TBool iShowFlagIcon;
    
    /**
     * Size of item when preview pane is on.
     */
    TSize iPreviewPaneEnabledSize;
    
    /**
     * Visual for a flag icon.
     */
    CAlfImageVisual* iIconFlagVisual;
    
    /**
     * Date and time text visual.
     * Own.
     */
    CAlfTextVisual* iDateTimeTextVisual;

    /**
     * Preview pane text visual.
     * Own.
     */
    CAlfTextVisual* iPreviewPaneTextVisual;
    
    /**
     * Clet for secondary text marquee.
     */
    TFsMarqueeClet* iSecondaryTextMarquee;
    
    /**
     * Time visual shown/hidden in not focused one line layout.
     */
    TBool iIsTimeVisibleWhenNotFocused;
    
    RPointerArray<TDesC> iKeyWords;
    };
    
#endif  // C_FSTREEPLAINTWOLINEITEMVISUALIZER_H
