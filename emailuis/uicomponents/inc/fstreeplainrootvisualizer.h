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
* Description:  A visualizer for empty tree root's data.
*
*/


#ifndef FSTREEPLAINROOTVISUALIZER_H_
#define FSTREEPLAINROOTVISUALIZER_H_


#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstreenodevisualizerbase.h"

class CAlfTextVisual;

class MFsTreeItemData;
class CFsTreeList;
class CFsLayoutManager;

/**
 * CFsTreePlainRootVisualizer is a visualizer for the empty tree root's data.
 *
 * Thic class is a concrete implementation for the MFsTreeNodeVisualizer 
 * interface. It provides visualization for the empty tree root's data. This 
 * visualizer is used when the list is empty. The list may for example display
 * "empty" text then.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( CFsTreePlainRootVisualizer ): 
                                            public CFsTreeNodeVisualizerBase
    {
public:

    /**
     * Two-phased constructor
     * 
     * @param aOwnerControl Owner control.
     */
    IMPORT_C static CFsTreePlainRootVisualizer* NewL( 
                                                  CAlfControl& aOwnerControl);
    
    /**
    * C++ destructor
    */
    virtual ~CFsTreePlainRootVisualizer();
    
    
    /**
     * Returns the type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const;


public: // from base class MFsTreeItemVisualizer

    
    /**
     * From MFsTreeItemVisualizer.
     * A call to this function means that the visualization of empty root 
     * becomes visible and should draw itself within specified parent layout. 
     * Data to be visualized is passed as an argument. 
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
     * A call to this function means that the the empty root visualization 
     * should become hidden.The visualizer should destroy all its visuals to 
     * save memory.
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
    CFsTreePlainRootVisualizer( CAlfControl& aOwnerControl );
    
    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
private:
    
    /**
     * Updates the layout metrics using layout manager that was set by 
     * tree visualizer. 
     * 
     * @param aLayoutManager Reference to the layout manager.
     */
    virtual void UpdateLayout( );

private: // member data
    
    
    /**
     * Text visual.
     * Own.
     */
    CAlfTextVisual* iTextVisual;  

    };

#endif /*FSTREEPLAINROOTVISUALIZER_H_*/
