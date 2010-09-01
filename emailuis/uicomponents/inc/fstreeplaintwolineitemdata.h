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
* Description:  Tree item data derived class able to store plain text.
*
*/


#ifndef C_FSTREEPLAINTWOLINEITEMDATA_H
#define C_FSTREEPLAINTWOLINEITEMDATA_H

#include <e32base.h>

#include "fstreeplainonelineitemdata.h"
 
const TFsTreeItemDataType KFsTreePlainTwoLineItemDataType = 3;

class CAlfTexture;

NONSHARABLE_CLASS( CFsTreePlainTwoLineItemData ): 
    public CFsTreePlainOneLineItemData 
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    IMPORT_C static CFsTreePlainTwoLineItemData* NewL( );
    
    /**
    * C++ destructor
    */
    virtual ~CFsTreePlainTwoLineItemData();
    
public:

    /**
     * Returns secondary text.
     * 
     */
    virtual TDesC& SecondaryData( ) const;
    
    /**
     * Copies data from the descriptor given as a parameter.
     * 
     * @param aData Reference to the descriptor that the data will be copied
     *              from.
     */
    virtual void SetSecondaryDataL( const TDesC& aData );
    
    /**
     * Returns length (not size) of the secondary data descriptor.
     * 
     * @return The length of the descriptor holding data.
     */
    virtual TUint SecondaryDataLength() const;
    
    /**
     * Returns preview pane text.
     * 
     */
    virtual TDesC& PreviewPaneData( ) const;
    
    /**
     * Sets preview pane text.
     * 
     * @param aData Reference to the descriptor that the data will be copied
     *              from.
     */
    virtual void SetPreviewPaneDataL( const TDesC& aData );
    
    /**
     * Returns length (not size) of the preview pane data descriptor.
     * 
     * @return The length of the descriptor holding data.
     */
    virtual TUint PreviewPaneDataLength() const;
    
    /**
     * Sets texture for a flag icon.
     * 
     * @param aIcon Flag icon texture.
     */
    virtual void SetFlagIcon ( CAlfTexture& aIcon );
    
    /**
     * Returns flag icon texture.
     */
    virtual const CAlfTexture& FlagIcon ( ) const;        

    /**
     * Returns information wether icon for a flag is set.
     * 
     * @return ETrue if the flag icon has been set.
     */
    virtual TBool IsFlagIconSet ( ) const;
    
    /**
     * Functions sets text for a time and date visual.
     * 
     * @param aData Reference to the descriptor that the data will be copied
     *              from.
     */
    virtual void SetDateTimeDataL ( const TDesC& aData );
    
    /**
     *  Returns date and time text.
     * 
     * @return Date and Time text.
     */
    virtual TDesC& DateTimeData ( ) const;
    
    /**
     * Returns length of the date and time text.
     * 
     * @return The length of the descriptor holding date and time data.
     */
    virtual TUint DateTimeDataLength ( ) const;
    
    /**
     * Sets texture for a priority icon.
     * 
     * @param aIcon Priority icon texture.
     */
    virtual void SetPriorityIcon ( CAlfTexture& aIcon );
    
    /**
     * Returns priority texture.
     */
    virtual const CAlfTexture& PriorityIcon ( ) const;        

    /**
     * Returns information wether texture for a priority icon is set.
     * 
     * @return ETrue if the priority texture has been set.
     */
    virtual TBool IsPriorityIconSet ( ) const;
    
// from base class MFsTreeItemData
    
    /**
     * Type of the data item.
     */    
    virtual TFsTreeItemDataType Type() const;
    
protected:

    /**
     * C++ constructor
     */
    CFsTreePlainTwoLineItemData( );
    
    /**
     * Second phase constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    void ConstructL( );
    
protected: //Data
    
    /**
     * Descriptor holding secondaty text.
     * Own.
     */
    HBufC* iSecondaryData;
    
    /**
     * Descriptor holding preview pane text (3rd line of text).
     * Own.
     */
    HBufC* iPreviewPaneData;
    
    /**
     * Flag icon
     */
    CAlfTexture* iFlagIcon;
    
    /**
     * Date string.
     */
    HBufC* iDateTimeData;

    /**
     * Priority icon.
     */
    CAlfTexture* iPriorityIcon;
    
    };
    

#endif  // C_FSTREEPLAINTWOLINEITEMDATA_H
