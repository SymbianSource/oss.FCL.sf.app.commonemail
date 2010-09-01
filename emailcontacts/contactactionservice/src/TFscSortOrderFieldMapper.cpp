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
* Description:  Implementation of sort order field mapper.
*
*/


#include "emailtrace.h"
#include "TFscSortOrderFieldMapper.h"

// From Virtual Phonebook Engine
#include <MVPbkFieldType.h>
#include <MVPbkViewContact.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkBaseContactFieldCollection.h>

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// TFscSortOrderFieldMapper::TFscSortOrderFieldMapper
// --------------------------------------------------------------------------
//
 TFscSortOrderFieldMapper::TFscSortOrderFieldMapper()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// TFscSortOrderFieldMapper::SetSortOrder
// --------------------------------------------------------------------------
//
 void TFscSortOrderFieldMapper::SetSortOrder(
        const MVPbkFieldTypeList& aSortOrder )
    {
    FUNC_LOG;
    iSortOrder = &aSortOrder;
    }

// --------------------------------------------------------------------------
// TFscSortOrderFieldMapper::SetContactFields
// --------------------------------------------------------------------------
//
 void TFscSortOrderFieldMapper::SetContactFields(
        const MVPbkBaseContactFieldCollection& aContactFields )
    {
    FUNC_LOG;
    iContactFields = &aContactFields;
    }

// --------------------------------------------------------------------------
// TFscSortOrderFieldMapper::FieldCount
// --------------------------------------------------------------------------
//
 TInt TFscSortOrderFieldMapper::FieldCount() const
    {
    FUNC_LOG;
    return iSortOrder->FieldTypeCount();
    }

// --------------------------------------------------------------------------
// TFscSortOrderFieldMapper::FieldAt
// --------------------------------------------------------------------------
//
 const MVPbkBaseContactField* TFscSortOrderFieldMapper::FieldAt(
        TInt aIndex ) const
    {
    FUNC_LOG;
    const MVPbkBaseContactField* result = NULL;
    const MVPbkFieldType& sortOrderType = iSortOrder->FieldTypeAt( aIndex );

    const TInt maxMatchPriority = iSortOrder->MaxMatchPriority();
    const TInt fieldCount = iContactFields->FieldCount();

    for ( TInt matchPriority = 0; 
          !result && matchPriority <= maxMatchPriority; 
          ++matchPriority )
        {
        for ( TInt i = 0; i < fieldCount; ++i )
            {
            const MVPbkFieldType* contactFieldType =
                iContactFields->FieldAt( i ).MatchFieldType( matchPriority );
            if ( contactFieldType && sortOrderType.IsSame( *contactFieldType ) )
                {
                result = &iContactFields->FieldAt( i );
                break;
                }
            }
        }

    return result;
    }
// End of file

