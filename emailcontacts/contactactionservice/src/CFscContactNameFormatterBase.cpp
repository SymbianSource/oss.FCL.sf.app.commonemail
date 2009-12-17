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
* Description:  A base class for contact name formatters implementation
 *
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "CFscContactNameFormatterBase.h"

#include <f32file.h>
#include <barsread.h>
#include <barsc.h>
#include <coemain.h>

#include <MVPbkFieldType.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldTypeRefsList.h>

#include <CPbk2SortOrderManager.h>

#include "FscPresentationUtils.h"
#include <data_caging_path_literals.hrh>

//_LIT(KPbk2RomFileDrive, "z:");

namespace
    {

    // CONSTANTS
    _LIT(KSpace, " ");
    const TText KReplaceChar = ' ';

    // ============================= LOCAL FUNCTIONS ==========================

    void TrimNameAppend(TUint32 aFormattingFlags, const TDesC& aName,
            TDes& aTitle)
        {
        if (aFormattingFlags
                & MPbk2ContactNameFormatter::EPreserveLeadingSpaces)
            {
            // In lists, the leading spaces are not formatted,
            // only the end of the string is
            FscPresentationUtils::TrimRightAppend(aName, aTitle);
            }
        else
            {
            // String is trimmed from beginning and end
            FscPresentationUtils::TrimAllAppend(aName, aTitle);
            }

        if (aFormattingFlags
                & MPbk2ContactNameFormatter::EReplaceNonGraphicChars)
            {
            // Non-graphical characters are replaced with space
            FscPresentationUtils::ReplaceNonGraphicCharacters(aTitle,
                    KReplaceChar);
            }
        }

#ifdef _DEBUG
    namespace
        {

        enum TPanic
            {
            EPanic_PreCond_UnnamedText = 1
            };

        void Panic(TPanic aPanicCode)
            {
            _LIT( KPanicCat, "CFscContactNameFormatterBase" );
            User::Panic(KPanicCat, aPanicCode);
            }
        }
#endif // _DEBUG    
    } // namespace


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::CFscContactNameFormatterBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFscContactNameFormatterBase::CFscContactNameFormatterBase(
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        const CPbk2SortOrderManager& aSortOrderManager) :
    iMasterFieldTypeList(aMasterFieldTypeList),
            iSortOrderManager(aSortOrderManager)
    {
    FUNC_LOG;
    iFieldMapper.SetSortOrder(iSortOrderManager.SortOrder() );
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase:BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFscContactNameFormatterBase::BaseConstructL(const TDesC& aUnnamedText,
        CVPbkFieldTypeSelector* aTitleFieldSelector)
    {
    FUNC_LOG;
    iUnnamedText = aUnnamedText.AllocL();
    iTitleFieldSelector = aTitleFieldSelector;
    }

// Destructor
CFscContactNameFormatterBase::~CFscContactNameFormatterBase()
    {
    FUNC_LOG;
    delete iUnnamedText;
    delete iTitleFieldSelector;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::GetContactTitleOrNullL
// -----------------------------------------------------------------------------
//
HBufC* CFscContactNameFormatterBase::GetContactTitleOrNullL(
        const MVPbkBaseContactFieldCollection& aContactFields,
        TUint32 aFormattingFlags)
    {
    FUNC_LOG;
    HBufC* result = HBufC::NewL(MaxTitleLength(aContactFields,
            aFormattingFlags));
    TPtr resultPtr = result->Des();
    GetContactTitle(aContactFields, resultPtr, aFormattingFlags);

    if (resultPtr.Length() == 0)
        {
        delete result;
        result = NULL;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::GetContactTitleL
// -----------------------------------------------------------------------------
//
HBufC* CFscContactNameFormatterBase::GetContactTitleL(
        const MVPbkBaseContactFieldCollection& aContactFields,
        TUint32 aFormattingFlags)
    {
    FUNC_LOG;
    HBufC* result = GetContactTitleOrNullL(aContactFields, aFormattingFlags);
    if (!result)
        {
        result = UnnamedText().AllocL();
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::UnnamedText
// -----------------------------------------------------------------------------
//
const TDesC& CFscContactNameFormatterBase::UnnamedText() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iUnnamedText, Panic( EPanic_PreCond_UnnamedText ) );
    return *iUnnamedText;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::IsTitleField
// -----------------------------------------------------------------------------
//
TBool CFscContactNameFormatterBase::IsTitleField(
        const MVPbkBaseContactField& aContactField) const
    {
    FUNC_LOG;
    const TInt maxMatchPriority = iMasterFieldTypeList.MaxMatchPriority();
    for (TInt matchPriority = 0; matchPriority <= maxMatchPriority; ++matchPriority)
        {
        const MVPbkFieldType* fieldType =
                aContactField.MatchFieldType(matchPriority);
        if (fieldType)
            {
            return IsTitleFieldType(*fieldType);
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::IsTitleFieldType
// -----------------------------------------------------------------------------
//
TBool CFscContactNameFormatterBase::IsTitleFieldType(
        const MVPbkFieldType& aFieldType) const
    {
    FUNC_LOG;
    return iTitleFieldSelector->IsFieldTypeIncluded(aFieldType);
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::SetFieldMapper
// -----------------------------------------------------------------------------
//
void CFscContactNameFormatterBase::SetFieldMapper(
        const MVPbkBaseContactFieldCollection& aContactFields)
    {
    FUNC_LOG;
    iFieldMapper.SetSortOrder(iSortOrderManager.SortOrder() );
    iFieldMapper.SetContactFields(aContactFields);
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::RequiresSeparatorBetweenNames
// -----------------------------------------------------------------------------
//
TBool CFscContactNameFormatterBase::RequiresSeparatorBetweenNames(
        const TDesC& aLhs, const TDesC& aRhs) const
    {
    FUNC_LOG;
    return (aLhs.Length() > 0 && aRhs.Length() > 0);
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::DoGetContactTitle
// -----------------------------------------------------------------------------
//
void CFscContactNameFormatterBase::DoGetContactTitle(TDes& aTitle,
        TUint32 aFormattingFlags, const TInt aStartingPosition,
        const TInt aEndingPosition)
    {
    FUNC_LOG;
    // Find the fields in the sort order that have
    // data to build the title
    const TInt fieldCount = iFieldMapper.FieldCount();
    for (TInt i = aStartingPosition; i <= aEndingPosition && i < fieldCount; ++i)
        {
        TakePartOfName(iFieldMapper.FieldAt(i), aTitle, aFormattingFlags,
                ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::DoCalculateMaxTitleLength
// -----------------------------------------------------------------------------
//
TInt CFscContactNameFormatterBase::DoCalculateMaxTitleLength(
        const TUint32 aFormattingFlags, const TInt aStartingPosition,
        const TInt aEndingPosition)
    {
    FUNC_LOG;
    TInt result = 0;

    // Use all the fields in sort order to
    // build the title (ie. the company name)
    const TInt fieldCount = iFieldMapper.FieldCount();
    for (TInt i = aStartingPosition; i <= aEndingPosition && i < fieldCount; ++i)
        {
        CalculatePartOfName(iFieldMapper.FieldAt(i), result,
                aFormattingFlags, ETrue);
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::DoAppendNonEmptyTitleFieldTypesL
// -----------------------------------------------------------------------------
//
void CFscContactNameFormatterBase::DoAppendNonEmptyTitleFieldTypesL(
        CVPbkFieldTypeRefsList& aFieldTypeList, const TInt aStartingPosition,
        const TInt aEndingPosition)
    {
    FUNC_LOG;
    // Find the fields in the sort order that have
    // data to build the title
    for (TInt i = aStartingPosition; i <= aEndingPosition; ++i)
        {
        const MVPbkBaseContactField* field = iFieldMapper.FieldAt(i);
        if (field && IsTitleField( *field) && !field->FieldData().IsEmpty() )
            {
            // get the field type
            const TInt maxMatchPriority =
                    iMasterFieldTypeList.MaxMatchPriority();
            for (TInt matchPriority = 0; matchPriority <= maxMatchPriority; ++matchPriority)
                {
                const MVPbkFieldType* fieldType =
                        field->MatchFieldType(matchPriority);
                // Do not add duplicates to aFieldTypeList
                if (fieldType && !aFieldTypeList.ContainsSame( *fieldType) )
                    {
                    aFieldTypeList.AppendL( *fieldType);
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::AreTitleFieldsEmpty
// -----------------------------------------------------------------------------
//
TBool CFscContactNameFormatterBase::AreTitleFieldsEmpty(
        const TInt aStartingPosition, const TInt aEndingPosition) const
    {
    FUNC_LOG;
    TBool result = ETrue;

    // Find the fields in the sort order that have
    // data to build the title
    const TInt fieldCount = iFieldMapper.FieldCount();
    for (TInt i = aStartingPosition; i <= aEndingPosition && i < fieldCount; ++i)
        {
        const MVPbkBaseContactField* field = iFieldMapper.FieldAt(i);

        if (field && IsTitleField(*field))
            {
            const MVPbkContactFieldData& fieldData = field->FieldData();
            if (fieldData.DataType() == EVPbkFieldStorageTypeText)
                {
                const TDesC& fieldText = MVPbkContactFieldTextData::Cast(fieldData).Text();

                if (fieldText.Length() > 0)
                    {
                    result = EFalse;
                    break;
                    }
                }
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::TakePartOfName
// Inlined for performance reasons
// -----------------------------------------------------------------------------
//
inline void CFscContactNameFormatterBase::TakePartOfName(
        const MVPbkBaseContactField* aField, TDes& aTitle,
        const TUint32 aFormattingFlags, const TBool aUseSeparator) const
    {
    FUNC_LOG;
    if (aField && IsTitleField(*aField))
        {
        const MVPbkContactFieldData& fieldData = aField->FieldData();
        if (fieldData.DataType() == EVPbkFieldStorageTypeText)
            {
            const TDesC& fieldText = MVPbkContactFieldTextData::Cast(fieldData).Text();

            if (aUseSeparator && RequiresSeparatorBetweenNames(aTitle,
                    fieldText))
                {
                const TDesC& separator = iSortOrderManager.CurrentSeparator();
                if (aFormattingFlags
                        & MPbk2ContactNameFormatter::EUseSeparator)
                    {
                    aTitle.Append(separator);
                    // If the separator is other than space, append a space to it
                    if (separator.Compare(KSpace) != 0)
                        {
                        aTitle.Append(KSpace);
                        }
                    }
                else
                    {
                    // If separator is not used, add space then
                    aTitle.Append(KSpace);
                    }

                }
            TrimNameAppend(aFormattingFlags, fieldText, aTitle);
            }
        }
    }

// -----------------------------------------------------------------------------
// CFscContactNameFormatterBase::CalculatePartOfName
// Inlined for performance reasons
// -----------------------------------------------------------------------------
//
inline void CFscContactNameFormatterBase::CalculatePartOfName(
        const MVPbkBaseContactField* aField, TInt& aLength,
        const TUint32 aFormattingFlags, const TBool aUseSeparator) const
    {
    FUNC_LOG;
    if (aField && IsTitleField(*aField))
        {
        const MVPbkContactFieldData& fieldData = aField->FieldData();
        if (fieldData.DataType() == EVPbkFieldStorageTypeText)
            {
            const TDesC& fieldText = MVPbkContactFieldTextData::Cast(fieldData).Text();

            const TInt length = fieldText.Length();
            aLength += length;

            if (aUseSeparator && length > 0)
                {
                if (aFormattingFlags
                        & MPbk2ContactNameFormatter::EUseSeparator)
                    {
                    aLength += iSortOrderManager.CurrentSeparator().Length();
                    }
                // There is also a space after separator
                aLength += KSpace().Length();
                }
            }
        }
    }

//  End of File

