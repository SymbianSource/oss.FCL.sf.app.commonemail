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


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag

#include "emailtrace.h"
#include "fstreeplainonelinenodedata.h"
#include <e32cmn.h>
// <cmail>
#include <alf/alftexture.h>
// </cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreePlainOneLineNodeData* CFsTreePlainOneLineNodeData::NewL( )
    {
    FUNC_LOG;
    CFsTreePlainOneLineNodeData* self = 
                                   new( ELeave ) CFsTreePlainOneLineNodeData( );
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainOneLineNodeData::~CFsTreePlainOneLineNodeData() 
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainOneLineNodeData::IsIconExpandedSet() const
    {
    FUNC_LOG;
    return CFsTreePlainOneLineItemData::IsIconSet();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CAlfTexture& CFsTreePlainOneLineNodeData::IconExpanded () const
    {
    FUNC_LOG;
    return CFsTreePlainOneLineItemData::Icon();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineNodeData::SetIconExpanded ( CAlfTexture& aIcon )
    {
    FUNC_LOG;
    CFsTreePlainOneLineItemData::SetIcon( aIcon );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainOneLineNodeData::IsIconCollapsedSet() const
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iIconCollapsed )
        {
        retVal = ETrue;
        }
    else
        {
        retVal = EFalse;
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CAlfTexture& CFsTreePlainOneLineNodeData::IconCollapsed() const
    {
    FUNC_LOG;
    return *iIconCollapsed;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineNodeData::SetIconCollapsed( CAlfTexture& aIcon )
    {
    FUNC_LOG;
    iIconCollapsed = &aIcon;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainOneLineNodeData::IsIconSet() const
    {
    FUNC_LOG;
    return CFsTreePlainOneLineItemData::IsIconSet();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineNodeData::SetIcon ( CAlfTexture& aIcon )
    {
    FUNC_LOG;
    CFsTreePlainOneLineItemData::SetIcon( aIcon );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TFsTreeItemDataType CFsTreePlainOneLineNodeData::Type() const
    {
    FUNC_LOG;
    return KFsTreePlainOneLineNodeDataType;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainOneLineNodeData::CFsTreePlainOneLineNodeData( )
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineNodeData::ConstructL( )
    {
    FUNC_LOG;
    SetDataL( KNullDesC );
    }

