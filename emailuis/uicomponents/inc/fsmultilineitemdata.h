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
* Description:  A data class for the MultiLine item.
*
*/


#ifndef C_FSMULTILINEITEMDATA_H
#define C_FSMULTILINEITEMDATA_H

#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag


#include "fstreeitemdata.h"

class CAlfTexture;

const TFsTreeItemDataType KFsMultiLineItemDataType = 5;

/**
 * CFsMultiLineItemData is a data class for a Multiline(1-4) item.
 *
 * This class is a concrete implementation for the MFsTreeItemData data 
 * interface.
 * 
 * @code
 *
 * @endcode
 *
 * @lib 
 */
NONSHARABLE_CLASS( CFsMultiLineItemData ): public CBase, 
                                         public MFsTreeItemData 
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    IMPORT_C static CFsMultiLineItemData* NewL( );
    
    /**
    * C++ destructor
    */
    virtual ~CFsMultiLineItemData();

public:
    
    /**
     * The function sets text to the specified line.
     * 
     * @param aText Text to be set to a line.
     * @param aLineNumber Line number (0..3).
     */
    virtual void SetLineTextL( const TDesC& aText, TInt aLineNumber );

    /**
     * The function return a text associated with a specified line.
     *      
     * @param aLineNumber Line number (0..3).
     *
     * @retun Text associated with the given line.First line's text in case
     *        given line number is out of range.
     */
    virtual TDesC& LineText( TInt aLineNumber ) const;
    
    /**
     * The function sets icon to a specified line.
     * 
     * @param aIcon Pointer to an icon. Not own.
     * @param aLineNumber Line number (0..3). In case of 0 the icon is a priority
     *                    icon. Checkbox icons are set by a seperate function.
     */
    virtual void SetLineIcon ( CAlfTexture* aIcon, TInt aLineNumber  );
    
    /**
     * The function returns an icon associated with a specified line.
     *      
     * @param aLineNumber Number of a line the icon is associated with.
     * 
     * @return Icon associated with the given line.First icon in case given
     *         line number is out of range.
     */
    virtual const CAlfTexture* LineIcon ( TInt aLineNumber  ) const;
        
    /**
     * The function sets icon for a checkbox.
     * 
     * @param aCheckedIcon An icon for a checked state. Not own.
     * @param aUncheckedIcon An icon for an unchecked state. Not own.
     */
    virtual void SetCheckBoxIcons( CAlfTexture* aCheckedIcon, CAlfTexture* aUncheckedIcon );
       
    /**
     * The function returns an icon for a checked checkbox's state.
     * 
     * @return An icon for a checked state.
     */
    virtual const CAlfTexture* CheckBoxCheckedIcon() const;
    
    
    /**
     * The function returns an icon for an unchecked checkbox's state.
     * 
     * @return An icon for an unchecked state.
     */
    virtual const CAlfTexture* CheckBoxUncheckedIcon() const;       
    
public: //from MFsTreeItemData.

    /**
     * From MFsTreeItemData.
     * 
     * Type of the data item.
     * 
     * @return Item's type identifier.
     */         
    virtual TFsTreeItemDataType Type() const;

protected:

    /**
     * The function copies the text from a given source to destination.
     */
    virtual void SetTextL( HBufC*& aDest, const TDesC& aSrcText );
    
protected:

    /**
     * C++ constructor
     */
    CFsMultiLineItemData( );
    
    /**
     * Second phase constructor
     *
     */
    void ConstructL( );
    
protected: //Data

    ///////////////////////////first line///////////////////////////
    /**
     * Descriptor holding first line's text.
     * Own.
     */
    HBufC* iFirstLineText;

    /**
     * Pointer holding first line's icon - priority icon.
     * Not own.
     */
    CAlfTexture* iFirstLineIcon;
    
    /**
     * Pointer holding first line's checked icon.
     * Not own.
     */
    CAlfTexture* iFirstLineCheckedIcon;    

    /**
     * Pointer holding first line's unchecked icon.
     * Not own.
     */
    CAlfTexture* iFirstLineUncheckedIcon;        
    
    ///////////////////////////second line///////////////////////////
    /**
     * Descriptor holding second line's text.
     * Own.
     */
    HBufC* iSecondLineText;
    
    /**
     * Pointer holding second line's icon.
     * Not own.
     */
    CAlfTexture* iSecondLineIcon;
    
    ///////////////////////////third line///////////////////////////
    /**
     * Descriptor holding third line's text.
     * Own.
     */
    HBufC* iThirdLineText;
    
    /**
     * Pointer holding third line's icon.
     * Not own.
     */
    CAlfTexture* iThirdLineIcon;
    
    ///////////////////////////fourth line///////////////////////////
    /**
     * Descriptor holding fourth line's text.
     * Own.
     */
    HBufC* iFourthLineText;
    
    /**
     * Pointer holding fourth line's icon.
     * Not own.
     */
    CAlfTexture* iFourthLineIcon;
        
    };

#endif //C_FSMULTILINEITEMDATA_H
