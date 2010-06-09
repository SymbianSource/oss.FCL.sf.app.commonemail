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
* Description:  A visualizer for data items with rich text.
*
*/



#ifndef C_FSTREERICHITEMVISUALIZER_H
#define C_FSTREERICHITEMVISUALIZER_H


#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag


#include "fstreeitemvisualizerbase.h"

class CAlfControl;
class CAlfLayout;
class CAlfTexture;
class CAlfFlowLayout;
class CAlfViewportLayout;
class CAlfTextVisual;
class CAlfImageVisual;


class CFsTreeRichItemData;
class CFsTreeList;
class CFsTextStyleManager;
class CFsLayoutManager;

/**
 * CFsTreeRichItemVisualizer is a visualizer for data items with rich text.
 *
 * Thic class is a concrete implementation for the MFsTreeItemVisualizer
 * interface. It provides visualization for data items with rich text.
 *
 * @code
 *
 * @endcode
 *
 * @lib
 */
NONSHARABLE_CLASS( CFsTreeRichItemVisualizer )
    : public CFsTreeItemVisualizerBase
    {

public:

    /**
     * Two-phased constructor
     *
     * @param aOwnerControl Owner control.
     * @param aTextStyleManager Text style manager.
     */
    IMPORT_C static CFsTreeRichItemVisualizer* NewL(
        CAlfControl& aOwnerControl, CFsAlfTextStyleManager& aTextStyleManager );

    /**
    * Destructor.
    */
    virtual ~CFsTreeRichItemVisualizer();
    
    /**
     * Returns the type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const;

    /**
     * Create a visual based on the text object.
     *
     * @param aOwnerControl Owner control.
     * @param aParentLayout Layout for the text.
     * @param aText Contains the text.
     * @param aSize Size where to fit the text.
     */
    CAlfFlowLayout* CreateTextLineL( CAlfControl& aOwnerControl,
        CAlfLayout* aParentLayout, const CFsTreeRichItemData* aText,
        TSize aSize );

    /**
     * Create a font style.
     *
     * @param charFormat Contains character format attributes.
     */
    TInt CreateStyleL( TCharFormat& charFormat );

// from base class MFsTreeItemVisualizer

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
     * From MFsTreeItemVisualizer.
     * A call to this function means that the item is requested to update its
     * visual content.
     */
    virtual void UpdateL( const MFsTreeItemData& aData,
                        TBool aFocused,
                        const TUint aLevel,
                        CAlfTexture*& aMarkIcon,
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
protected:

    /**
     * C++ constructor
     *
     * @param aOwnerControl Owner control.
     * @param aTextStyleManager Text style manager.
     */
    CFsTreeRichItemVisualizer(
        CAlfControl& aOwnerControl, CFsAlfTextStyleManager& aTextStyleManager );

    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
private:

    /**
     * Updates the layout metrics using layout manager that was set by 
     * tree visualizer. 
     */
    virtual void UpdateLayout(  
            const CFsTreeRichItemData* aData,
            const TInt aIndentation );

private:
    
    /**
     * Layout for the richtext.
     * Own.
     */
    CAlfFlowLayout* iTextLayout;
    
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
    CAlfImageVisual* iIconMarked;
    
    /**
     * Visual for a menu icon.
     */
    CAlfImageVisual* iIconMenu;

    };


#endif  // C_FSTREERICHITEMVISUALIZER_H
