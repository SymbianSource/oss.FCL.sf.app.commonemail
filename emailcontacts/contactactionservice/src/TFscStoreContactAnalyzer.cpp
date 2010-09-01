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
* Description:  Implementation of store contact analyzer.
*
*/


#include "emailtrace.h"
#include "TFscStoreContactAnalyzer.h"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkUtils.h>

// System includes
#include <barsread.h>
#include <coemain.h>

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::TFscStoreContactAnalyzer
// --------------------------------------------------------------------------
//
 TFscStoreContactAnalyzer::TFscStoreContactAnalyzer
        ( const CVPbkContactManager& aContactManager,
        const MVPbkStoreContact* aContact ) :
            iContactManager( aContactManager ),
            iContact( aContact )
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::HasFieldL
// --------------------------------------------------------------------------
//
 TInt TFscStoreContactAnalyzer::HasFieldL
        ( TInt aResId, TInt aStartIndex/* = 0*/,
        const MVPbkStoreContact* aContact/* = NULL*/ ) const
    {
    FUNC_LOG;
    TInt ret = KErrNotFound;
    const MVPbkStoreContact* tempContact = iContact;

    if ( aContact )
        {
        tempContact = aContact;
        }

    // Check that there is contact to analyze
    if ( tempContact )
        {
        // Read the contact field selector from resource
        MVPbkContactFieldSelector* fieldTypeSelector =
            static_cast<MVPbkContactFieldSelector*>
                ( CreateFieldTypeSelectorLC( aResId ) );

        // Check if the specified field is included to contact
        const MVPbkStoreContactFieldCollection& fields =
            tempContact->Fields();
        ret = IsFieldIncluded( fields, aStartIndex, *fieldTypeSelector );
        CleanupStack::PopAndDestroy(); // fieldTypeSelector;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::IsFieldTypeSupportedL
// --------------------------------------------------------------------------
//
 TBool TFscStoreContactAnalyzer::IsFieldTypeSupportedL
        ( TInt aResId, const MVPbkStoreContact* aContact /*= NULL*/ ) const
    {
    FUNC_LOG;
    TBool ret = EFalse;
    const MVPbkStoreContact* tempContact = iContact;

    if (aContact)
        {
        tempContact = aContact;
        }

    // Check that there is contact to analyze
    if ( tempContact )
        {
        // Read the field type selector from resource
        MVPbkFieldTypeSelector* fieldTypeSelector =
            static_cast<MVPbkFieldTypeSelector*>
                ( CreateFieldTypeSelectorLC( aResId ) );

        // Check if the specified field type is supported by contact
        const MVPbkFieldTypeList& fieldTypes =
            tempContact->ParentStore().StoreProperties().SupportedFields();
        ret = IsFieldTypeIncluded( fieldTypes, *fieldTypeSelector );
        CleanupStack::PopAndDestroy(); // fieldTypeSelector;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::IsFieldTypeIncludedL
// --------------------------------------------------------------------------
//
 TBool TFscStoreContactAnalyzer::IsFieldTypeIncludedL
        ( const MVPbkFieldType& aFieldType, TInt aResId ) const
    {
    FUNC_LOG;
    CVPbkFieldTypeSelector* selector = CreateFieldTypeSelectorLC( aResId );
    TBool ret = selector->IsFieldTypeIncluded( aFieldType );
    CleanupStack::PopAndDestroy( selector );
    return ret;
    }

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::IsFieldTypeIncludedL
// --------------------------------------------------------------------------
//
 TBool TFscStoreContactAnalyzer::IsFieldTypeIncludedL
        ( const MVPbkBaseContactField& aField, TInt aResId ) const
    {
    FUNC_LOG;
    // Get the field type
    const MVPbkFieldType* fieldType =
        VPbkUtils::MatchFieldType( iContactManager.FieldTypes(), aField );
    CVPbkFieldTypeSelector* selector = CreateFieldTypeSelectorLC( aResId );
    TBool ret = selector->IsFieldTypeIncluded( *fieldType );
    CleanupStack::PopAndDestroy( selector );
    return ret;
    }

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::IsFieldIncluded
// --------------------------------------------------------------------------
//
TInt TFscStoreContactAnalyzer::IsFieldIncluded
        (const MVPbkBaseContactFieldCollection& aFields,
        TInt aStartIndex,
        const MVPbkContactFieldSelector& aFieldTypeSelector) const
    {
    FUNC_LOG;
    TInt ret( KErrNotFound );
    TInt fieldCount( aFields.FieldCount() );

    for ( TInt i = aStartIndex; i < fieldCount && ret == KErrNotFound; ++i )
        {
        const MVPbkBaseContactField& field = aFields.FieldAt( i );
        if ( aFieldTypeSelector.IsFieldIncluded( field ) )
            {
            ret = i;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::IsFieldTypeIncluded
// --------------------------------------------------------------------------
//
TBool TFscStoreContactAnalyzer::IsFieldTypeIncluded
        ( const MVPbkFieldTypeList& aFieldTypes,
        const MVPbkFieldTypeSelector& aFieldTypeSelector ) const
    {
    FUNC_LOG;
    TBool ret( EFalse );
    TInt fieldTypeCount( aFieldTypes.FieldTypeCount() );

    for ( TInt i = 0; i < fieldTypeCount && !ret; ++i )
        {
        const MVPbkFieldType& fieldType = aFieldTypes.FieldTypeAt( i );
        if ( aFieldTypeSelector.IsFieldTypeIncluded( fieldType ) )
            {
            ret = ETrue;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// TFscStoreContactAnalyzer::CreateFieldTypeSelectorLC
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector* TFscStoreContactAnalyzer::CreateFieldTypeSelectorLC
        ( TInt aResId ) const
    {
    FUNC_LOG;
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC( resReader, aResId );

    CVPbkFieldTypeSelector* fieldTypeSelector = CVPbkFieldTypeSelector::NewL
        ( resReader, iContactManager.FieldTypes() );

    CleanupStack::PopAndDestroy(); // resReader
    CleanupStack::PushL( fieldTypeSelector );
    return fieldTypeSelector;
    }

// End of File

