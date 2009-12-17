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
* Description:  Definition of the class CPbkxRclBaseDlg.
*
*/


#ifndef CPBKXRCLBASEDLG_H
#define CPBKXRCLBASEDLG_H

#include <aknselectionlist.h>
#include <AknsItemID.h>
#include <AknsSkinInstance.h>

class CAknsListBoxBackgroundControlContext;

/**
* Base class for CAknSelectionListDialog based views.
*/
class CPbkxRclBaseDlg : public CAknSelectionListDialog
    {
public: // destructors

    virtual ~CPbkxRclBaseDlg();

protected: // methods from base classes
    
    /**
    * Handles resource change.
    *
    * @param aType Type of the change.
    */
    virtual void HandleResourceChange( TInt aType );

    /**
    * Checks if listbox type is acceptable.
    *
    * @param aControlType Type of the listbox.
    * @param aIsFormattedCellList ETrue if listbox is formatted cell listbox.
    * @return ETrue if listbox is acceptable type.
    */
    virtual TBool IsAcceptableListBoxType(
        TInt aControlType, 
        TBool &aIsFormattedCellList ) const;

    /**
    * Creates custom control of given type.
    *
    * @param aControlType Type of the control.
    * @return Information about created control.
    */
    virtual SEikControlInfo CreateCustomControlL( TInt aControlType );

protected: // constructors

    /**
    * Constructor.
    *
    * @param aIndex Index which user selected from the list (probably not used?)
    * @param aArray Array of items.
    * @param aCommand Command observer for observing menu commands.
    * @param aColumnListBox ETrue if listbox type is column.
    */
    CPbkxRclBaseDlg(
        TInt& aIndex,
        CDesCArray* aArray,
        MEikCommandObserver* aCommand,
        TBool aColumnListBox );

    /**
    * Second-phase constructor.
    *
    * @param aResourceId Resource identifier.
    */
    void ConstructL( TInt aResourceId );

protected: // new methods
    
    /**
    * Updates graphics used in the component.
    */
    void UpdateGraphics();
    
    /**
    * Updates colors used in the component.
    */
    void UpdateColors();

private: // data
    
    // Skin ids.
    TAknsItemID iListItemId;
    TAknsItemID iListBottomId;
    TAknsItemID iHighlightId;
    TAknsItemID iHighlightCenterId;
    TAknsItemID iHighlightTextColorId;
    TAknsItemID iTextColorId;
    
    TBool iColumnListBox;
    };

#endif

