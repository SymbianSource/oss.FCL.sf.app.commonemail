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


//#include <fsconfig.h>

#include "emailtrace.h"
#include "fsseparatordata.h"
// <cmail> SF
#include <alf/alftexture.h>
// </cmail>
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFsSeparatorData* CFsSeparatorData::NewL( )
    {
    FUNC_LOG;
    CFsSeparatorData* self = new( ELeave ) CFsSeparatorData();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsSeparatorData::~CFsSeparatorData() 
    {
    FUNC_LOG;

    }


// ---------------------------------------------------------------------------
//  The function sets separator line's color.
// ---------------------------------------------------------------------------
//
void CFsSeparatorData::SetSeparatorColor( const TRgb& aColor )
    {
    FUNC_LOG;
    iColor = aColor;
    }


// ---------------------------------------------------------------------------
//  The function returns separator line's color.
// ---------------------------------------------------------------------------
//
TRgb CFsSeparatorData::SeparatorColor() const
    {
    FUNC_LOG;
    return iColor;
    }

// ---------------------------------------------------------------------------
//  Type of the data item.
// ---------------------------------------------------------------------------
//
TFsTreeItemDataType CFsSeparatorData::Type() const
    {
    FUNC_LOG;
    return KFsSeparatorDataType;
    }

// ---------------------------------------------------------------------------
//  C++ constructor.
// ---------------------------------------------------------------------------
//
CFsSeparatorData::CFsSeparatorData( )
    :iColor(KRgbGray)
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
//  Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFsSeparatorData::ConstructL( )
    {
    FUNC_LOG;

    }

