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


#ifndef C_FSTREEPLAINONELINENODEDATA_H
#define C_FSTREEPLAINONELINENODEDATA_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "fstreeplainonelineitemdata.h"
 
const TFsTreeItemDataType KFsTreePlainOneLineNodeDataType = 2;

class CAlfTexture;

NONSHARABLE_CLASS( CFsTreePlainOneLineNodeData ): 
    public CFsTreePlainOneLineItemData
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    IMPORT_C static CFsTreePlainOneLineNodeData* NewL( );
    
    /**
    * C++ destructor
    */
    virtual ~CFsTreePlainOneLineNodeData();
    
public:
    
    /**
     * 
     */
    virtual TFsTreeItemDataType Type() const;
    
    /**
     * 
     */
    virtual TBool IsIconExpandedSet() const;
    
    /**
     * 
     */
    virtual const CAlfTexture& IconExpanded () const;

    
    /**
     * 
     */
    virtual void SetIconExpanded ( CAlfTexture& aIcon );
    
    /**
     * 
     */
    virtual TBool IsIconCollapsedSet() const;
    
    /**
     * 
     */
    virtual const CAlfTexture& IconCollapsed() const;
    
    /**
     * 
     */
    virtual void SetIconCollapsed( CAlfTexture& aIcon );
            
protected:
    
    /**
     * 
     */
    virtual TBool IsIconSet() const;
    
    /**
     * 
     */
    virtual void SetIcon ( CAlfTexture& aIcon );

protected:

    /**
     * C++ constructor
     */
    CFsTreePlainOneLineNodeData( );
    
    /**
     * Second phase constructor
     *
     * @param aText Reference to the descriptor that the data will be copied
     *              from.
     */
    void ConstructL( );
    
protected: //Data

    /**
     * 
     */
    CAlfTexture* iIconCollapsed;
    
    };
    

#endif  // C_FSTREEPLAINONELINENODEDATA_H
