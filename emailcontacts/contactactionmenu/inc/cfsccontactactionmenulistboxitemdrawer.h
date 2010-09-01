/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class CFscContactActionMenuListBoxItemDrawer.
*
*/


#ifndef C_FSCCONTACTACTIONMENULISTBOXITEMDRAWER_H
#define C_FSCCONTACTACTIONMENULISTBOXITEMDRAWER_H

#include <e32base.h>
#include <eikclb.h>

// DEFINES

// FORWARD DECLARATIONS

/**
 *  Contact Action Menu list box item drawer declaration.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuListBoxItemDrawer 
    : public CColumnListBoxItemDrawer
    {

public: // Public constructor and destructor
    
    /**
     * C++ default constructor.
     *
     * Initialises the list box item drawer with the specified model, 
     * font and data.
     *
     * @param aTextListBoxModel The model whose items will be drawn. 
     *        This provides the item text that is drawn using 
     *        @c DrawItemText().
     * @param aFont The font in which the items will be drawn. 
     * @param aColumnData The column list box data. This comprises: 
     *        the column widths, gaps, fonts, aligments and bitmaps.
     */
    CFscContactActionMenuListBoxItemDrawer(
        MTextListBoxModel* aTextListBoxModel, 
        const CFont* aFont, 
        CColumnListBoxData* aColumnData );

    /**
     * Destructor.
     */
    virtual ~CFscContactActionMenuListBoxItemDrawer();
    
public: // From Base class

    /**
     * Draw item text.
     * 
     * This function is invoked by @c DrawActualItem() to draw the item text.
     *
     * @param aItemIndex Index of the item to draw. 
     * @param aItemTextRect Area to draw into. 
     * @param aItemIsCurrent @c ETrue if the item is current. 
     * @param aViewIsEmphasized @c ETrue if the view is emphasised.
     * @param aItemIsSelected @c ETrue if the item is selected. 
     */
    virtual void DrawItemText(
        TInt aItemIndex, 
        const TRect& aItemTextRect,
        TBool aItemIsCurrent, 
        TBool aViewIsEmphasized, 
        TBool aItemIsSelected ) const;
    
    };

#endif // C_FSCCONTACTACTIONMENULISTBOXITEMDRAWER_H
