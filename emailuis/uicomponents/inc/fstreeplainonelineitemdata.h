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


#ifndef C_FSTREEPLAINONELINEITEMDATA_H
#define C_FSTREEPLAINONELINEITEMDATA_H

#include <e32base.h>
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstreeitemdata.h"
 
const TFsTreeItemDataType KFsTreePlainOneLineItemDataType = 1;

class CAlfTexture;

NONSHARABLE_CLASS( CFsTreePlainOneLineItemData ): public CBase, 
                                                  public MFsTreeItemData 
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    IMPORT_C static CFsTreePlainOneLineItemData* NewL( );
    
    /**
    * C++ destructor
    */
    virtual ~CFsTreePlainOneLineItemData();
    
public:

    /**
     * Returns item's data.
     */
    virtual TDesC& Data( ) const;
    
    /**
     * Copies data from the descriptor given as a parameter.
     * 
     * @param aData Reference to the descriptor that the data will be copied
     *              from.
     */
    virtual void SetDataL( const TDesC& aData );
    
    /**
     * Returns length (not size) of the data descriptor.
     * 
     * @return The length of the descriptor holding data.
     */
    virtual TUint DataLength() const;
    
    /**
     * 
     */
    virtual TBool IsIconSet() const;
    
    /**
     * 
     */
    virtual const CAlfTexture& Icon () const;

    /**
     * 
     */
    virtual void SetIcon ( CAlfTexture& aIcon );

    /**
     * Returns type id of the separator's data class.
     */
    virtual TFsTreeItemDataType Type() const;
    
protected:

    /**
     * C++ constructor
     */
    CFsTreePlainOneLineItemData( );
    
    /**
     * Second phase constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    void ConstructL( );
    
protected: //Data
    
    /**
     * Descriptor holding data.
     * Own.
     */
    HBufC* iData;

    /**
     * 
     */
    CAlfTexture* iIcon;

    };
    

#endif  // C_FSTREEPLAINONELINEITEMDATA_H
