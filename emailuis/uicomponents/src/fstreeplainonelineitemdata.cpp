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
#include "fstreeplainonelineitemdata.h"
#include <e32cmn.h>
// <cmail> SF

#include <alf/alftexture.h>
// </cmail>
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreePlainOneLineItemData* CFsTreePlainOneLineItemData::NewL( )
    {
    FUNC_LOG;
    CFsTreePlainOneLineItemData* self = 
                                  new( ELeave ) CFsTreePlainOneLineItemData();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainOneLineItemData::~CFsTreePlainOneLineItemData() 
    {
    FUNC_LOG;
    delete iData;
    }

// ---------------------------------------------------------------------------
// Copies data to the descriptor given as a parameter.
// ---------------------------------------------------------------------------
//
TDesC& CFsTreePlainOneLineItemData::Data( ) const
    {
    FUNC_LOG;
    return *iData;
    }

// ---------------------------------------------------------------------------
// Copies data from the descriptor given as a parameter.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemData::SetDataL( const TDesC& aData )
    {
    FUNC_LOG;
    delete iData;
    iData = NULL;
    
    TInt eolIndex = aData.Find(_L("\n"));
    if (eolIndex==KErrNotFound)
        {
        iData = aData.AllocL();
        }
    else
        {
        iData = HBufC::NewL(eolIndex+3);
        *iData = aData.Left(eolIndex);
        TPtr16 desc = iData->Des();
        desc.Append(_L("..."));
        }
    }

// ---------------------------------------------------------------------------
// Returns length (not size) of the data descriptor.
// ---------------------------------------------------------------------------
//
TUint CFsTreePlainOneLineItemData::DataLength() const
    {
    FUNC_LOG;
    return iData->Length();
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainOneLineItemData::IsIconSet() const
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    
    if ( iIcon )
        {
        retVal = ETrue;
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
const CAlfTexture& CFsTreePlainOneLineItemData::Icon () const
    {
    FUNC_LOG;
    return *iIcon;
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemData::SetIcon ( CAlfTexture& aIcon )
    {
    FUNC_LOG;
    iIcon = &aIcon;
    }

// ---------------------------------------------------------------------------
//  Type of the data item.
// ---------------------------------------------------------------------------
//
TFsTreeItemDataType CFsTreePlainOneLineItemData::Type() const
    {
    FUNC_LOG;
    return KFsTreePlainOneLineItemDataType;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainOneLineItemData::CFsTreePlainOneLineItemData( )
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreePlainOneLineItemData::ConstructL( )
    {
    FUNC_LOG;
    SetDataL( KNullDesC );
    }

