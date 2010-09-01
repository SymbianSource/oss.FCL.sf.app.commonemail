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
* Description:  Implementation of field list box model.
 *
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "CFscFieldListBoxModel.h"

// Phonebook 2
#include "MFscClipListBoxText.h"
#include "MFscFieldAnalyzer.h"
#include "CFscFieldFormatter.h"
#include "MFscContactFieldDynamicProperties.h"
#include "CFscContactFieldDynamicProperties.h"
#include <CPbk2IconArray.h>
#include "FscPresentationUtils.h"
#include "CFscPresentationContactField.h"
#include "CFscPresentationContactFieldCollection.h"
#include <MPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include "TFscStoreContactAnalyzer.h"
#include <FscActionUtils.rsg>

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactFieldData.h>
#include <VPbkUtils.h>

/// Unnamed namespace for local definitions
namespace
    {

    _LIT( KCharsToReplace, "\t" );
    _LIT( KReplacementChars, " " );
    const TInt KMaxFormattedTIntLength( 15);
    const TInt KInitialBufferLength( 128);
    const TInt KGranularity( 8);

    } /// namespace


// --------------------------------------------------------------------------
// CFscFieldListBoxRow::CFscFieldListBoxRow
// --------------------------------------------------------------------------
//
inline CFscFieldListBoxRow::CFscFieldListBoxRow()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxRow::~CFscFieldListBoxRow
// --------------------------------------------------------------------------
//
CFscFieldListBoxRow::~CFscFieldListBoxRow()
    {
    FUNC_LOG;
    iColumns.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxRow::NewL
// --------------------------------------------------------------------------
//
CFscFieldListBoxRow* CFscFieldListBoxRow::NewL()
    {
    FUNC_LOG;
    return new( ELeave ) CFscFieldListBoxRow;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxRow::ColumnCount
// --------------------------------------------------------------------------
//
TInt CFscFieldListBoxRow::ColumnCount() const
    {
    FUNC_LOG;
    return iColumns.Count();
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxRow::At
// --------------------------------------------------------------------------
//
TPtrC CFscFieldListBoxRow::At(TInt aColumnIndex) const
    {
    FUNC_LOG;
    return *iColumns[aColumnIndex];
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxRow::AppendColumnL
// --------------------------------------------------------------------------
//
void CFscFieldListBoxRow::AppendColumnL(const TDesC& aColumnText)
    {
    FUNC_LOG;
    HBufC* buf = aColumnText.AllocLC();
    User::LeaveIfError(iColumns.Append(buf) );
    CleanupStack::Pop(buf);
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxRow::TotalLength
// --------------------------------------------------------------------------
//
TInt CFscFieldListBoxRow::TotalLength() const
    {
    FUNC_LOG;
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result += At(i).Length();
        }
    return result;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxRow::MaxColumnLength
// --------------------------------------------------------------------------
//
TInt CFscFieldListBoxRow::MaxColumnLength() const
    {
    FUNC_LOG;
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result = Max(result, At(i).Length());
        }
    return result;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::CFscFieldListBoxModel
// --------------------------------------------------------------------------
//
inline CFscFieldListBoxModel::CFscFieldListBoxModel(TParams& aParams) :
    iRows(KGranularity), iParams(aParams)
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::~CFscFieldListBoxModel
// --------------------------------------------------------------------------
//
CFscFieldListBoxModel::~CFscFieldListBoxModel()
    {
    FUNC_LOG;
    iRows.ResetAndDestroy();
    delete iColumnBuf;
    delete iLineBuf;
    delete iFieldFormatter;
    delete iDynamicProperties;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::NewL
// --------------------------------------------------------------------------
//
CFscFieldListBoxModel* CFscFieldListBoxModel::NewL(TParams& aParams)
    {
    FUNC_LOG;
    CFscFieldListBoxModel* self = new ( ELeave ) CFscFieldListBoxModel( aParams );
    CleanupStack::PushL(self);
    self->ConstructL(aParams);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::ConstructL
// --------------------------------------------------------------------------
//
void CFscFieldListBoxModel::ConstructL(TParams& aParams)
    {
    FUNC_LOG;
    iFieldFormatter = CFscFieldFormatter::NewL(aParams.iFieldProperties,
            aParams.iContactManager.FieldTypes() );
    iFieldFormatter->SetTimeFormatL(aParams.iTimeFormat);
    iLineBuf = HBufC::NewL(KInitialBufferLength);
    iColumnBuf = HBufC::NewL(KInitialBufferLength);
    iDynamicProperties = aParams.iDynamicProperties;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::AppendRowL
// --------------------------------------------------------------------------
//
void CFscFieldListBoxModel::AppendRowL(CFscFieldListBoxRow* aLine)
    {
    FUNC_LOG;
    ExpandBuffersL( *aLine);
    iRows.AppendL(aLine);
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::SetClipper
// --------------------------------------------------------------------------
//
void CFscFieldListBoxModel::SetClipper(MFscClipListBoxText& aTextClipper)
    {
    FUNC_LOG;
    this->iTextClipper = &aTextClipper;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::FormatFieldsL
// --------------------------------------------------------------------------
//
void CFscFieldListBoxModel::FormatFieldsL()
    {
    FUNC_LOG;
    // Format all the fields and add lines to iTextArray
    const TInt fieldCount = iParams.iFields.FieldCount();
    for (TInt i=0; i < fieldCount; ++i)
        {
        FormatFieldL(iParams.iFields.At(i), iParams);
        }
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::MdcaCount
// --------------------------------------------------------------------------
//
TInt CFscFieldListBoxModel::MdcaCount() const
    {
    FUNC_LOG;
    return iRows.Count();
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC16 CFscFieldListBoxModel::MdcaPoint(TInt aIndex) const
    {
    FUNC_LOG;
    const TText KSeparator = '\t';
    TPtr rowText(iLineBuf->Des() );
    rowText.Zero();

    const CFscFieldListBoxRow& row = *iRows[aIndex];
    const TInt columnCount = row.ColumnCount();
    for (TInt columnIndex = 0; columnIndex < columnCount; ++columnIndex)
        {
        TPtr columnText(iColumnBuf->Des() );
        columnText.Copy(row.At(columnIndex) );

        // Clip the column if required
        if (iTextClipper && columnIndex == EContentColumn && aIndex
                < iParams.iFields.FieldCount() )
            {
            TRAP_IGNORE( ClipTextL(iParams.iFields.At(aIndex), aIndex, columnText,
                    columnIndex) );
            }

        // Append the column and separator to the formatted row
        rowText.Append(columnText);
        rowText.Append(KSeparator);
        }

    return rowText;
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::FormatFieldL
// --------------------------------------------------------------------------
//
void CFscFieldListBoxModel::FormatFieldL(
        const CFscPresentationContactField& aField, TParams& aParams)
    {
    FUNC_LOG;
    // Figure out whether the field should be shown
    const MFscFieldAnalyzer* fieldAnalyzer = aParams.iFieldAnalyzer;
    const MFscContactFieldDynamicProperties* dynamicProps =
            iDynamicProperties;

    TBool showField = ETrue;
    if (fieldAnalyzer && fieldAnalyzer->IsHiddenField(aField) || dynamicProps
            && dynamicProps->IsHiddenField(aField) )
        {
        showField = EFalse;
        }

    if (showField)
        {
        CFscFieldListBoxRow* row = CFscFieldListBoxRow::NewL();
        CleanupStack::PushL(row);

        // Get master field type list and match field's type against it
        const MVPbkFieldTypeList* masterFieldTypeList =
                &iParams.iContactManager.FieldTypes();

        const MVPbkFieldType* fieldType = VPbkUtils::MatchFieldType(
                *masterFieldTypeList, aField);

        TPtr columnBuf(iColumnBuf->Des());
        AppendIconsToBeginningL(aField, *fieldType, *row, aParams);
        AppendFieldLabelL(aField, *row, columnBuf);
        AppendFieldContentL(aField, *fieldType, *row, columnBuf);

        // Expand row formatting buffer if required
        ExpandBuffersL(*row);

        // Add the row
        iRows.AppendL(row);
        CleanupStack::Pop(row);
        }
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::ExpandBuffersL
// --------------------------------------------------------------------------
//
void CFscFieldListBoxModel::ExpandBuffersL(const CFscFieldListBoxRow& aRow)
    {
    FUNC_LOG;
    // Row formatting buffer
    const TInt rowLength = aRow.TotalLength() + aRow.ColumnCount(); // for separator characters

    if (rowLength > iLineBuf->Des().MaxLength())
        {
        iLineBuf = iLineBuf->ReAllocL(rowLength);
        }

    ExpandColumnBufferL(aRow.MaxColumnLength() );
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::ExpandColumnBufferL
// --------------------------------------------------------------------------
//
TPtr CFscFieldListBoxModel::ExpandColumnBufferL(TInt aRequiredLength)
    {
    FUNC_LOG;
    if (aRequiredLength > iColumnBuf->Des().MaxLength() )
        {
        iColumnBuf = iColumnBuf->ReAllocL(aRequiredLength);
        }
    return (iColumnBuf->Des() );
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::ClipText
// Clip text from beginning if field is numeric field or e-mail field.
// --------------------------------------------------------------------------
//
inline void CFscFieldListBoxModel::ClipTextL(
        const CFscPresentationContactField& aField, TInt aIndex,
        TPtr& aColumnText, TInt aColumnIndex) const
    {
    FUNC_LOG;
    TFscStoreContactAnalyzer analyzer(iParams.iContactManager, NULL);
    TBool clip = analyzer.IsFieldTypeIncludedL(aField,
            R_FSC_PHONENUMBER_SELECTOR);

    // Clip if required
    if (clip)
        {
        iTextClipper->ClipFromBeginning(aColumnText, aIndex, aColumnIndex);
        }
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::AppendFieldLabelL
// Appends field label. Removes any listbox separator characters.
// --------------------------------------------------------------------------
//
inline void CFscFieldListBoxModel::AppendFieldLabelL(
        const CFscPresentationContactField& aField,
        CFscFieldListBoxRow& aRow, TPtr& aColumnBuf)
    {
    FUNC_LOG;
    aColumnBuf.Set(ExpandColumnBufferL(aField.FieldLabel().Length() ) );
    aColumnBuf.Zero();
    FscPresentationUtils::AppendAndReplaceChars(aColumnBuf,
            aField.FieldLabel(), KCharsToReplace, KReplacementChars);
    // Replace characters that can not be displayed correctly
    FscPresentationUtils::ReplaceNonGraphicCharacters(aColumnBuf, ' ');
    aRow.AppendColumnL(aColumnBuf);
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::AppendFieldContentL
// Appends field content.
// --------------------------------------------------------------------------
//
inline void CFscFieldListBoxModel::AppendFieldContentL(
        const CFscPresentationContactField& aField,
        const MVPbkFieldType& aFieldType, CFscFieldListBoxRow& aRow,
        TPtr& aColumnBuf)
    {
    FUNC_LOG;
    TPtrC fieldText(iFieldFormatter->FormatFieldContentL(aField, aFieldType) );
    aColumnBuf.Set(ExpandColumnBufferL(fieldText.Length() ) );
    aColumnBuf.Zero();
    FscPresentationUtils::AppendAndReplaceChars(aColumnBuf, fieldText,
            KCharsToReplace, KReplacementChars);
    // Replace characters that can not be displayed correctly
    FscPresentationUtils::ReplaceNonGraphicCharacters(aColumnBuf, ' ');
    aRow.AppendColumnL(aColumnBuf);
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::AppendIconsToBeginningL
// Adds icons to the beginning.
// --------------------------------------------------------------------------
//
inline void CFscFieldListBoxModel::AppendIconsToBeginningL(
        const CFscPresentationContactField& /*aField*/,
        const MVPbkFieldType& aFieldType, CFscFieldListBoxRow& aRow,
        TParams& aParams)
    {
    FUNC_LOG;
    TBuf<KMaxFormattedTIntLength> iconText;

    // Format icon index
    TInt iconIndex = aParams.iIconArray.FindIcon(iParams.iDefaultIconId);
    const MPbk2FieldProperty* property =
            aParams.iFieldProperties.FindProperty(aFieldType);
    if (property)
        {
        iconIndex = aParams.iIconArray.FindIcon(property->IconId() );
        }

    iconText.Num(iconIndex);
    aRow.AppendColumnL(iconText);
    }

// --------------------------------------------------------------------------
// CFscFieldListBoxModel::AppendIconsToEndL
// Adds additional icons to the end.
// --------------------------------------------------------------------------
//
inline void CFscFieldListBoxModel::AppendIconsToEndL(
        const CFscPresentationContactField& aField,
        CFscFieldListBoxRow& aRow, TParams& aParams)
    {
    FUNC_LOG;
    if (aParams.iFieldAnalyzer)
        {
        // Check and add speed dial icon
        if (aParams.iFieldAnalyzer->HasSpeedDialL(aField.StoreField() ) )
            {
            TBuf<KMaxFormattedTIntLength> iconText;
            iconText.Num(aParams.iIconArray.FindIcon(TFscAppIconId(EPbk2qgn_indi_qdial_add) ) );
            aRow.AppendColumnL(iconText);
            }
        }
    }

//  End of File

