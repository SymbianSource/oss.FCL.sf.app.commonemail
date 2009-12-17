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



#include "emailtrace.h"
#include "fstreeplaintwolineitemdata.h"
#include <e32cmn.h>
// <cmail> SF
#include <alf/alftexture.h>
// </cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreePlainTwoLineItemData* CFsTreePlainTwoLineItemData::NewL( )
    {
    FUNC_LOG;
    CFsTreePlainTwoLineItemData* self = 
                                   new( ELeave ) CFsTreePlainTwoLineItemData( );
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainTwoLineItemData::~CFsTreePlainTwoLineItemData() 
    {
    FUNC_LOG;
    delete iSecondaryData;
    delete iPreviewPaneData;
    delete iDateTimeData;
    }

// ---------------------------------------------------------------------------
// Copies data to the descriptor given as a parameter.
// ---------------------------------------------------------------------------
//
TDesC& CFsTreePlainTwoLineItemData::SecondaryData( ) const
    {
    FUNC_LOG;
    return *iSecondaryData;
    }

// ---------------------------------------------------------------------------
// Copies data from the descriptor given as a parameter.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemData::SetSecondaryDataL( const TDesC& aData )
    {
    FUNC_LOG;
    delete iSecondaryData;
    iSecondaryData = NULL;
    
    TInt eolIndex = aData.Find(_L("\n"));
    if (eolIndex==KErrNotFound)
        {
        iSecondaryData = aData.AllocL();
        }
    else
        {
        iSecondaryData = HBufC::NewL(eolIndex+3);
        *iSecondaryData = aData.Left(eolIndex);
        TPtr16 desc = iSecondaryData->Des();
        desc.Append(_L("..."));
        }
    }

// ---------------------------------------------------------------------------
// Returns length (not size) of the data descriptor.
// ---------------------------------------------------------------------------
//
TUint CFsTreePlainTwoLineItemData::SecondaryDataLength() const
    {
    FUNC_LOG;
    TUint length = 0;
    if ( iSecondaryData )
        {
        length = iSecondaryData->Length();
        }
    else
        {
        length = 0;
        }
    return length;
    }

// ---------------------------------------------------------------------------
// Sets preview pane text.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemData::SetPreviewPaneDataL( const TDesC& aData )
    {
    FUNC_LOG;
    delete iPreviewPaneData;
    iPreviewPaneData = NULL;
    
    TInt eolIndex = aData.Find(_L("\n"));
    if (eolIndex==KErrNotFound)
        {
        iPreviewPaneData = aData.AllocL();
        }
    else
        {
        iPreviewPaneData = HBufC::NewL(eolIndex+3);
        *iPreviewPaneData = aData.Left(eolIndex);
        TPtr16 desc = iPreviewPaneData->Des();
        desc.Append(_L("..."));
        }
    }

// ---------------------------------------------------------------------------
// Returns preview pane text.
// ---------------------------------------------------------------------------
//
TDesC& CFsTreePlainTwoLineItemData::PreviewPaneData( ) const
    {
    FUNC_LOG;
    return *iPreviewPaneData;
    }

// ---------------------------------------------------------------------------
// Returns length (not size) of the preview pane data descriptor.
// ---------------------------------------------------------------------------
//
TUint CFsTreePlainTwoLineItemData::PreviewPaneDataLength() const
    {
    FUNC_LOG;
    TUint length = 0;
    if ( iPreviewPaneData )
        {
        length = iPreviewPaneData->Length();
        }
    else
        {
        length = 0;
        }
    return length;
    }

// ---------------------------------------------------------------------------
// Sets texture for a flag icon.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemData::SetFlagIcon ( CAlfTexture& aIcon )
    {
    FUNC_LOG;
    iFlagIcon = &aIcon;
    }

// ---------------------------------------------------------------------------
// Returns flag icon texture.
// ---------------------------------------------------------------------------
//
const CAlfTexture& CFsTreePlainTwoLineItemData::FlagIcon ( ) const
    {
    FUNC_LOG;
    return *iFlagIcon;
    }

// ---------------------------------------------------------------------------
// Returns information wether icon for a flag is set.
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainTwoLineItemData::IsFlagIconSet ( ) const
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iFlagIcon )
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
// Functions sets text for a time and date visual.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemData::SetDateTimeDataL ( const TDesC& aData )
    {
    FUNC_LOG;
    delete iDateTimeData;
    iDateTimeData = NULL;
    
    TInt eolIndex = aData.Find(_L("\n"));
    if (eolIndex==KErrNotFound)
        {
        iDateTimeData = aData.AllocL();
        }
    else
        {
        iDateTimeData = HBufC::NewL(eolIndex+3);
        *iDateTimeData = aData.Left(eolIndex);
        TPtr16 desc = iDateTimeData->Des();
        desc.Append(_L("..."));
        }
    }

// ---------------------------------------------------------------------------
// Returns date and time text.
// ---------------------------------------------------------------------------
//
TDesC& CFsTreePlainTwoLineItemData::DateTimeData ( ) const
    {
    FUNC_LOG;
    return *iDateTimeData;
    }

// ---------------------------------------------------------------------------
// Returns length of the date and time text.
// ---------------------------------------------------------------------------
//
TUint CFsTreePlainTwoLineItemData::DateTimeDataLength ( ) const
    {
    FUNC_LOG;
    TUint length = 0;
    if ( iDateTimeData )
        {
        length = iDateTimeData->Length();
        }
    else
        {
        length = 0;
        }
    return length;
    }

// ---------------------------------------------------------------------------
// Sets texture for a priority icon.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemData::SetPriorityIcon ( CAlfTexture& aIcon )
    {
    FUNC_LOG;
    iPriorityIcon = &aIcon;
    }

// ---------------------------------------------------------------------------
// Returns priority texture.
// ---------------------------------------------------------------------------
//
const CAlfTexture& CFsTreePlainTwoLineItemData::PriorityIcon ( ) const     
    {
    FUNC_LOG;
    return *iPriorityIcon;
    }

// ---------------------------------------------------------------------------
// Returns information wether texture for a priority icon is set.
// ---------------------------------------------------------------------------
//
TBool CFsTreePlainTwoLineItemData::IsPriorityIconSet ( ) const
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iPriorityIcon )
        {
        retVal = ETrue;
        }
    else
        {
        retVal = EFalse;
        }    
    return retVal;
    }


// from base class MFsTreeItemData

// ---------------------------------------------------------------------------
//  Type of the data item.
//  From base class MFsTreeItemData
// ---------------------------------------------------------------------------
//
TFsTreeItemDataType CFsTreePlainTwoLineItemData::Type() const
    {
    FUNC_LOG;
    return KFsTreePlainTwoLineItemDataType;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreePlainTwoLineItemData::CFsTreePlainTwoLineItemData( )
    : iFlagIcon ( NULL ),
      iPriorityIcon ( NULL )
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreePlainTwoLineItemData::ConstructL( )
    {
    FUNC_LOG;
    SetSecondaryDataL( KNullDesC );
    SetPreviewPaneDataL( KNullDesC );
    SetDateTimeDataL( KNullDesC );
    }

