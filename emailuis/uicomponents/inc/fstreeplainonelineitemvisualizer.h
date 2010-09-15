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


#ifndef C_FSTREEPLAINONELINEITEMVISUALIZER_H
#define C_FSTREEPLAINONELINEITEMVISUALIZER_H

#include <e32base.h>
#include <alf/alfeventhandler.h>
#include "fstreeitemvisualizerbase.h"

class CAlfViewportLayout;
class CAlfTextVisual;
class CAlfImageVisual;

class CFsTreePlainOneLineItemData;
class TFsMarqueeClet;

/**
 * CFsTreePlainItemVisualizer is a visualizer for data items with plain text.
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
NONSHARABLE_CLASS( CFsTreePlainOneLineItemVisualizer ): 
                                  public CFsTreeItemVisualizerBase
    {
public:

    /**
     * Two-phased constructor
     * 
     * @param aOwnerControl Owner control.
     */
    IMPORT_C static CFsTreePlainOneLineItemVisualizer* NewL( 
                                                 CAlfControl& aOwnerControl );
                                              
    /** 
    * C++ destructor
    */
    virtual ~CFsTreePlainOneLineItemVisualizer();    
    
public: 
    
// from base class MFsTreeItemVisualizer
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns the type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const;

    
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
     * This method sets flag disable wrapping the text item.
     */    
    virtual void OffWrapping();
    
    /**
     * 
     */
    virtual TBool OfferEventL(const TAlfEvent& aEvent);

protected:

    /**
     * C++ constructor
     * 
     * @param aOwnerControl Owner control.
     */
    CFsTreePlainOneLineItemVisualizer( CAlfControl& aOwnerControl );
    
    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
private:

    void UpdateLayout(const CFsTreePlainOneLineItemData* aData, 
            const TInt aIndentation );
    
protected:

    /**
     * Viewport layouut containing text visual. Used for text marquee.
     * Own.
     */
    CAlfViewportLayout* iViewportLayout;
    
    /**
     * Text visual.
     * Own.
     */
    CAlfTextVisual* iTextVisual;
    
    /**
     * Visual for an icon.
     */
    CAlfImageVisual* iIconVisual;
    
    /**
     * Visual for a mark icon.
     */
    CAlfImageVisual* iIconMark;
    
    /**
     * Visual for a menu icon.
     */
    CAlfImageVisual* iIconMenu;
    
    /**
     * Clet for text marquee.
     */
    TFsMarqueeClet* iTextMarquee;
    
    TBool iManualWrapper;

    };
    
#endif  // C_FSTREEPLAINONELINEITEMVISUALIZER_H
