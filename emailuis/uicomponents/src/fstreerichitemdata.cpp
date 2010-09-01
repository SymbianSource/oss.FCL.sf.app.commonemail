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
* Description:  Tree item data derived class able to store rich text.
*
*/



#include "emailtrace.h"
#include <txtrich.h>
#include <eikenv.h>
// <cmail> SF
#include <alf/alftexture.h>
// </cmail>
#include "fstreerichitemdata.h"
#include "fsrichtext.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFsTreeRichItemData::CFsTreeRichItemData( CFsRichText* aText )
    : iFsRichText( aText )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemData::ConstructL()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Two-phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsTreeRichItemData* CFsTreeRichItemData::NewL( CFsRichText* aText )
    {
    FUNC_LOG;
    CFsTreeRichItemData* self = new( ELeave ) CFsTreeRichItemData( aText );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// Set the FsRichText object. Ownership of this object is gained.
// ---------------------------------------------------------------------------
//
CFsTreeRichItemData::~CFsTreeRichItemData()
    {
    FUNC_LOG;
    delete iFsRichText;
    }


// ---------------------------------------------------------------------------
// Set the FsRichText object. Ownership of this object is gained.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemData::SetText( CFsRichText* aText )
    {
    FUNC_LOG;
    delete iFsRichText;
    iFsRichText = aText;
    }


// ---------------------------------------------------------------------------
// Return the FsRichText object.
// ---------------------------------------------------------------------------
//
EXPORT_C const CFsRichText& CFsTreeRichItemData::Text() const
    {
    FUNC_LOG;
    return *iFsRichText;
    }


// ---------------------------------------------------------------------------
// Determines if an icon is set.
// ---------------------------------------------------------------------------
//
TBool CFsTreeRichItemData::IsIconSet() const
    {
    FUNC_LOG;
    return iIcon ? ETrue : EFalse;
    }


// ---------------------------------------------------------------------------
// Get the current icon in use.
// ---------------------------------------------------------------------------
//
const CAlfTexture& CFsTreeRichItemData::Icon() const
    {
    FUNC_LOG;
    return *iIcon;
    }


// ---------------------------------------------------------------------------
// Specify an icon to use.
// ---------------------------------------------------------------------------
//
void CFsTreeRichItemData::SetIcon(CAlfTexture& aIcon)
    {
    FUNC_LOG;
    iIcon = &aIcon;
    }


// ---------------------------------------------------------------------------
// From class MFsTreeItemData.
// Type of the data item.
// ---------------------------------------------------------------------------
//
TFsTreeItemDataType CFsTreeRichItemData::Type() const
    {
    FUNC_LOG;
    return KFsTreeRichItemDataType;
    }

