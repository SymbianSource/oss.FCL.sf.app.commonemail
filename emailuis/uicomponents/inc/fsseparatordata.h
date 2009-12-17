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
* Description:  Data class for a separator item.
*
*/


#ifndef C_FSTSEPARATORDATA_H
#define C_FSTSEPARATORDATA_H

#include <e32base.h>
//#include <fsconfig.h> <cmail>
#include <gdi.h>

#include "fstreeitemdata.h"
 
const TFsTreeItemDataType KFsSeparatorDataType = 6;

NONSHARABLE_CLASS( CFsSeparatorData ): public CBase, public MFsTreeItemData 
    {
public:

    /**
     * Two-phased constructor    
     */
    IMPORT_C static CFsSeparatorData* NewL( );
    
    /**
    * C++ destructor
    */
    virtual ~CFsSeparatorData();
    
public:

    /**
     * The function sets separator line's color.
     * 
     * @aColor Color of the separator line.
     */
    virtual void SetSeparatorColor( const TRgb& aColor );
    
    
    /**
     * The function returns separator line's color.
     * 
     * @return Color of the separator line. 
     */
    virtual TRgb SeparatorColor() const;

// from base class MFsTreeItemData
    
    /**
     * From MFsTreeItemData.
     * Returns type id of the separator's data class.
     */
    virtual TFsTreeItemDataType Type() const;
    
protected:

    /**
     * C++ constructor
     */
    CFsSeparatorData( );
    
    /**
     * Second phase constructor.     
     */
    void ConstructL( );
    
protected: //Data

    /**
     * Separator's color.
     */
    TRgb iColor;

    };
    

#endif  // C_FSTSEPARATORDATA_H
