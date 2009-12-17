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
* Description:  Definition of the class CPbkxRclResultInfoBox.
*
*/


#ifndef CPBKXRCLRESULTINFOLISTBOX_H
#define CPBKXRCLRESULTINFOLISTBOX_H

#include <aknlists.h>
#include <eikfrlb.h>

////////////////////////////////////////////////////////////////////////////
// CPbkxRclResultInfoListBox
////////////////////////////////////////////////////////////////////////////

/**
* Listbox for result info dialog.
*/
class CPbkxRclResultInfoListBox : public CAknDoubleStyleListBox
    {
public: // constructor and destructor

    /**
    * Constructor.
    */
    CPbkxRclResultInfoListBox();

    /**
    * Destructor.
    */
    virtual ~CPbkxRclResultInfoListBox();

protected: // methods from base classes

    /**
    * Creates item drawer for the listbox.
    */
    virtual void CreateItemDrawerL();

    };

////////////////////////////////////////////////////////////////////////////
// CPbkxRclResultInfoItemDrawer
////////////////////////////////////////////////////////////////////////////

/**
* Item drawer for result info listbox.
*/
class CPbkxRclResultInfoItemDrawer : public CFormattedCellListBoxItemDrawer
    {
public: // constructor and destructor

    /**
    * Constructor.
    *
    * @param aTextListBoxModel Listbox model.
    * @param aFont Used font.
    * @param aFormattedCellData Data class.
    */
    CPbkxRclResultInfoItemDrawer(
        MTextListBoxModel* aTextListBoxModel, 
        const CFont* aFont, 
        CFormattedCellListBoxData* aFormattedCellData );

    /**
    * Destructor.
    */
    virtual ~CPbkxRclResultInfoItemDrawer();
    
public: // new methods

    /**
    * Sets color used in highlighted text.
    *
    * @param aColor Color.
    */
    void SetHighlightColor( TRgb aColor );

    /**
    * Sets color used in normal text.
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
    * @param aItemTextRect Rect for text.
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

    // Color used in normal text.
    TRgb iColor;
    
    // Color used in highlighted text.
    TRgb iHighlightColor;

    // Flag indicating that colors are set.
    TInt iColorsSet;

    };

#endif

