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
* Description:  Implementation of the class CFscActionUtils.
 *
*/


// INCLUDES
#include "emailtrace.h"
#include <e32std.h>
#include <e32property.h>
#include <StringLoader.h>
#include <commonphoneparser.h>
#include <featmgr.h>
#include <aknnotewrappers.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <eikapp.h>
#include <apgcli.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h> 
#include <settingsinternalcrkeys.h>
//<cmail> Header neither in Cmail nor in the platform
//#include <crcseprofileregistry.h>
//</cmail>
#include <NetworkHandlingDomainPSKeys.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
//<cmail>
#include "fscactionplugincrkeys.h"
#include "fsccontactactionservicedefines.h"
//</cmail>
#include <FscActionUtils.rsg>
#include <barsread.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2SortOrderManager.h>
#include <CVPbkContactManager.h>
#include <MPbk2FieldProperty.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldFilter.h>
#include <commonphoneparser.h>
#include <bautils.h> // Fix for EMZG-7M23KX
//<cmail> hardcoded paths removal
#include <data_caging_path_literals.hrh>
//</cmail>

#include "cfscactionutils.h"
#include "cfsccontactaction.h"
#include "mfscreasoncallback.h"
#include <VPbkFieldType.hrh>
#include "FscPresentationUtils.h"
#include "CFscFieldPropertyArray.h"
#include "TFscAddressSelectParams.h"
#include "CFscAddressSelect.h"


// CONSTANTS DECLARATIONS
const TUid KPoCOmaServerUid = 
	{ 
	0x102071C4 
	};

// Unnamed namespace
namespace {
// These values come from cfsccallpluginimpl.cpp 
const TInt KMaxConfIdLen = 32;
const TInt KMaxConfPinLen = 32; 
}

const TUint32 KPoCDefaultSettings = 0x199;

const TInt KMaxLengthOfNumber = 100;

//Fix for: ECWG-7QYAVS
const TInt KMaxLengthOfName = 100;
//end for fix

_LIT( KSpace, " " );
//<cmail> hard coded path removed
_LIT( KFscRscFileName, "fscactionutils.rsc" );
//</cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscActionUtils::NewL
// ---------------------------------------------------------------------------
//
CFscActionUtils* CFscActionUtils::NewL( CVPbkContactManager& aContactManager )
    {
    FUNC_LOG;
    CFscActionUtils* self = CFscActionUtils::NewLC( aContactManager );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::NewLC
// ---------------------------------------------------------------------------
//
CFscActionUtils* CFscActionUtils::NewLC( 
    CVPbkContactManager& aContactManager )
    {
    FUNC_LOG;
    CFscActionUtils* self = new ( ELeave ) CFscActionUtils( aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::~CFscActionUtils
// ---------------------------------------------------------------------------
//
CFscActionUtils::~CFscActionUtils()
    {
    FUNC_LOG;
    delete iRep;
    if ( iRCSEProfileRegistry != NULL )
        {
        //<cmail>
        //delete iRCSEProfileRegistry;
        //</cmail>
        }
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::ContactManager
// ---------------------------------------------------------------------------
//  
CVPbkContactManager& CFscActionUtils::ContactManager()
    {
    FUNC_LOG;
    return iContactManager;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::ShowInfoNoteL
// ---------------------------------------------------------------------------
//
void CFscActionUtils::ShowInfoNoteL( TInt aNote, const HBufC* aContactName )
    {
    FUNC_LOG;
    HBufC* noteText = NULL;
    
    // Fix for EMZG-7M23KX
    TInt offset = FindAndAddResourceFileL();
    
    if ( aContactName )
        {
        noteText = StringLoader::LoadL( aNote, *aContactName );
        }
    else
        {
        noteText = StringLoader::LoadL( aNote );
        }
    
    CleanupStack::PushL( noteText );
    CAknInformationNote* infoNote = 
        new ( ELeave ) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy( noteText );
    CCoeEnv::Static()->DeleteResourceFile( offset );
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::ShowInfoNoteL
// ---------------------------------------------------------------------------
//
void CFscActionUtils::ShowInfoNoteL( TInt aNote, TInt aCounter )
    {
    FUNC_LOG;
    HBufC* noteText = NULL;
    
    // Fix for EMZG-7M23KX
    TInt offset = FindAndAddResourceFileL();
    
    noteText = StringLoader::LoadL( aNote, aCounter );
  
    CleanupStack::PushL( noteText );
    CAknInformationNote* infoNote = 
        new ( ELeave ) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy( noteText );
    CCoeEnv::Static()->DeleteResourceFile( offset );
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::ShowCnfNoteL
// ---------------------------------------------------------------------------
//
void CFscActionUtils::ShowCnfNoteL( TInt aNote, const HBufC* aName )
    {
    FUNC_LOG;
    
    // Fix for EMZG-7M23KX
    TInt offset = FindAndAddResourceFileL();
    
    HBufC* noteText = NULL;
    
    if ( aName )
        {
        noteText = StringLoader::LoadL( aNote, *aName );
        }
    else
        {
        noteText = StringLoader::LoadL( aNote );
        }
    
    CleanupStack::PushL( noteText );
    CAknConfirmationNote* cnfNote = 
        new ( ELeave ) CAknConfirmationNote( ETrue );
    cnfNote->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy( noteText );
    
    CCoeEnv::Static()->DeleteResourceFile( offset );
    
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::ShowNumberMissingDialogL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::ShowNumberMissingNoteL( TInt aMissingCount, 
    TInt aTotalCount, TInt aNote, TInt aDialog )
    {
    FUNC_LOG;
    
    // Add two numbers aMissingCount and aTotalCount to queryText
    CArrayFix<TInt>* numbers = new( ELeave ) CArrayFixFlat<TInt>( 2 );
    CleanupStack::PushL( numbers );
    numbers->AppendL( aMissingCount );
    numbers->AppendL( aTotalCount );

    HBufC* queryText = StringLoader::LoadLC( aNote, *numbers );

    CAknQueryDialog *queryDlg = new ( ELeave ) CAknQueryDialog();
    CleanupStack::PushL( queryDlg );
    queryDlg->SetPromptL( *queryText );
    CleanupStack::Pop( queryDlg );
    CleanupStack::PopAndDestroy( queryText );
    CleanupStack::PopAndDestroy( numbers );

    TInt ret = queryDlg->ExecuteLD( aDialog );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetContactNameL
// ---------------------------------------------------------------------------
HBufC* CFscActionUtils::GetContactNameL( MVPbkStoreContact& aStoreContact )
    {
    FUNC_LOG;
    HBufC* contactName = HBufC::NewLC( KMaxLengthOfName + 1 ); // one ' ' char
    
    // Search for the contact name
    // Fix for EMZG-7M23KX
    TInt offset = FindAndAddResourceFileL();
    
    TResourceReader selectorReader;
    CCoeEnv::Static()->CreateResourceReaderLC( selectorReader,
            R_FSC_FIRST_NAME_SELECTOR );

    CVPbkFieldTypeSelector* fieldTypeSelector = CVPbkFieldTypeSelector::NewL(
            selectorReader, iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy(); // selectorReader

    CVPbkFieldFilter::TConfig config(
            const_cast<MVPbkStoreContactFieldCollection&> (
                    aStoreContact.Fields() ), fieldTypeSelector, NULL );

    CVPbkFieldFilter* fieldFilter = CVPbkFieldFilter::NewL( config );
    TInt fieldCount = fieldFilter->FieldCount();
    const MVPbkStoreContactField* field = NULL;
    if ( fieldCount == 1 )
        {
        field = fieldFilter->FieldAtLC(0);
        contactName->Des().Append(
                MVPbkContactFieldTextData::Cast( 
                        field->FieldData() ).Text() );
        CleanupStack::PopAndDestroy();//field
        }

    delete fieldFilter;
    delete fieldTypeSelector;

    // Search for the contact second name
    CCoeEnv::Static()->CreateResourceReaderLC( selectorReader,
            R_FSC_LAST_NAME_SELECTOR );
    fieldTypeSelector = CVPbkFieldTypeSelector::NewL( selectorReader,
            iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy();//selectorReader
    config.iFieldSelector = fieldTypeSelector;

    fieldFilter = CVPbkFieldFilter::NewL( config );
    fieldCount = fieldFilter->FieldCount();
    if ( fieldCount == 1 )
        {
        field = fieldFilter->FieldAtLC(0);
        if ( contactName->Des().Length() > 0 )
            {
            contactName->Des().Append( KSpace );
            }
        contactName->Des().Append(
                MVPbkContactFieldTextData::Cast( 
                        field->FieldData() ).Text() );
        CleanupStack::PopAndDestroy();//field
        }

    // If first or second name hasn't been found -> find company name
    if ( contactName->Des().Length() <= 0 )
        {
        delete fieldFilter;
        delete fieldTypeSelector;

        CCoeEnv::Static()->CreateResourceReaderLC( selectorReader,
                R_FSC_COMPANY_NAME_SELECTOR );
        fieldTypeSelector = CVPbkFieldTypeSelector::NewL( selectorReader,
                iContactManager.FieldTypes() );
        CleanupStack::PopAndDestroy();//selectorReader
        config.iFieldSelector = fieldTypeSelector;

        fieldFilter = CVPbkFieldFilter::NewL( config );
        fieldCount = fieldFilter->FieldCount();
        if ( fieldCount == 1 )
            {
            field = fieldFilter->FieldAtLC(0);
            contactName->Des().Append(
                    MVPbkContactFieldTextData::Cast(
                            field->FieldData() ).Text() );
            CleanupStack::PopAndDestroy();//field
            }
        }
    // no first or second name, nor company name -> return "unnamed" string
    if ( contactName->Des().Length() <= 0 )
        {
        //append (Unnamed)
        HBufC* unnamed = StringLoader::LoadL( R_QTN_FS_UNNAMED );
        contactName->Des().Append( *unnamed );
        delete unnamed;
        }

    delete fieldFilter;
    delete fieldTypeSelector;
    CleanupStack::Pop( contactName );

    CCoeEnv::Static()->DeleteResourceFile( offset );

    return contactName;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::OpenSelectDialogL
// ---------------------------------------------------------------------------
// 
MVPbkStoreContactField* CFscActionUtils::OpenSelectDialogL( TInt aResourceId,
    MVPbkStoreContact& aStoreContact )
    {
    FUNC_LOG;
    // Create sort order manager
    CPbk2SortOrderManager* sortOrderManager =
            CPbk2SortOrderManager::NewL( iContactManager.FieldTypes() );
    CleanupStack::PushL( sortOrderManager );

    // Create name formatter
    MPbk2ContactNameFormatter* nameFormatter =
            Pbk2ContactNameFormatterFactory::CreateL(
                    iContactManager.FieldTypes(), *sortOrderManager );

    CFscFieldPropertyArray* fieldProperties =
            CFscFieldPropertyArray::NewL( iContactManager.FieldTypes() );

    // Fix for EMZG-7M23KX
    TInt offset = FindAndAddResourceFileL();

    // Create resource reader for new resource
    TResourceReader resReader;
    
    CCoeEnv::Static()->CreateResourceReaderLC( resReader, 
            aResourceId );

    TFscAddressSelectParams params( aStoreContact, iContactManager,
            *nameFormatter, *fieldProperties, resReader );

    RVPbkContactFieldDefaultPriorities defaultPrioritiesArray;
    
    switch ( aResourceId )
        {
        case R_FSC_PHONE_NUMBER_SELECT:
            {
            defaultPrioritiesArray.Append( EVPbkDefaultTypePhoneNumber );
            // Launch call directly using default values.
            params.SetUseDefaultDirectly( ETrue );
            break;
            }
            
        case R_FSC_VIDEO_NUMBER_SELECT:
            {
            defaultPrioritiesArray.Append( EVPbkDefaultTypeVideoNumber );
            // Launch call directly using default values.
            params.SetUseDefaultDirectly( ETrue );
            break;
            }
            
        case R_FSC_VOIP_SELECT:
            {
            defaultPrioritiesArray.Append( EVPbkDefaultTypeVoIP );
            // Launch call directly using default values.
            params.SetUseDefaultDirectly( ETrue );
            break;
            }
            
        case R_FSC_PTT_SELECT:
            {
            defaultPrioritiesArray.Append( EVPbkDefaultTypePOC );
            // Launch call directly using default values.
            params.SetUseDefaultDirectly( ETrue );
            break;
            }
            
        case R_FSC_MSG_SELECT:
            {
            defaultPrioritiesArray.Append( EVPbkDefaultTypeSms );
            defaultPrioritiesArray.Append( EVPbkDefaultTypeMms );
            // Fix for ELWG-7SPH5H 
            params.SetUseDefaultDirectly( ETrue );
            break;
            }
            
        case R_FSC_EMAIL_SELECT:
            {
            defaultPrioritiesArray.Append( EVPbkDefaultTypeEmail );
            params.SetUseDefaultDirectly( ETrue );
            break;
            }
            
        default:
            {
            User::Leave( KErrArgument );
            break;
            }
        }

    params.SetDefaultPriorities( defaultPrioritiesArray );

    // Create select dialog
    delete iAddressSelect;
    iAddressSelect = NULL;
    iAddressSelect = CFscAddressSelect::NewL( params );
    // Run select dialog
    MVPbkStoreContactField* selectedField = iAddressSelect->ExecuteLD();
    iAddressSelect = NULL;
    
    defaultPrioritiesArray.Close();
    CleanupStack::PopAndDestroy(); //resReader
    CCoeEnv::Static()->DeleteResourceFile( offset );
    delete fieldProperties;
    delete nameFormatter;
    CleanupStack::PopAndDestroy( sortOrderManager );
    
    return selectedField;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetVoiceCallNumberL
// ---------------------------------------------------------------------------
// 
TInt CFscActionUtils::GetVoiceCallNumberL( MVPbkStoreContact& aStoreContact,
    TDes& aNumber )
    {
    FUNC_LOG;
    aNumber.Zero();

    if( !IsContactNumberAvailableL( aStoreContact, ETypeVoiceCallNumber ) )
        {
        return KErrNotFound;
        }
    
    TInt ret = KErrNone;
    
    // open select dialog for voice call number 
    MVPbkStoreContactField* selectedField = OpenSelectDialogL(
            R_FSC_PHONE_NUMBER_SELECT, aStoreContact );

    // If select number dialog was cancelled, we will return KErrCancel
    if ( !selectedField )
        {
        ret = KErrCancel;
        }
    else
        {
        // Else, copy phone number to the aNubmer argument
        TPtrC phoneNumberFieldData = MVPbkContactFieldTextData::Cast(
                    selectedField->FieldData() ).Text();

        // Copy number from selected field
        if ( phoneNumberFieldData.Length() )
            {
            aNumber.Copy( phoneNumberFieldData );
            }

        // Format phone number
        //Fix for: MHOA-7SQF6T
        CommonPhoneParser::ParsePhoneNumber( aNumber,
                            CommonPhoneParser::EPlainPhoneNumber );
        //end for fix
        }

    // Destroy objects
    delete selectedField;

    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetVideoCallNumberL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::GetVideoCallNumberL( MVPbkStoreContact& aStoreContact,
    TDes& aNumber )
    {
    FUNC_LOG;
    
    if( !IsContactNumberAvailableL( aStoreContact, ETypeVideoCallNumber ) )
        {
        return KErrNotFound;
        }
    
    aNumber.Zero();
    TInt ret = KErrNone;
    
    // open select dialog for video call number
    MVPbkStoreContactField* selectedField = OpenSelectDialogL(
            R_FSC_VIDEO_NUMBER_SELECT, aStoreContact );
    
    // If select number dialog was cancelled, we will return KErrCancel
    if ( !selectedField )
        {
        ret = KErrCancel;
        }
    else
        {
        // Else, copy phone number to the aNubmer argument
        TPtrC phoneNumberFieldData = MVPbkContactFieldTextData::Cast(
                selectedField->FieldData() ).Text();

        // Copy number from selected field
        if ( phoneNumberFieldData.Length() )
            {
            aNumber.Copy( phoneNumberFieldData );
            }

        // Format phone number
        //Fix for: MHOA-7SQF6T
        CommonPhoneParser::ParsePhoneNumber( aNumber,
                            CommonPhoneParser::EPlainPhoneNumber );
        //end for fix
        }

    // Destroy objects
    delete selectedField;

    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetVoipAddressL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::GetVoipAddressL( MVPbkStoreContact& aStoreContact,
    TDes& aVoipAddress )
    {
    FUNC_LOG;
    
    if( !IsContactNumberAvailableL( aStoreContact, ETypeInternetTel ) )
        {
        return KErrNotFound;
        }
    
    aVoipAddress.Zero();
    TInt ret = KErrNone;
    
    // open select dialog for voip address 
    MVPbkStoreContactField* selectedField = OpenSelectDialogL(
            R_FSC_VOIP_SELECT, aStoreContact );

    // If select VOIP address dialog was cancelled, we will return KErrCancel
    if ( !selectedField )
        {
        ret = KErrCancel;
        }
    else
        {
        // Copy VOIP address from selected field
        TPtrC voipAddress = MVPbkContactFieldTextData::Cast(
                selectedField->FieldData() ).Text();
        
        if ( voipAddress.Length() )
            {
            aVoipAddress.Copy( voipAddress );
            }
        else
            {
            ret = KErrNotFound;
            }
        }

    // Format phone number
    //Fix for: MHOA-7SQF6T
    CommonPhoneParser::ParsePhoneNumber( aVoipAddress,
                        CommonPhoneParser::EPlainPhoneNumber );
    //end for fix
    
    // Destroy objects
    delete selectedField;

    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetConfInfoL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::GetConfInfoL( MVPbkStoreContact& aStoreContact,
    TDes& aConfNum, TDes& aConfId, TDes& aConfPin )
    {
    FUNC_LOG;
    aConfNum.Zero();
    aConfId.Zero();
    aConfPin.Zero();
    
    if( !IsContactNumberAvailableL( aStoreContact, ETypeConfNumber ) )
        {
        return KErrNotFound;
        }
    
    TInt ret = KErrNone;
    
    // Search for conference number
    // Fix for EMZG-7M23KX
    TInt offset = FindAndAddResourceFileL();
    TResourceReader selectorReader;
    CCoeEnv::Static()->CreateResourceReaderLC( selectorReader,
            R_FSC_CONF_NUMBER_SELECTOR );

    CVPbkFieldTypeSelector* fieldTypeSelector = CVPbkFieldTypeSelector::NewL(
            selectorReader, iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy(); // selectorReader

    CVPbkFieldFilter::TConfig config(
            const_cast<MVPbkStoreContactFieldCollection&> (
                    aStoreContact.Fields() ), fieldTypeSelector, NULL );

    CVPbkFieldFilter* fieldFilter = CVPbkFieldFilter::NewL( config );
    TInt fieldCount = fieldFilter->FieldCount();
    const MVPbkStoreContactField* field = NULL;
    if ( fieldCount == 1 )
        {
        field = fieldFilter->FieldAtLC(0);
        aConfNum.Append( MVPbkContactFieldTextData::Cast( 
                field->FieldData() ).Text() );
        CleanupStack::PopAndDestroy();//field
        }

    delete fieldFilter;
    fieldFilter = NULL;
    delete fieldTypeSelector;
    fieldTypeSelector = NULL;
    
    // if conference number field exists but no number set
    if ( !( CommonPhoneParser::IsValidPhoneNumber( aConfNum, 
            CommonPhoneParser::EContactCardNumber ) ) )
        {
        return KErrNotFound;
        }
    
    // Search for conference id
    CCoeEnv::Static()->CreateResourceReaderLC( selectorReader,
                R_FSC_CONF_ID_SELECTOR );

    fieldTypeSelector = CVPbkFieldTypeSelector::NewL(
            selectorReader, iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy(); // selectorReader
    config.iFieldSelector = fieldTypeSelector;

    fieldFilter = CVPbkFieldFilter::NewL( config );
    fieldCount = fieldFilter->FieldCount();
    field = NULL;
    
    if ( fieldCount == 1 )
        {
        field = fieldFilter->FieldAtLC(0);
        const TDesC& confIdData = MVPbkContactFieldTextData::Cast( 
                field->FieldData() ).Text();
        if ( confIdData.Length() <= KMaxConfIdLen )
            {
            aConfId.Append( confIdData );
            }
        else
            {
            aConfId = KNullDesC;
            }
        CleanupStack::PopAndDestroy();//field
        if ( !( CommonPhoneParser::IsValidPhoneNumber( aConfId, 
                    CommonPhoneParser::EContactCardNumber ) ) )
            {
            aConfId = KNullDesC;
            }
        }

    delete fieldFilter;
    fieldFilter = NULL;
    delete fieldTypeSelector;
    fieldTypeSelector = NULL;

    // Search for conference id
    CCoeEnv::Static()->CreateResourceReaderLC( selectorReader,
                R_FSC_CONF_PIN_SELECTOR );

    fieldTypeSelector = CVPbkFieldTypeSelector::NewL(
            selectorReader, iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy(); // selectorReader
    config.iFieldSelector = fieldTypeSelector;

    fieldFilter = CVPbkFieldFilter::NewL( config );
    fieldCount = fieldFilter->FieldCount();
    field = NULL;
    if ( fieldCount == 1 )
        {
        field = fieldFilter->FieldAtLC(0);
        const TDesC& confPinData = MVPbkContactFieldTextData::Cast( 
                        field->FieldData() ).Text();
        if ( confPinData.Length() <= KMaxConfPinLen )
            {
            aConfPin.Append( confPinData );
            }
        else
            {
            aConfPin = KNullDesC;
            }
        CleanupStack::PopAndDestroy();//field
        if ( !( CommonPhoneParser::IsValidPhoneNumber( aConfPin, 
                    CommonPhoneParser::EContactCardNumber ) ) )
            {
            aConfPin = KNullDesC;
            }
        }

    delete fieldFilter;
    fieldFilter = NULL;
    delete fieldTypeSelector;
    fieldTypeSelector = NULL;
    
    CCoeEnv::Static()->DeleteResourceFile( offset );
  
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetPocAddressL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::GetPocAddressL( MVPbkStoreContact& aStoreContact,
    TDes& aPocAddress )
    {
    FUNC_LOG;
    
    if( !IsContactNumberAvailableL( aStoreContact, ETypePttNumber ) )
        {
        return KErrNotFound;
        }
    
    aPocAddress.Zero();
    TInt ret = KErrNone;
    
    // open select dialog for poc address 
    MVPbkStoreContactField* selectedField = OpenSelectDialogL(
            R_FSC_PTT_SELECT, aStoreContact );

    // If select POC address dialog was cancelled, we will return KErrCancel
    if ( !selectedField )
        {
        ret = KErrCancel;
        }
    else
        {
        // Else, copy POC address to the aPocAddress argument
        TPtrC pocAddress = MVPbkContactFieldTextData::Cast(
                selectedField->FieldData() ).Text();
        
        // Copy POC address from selected field
        if ( pocAddress.Length() )
            {
            aPocAddress.Copy( pocAddress );
            }
        else
            {
            ret = KErrNotFound;
            }
        }

    // Destroy objects
    delete selectedField;
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetEmailAddressL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::GetMessageAddressL( MVPbkStoreContact& aStoreContact,
    TDes& aMsgAddress )
    {
    FUNC_LOG;
    
    if( !IsContactNumberAvailableL( aStoreContact, ETypeMsgAddress ) )
        {
        return KErrNotFound;
        }
    
    aMsgAddress.Zero();

    TInt ret = KErrNone;
    
    // open select dialog for message address 
    MVPbkStoreContactField* selectedField = OpenSelectDialogL(
            R_FSC_MSG_SELECT, aStoreContact );
    
    // If select message address dialog was cancelled,
    // we will return KErrCancel
    if ( !selectedField )
        {
        ret = KErrCancel;
        }
    else
        {
        // Copy message address from selected field
        TPtrC msgAddress = MVPbkContactFieldTextData::Cast(
                selectedField->FieldData() ).Text();
        
        if ( msgAddress.Length() )
            {
            aMsgAddress.Copy( msgAddress );
            }
        else
            {
            ret = KErrNotFound;
            }
        }

    // Destroy objects
    delete selectedField;
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetEmailAddressL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::GetEmailAddressL( MVPbkStoreContact& aStoreContact,
    TDes& aEmailAddress )
    {
    FUNC_LOG;
    
    if( !IsContactNumberAvailableL( aStoreContact, ETypeEmailAddress ) )
        {
        return KErrNotFound;
        }
    
    aEmailAddress.Zero();

    TInt ret = KErrNone;
    
    // open select dialog for email address 
    MVPbkStoreContactField* selectedField = OpenSelectDialogL(
            R_FSC_EMAIL_SELECT, aStoreContact );
    
    // If select email address dialog was cancelled, we will return KErrCancel
    if ( !selectedField )
        {
        ret = KErrCancel;
        }
    else
        {
        // Copy email address from selected field
        TPtrC emailAddress = MVPbkContactFieldTextData::Cast(
                selectedField->FieldData() ).Text();
        
        if ( emailAddress.Length() )
            {
            aEmailAddress.Copy( emailAddress );
            }
        else
            {
            ret = KErrNotFound;
            }
        }

    // Destroy objects
    delete selectedField;
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::RemoveInvalidChar 
// ---------------------------------------------------------------------------
//
void CFscActionUtils::RemoveInvalidChar( const TDesC& aNumber,
    const TDesC& aValidChars, TDes& aOutNumber )
    {
    FUNC_LOG;
    for ( TInt ii = 0; ii < aNumber.Length(); ++ii )
        {
        TChar ch = aNumber[ ii ];
        if ( aValidChars.Locate( ch ) != KErrNotFound )
            {
            aOutNumber.Append( ch );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::IsContactNumberAvailableL 
// ---------------------------------------------------------------------------
//
TBool CFscActionUtils::IsContactNumberAvailableL( 
    MVPbkStoreContact& aStoreContact, TContactNumberType aContactNumberType )
    {
    FUNC_LOG;

    TBool available( EFalse );

    // Search for the contact name
    // Fix for EMZG-7M23KX  
    TInt offset = FindAndAddResourceFileL();
    
    TResourceReader selectorReader;
    CVPbkFieldTypeSelector* fieldTypeSelector = NULL;
    CVPbkFieldFilter* fieldFilter = NULL;
    TInt fieldCount = 0;
    TInt selector = 0;

    switch ( aContactNumberType )
        {
        case ETypeVoiceCallNumber:
            {
            selector = R_FSC_PHONENUMBER_SELECTOR;
            }
            break;

        case ETypeVideoCallNumber:
            {
            selector = R_FSC_VIDEO_SELECTOR;
            }
            break;
            
        case ETypeConfNumber:
            {
            selector = R_FSC_CONF_NUMBER_SELECTOR;
            }
            break;
            
        case ETypeInternetTel:
            {
            selector = R_FSC_VOIP_SELECTOR;
            }
            break;

        case ETypePttNumber:
            {
            selector = R_FSC_PTT_SELECTOR;
            }
            break;

        case ETypeMsgAddress:
            {
            selector = R_FSC_MSG_SELECTOR;
            }
            break;
            
        case ETypeAudioAddress:
            {
            selector = R_FSC_MSG_SELECTOR;
            }
            break;

        case ETypeEmailAddress:
            {
            selector = R_FSC_EMAIL_SELECTOR;
            }
            break;
            
        default:
            {
            break;
            }
        }
    
    if ( selector )
        {
        CCoeEnv::Static()->CreateResourceReaderLC( selectorReader, selector );

        fieldTypeSelector = CVPbkFieldTypeSelector::NewL( selectorReader,
                iContactManager.FieldTypes() );
        CleanupStack::PopAndDestroy(); // selectorReader

        CVPbkFieldFilter::TConfig config(
                const_cast<MVPbkStoreContactFieldCollection&> (
                        aStoreContact.Fields() ), fieldTypeSelector, NULL);

        fieldFilter = CVPbkFieldFilter::NewL( config );
        fieldCount = fieldFilter->FieldCount();
        
        if ( fieldCount )
            {
            if ( selector == R_FSC_CONF_NUMBER_SELECTOR )
                {
                TBuf<KMaxLengthOfNumber> confNum;
                const MVPbkStoreContactField* field = NULL;
                field = fieldFilter->FieldAtLC(0);
                confNum.Append( MVPbkContactFieldTextData::Cast( 
                        field->FieldData() ).Text() );
                CleanupStack::PopAndDestroy();//field
                
                if ( CommonPhoneParser::IsValidPhoneNumber( confNum, 
                        CommonPhoneParser::EContactCardNumber ) )
                    {
                    available = ETrue;
                    }
                }
            else
                {
                available = ETrue;
                }
            }

        delete fieldFilter;
        delete fieldTypeSelector;
        }
    
    CCoeEnv::Static()->DeleteResourceFile( offset );
    
    return available;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::Is3GNetworkAvailable
// ---------------------------------------------------------------------------
//  
TBool CFscActionUtils::Is3GNetworkAvailable()
    {
    FUNC_LOG;
    TInt err = KErrNone;
    TBool available( EFalse );

    TInt networkMode = ENWNetworkModeGsm;
    err = RProperty::Get( KPSUidNetworkInfo, KNWTelephonyNetworkMode, 
            networkMode );
     
    if ( err == KErrNone && networkMode == ENWNetworkModeWcdma )
        {
        available = FeatureManager::FeatureSupported( 
                KFeatureIdProtocolWcdma );
        }

    return available;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::IsOfflineModeL
// ---------------------------------------------------------------------------
//
TBool CFscActionUtils::IsOfflineModeL()
    {
    FUNC_LOG;
    TBool mode( EFalse );
    TInt status = ECoreAppUIsNetworkConnectionAllowed;

    CRepository* cenRep = CRepository::NewL( KCRUidCoreApplicationUIs );
    if ( cenRep )
        {
        cenRep->Get( KCoreAppUIsNetworkConnectionAllowed, status );
        }
    delete cenRep;

    if ( status == ECoreAppUIsNetworkConnectionNotAllowed )
        {
        mode = ETrue;
        }

    return mode;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::IsPttConfiguredL
// ---------------------------------------------------------------------------
//
TBool CFscActionUtils::IsPttConfiguredL()
    {
    FUNC_LOG;

    TBool result = EFalse;

    // Check configuration status from cenrep. 
    // If configuration status can not be found EFalse is returned. 
    CRepository* cenRep= NULL;
    TRAPD( err, cenRep = CRepository::NewL( KPoCOmaServerUid ) ) ;
    if ( err == KErrNone )
        {
        TInt settingsId;
        TInt err = cenRep->Get( KPoCDefaultSettings, settingsId );
		if ( err == KErrNone )
			{
			result = (settingsId != KErrNotFound );
			}
        delete cenRep;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::IsVoipConfiguredL
// ---------------------------------------------------------------------------
//
TBool CFscActionUtils::IsVoipConfiguredL()
    {
    FUNC_LOG;

    TBool configured = EFalse;
    if ( iRCSEProfileRegistry != NULL )
        {
        RArray<TUint32> ids;
        CleanupClosePushL( ids );
        //<cmail>
        //iRCSEProfileRegistry->GetAllIdsL( ids );
        //</cmail>
        configured = ids.Count() > 0;
        CleanupStack::PopAndDestroy( &ids );
        }

    return configured;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::SkinIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CFscActionUtils::SkinIconL( TAknsItemID aItemId,
    const TDesC& aFilename, TInt aFileBitmapId, TInt aFileMaskId )
    {
    FUNC_LOG;
    CGulIcon* returnIcon( NULL );
    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::CreateIconL( skin, aItemId, bitmap, mask, aFilename,
            aFileBitmapId, aFileMaskId );

    CleanupStack::PushL( mask );
    CleanupStack::PushL( bitmap );
    returnIcon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( bitmap );
    CleanupStack::Pop( mask );

    return returnIcon;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::ActionPriority
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::ActionPriority( TUint32 aActionPriorityCrKey,
    TInt aDefaultPriority )
    {
    FUNC_LOG;
    TInt result;
    TInt error = KErrNotFound;
    
    if ( iRep != NULL )
        {
        // Get() returns KErrNone if successful
        error = iRep->Get( aActionPriorityCrKey, result );
        }

    if ( error )
        {
        result = aDefaultPriority;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::CreateActionL
// ---------------------------------------------------------------------------
//
CFscContactAction* CFscActionUtils::CreateActionL(
    MFscReasonCallback& aReasonCallback, TUid aContactActionUid,
    TUint64 aActionType, TInt aActionMenuTextResourceId, CGulIcon* aIcon )
    {
    FUNC_LOG;
    CFscContactAction* contactAction = CFscContactAction::NewL(
            aReasonCallback, aContactActionUid, aActionType,
            aActionMenuTextResourceId, aIcon );

    return contactAction;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::CFscActionUtils
// ---------------------------------------------------------------------------
//
CFscActionUtils::CFscActionUtils( CVPbkContactManager& aContactManager ) :
    iContactManager( aContactManager )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscActionUtils::ConstructL
// ---------------------------------------------------------------------------
//
void CFscActionUtils::ConstructL()
    {
    FUNC_LOG;
    TRAP_IGNORE( iRep = CRepository::NewL( KCRUidFsContactActionService ) );

    if ( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        //<cmail>
        //TRAP_IGNORE( iRCSEProfileRegistry = CRCSEProfileRegistry::NewL() );
        //</cmail>
        }
    
    //<cmail>
    iRCSEProfileRegistry = NULL;
    //</cmail>

    }

// ---------------------------------------------------------------------------
// CFscActionUtils::GetFileDir
// ---------------------------------------------------------------------------
//
TPtrC CFscActionUtils::GetFileDirL( const TDesC& aFilePath )
    {
    FUNC_LOG;
    if ( iLetterDrive.Size() == 0 )
        {
        CEikonEnv* eikonEnv = CEikonEnv::Static();
        const TUid applUid = eikonEnv->EikAppUi()->Application()->AppDllUid();
        RApaLsSession ls;
        TApaAppInfo appInfo;
        User::LeaveIfError( ls.Connect() );
        User::LeaveIfError( ls.GetAppInfo( appInfo, applUid ) );
        // Extract application drive, path, name and extension
        const TFileName appFullName = appInfo.iFullName;
        ls.Close();
        TParse appNameParse;
        User::LeaveIfError( appNameParse.Set( appFullName, NULL, NULL ) );
        iLetterDrive = appNameParse.Drive();
        }
    TFileName fileName;
    fileName.Append( iLetterDrive );
    fileName.Append( aFilePath );
    
    return fileName;
    }

// Fix for EMZG-7M23KX
// ---------------------------------------------------------------------------
// CFscActionUtils::FindAndAddResourceFileL
// ---------------------------------------------------------------------------
//
TInt CFscActionUtils::FindAndAddResourceFileL()
    {
    FUNC_LOG;
    //<cmail> hardcoded removed
    TFileName resourceFileName( KDC_RESOURCE_FILES_DIR );
    resourceFileName.Append(KFscRscFileName);
    //</cmail>
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resourceFileName );
    TInt offset = CCoeEnv::Static()->AddResourceFileL( resourceFileName );
    
    return offset;
    }
// ---------------------------------------------------------------------------
// CFscActionUtils::CloseSelectDialogL
// ---------------------------------------------------------------------------
// 
void CFscActionUtils::CloseSelectDialogL()
    {
    FUNC_LOG;
    if ( iAddressSelect )
        {
        iAddressSelect->AttemptExitL( EFalse );
        }
    }


// ======== GLOBAL FUNCTIONS ========

