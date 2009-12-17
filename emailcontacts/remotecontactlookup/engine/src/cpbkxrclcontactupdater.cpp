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
* Description:  Implementation of the class CPbkxRclContactUpdater.
*
*/


#include "emailtrace.h"
#include <pbkxrclengine.rsg>
#include <CPbkSingleEntryFetchDlg.h>
#include <CPbkContactEngine.h>
#include <CPbkMemoryEntryAddItemDlg.h>
#include <StringLoader.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldsInfo.h>
#include <aknnotedialog.h>
#include <pbkxrclengine.rsg>
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

#include "cpbkxrclcontactupdater.h"
#include "pbkxrclutils.h"
#include "pbkxrclengineconstants.h"
#include "pbkxremotecontactlookuppanic.h"

// Local constants
_LIT(KPbkRomFileDrive, "z:");
_LIT(KPbkViewResourceFile, "PbkView.rsc");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclContactUpdater* CPbkxRclContactUpdater::NewL(
    CPbkContactEngine& aEngine )
    {
    FUNC_LOG;
    CPbkxRclContactUpdater* updater = CPbkxRclContactUpdater::NewLC( aEngine );
    CleanupStack::Pop( updater );
    return updater;
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::NewLC
// ---------------------------------------------------------------------------
//
CPbkxRclContactUpdater* CPbkxRclContactUpdater::NewLC(
    CPbkContactEngine& aEngine )
    {
    FUNC_LOG;
    CPbkxRclContactUpdater* updater = 
        new ( ELeave ) CPbkxRclContactUpdater( aEngine );
    CleanupStack::PushL( updater );
    updater->ConstructL();
    return updater;
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::CPbkxRclContactUpdater
// ---------------------------------------------------------------------------
//
CPbkxRclContactUpdater::CPbkxRclContactUpdater( CPbkContactEngine& aEngine ) :
    CBase(), iEngine( aEngine )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::~CPbkxRclContactUpdater
// ---------------------------------------------------------------------------
//
CPbkxRclContactUpdater::~CPbkxRclContactUpdater()
    {
    FUNC_LOG;
    CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclContactUpdater::ConstructL()
    {
    FUNC_LOG;

    // First load Phonebook resource file so that dialog will find resources
    CCoeEnv* coeEnv = CCoeEnv::Static();
    TFileName resourceFile;
    resourceFile.Copy( KPbkRomFileDrive );
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KPbkViewResourceFile );    
    
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    
    TRAPD( status,
            iResourceFileOffset = coeEnv->AddResourceFileL( resourceFile ) );
    
    if( status != KErrNone )
        {
        PbkxRclPanic( EPbkxRclPanicGeneral );       
        }
    else 
        {
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::UpdateContactL
// ---------------------------------------------------------------------------
//
HBufC* CPbkxRclContactUpdater::UpdateContactL( TPbkContactItemField& aField )
    {
    FUNC_LOG;

    // first ask user to select the contact in which the field is added
    CPbkSingleEntryFetchDlg::TParams params;
    params.iContactView = &iEngine.AllContactsView();

    CPbkSingleEntryFetchDlg* fetchDlg = CPbkSingleEntryFetchDlg::NewL( params );
    fetchDlg->ResetWhenDestroyed( &fetchDlg );
    TInt res = 0;
    res = fetchDlg->ExecuteLD();

    CPbkContactItem* contactItem = NULL;
    HBufC* text = NULL;
    if ( res != 0 && params.iSelectedEntry != KNullContactId )
        {
        contactItem = iEngine.OpenContactL( params.iSelectedEntry );
        CleanupStack::PushL( contactItem );
        
        CPbkFieldInfo& fieldInfo = aField.FieldInfo();
        TPbkContactItemField* field = NULL;
        if ( fieldInfo.Multiplicity() == EPbkFieldMultiplicityOne )
            {
            // there can be only one field of this type. query user to select
            // a field in which data is to be saved
            field = SelectUpdatedFieldL( *contactItem, fieldInfo );
            }
        else
            {
            // add new field to contact
            field = &( contactItem->AddFieldL( aField.FieldInfo() ) );
            }
        
        if ( field != NULL )
            {
            if ( field->StorageType() == KStorageTypeText )
                {
                field->TextStorage()->SetTextL( aField.Text() );
                }
            else if ( field->StorageType() == KStorageTypeDateTime )
                {
                field->DateTimeStorage()->SetTime( aField.Time() );
                }
            iEngine.CommitContactL( *contactItem );
            // build string returned to the caller
            text = ConstructUpdatedTextL( *contactItem, *field );
            }
        else
            {
            iEngine.CloseContactL( contactItem->Id() );
            }
        
        CleanupStack::PopAndDestroy( contactItem );
        }
    return text;
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::SelectUpdatedFieldL
// ---------------------------------------------------------------------------
//
TPbkContactItemField* CPbkxRclContactUpdater::SelectUpdatedFieldL(
    CPbkContactItem& aContactItem,
    CPbkFieldInfo& aFieldInfo )
    {
    FUNC_LOG;
    TPbkContactItemField* retField = NULL;
    TInt fieldCount = 0;
    CPbkFieldInfo* info = SelectFieldL( aFieldInfo, fieldCount );
    while ( info != NULL && retField == NULL )
        {
        retField = aContactItem.AddOrReturnUnusedFieldL( *info );
        // iterate until user selects field or cancels
        if ( retField == NULL )
            {
            // there is already given field. ask user if it is ok to replace
            // existing data
            HBufC* dlgText = StringLoader::LoadLC( 
                R_QTN_RCL_REPLACE_EXISTING_DETAIL_NOTE,
                info->FieldName() );
            CAknNoteDialog* dialog = new ( ELeave ) CAknNoteDialog();
            dialog->PrepareLC( R_RCL_CONFIRMATION_NOTE_YES_CANCEL );
            dialog->SetTextL( *dlgText );
            
            if ( dialog->RunLD() )
                {
                retField = aContactItem.FindField( *info );
                }
            else
                {
                // user cancelled dialog
                if ( fieldCount > 1 )
                    {
                    // show select field dialog again
                    info = SelectFieldL( aFieldInfo, fieldCount );
                    }
                else
                    {
                    // exit the loop because user refused to update existing 
                    // field and there are no other choices for the fields
                    // to be updated
                    info = NULL;
                    }
                }
            CleanupStack::PopAndDestroy( dlgText );
            }
        }
    return retField;
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::ConstructUpdatedTextL
// ---------------------------------------------------------------------------
//
HBufC* CPbkxRclContactUpdater::ConstructUpdatedTextL(
    CPbkContactItem& aContactItem,
    TPbkContactItemField& aField )
    {
    FUNC_LOG;

    HBufC* firstName = PbkxRclUtils::FieldTextL( 
        &aContactItem, 
        EPbkFieldIdFirstName );
    CleanupStack::PushL( firstName );
    
    HBufC* lastName = PbkxRclUtils::FieldTextL( 
        &aContactItem, 
        EPbkFieldIdLastName );
    CleanupStack::PushL( lastName );
    
    RBuf name;
    CleanupClosePushL( name );
    // +1 comes for the space in the format
    name.CreateL( firstName->Length() + lastName->Length() + 1 );
    
    name.Format( KNameFormat, firstName, lastName );
           
    CDesCArrayFlat* textArray = 
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( textArray );
    textArray->AppendL( aField.FieldInfo().FieldName() );
    textArray->AppendL( name );
    
    HBufC* text = StringLoader::LoadL( 
        R_QTN_RCL_UPDATE_NOTE,
        *textArray );
    
    CleanupStack::PopAndDestroy( textArray );
    CleanupStack::PopAndDestroy( &name );
    CleanupStack::PopAndDestroy( lastName );
    CleanupStack::PopAndDestroy( firstName );
    
    return text;
    }

// ---------------------------------------------------------------------------
// CPbkxRclContactUpdater::SelectFieldL
// ---------------------------------------------------------------------------
//
CPbkFieldInfo* CPbkxRclContactUpdater::SelectFieldL(
    CPbkFieldInfo& aFieldInfo,
    TInt& aFieldCount )
    {
    FUNC_LOG;
    // construct possible field types
    const CPbkFieldsInfo& fields = iEngine.FieldsInfo();
    CArrayPtrFlat<CPbkFieldInfo>* fieldArray = 
        new ( ELeave ) CArrayPtrFlat<CPbkFieldInfo>( KArrayGranularity );
    CleanupStack::PushL( fieldArray );

    TPbkFieldLocation locs[] = { 
        EPbkFieldLocationNone, 
        EPbkFieldLocationHome,
        EPbkFieldLocationWork };

    TInt locCount = sizeof( locs ) / sizeof( TPbkFieldLocation );

    for ( TInt i = 0; i < locCount; i++ )
        {
        CPbkFieldInfo* field = fields.Find( aFieldInfo.FieldId(), locs[i] );
        if ( field != NULL )
            {
            fieldArray->AppendL( field );
            }
        }
    
    aFieldCount = fieldArray->Count();
    CPbkFieldInfo* retField = NULL;
    if ( fieldArray->Count() == 1 )
        {
        retField = ( *fieldArray )[0];
        }
    else
        {
        CPbkItemTypeSelectAddToExisting* selectionDlg = 
            new ( ELeave ) CPbkItemTypeSelectAddToExisting();
        retField = selectionDlg->ExecuteLD( *fieldArray );
        }
    CleanupStack::PopAndDestroy( fieldArray );
    return retField;
    }

