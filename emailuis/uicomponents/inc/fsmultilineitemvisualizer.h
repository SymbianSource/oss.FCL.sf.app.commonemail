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
* Description:  A visualizer class for the MultiLine item.
*
*/


#ifndef C_FSMULTILINEITEMVISUALIZER_H
#define C_FSMULTILINEITEMVISUALIZER_H

class CAlfControl;
class CAlfLayout;
class CAlfTexture;

class CFsMultiLineItemData;

#include "fstreeitemvisualizerbase.h"

/**
 * CFsMultiLineItemVisualizer is a visualizer for an item with 1-4 lines.
 *
 * This class is a concrete implementation for the MFsTreeItemVisualizer 
 * interface. It provides visualization for a Multiline item's data.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( CFsMultiLineItemVisualizer ): 
                                  public CFsTreeItemVisualizerBase
    {
public:

    /**
     * Two-phased constructor
     * 
     * @param aOwnerControl Owner control.
     */
    IMPORT_C static CFsMultiLineItemVisualizer* NewL( 
                                                 CAlfControl& aOwnerControl );
                                              
    /** 
    * C++ destructor
    */
    virtual ~CFsMultiLineItemVisualizer();    

public:    
    
    //visualizer's interface
    
    /**
     * The function sets how many lines the multiline item has when it's not extended.
     * 
     * @param aNumberOfLines Number of lines in not extended state (1-4).Note that
     *        the value should be <= than the number of lines in extended state.
     */
    virtual void SetNumberOfLinesWhenNotExtended( TInt aNumberOfLines );
    
    /**
     * The function returns how many lines the item has when it's not extended.
     * 
     * @return Number of lines in not extended state (1-4).
     */
    virtual TInt NumberOfLinesWhenNotExtended( ) const;
    
    /**
     * The function sets how many lines the multiline item has when it's extended.
     * 
     * @param aNumberOfLines Number of lines in extended state (1-4).Note that
     *        the value should be >= than the number of lines in not extended state.
     */
    virtual void SetNumberOfLinesWhenExtended( TInt aNumberOfLines );
    
    /**
     * The function returns how many lines the item has when it's extended.
     * 
     * @return Number of lines in extended state (1-4).
     */
    virtual TInt NumberOfLinesWhenExtended( ) const;

    /**
     * The function sets state of the item's checkbox.
     * A call of this function should be followed by a call of list's 
     * visualizer's function UpdateItemL() or RefreshListViewL().
     * 
     * @param ETrue Sets checkbox to "on" state.
     *        EFalse Sets checkbox to "off" state.
     */
    virtual void SetCheckboxState( TBool aChecked );
    
    /**
     * The function returns state of the item's checkbox.
     * 
     * @return ETrue Checkbox is in "on" state.
     *         EFalse Checkbox is in "off" state.
     */
    virtual TBool CheckboxState () const;
    
    /**
     * The function sets whether place holders should be kept when icons in the 
     * first column are not present.
     * 
     * @param ETrue if place holders should be present. EFalse otherwise.
     */
    virtual void SetKeepEmptyIconsPlaceHolders( TBool aKeep );
    
    /**
     * The function returns whether place holders are kept in case icons are
     * not set.
     * 
     * @returm ETrue if place holders are present. EFasle otherwise.
     */
    virtual TBool IsKeepEmptyIconsPlaceHolders( ) const;

public: 
    
// from base class MFsTreeItemVisualizer
    
    /**
     * Returns the type of the item's visualizer.
     */
    virtual TFsTreeItemVisualizerType Type() const;
    
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
                          CAlfTexture*& aMarkIcon,
                          CAlfTexture*& aMenuIcon,
                          const TUint aTimeout = 0,
                          TBool aUpdateData = ETrue);

    
    /**
     * This method sets flag disable wrapping the text item.
     */
    virtual void OffWrapping();
    
    /**
     * From MFsTreeItemVisualizer.
     * A call to this function means that the item goes out of the visible 
     * items scope. The visualizer should destroy all its visuals to save 
     * memory.
     */
    virtual void Hide( const TInt aTimeout = 0 );
      

protected:

    /**
     * C++ constructor
     * 
     * @param aOwnerControl Owner control.
     */
    CFsMultiLineItemVisualizer( CAlfControl& aOwnerControl );
    
    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
private:

    /**
     */
    void UpdateLayout(const CFsMultiLineItemData* aData, const TInt aIndentation );
    
    /**
     * Returns item's size in state with given number of lines.
     * 
     * @param  aNumberLines Number of item's lines (1-4)
     * @return Size of item. TSize(0,0) in case invalid number of lines was passed to the func.
     */
    TSize SizeWithNumberOfLines( const TInt aNumberLines ) const;
    
protected: //data

    /**
     * State of item's checkbox.
     * 
     * ETrue - checkbox is "on"
     * EFalse - checkbox is "off"
     */
    TBool iCheckState;

    /**
     * Place holders for empty icons should be kept or not.
     */
    TBool iKeepPlaceHolders;
    
    /**
     * Number of lines (1-4) when item is not extended.
     */
    TInt iNumLinesNotExtended;
    
    /**
     * Number of lines (1-4) when item is extended.
     */
    TInt iNumLinesExtended;

///////////////////////////first line///////////////////////////    
    /**
     * Text visual.
     * Own.
     */
    CAlfTextVisual* iFirstLineTextVis;
    
    /**
     * Visual for an icon.
     */
    CAlfImageVisual* iFirstLineIconVis;
    
    /**
     * Visual for a checkbox icon.
     */
    CAlfImageVisual* iCheckBoxIconVis;

    /**
     * Visual for a mark icon.
     */
    CAlfImageVisual* iMarkIconVis;
    
    /**
     * Visual for a menu icon.
     */
    CAlfImageVisual* iMenuIconVis;

///////////////////////////second line///////////////////////////
    /**
     * 
     */
    CAlfTextVisual* iSecondLineTextVis;
        
    /**
     * Visual for an icon.
     */
    CAlfImageVisual* iSecondLineIconVis;

///////////////////////////third line///////////////////////////
    /**
     */
    CAlfTextVisual* iThirdLineTextVis;
        
    /**
     * Visual for an icon.
     */
    CAlfImageVisual* iThirdLineIconVis;
    
///////////////////////////fourth line///////////////////////////
    /**
     */
    CAlfTextVisual* iFourthLineTextVis;
        
    /**
     * Visual for an icon.
     */
    CAlfImageVisual* iFourthLineIconVis;
    
    };

#endif //C_FSMULTILINEITEMVISUALIZER_H
