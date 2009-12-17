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
* Description:  Definition of the class CPbkxRclSearchResultListBox.
*
*/


#ifndef CPBKXRCLSEARCHRESULTLISTBOX_H
#define CPBKXRCLSEARCHRESULTLISTBOX_H

#include <aknlists.h>
#include <eikclb.h>

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultListBox
////////////////////////////////////////////////////////////////////////////

/**
* Listbox for search result dialog.
*/
class CPbkxRclSearchResultListBox : public CAknSingleStyleListBox
    {
public: // constructor and destructor

    /**
    * Constructor.
    */
    CPbkxRclSearchResultListBox();

    /**
    * Destructor.
    */
    virtual ~CPbkxRclSearchResultListBox();

protected: // methods from base classes

    /**
    * Creates item drawer for the listbox.
    */
    virtual void CreateItemDrawerL();
    
    /**
    * Creates view for the listbox.
    */
    virtual CListBoxView* MakeViewClassInstanceL();
    };

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultItemDrawer
////////////////////////////////////////////////////////////////////////////

/**
* Item drawer for search result listbox.
*/
class CPbkxRclSearchResultItemDrawer : public CColumnListBoxItemDrawer
    {
public: // constructor and destructor

    /**
    * Constructor.
    *
    * @param aTextListBoxModel Listbox model.
    * @param aFont Used font.
    * @param aColumnData Data class.
    */
    CPbkxRclSearchResultItemDrawer(
        MTextListBoxModel* aTextListBoxModel, 
        const CFont* aFont, 
        CColumnListBoxData* aColumnData );

    /**
    * Destructor.
    */
    virtual ~CPbkxRclSearchResultItemDrawer();

public: // new methods

    /**
    * Sets highlighted text color.
    *
    * @param aColor Color.
    */
    void SetHighlightColor( TRgb aColor );

    /**
    * Sets normal text color.
    *
    * @param aColor Color.
    */
    void SetColor( TRgb aColor );

    /**
    * Reset color settings.
    *
    * @param aHighlightColor ETrue if highlight is reset, EFalse for normal.
    */
    void ResetColors( TBool aHighlightColor = EFalse );

public: // from base classes

    /**
    * Draws item text.
    *
    * @param aItemIndex Item index.
    * @param aItemTextRect Item text rect.
    * @param aItemIsCurrent
    * @param aViewIsEmphasized
    * @param aItemIsSelected
    */
    virtual void DrawItemText(
        TInt aItemIndex,
        const TRect& aItemTextRect,
        TBool aItemIsCurrent,
        TBool aViewIsEmphasized, 
        TBool aItemIsSelected ) const;

private: // data

    enum TColors
        {
        EColorSet = 0x1,
        EHighlighColorSet = 0x2
        };

    // Normal text color.
    TRgb iColor;
    
    // Highlighted text color.
    TRgb iHighlightColor;
    
    // Flag indicating that colors are set.
    TInt iColorsSet;
    
    };

////////////////////////////////////////////////////////////////////////////
// CPbkxRclSearchResultListView
////////////////////////////////////////////////////////////////////////////

/**
* View for search result listbox.
*/
class CPbkxRclSearchResultListView: public CAknColumnListBoxView 
    {
protected:
    // From CListBoxView
    
    /**
     * Called when drawing empty list
     * 
     * @param aClientRect Not used.
     */
    void DrawEmptyList(const TRect &aClientRect) const;  
    };
#endif


