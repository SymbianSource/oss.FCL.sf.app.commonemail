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
* Description:  A visualizer for nodes with plain text.
*
*/


#ifndef C_FSTREEPLAINONELINENODEVISUALIZER_H
#define C_FSTREEPLAINONELINENODEVISUALIZER_H

#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstreenodevisualizerbase.h"

class CAlfViewportLayout;
class CAlfTextVisual;
class CAlfImageVisual;

class CFsTreeList;
class CFsLayoutManager;
class CFsTreePlainOneLineNodeData;
class TFsMarqueeClet;


/**
 * CFsTreePlainNodeVisualizer is a visualizer for nodes with plain text.
 *
 * Thic class is a concrete implementation for the MFsTreeNodeVisualizer 
 * interface. It provides visualization for nodes with plain text.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( CFsTreePlainOneLineNodeVisualizer ): 
                                            public CFsTreeNodeVisualizerBase
    {
public:

    /**
     * Two-phased constructor
     * 
     * @param aOwnerControl Owner control.
     */
    IMPORT_C static CFsTreePlainOneLineNodeVisualizer* NewL( 
                                                 CAlfControl& aOwnerControl );
    
    /**
    * C++ destructor
    */
    virtual ~CFsTreePlainOneLineNodeVisualizer(); 

public: // from base class MFsTreeItemVisualizer

    /**
     * From MFsTreeItemVisualizer.
     * Returns the type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const;
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets multiple flags for a node at one time.
     * 
     * KFsTreeListItemExtendable flag has no effect.
     * KFsTreeListItemExtended flag has no effect.
     * KFsTreeListItemAlwaysExtended  flag has no effect.
     * 
     * @param aFlags Flags for node visualizer.
     */
    virtual void SetFlags( TUint32 aFlags );
    
    void SetExpanded( TBool aFlag, const MFsTreeItemData* aData = 0 );
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets item's visualization state to extended.
     *  
     * Node cannot be extended - the function has no effect. 
     * 
     * @param aIsExtended ETrue to visualize item as extended.
     */
    virtual void SetExtended( TBool aIsExtended );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns extended/not extended state of the item visualization.
     * 
     * Node cannot be extended - the function always returns EFalse.
     * 
     * @return EFalse
     */
    virtual TBool IsExtended() const;
    
    /**
     * From MFsTreeItemVisualizer.
     * Sets if an item can be in expanded state or not.
     * 
     * Node is not extendable - the function has no effect.
     * 
     * @param aIsExtendable ETrue if item can be in expanded state.
     */
    virtual void SetExtendable( TBool aIsExtendable );
    
    /**
     * From MFsTreeItemVisualizer.
     * Returns an information if item can be in expanded state.
     * 
     * Node is not extendable - the function always returns EFalse.
     * 
     * @return EFalse
     */
    virtual TBool IsExtendable( );
    
    /**
     * From MFsTreeItemVisualizer.
     * This functions sets wether an item should always be in extended state.
     * 
     * Node cannot be made always extended. This function has no effect.
     * 
     * @param aAlwaysExtended ETrue if item should always be in extended state.
     */
    virtual void SetAlwaysExtended( TBool aAlwaysExtended );
    
    /**
     * From MFsTreeItemVisualizer.
     * This function returns wether an item is always in extended state.
     * 
     * Node cannot be made always extended. The function always returns EFalse.
     * 
     * @return EFalse.
     */
    virtual TBool IsAlwaysExtended( ) const;
    
    /**
     * From MFsTreeItemVisualizer.
     * A call to this function means that the node is requested to draw itself
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
     * A call to this function means that the node goes out of the visible 
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
        
protected:

    /**
     * C++ constructor
     * 
     * @param aOwnerControl Owner control.
     */
    CFsTreePlainOneLineNodeVisualizer( CAlfControl& aOwnerControl );
    
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
    void UpdateLayout( const CFsTreePlainOneLineNodeData* aData,
            const TInt aIndentation,
            CAlfTexture*& aMarkIcon,
            CAlfTexture*& aMenuIcon );
    
private: // member data
    
    
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
    
    /**
     * Clet for text marquee.
     * Own.
     */
    TFsMarqueeClet* iTextMarquee;
    };
    

#endif  // C_FSTREEPLAINONELINENODEVISUALIZER_H
