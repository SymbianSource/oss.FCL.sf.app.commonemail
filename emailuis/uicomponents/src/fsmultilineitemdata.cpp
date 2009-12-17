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


//////TOOLKIT INCLUDES
// <cmail> SF
#include "emailtrace.h"
#include <alf/alftexture.h>
// </cmail>

//////PROJECT INCLUDES
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include "fsmultilineitemdata.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsMultiLineItemData* CFsMultiLineItemData::NewL( )
    {
    FUNC_LOG;
    CFsMultiLineItemData* self = new( ELeave ) CFsMultiLineItemData();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    return self;
    }
    
// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsMultiLineItemData::~CFsMultiLineItemData()
    {
    FUNC_LOG;
    delete iFirstLineText;
    delete iSecondLineText;
    delete iThirdLineText;
    delete iFourthLineText;
    }

// ---------------------------------------------------------------------------
// The function sets text to a specified line.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemData::SetLineTextL( const TDesC& aText, TInt aLineNumber )
    {
    FUNC_LOG;
    
    if ( aLineNumber >= 0 && aLineNumber < 4 )
        {
        switch ( aLineNumber )
            {
            case 0:
                SetTextL( iFirstLineText, aText );
                break;
            case 1:
                SetTextL( iSecondLineText, aText );
                break;
            case 2:
                SetTextL( iThirdLineText, aText );
                break;
            case 3:                
                SetTextL( iFourthLineText, aText );
                break;
            default:
                break;
            }                        
        }
    }

// ---------------------------------------------------------------------------
// The function return a text associated with a specified line.
// ---------------------------------------------------------------------------
//
TDesC& CFsMultiLineItemData::LineText( TInt aLineNumber ) const
    {
    FUNC_LOG;
    HBufC* text(NULL);
    
    switch ( aLineNumber )
        {
        case 0:
            text = iFirstLineText;            
            break;
        case 1:
            text = iSecondLineText;            
            break;
        case 2:
            text = iThirdLineText;            
            break;
        case 3:
            text = iFourthLineText;
            break;
        default:
            text = iFirstLineText;
            break;
        }
    return *text;
    }

// ---------------------------------------------------------------------------
// The function sets icon to a specified line.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemData::SetLineIcon ( CAlfTexture* aIcon, TInt aLineNumber  )
    {
    FUNC_LOG;
    if ( aLineNumber >= 0 && aLineNumber < 4 )
        {
        switch ( aLineNumber )
            {
            case 0:
                iFirstLineIcon = aIcon;
                break;
            case 1:
                iSecondLineIcon = aIcon;
                break;
            case 2:
                iThirdLineIcon = aIcon;
                break;
            case 3:
                iFourthLineIcon = aIcon;
                break;
            default:
                break;
            }                
        }
    }

// ---------------------------------------------------------------------------
// The function returns an icon associated with a specified line.
// ---------------------------------------------------------------------------
//
const CAlfTexture* CFsMultiLineItemData::LineIcon ( TInt aLineNumber  ) const
    {
    FUNC_LOG;
    CAlfTexture* icon(NULL);
    
    switch ( aLineNumber )
        {
        case 0:
            icon = iFirstLineIcon;
            break;
        case 1:
            icon = iSecondLineIcon;
            break;
        case 2:
            icon = iThirdLineIcon;
            break;
        case 3:
            icon = iFourthLineIcon;            
            break;
        default:
            icon = iFirstLineIcon;            
            break;
        }
    return icon;
    }
    
// ---------------------------------------------------------------------------
// The function sets icons for a checkbox.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemData::SetCheckBoxIcons( CAlfTexture* aCheckedIcon, CAlfTexture* aUncheckedIcon )
    {
    FUNC_LOG;
    iFirstLineCheckedIcon = aCheckedIcon;

    iFirstLineUncheckedIcon = aUncheckedIcon;
    }

// ---------------------------------------------------------------------------
// The function returns an icon for a checked checkbox's state.
// ---------------------------------------------------------------------------
//
const CAlfTexture* CFsMultiLineItemData::CheckBoxCheckedIcon() const
    {
    FUNC_LOG;
    return iFirstLineCheckedIcon;
    }

// ---------------------------------------------------------------------------
// The function returns an icon for an unchecked checkbox's state.
// ---------------------------------------------------------------------------
//
const CAlfTexture* CFsMultiLineItemData::CheckBoxUncheckedIcon() const
    {
    FUNC_LOG;
    return iFirstLineUncheckedIcon;
    }

//from MFsTreeItemData.

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TFsTreeItemDataType CFsMultiLineItemData::Type() const
    {
    FUNC_LOG;
    return KFsMultiLineItemDataType;
    }

// ---------------------------------------------------------------------------
// The function copies the text from a given source to destination.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemData::SetTextL( HBufC*& aDest, const TDesC& aSrcText )
    {
    FUNC_LOG;
    delete aDest;
    aDest = NULL;       
    
    TInt eolIndex = aSrcText.Find(_L("\n"));
    if (eolIndex==KErrNotFound)
        {
        aDest = aSrcText.AllocL();
        }
    else
        {
        aDest = HBufC::NewL(eolIndex+3);
        *aDest = aSrcText.Left(eolIndex);
        TPtr16 desc = aDest->Des();
        desc.Append(_L("..."));
        }
    }
    
// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsMultiLineItemData::CFsMultiLineItemData( )
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsMultiLineItemData::ConstructL( )
    {
    FUNC_LOG;
    TInt line(0);
    SetLineTextL( KNullDesC, line++ );
    SetLineTextL( KNullDesC, line++ );
    SetLineTextL( KNullDesC, line++ );
    SetLineTextL( KNullDesC, line );
    }

//EOF

