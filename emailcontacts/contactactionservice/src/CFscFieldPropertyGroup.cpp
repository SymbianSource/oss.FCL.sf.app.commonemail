/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation field property group.
 *
*/


#include "emailtrace.h"
#include "CFscFieldPropertyGroup.h"

// System includes
#include <barsread.h>

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::CFscFieldPropertyGroup
// --------------------------------------------------------------------------
//
CFscFieldPropertyGroup::CFscFieldPropertyGroup()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::~CFscFieldPropertyGroup
// --------------------------------------------------------------------------
//
CFscFieldPropertyGroup::~CFscFieldPropertyGroup()
    {
    FUNC_LOG;
    iProperties.Close();
    delete iLabel;
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::ConstructL
// --------------------------------------------------------------------------
//
inline void CFscFieldPropertyGroup::ConstructL(TResourceReader& aReader)
    {
    FUNC_LOG;
    aReader.ReadInt8(); // read version number
    iGroupId = static_cast<TPbk2FieldGroupId>(aReader.ReadInt8() );
    iLabel = aReader.ReadHBufC16L();
    iIconId = TPbk2IconId(aReader);
    iAddItemOrdering = aReader.ReadInt8();
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::NewLC
// --------------------------------------------------------------------------
//
CFscFieldPropertyGroup* CFscFieldPropertyGroup::NewLC(
        TResourceReader& aReader)
    {
    FUNC_LOG;
    CFscFieldPropertyGroup* self = new ( ELeave ) CFscFieldPropertyGroup;
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    return self;
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::Count
// --------------------------------------------------------------------------
//
 TInt CFscFieldPropertyGroup::Count() const
    {
    FUNC_LOG;
    return iProperties.Count();
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::At
// --------------------------------------------------------------------------
//
 const MPbk2FieldProperty& CFscFieldPropertyGroup::At
( TInt aIndex ) const
    {
    FUNC_LOG;
    return *iProperties[aIndex];
    }

// --------------------------------------------------------------------------
// CFscFieldPropertyGroup::AddFieldPropertyL
// --------------------------------------------------------------------------
//  
void CFscFieldPropertyGroup::AddFieldPropertyL(
        const MPbk2FieldProperty& aProperty)
    {
    FUNC_LOG;
    iProperties.AppendL( &aProperty);
    }

// End of File  

