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
* Description:  A visualizer for data items with separator.
*
*/


#ifndef C_FSSEPARATORVISUALIZER_H
#define C_FSSEPARATORVISUALIZER_H

#include <e32base.h>
//#include <fsconfig.h> <cmail>
// <cmail> SF
#include <alf/alfeventhandler.h>
#include <alf/alflinevisual.h>
// </cmail>
#include "fstreeitemvisualizerbase.h"

class CAlfViewportLayout;

class CFsSeparatorData;

const TInt KDefaultSeparatorThickness = 1;
const TInt KDefaultWidth = 200;
const TInt KDefaultHeight = 5;
/**
 * CFsSeparatorVisualizer is a visualizer for data items separator.
 *
 * Thic class is a concrete implementation for the MFsTreeItemVisualizer 
 * interface. It provides visualization for data items separator.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( CFsSeparatorVisualizer ): 
                                  public CFsTreeItemVisualizerBase
    {
public:

    /**
     * Two-phased constructor
     * 
     * @param aOwnerControl Owner control.
     */
    IMPORT_C static CFsSeparatorVisualizer* NewL( 
                                                 CAlfControl& aOwnerControl );
                                              
    /** 
    * C++ destructor
    */
    virtual ~CFsSeparatorVisualizer();    
    
public: 

    /**
     * The function sets separator's thickness in pixels.
     * 
     * @param aPixelThickness Thickness of the separator line (in pixels)
     */
    virtual void SetSeparatorThickness( const TInt aThickness );
    
    /**
     * The function returns thikness of the separator line.
     * 
     * @return Thickness of the separator line (in pixels)
     */
    virtual TInt SeparatorThickness( ) const;
    
    /**
     * Sets the width of separator line. The separator line is always centered 
     * horizontaly. The line width is given as the percentage of the whole item's width.
     * 
     * @param aWidth Percent of item's width covered by separator line. 
     *               Valid values are in range <1.0 , 0.1>    
     */
    virtual void SetSeparatorWidth ( TReal32 aWidth );
    
    /**
     * Returns the width of the separator line.
     * 
     * @return Percent of item's width covered by separator line.
     */
    virtual TReal32 SetSeparatorWidth ( ) const;
    
// from base class MFsTreeItemVisualizer
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns the type of the separator visualizer class.
     */
    virtual TFsTreeItemVisualizerType Type() const;

    /**
     * Sets multiple flags for an item at one time.
     * Setting KFsTreeListItemFocusable flag has o effect - item is always non-focusable.
     * KFsTreeListItemManagedLayout flag has no effect.
     * KFsTreeListItemExtendable flag has no effect.
     * KFsTreeListItemExtended flag has no effect.
     * KFsTreeListItemAlwaysExtended  flag has no effect.
     * 
     * @param aFlags Flags for separator visualizer.
     */
    virtual void SetFlags( TUint32 aFlags );
    
    /**
     * Returns the size of an item in a normal (not expanded) state.
     * 
     * @return Size in pixels of an item when it is not expanded.
     */
    virtual TSize Size() const;   
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets the item's visualization state to extended. 
     * Separator cannot be extended - the function has no effect. 
     * 
     * @param aFlag ETrue to visualize item as extended.
     */
    virtual void SetExtended( TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns extended/not extended state of the item visualization.
     * Separator cannot be extended - the function always returns EFalse.
     * 
     * @return EFalse
     */
    virtual TBool IsExtended() const;
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets if an item can be in expanded state or not.
     * Separator is not expandable - the function has no effect.
     * 
     * @param aFlag ETrue if item can be in expanded state.
     */
    virtual void SetExtendable( TBool aFlag );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns an information if item can be in expanded state.
     * Separator is not expandable - the function always returns EFalse.
     * 
     * @return EFalse
     */
    virtual TBool IsExtendable( );
    
    /**
     * From MFsTreeItemVisualizer.
     * This functions sets wether an item should always be in extended state.
     * 
     * @param ETrue if item should always be in extended state.
     */
    virtual void SetAlwaysExtended( TBool aAlwaysExtended );
    
    /**
     * From MFsTreeItemVisualizer.
     * This function returns wether an item is always in extended state.
     * 
     * @return ETrue if item is always in extended state.
     */
    virtual TBool IsAlwaysExtended( ) const;
    
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
     */
    CFsSeparatorVisualizer( CAlfControl& aOwnerControl );
    
    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
protected:

    /**
     * Visual for drawing a separator line.
     */
    CAlfLineVisual* iLineVisual;
    
    /**
     * Thickness of the separator line (in pixels).
     */
    TInt iThickness;
    
    /**
     * Width of a separator line - percentage of item's width covered by a separator line.  
     */
    TReal32 iWidth;
    
    };
    
#endif  // C_FSSEPARATORVISUALIZER_H
