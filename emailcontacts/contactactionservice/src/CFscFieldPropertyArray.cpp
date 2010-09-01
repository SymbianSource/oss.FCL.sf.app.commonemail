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
* Description:  Field property array for field types.
 *
*/


#include "emailtrace.h"
#include "CFscFieldPropertyArray.h"

// Phonebook 2
#include "FscPresentationUtils.h"
#include "CFscFieldProperty.h"
#include "CFscFieldOrderingManager.h"
#include <FscActionUtils.rsg>
#include "CFscFieldPropertyGroup.h"
#include <Pbk2ContentIdentifiers.hrh>

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkEng.rsg>

// System includes
#include <barsc.h>
#include <barsread.h>
#include <featmgr.h>
#include <eikenv.h>
#include <apgcli.h>
#include <eikappui.h>
#include <eikapp.h>
//<cmail> hardcoded paths removal
#include <data_caging_path_literals.hrh>
#include <bautils.h>
//</cmail>

#include <coemain.h>

// CONSTANTS DECLARATIONS
//<cmail> hard coded path removed
_LIT( KFscRscFileName, "fscactionutils.rsc" );
//</cmail>


// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::CFscFieldPropertyArray
// ---------------------------------------------------------------------------
//
inline CFscFieldPropertyArray::CFscFieldPropertyArray()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::ConstructL
// ---------------------------------------------------------------------------
//
inline void CFscFieldPropertyArray::ConstructL(
        const MVPbkFieldTypeList& aSupportedFieldTypeList)
    {
    FUNC_LOG;

    /*TFileName dllFileName;
    Dll::FileName( dllFileName );
    TParse parse;
    User::LeaveIfError(
            parse.Set( KFscRscFilePath, &dllFileName, NULL ) );
    TFileName resourceFileName( parse.FullName() );*/
    //<cmail> hardcoded removed, BaflUtils::NearestLanguageFile adds drive letter
    TFileName resourceFileName( KDC_RESOURCE_FILES_DIR );
    resourceFileName.Append(KFscRscFileName);
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resourceFileName );
    TInt offset = CCoeEnv::Static()->AddResourceFileL( resourceFileName );    
    //</cmail>
    
    
    // Read field type presentation order information
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, 
    R_FSC_FIELDTYPE_PRESENTATION_ORDERS);

    CFscFieldOrderingManager* orderings =
            CFscFieldOrderingManager::NewL(reader);
    CleanupStack::PopAndDestroy(); // R_FSC_FIELDTYPE_PRESENTATION_ORDERS
    CleanupStack::PushL(orderings);

    // Read field property resource strucutre array
    CCoeEnv::Static()->CreateResourceReaderLC(reader, R_FSC_FIELD_PROPERTIES);
    ReadFieldPropertiesL(reader, aSupportedFieldTypeList, *orderings);

    CCoeEnv::Static()->DeleteResourceFile(offset);

    CleanupStack::PopAndDestroy(2);// orderings, r_fsc_field_properties
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::NewL
// ---------------------------------------------------------------------------
//
 CFscFieldPropertyArray* CFscFieldPropertyArray::NewL
(const MVPbkFieldTypeList& aSupportedFieldTypeList)
    {
    FUNC_LOG;
    CFscFieldPropertyArray* self = new(ELeave) CFscFieldPropertyArray;
    CleanupStack::PushL(self);
    self->ConstructL( aSupportedFieldTypeList);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::NewL
// ---------------------------------------------------------------------------
//
 CFscFieldPropertyArray* CFscFieldPropertyArray::NewL(
        const MVPbkFieldTypeList& aSupportedFieldTypeList,
        TResourceReader& aReader)
    {
    FUNC_LOG;
    CFscFieldPropertyArray* self = new(ELeave) CFscFieldPropertyArray;
    CleanupStack::PushL(self);
    CFscFieldOrderingManager* orderings =
    self->CreateFieldOrderingsLC();
    self->ReadFieldPropertiesL( aReader, aSupportedFieldTypeList, *orderings );
    CleanupStack::PopAndDestroy( orderings );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::~CFscFieldPropertyArray
// ---------------------------------------------------------------------------
//
CFscFieldPropertyArray::~CFscFieldPropertyArray()
    {
    FUNC_LOG;
    iFieldProperties.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::FindProperty
// ---------------------------------------------------------------------------
//
const MPbk2FieldProperty* CFscFieldPropertyArray::FindProperty(
        const MVPbkFieldType& aFieldType) const
    {
    FUNC_LOG;
    const CFscFieldProperty* result= NULL;
    const TInt count = iFieldProperties.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const CFscFieldProperty* property = iFieldProperties[i];
        if (property->FieldType().IsSame(aFieldType))
            {
            result = property;
            break;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::Count
// ---------------------------------------------------------------------------
//
TInt CFscFieldPropertyArray::Count() const
    {
    FUNC_LOG;
    return iFieldProperties.Count();
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::At
// ---------------------------------------------------------------------------
//
const MPbk2FieldProperty& CFscFieldPropertyArray::At(TInt aIndex) const
    {
    FUNC_LOG;
    return *iFieldProperties[aIndex];
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::ReadFieldPropertiesL
// ---------------------------------------------------------------------------
//
void CFscFieldPropertyArray::ReadFieldPropertiesL(TResourceReader& aReader,
        const MVPbkFieldTypeList& aSupportedFieldTypeList,
        CFscFieldOrderingManager& aOrderingManager)
    {
    FUNC_LOG;
    // aSupportedFieldTypeList is a list of the supported field type of the used store
    const TInt count = aReader.ReadInt16();
    for (TInt i = 0; i < count; ++i)
        {
        CFscFieldProperty* property = CFscFieldProperty::NewLC(aReader,
                aSupportedFieldTypeList, aOrderingManager);
        TBool doNotAdd = EFalse;
        if (property->IsSupported())
            {
            if (!FeatureManager::FeatureSupported(KFeatureIdJapanese))
                {
                // Non-Japanese variants use all the other properties but
                // Japanese reading fields
                const MVPbkFieldType* lastNameReading =
                        aSupportedFieldTypeList.Find(
                        R_VPBK_FIELD_TYPE_LASTNAMEREADING);
                const MVPbkFieldType* firstNameReading =
                        aSupportedFieldTypeList.Find(
                        R_VPBK_FIELD_TYPE_FIRSTNAMEREADING);
                const MVPbkFieldType& type = property->FieldType();
                if ( (lastNameReading && lastNameReading->IsSame(type) )
                        || firstNameReading && firstNameReading->IsSame(type) )
                    {
                    doNotAdd = ETrue;
                    }
                }
            }
        else
            {
            doNotAdd = ETrue;
            }

        if (doNotAdd)
            {
            CleanupStack::PopAndDestroy(property);
            }
        else
            {
            iFieldProperties.AppendL(property);
            CleanupStack::Pop(property);
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscFieldPropertyArray::CreateFieldOrderingsLC
// ---------------------------------------------------------------------------
//
CFscFieldOrderingManager* CFscFieldPropertyArray::CreateFieldOrderingsLC(void)
    {
    FUNC_LOG;

    /*TFileName dllFileName;
    Dll::FileName( dllFileName );
    TParse parse;
    User::LeaveIfError(
            parse.Set( GetFileDir, &dllFileName, NULL ) );
    TFileName resourceFileName( parse.FullName() );*/
    //<cmail> hardcoded removed
    TFileName resourceFileName( KDC_RESOURCE_FILES_DIR );
    resourceFileName.Append(KFscRscFileName);
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resourceFileName );
    TInt offset = CCoeEnv::Static()->AddResourceFileL( resourceFileName );    
    //</cmail>
    
    // Read field type presentation order information
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader,
            R_FSC_FIELDTYPE_PRESENTATION_ORDERS);
    CFscFieldOrderingManager* orderings =
            CFscFieldOrderingManager::NewL(reader);

    CCoeEnv::Static()->DeleteResourceFile(offset);

    CleanupStack::PopAndDestroy(2);// R_FSC_FIELDTYPE_PRESENTATION_ORDERS, resFile
    CleanupStack::PushL(orderings);
    return orderings;
    }

// End of File

