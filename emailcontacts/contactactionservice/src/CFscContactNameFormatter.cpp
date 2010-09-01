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
* Description:  Implementation of the class CFscContactNameFormatter.
 *
*/


#include "emailtrace.h"
#include "CFscContactNameFormatter.h"
#include <CVPbkContactFieldIterator.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeRefsList.h>

namespace
    {

    enum TNameOrder
        {
        ENameFirstPart = 0,
        ENameSecondPart
        };
    } // namespace

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::CFscContactNameFormatter
// -----------------------------------------------------------------------------
//
CFscContactNameFormatter::CFscContactNameFormatter(
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        const CPbk2SortOrderManager& aSortOrderManager) :
    CFscContactNameFormatterBase(aMasterFieldTypeList, aSortOrderManager)
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::NewL
// -----------------------------------------------------------------------------
//
CFscContactNameFormatter* CFscContactNameFormatter::NewL(
        const TDesC& aUnnamedText,
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        const CPbk2SortOrderManager& aSortOrderManager,
        CVPbkFieldTypeSelector* aTitleFieldSelector)
    {
    FUNC_LOG;
    CFscContactNameFormatter* self = new(ELeave) CFscContactNameFormatter
    (aMasterFieldTypeList, aSortOrderManager);
    CleanupStack::PushL(self);
    self->ConstructL(aUnnamedText, aTitleFieldSelector);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::ConstructL
// -----------------------------------------------------------------------------
//
void CFscContactNameFormatter::ConstructL(const TDesC& aUnnamedText,
        CVPbkFieldTypeSelector* aTitleFieldSelector)
    {
    FUNC_LOG;
    // Base class takes ownership of aTitleFieldSelector.
    // Must not leave after BaseConstructL
    BaseConstructL(aUnnamedText, aTitleFieldSelector);
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::~CFscContactNameFormatter
// -----------------------------------------------------------------------------
//
CFscContactNameFormatter::~CFscContactNameFormatter()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::GetContactTitle
// -----------------------------------------------------------------------------
//
void CFscContactNameFormatter::GetContactTitle(
        const MVPbkBaseContactFieldCollection& aContactFields, TDes& aTitle,
        TUint32 aFormattingFlags)
    {
    FUNC_LOG;
    aTitle.Zero();
    SetFieldMapper(aContactFields);

    // Try to create title using first two fields of the sort order
    // If the field type in the sort order is not defined as a title field
    // then it's not used for title creation.
    DoGetContactTitle(aTitle, aFormattingFlags, ENameFirstPart,
            ENameSecondPart);

    // If there was no data in the first two fields then
    // use the third and the rest of the fields in the sort order
    // for title creation.
    if (aTitle.Length() == 0 && AreTitleFieldsEmpty(ENameFirstPart,
            ENameSecondPart) )
        {
        // Find the next field in the sort order that has
        // data to build the title
        const TInt count = iFieldMapper.FieldCount();
        DoGetContactTitle(aTitle, aFormattingFlags, ENameSecondPart+1, count
                -1); // zero-based
        }
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::GetContactTitleForFind
// -----------------------------------------------------------------------------
//
void CFscContactNameFormatter::GetContactTitleForFind(
        const MVPbkBaseContactFieldCollection& aContactFields, TDes& aTitle,
        TUint32 aFormattingFlags)
    {
    FUNC_LOG;
    aTitle.Zero();
    SetFieldMapper(aContactFields);

    const TInt count = iFieldMapper.FieldCount();
    DoGetContactTitle(aTitle, aFormattingFlags, ENameFirstPart, count-1); // zero-based
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::ActualTitleFieldsLC
// See function CFscContactNameFormatter::GetContactTitle.
// -----------------------------------------------------------------------------
//
CVPbkBaseContactFieldTypeListIterator* CFscContactNameFormatter::ActualTitleFieldsLC(
        CVPbkFieldTypeRefsList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aContactFields)
    {
    FUNC_LOG;
    aFieldTypeList.Reset();

    SetFieldMapper(aContactFields);

    DoAppendNonEmptyTitleFieldTypesL(aFieldTypeList, ENameFirstPart,
            ENameSecondPart);

    if (aFieldTypeList.FieldTypeCount() == 0)
        {
        const TInt count = iFieldMapper.FieldCount();
        DoAppendNonEmptyTitleFieldTypesL(aFieldTypeList, ENameSecondPart+1,
                count-1);
        }

    return CVPbkBaseContactFieldTypeListIterator::NewLC(aFieldTypeList,
            aContactFields);
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::MaxTitleLength
// -----------------------------------------------------------------------------
//
TInt CFscContactNameFormatter::MaxTitleLength(
        const MVPbkBaseContactFieldCollection& aContactFields,
        const TUint32 aFormattingFlags)
    {
    FUNC_LOG;
    TInt result = 0;
    SetFieldMapper(aContactFields);

    result = DoCalculateMaxTitleLength(aFormattingFlags, ENameFirstPart,
            ENameSecondPart);

    if (result == 0)
        {
        // Use the rest of the fields in sort order to
        // build the title (ie. the company name)
        const TInt count = iFieldMapper.FieldCount();
        result = DoCalculateMaxTitleLength(aFormattingFlags, ENameSecondPart
                +1, count-1); // zero-based
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::MaxTitleLengthForFind
// -----------------------------------------------------------------------------
//
TInt CFscContactNameFormatter::MaxTitleLengthForFind(
        const MVPbkBaseContactFieldCollection& aContactFields,
        const TUint32 aFormattingFlags)
    {
    FUNC_LOG;
    TInt result = 0;
    SetFieldMapper(aContactFields);

    const TInt count = iFieldMapper.FieldCount();
    result = DoCalculateMaxTitleLength(aFormattingFlags, ENameFirstPart,
            count-1); // zero-based

    return result;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatter::IsFindSeparatorChar
// -----------------------------------------------------------------------------
//
TBool CFscContactNameFormatter::IsFindSeparatorChar(TChar aCh)
    {
    FUNC_LOG;
    return aCh.IsSpace();
    }

// End of File

