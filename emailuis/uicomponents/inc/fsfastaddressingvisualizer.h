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
* Description:  Declaration of CFsFastAddressingVisualizer class
*
*/


#ifndef C_FSFASTADDRESSINGVISUALIZER_H
#define C_FSFASTADDRESSINGVISUALIZER_H


#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag (other places in this file not tagged!)
//#include <fsconfig.h>
/*#ifdef __FS_ALFRED_SUPPORT
#else // !__FS_ALFRED_SUPPORT
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include "fsfastaddressinglist.h"

class CAlfAnchorLayout;
class CAlfFlowLayout;
class CFsTextInputVisual;

/**
 * CFsFastAddressingVisualizer class is responsible for visualisation
 * of the fast addressing list component. The class constructs the
 * visualization tree and manages the visualization of the text input field.
 *
 */

NONSHARABLE_CLASS( CFsFastAddressingVisualizer ) : public CBase
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aOwner Owner of visualizer
     * @param aTextControl CoeControl of which the Rect will be handled
     * @return Created instance of CFsFastAddressingVisualizer class
     */
    static CFsFastAddressingVisualizer* NewL( CAlfControl& aOwner,
        CCoeControl& aTextControl );

    /**
     * Two-phased constructor
     *
     * @param aOwner Owner of visualizer
     * @param aTextControl CoeControl of which the Rect will be handled
     * @return Created instance of CFsFastAddressingVisualizer class
     */
    static CFsFastAddressingVisualizer* NewLC( CAlfControl& aOwner,
        CCoeControl& aTextControl );

    /**
     * Destructor
     */
    virtual ~CFsFastAddressingVisualizer();

    /**
     * Set the icon image.
     *
     * @param aIconID Sets icon image
     * @param aImage Image to be set
     */
    void SetIcon(
        const CFsFastAddressingList::TIconID aIconID,
        const TAlfImage& aImage );

    /**
     * Sets icon visibility
     *
     * @param aIconID Sets icon image
     * @param aVisible State of icon visibility.
     */
    void SetIconVisible( const CFsFastAddressingList::TIconID aIconID,
                         const TBool aVisible );

    /**
     * Returns the icon visibility.
     * @param aIconID Id of icon which visiblity will be set
     * @return TBool icon visibility status.
     */
    TBool IconVisible( const CFsFastAddressingList::TIconID aIconID ) const;

    /**
     * Redraw the FAL.
     */
    void Refresh();


    /**
     * Sets list visiblity
     *
     * @param aVisible If ETrue list is set to be visible.
     *                 If EFalse list is set to invisible.
     */
    void SetVisible( const TBool aVisible );

    /**
     * Move selection up.
     * Selection implemented in TreeList.
     */

    void MoveSelectionUp();
    /**
     * Move selection down.
     * Selection implemented in TreeList.
     */

    void MoveSelectionDown();

    /**
     * Returns pointer to the layout for the text input field.
     * @return Layout for the text input field.
     */
    CAlfLayout* GetTextLayout() const;

    /**
     * Sets visibility of the text field layout.
     * @param iVisible visibility of the text input field.
     */
    void SetTextFieldVisible( const TBool iVisible );

    /**
     * Returns status of visibility of the text field layout.
     * @return TBool visibility status of the text input field.
     */
    TBool IsTextFieldVisible() const;

    /**
     * Sets position of the list (bottom of text field or on top of text field)
     * @param aPos position of the list
     *             {EListOnTopOfTextField,EListBelowTextField}
     */
    void SetListPosition( CFsFastAddressingList::TFsListPosition aPos );

    /**
     * Returns position of the list
     * @return TFsListPosition position of the list
     *                         {EListOnTopOfTextField,EListBelowTextField}
     */
    CFsFastAddressingList::TFsListPosition ListPosition() const;

    /**
     * Returns main layout of FAL
     * @return CHuiLayout* pointer to the main layout.
     */
    CAlfLayout* GetMainLayout() const;

    /**
     * Returns layout for list component
     * @return CHuiLayout* pointer to the main layout.
     */
    CAlfLayout* GetListLayout() const;

private:
    /**
     * Default constructor
     *
     * @param aOwner Owner of the visualizer.
     */
    CFsFastAddressingVisualizer( CAlfControl& aOwner );

    /**
     * Second phase constructor
     * @param aTextControl CCoeControl of which the TRect will be managed
     */
    void ConstructL( CCoeControl& aTextControl );

    /**
     * Draw underline text
     */
    void DrawUnderline();

    /**
     * Sets anchors for the textinput layout and for the list layout
     */
    void SetAnchors();


private: //Data

    /**
     * Root laout of the fast addresing list component.
     * Not owned.
     */
    CAlfAnchorLayout* iRootLayout;

    /**
     * Layout passed to the tree list component.
     * Not owned.
     */
    CAlfLayout* iItemsList;

    /**
     * Layout for text input field.
     * Not owned.
     */
    CAlfFlowLayout* iTextField;

    /**
     * Image visual for icon A
     * Not owned.
     */
    CAlfImageVisual* iIconA;

    /**
     * Image visual for icon B
     * Not owned.
     */
    CAlfImageVisual* iIconB;

    /**
     * Controls which owns the visualizer.
     * Not owned.
     */
    CAlfControl& iControlOwner;

    /**
     * Visualizer for the text input field
     * Not owned.
     */
    CFsTextInputVisual* iTextInputVisual;

    /**
     * Visibility of text field.
     */
    TBool iTextFieldVisible;

    /**
     * List position.
     * {EListOnTopOfTextField,EListBelowTextField}
     */
    CFsFastAddressingList::TFsListPosition iListPosition;
    };


#endif  // C_FSFASTADDRESSINGVISUALIZER_H
