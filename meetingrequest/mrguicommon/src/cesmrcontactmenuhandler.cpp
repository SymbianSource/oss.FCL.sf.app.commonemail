/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CESMRContactMenuHandler.
*
*/


#include "emailtrace.h"
#include "cesmrcontactmenuhandler.h"
#include "cesmrcontactmenuurlhandler.h"
#include "mmrcontactmenuobserver.h"

#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactFieldTextData.h>
#include <TVPbkFieldVersitProperty.h>
#include <CVPbkFieldTypeRefsList.h>

//<cmail>
#include "cfsccontactactionservice.h"
#include "cfsccontactactionmenu.h"
#include "tfsccontactactionqueryresult.h"
#include "tfsccontactactionvisibility.h"
#include "mfsccontactaction.h"
#include "mfsccontactactionmenumodel.h"

#include <CVPbkFieldTypeIterator.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkEng.rsg>

#include <esmrgui.rsg>
#include "esmrcommands.h"
//</cmail>
#include <eikenv.h>
#include <eikmenup.h>
#include <commonphoneparser.h>
// <cmail> Removed profiling. </cmail>

/// Unnamed namespace for local definitions
namespace{ // codescanner::namespace

const TInt KGranularity( 8 );

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::CESMRContactMenuHandler( )
// ---------------------------------------------------------------------------
//
CESMRContactMenuHandler::CESMRContactMenuHandler( 
		CESMRContactManagerHandler& aContactManagerHandler ):
    iContactManagerHandler( aContactManagerHandler ), 
    iContactActionList( KGranularity )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::NewL( )
// ---------------------------------------------------------------------------
//
CESMRContactMenuHandler* CESMRContactMenuHandler::NewL( 
		CESMRContactManagerHandler& aContactManagerHandler )
    {
    FUNC_LOG;
    CESMRContactMenuHandler* self = new (ELeave) CESMRContactMenuHandler( 
													aContactManagerHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::ConstructL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::ConstructL( )
    {
    FUNC_LOG;
    
    CVPbkContactManager& contactManager =
            iContactManagerHandler.GetContactManager();
    
    iContactActionService = CFscContactActionService::NewL( contactManager );
    iContactActionMenu = CFscContactActionMenu::NewL( *iContactActionService );

    iContactMenuUrlHandler
            = CESMRContactMenuUrlHandler::NewL( *iContactActionMenu );

    if ( iContactManagerHandler.IsReady() )
        {
        iContactManagerReady = ETrue;
        }
    else
        {
        iContactManagerHandler.AddObserverL ( this );
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::~CESMRContactMenuHandler( )
// ---------------------------------------------------------------------------
//
CESMRContactMenuHandler::~CESMRContactMenuHandler( )
    {
    FUNC_LOG;
    iStoreContactList.ResetAndDestroy();
    delete iFindResultsArray;
    delete iContactMenuUrlHandler;    

    delete iContactOperationBase;
    delete iContactActionMenu;
    delete iContactActionService;
    delete iValue;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::OptionsMenuAvailable( )
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRContactMenuHandler::OptionsMenuAvailable( )
    {
    FUNC_LOG;
    return iOptionsMenuReady || iContactMenuUrlHandler->OptionsMenuAvailable( );
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::InitOptionsMenuL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactMenuHandler::InitOptionsMenuL(
		CEikMenuPane* aActionMenuPane )
    {
    FUNC_LOG;
    switch ( iValueType )
        {
        case EValueTypeEmail: // Fall through
        case EValueTypePhoneNumber:
            {
            // Add actions to options / actions submenu
            const TInt count(iContactActionList.Count());
            for ( TInt i = 0; i < count; ++i )
                {
                CEikMenuPaneItem::SData data;
                data.iFlags = 0;
                data.iCascadeId = 0;
                data.iCommandId = EESMRCmdActionMenuFirst + i;
                data.iText = iContactActionList[i].iAction->ActionMenuText();
                aActionMenuPane->AddMenuItemL( data );

                aActionMenuPane->SetItemDimmed( 
                		data.iCommandId,
                        iContactActionList[i].iOptionsMenuVisibility.iVisibility
                        == TFscContactActionVisibility::EFscActionDimmed );
                }
            break;
            }
        case EValueTypeURL:
            {
            iContactMenuUrlHandler->InitOptionsMenuL( aActionMenuPane );
            break;
            }  
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::ExecuteOptionsMenuL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactMenuHandler::ExecuteOptionsMenuL( TInt aCommand )
    {
    FUNC_LOG;
    switch ( iValueType )
        {
        case EValueTypeEmail: // Fall through
        case EValueTypePhoneNumber:
            {
            TInt index = aCommand - EESMRCmdActionMenuFirst;
            if ( index >= 0 && index < iContactActionList.Count() )
                {
                TUid uid = iContactActionList[index].iAction->Uid();
                iContactActionService->ExecuteL( uid, this );
                }
            break;
            }
        case EValueTypeURL:
            {
            iContactMenuUrlHandler->ExecuteOptionsMenuL( aCommand );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::Reset( )
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactMenuHandler::Reset( )
    {
    FUNC_LOG;
    delete iContactOperationBase;
    iContactOperationBase = NULL;

    delete iFindResultsArray;
    iFindResultsArray = NULL;

    iContactMenuUrlHandler->Reset();
    
    iContactActionMenu->Model().RemoveAll();
    
    delete iValue;
    iValue = NULL;
    iValueType = EValueTypeNone;
    
    iContactActionList.Reset();
    iStoreContactList.ResetAndDestroy();
    iActionMenuReady = EFalse;
    iOptionsMenuReady = EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::SetValueL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactMenuHandler::SetValueL(
        const TDesC& aValue, TValueType aValueType )
    {
    FUNC_LOG;
    Reset();

    if ( aValueType == EValueTypeURL )
        {
        iContactMenuUrlHandler->SetUrlL( aValue );
        iValueType = aValueType;
        }
    else if ( aValueType != EValueTypeNone )
        {
        iValue = aValue.AllocL();
        iValueType = aValueType;
        if ( iContactManagerReady )
            {
            SearchContactL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::SetContactMenuObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactMenuHandler::SetContactMenuObserver(
        MMRContactMenuObserver* aObserver )
    {
    iContactMenuObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::SetCommandObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactMenuHandler::SetCommandObserver(
            MEikCommandObserver* aCommandObserver )
    {
    FUNC_LOG;
    iCommandObserver = aCommandObserver;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::ShowActionMenuL( )
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRContactMenuHandler::ShowActionMenuL( )
    {
    FUNC_LOG;
    if ( iValueType == EValueTypeURL )
        {
        iContactMenuUrlHandler->ShowActionMenuL();
        }  
    else if ( iActionMenuReady )
        {
        iContactActionMenu->ExecuteL( );
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::VerifyContactDetailsL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::VerifyContactDetailsL()
    {
    FUNC_LOG;
    if ( iFindResultsArrayIndex < iFindResultsArray->Count() )
        {
        CVPbkContactManager& contactManager =
                iContactManagerHandler.GetContactManager();

        iContactOperationBase =
            contactManager.RetrieveContactL( 
            		iFindResultsArray->At( iFindResultsArrayIndex ), 
            		*this );
        }
    else
        {
        delete iFindResultsArray;
        iFindResultsArray = NULL;

        if ( iStoreContactList.Count() > 0 )
            {
            iContactActionService->SetContactSetL( iStoreContactList );
            iContactActionService->QueryActionsL( this );
            }
        else
            {
            CreateContactL ( );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::ContactManagerReady( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::ContactManagerReady( )
    {
    FUNC_LOG;
    iContactManagerReady = ETrue;
    iContactManagerHandler.RemoveObserver( this );
    if ( iValue )
        {
        TRAP_IGNORE( SearchContactL() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    delete iContactOperationBase;
    iContactOperationBase = NULL;

    MVPbkStoreContactFieldCollection& fieldCollection =
        aContact->Fields();

    TBool valueMatchFound = EFalse;
    const TInt count(fieldCollection.FieldCount());
    for ( TInt i = 0; valueMatchFound == EFalse && i < count; ++i )
        {
        MVPbkStoreContactField& contactField = fieldCollection.FieldAt( i );
        MVPbkContactFieldData& fieldData = contactField.FieldData();
        if ( fieldData.DataType() == EVPbkFieldStorageTypeText )
            {
            MVPbkContactFieldTextData& fieldTextData =
                MVPbkContactFieldTextData::Cast( fieldData );

            if ( fieldTextData.Text().CompareF( *iValue ) == 0 )
                {
                valueMatchFound = ETrue;
                }
            }
        }

    if ( valueMatchFound )
        {
        TRAP_IGNORE( iStoreContactList.AppendL( aContact ) );
        }
    else
        {
        delete aContact;
        }

    ++iFindResultsArrayIndex;
    TRAPD( err, VerifyContactDetailsL() );
    if ( err != KErrNone )
        {
        Reset();
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aError*/ )
    {
    FUNC_LOG;
    delete iContactOperationBase;
    iContactOperationBase = NULL;

    ++iFindResultsArrayIndex;
    TRAPD( err, VerifyContactDetailsL() );
    if ( err != KErrNone )
        {
        Reset();
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::FindCompleteL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::FindCompleteL( MVPbkContactLinkArray* aResults )
    {
    FUNC_LOG;
    delete iContactOperationBase;
    iContactOperationBase = NULL;

    iStoreContactList.ResetAndDestroy();
    iFindResultsArray = aResults;
    iFindResultsArrayIndex = 0;

    VerifyContactDetailsL();
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::FindFailed
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::FindFailed( TInt /*aError*/)
    {
    FUNC_LOG;
    delete iContactOperationBase;
    iContactOperationBase = NULL;

    Reset ( );
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::QueryActionsComplete( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::QueryActionsComplete( )
    {
    FUNC_LOG;
    const CFscContactActionList& actionList =
            iContactActionService->QueryResults();

    // Add visible actions to local contact action list for
    // options menu / actions.
    const TInt count(actionList.Count());
    for ( TInt i = 0; i < count; ++i )
        {
        TFscContactActionVisibility::TVisibility visibility =
                actionList[i].iOptionsMenuVisibility.iVisibility;
        if ( visibility == TFscContactActionVisibility::EFscActionVisible ||
             visibility == TFscContactActionVisibility::EFscActionDimmed )
            {
            TRAP_IGNORE( iContactActionList.AppendL( actionList.At(i) ) );
            }
        }

    const TInt count2(iContactActionList.Count());
    for ( TInt i = 0; i < count2; ++i )
        {
        iContactActionList[i].iPriority = -iContactActionList[i].iPriority;
        }

    // Sort local list of actions by priority.
    TKeyArrayFix sortKey( _FOFF( TFscContactActionQueryResult, // codescanner::foff
                                 iPriority ),
                          ECmpTInt);
    iContactActionList.Sort( sortKey );

    // Enable options menu / actions if there are actions available.
    iOptionsMenuReady = iContactActionList.Count() > 0;

    // Initialize contact action menu and enable it.
    TRAPD( error, iContactActionMenu->Model().AddPreQueriedCasItemsL ( ) );
    if ( error != KErrNone )
        {
        iActionMenuReady = EFalse;
        }
    else
        {
        iActionMenuReady = ETrue;
        }
    
    if ( iContactMenuObserver )
        {
        iContactMenuObserver->ContactActionQueryComplete();
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::QueryActionsFailed
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::QueryActionsFailed( TInt /*aError*/)
    {
    FUNC_LOG;
    Reset();

    if ( iContactMenuObserver )
        {
        iContactMenuObserver->ContactActionQueryComplete();
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::ExecuteComplete( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::ExecuteComplete( )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::ExecuteFailed
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::ExecuteFailed( TInt /*aError*/)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::SearchContactL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::SearchContactL( )
    {
    FUNC_LOG;
    // Cancel ongoing search if one exists.
    if ( iContactOperationBase )
        {
        delete iContactOperationBase;
        iContactOperationBase = NULL;
        }

    CVPbkContactManager& contactManager =
            iContactManagerHandler.GetContactManager();

    switch ( iValueType )
        {
        case EValueTypeEmail:
            {
            const MVPbkFieldTypeList& fieldTypeList = 
							contactManager.FieldTypes();

            CVPbkFieldTypeRefsList* fieldTypeRefsList =
                    CVPbkFieldTypeRefsList::NewL();
            CleanupStack::PushL( fieldTypeRefsList );

            // Search for all email fields.
            const TInt count(fieldTypeList.FieldTypeCount());
            for ( TInt i = 0; i < count; ++i )
                {
                const MVPbkFieldType& fieldType = 
					fieldTypeList.FieldTypeAt( i );
                TArray<TVPbkFieldVersitProperty> versitProperties =
                        fieldType.VersitProperties();

                const TInt count2(versitProperties.Count());
                for ( TInt ii=0; ii < count2; ++ii )
                    {
                    if ( versitProperties[ii].Name() == EVPbkVersitNameEMAIL )
                        {
                        fieldTypeRefsList->AppendL( fieldType );
                        break;
                        }
                    }
                }

            // Initiate find operation, returns to FindCompleteL.
            iContactOperationBase = contactManager.FindL( *iValue,
                    *fieldTypeRefsList, *this );

            CleanupStack::PopAndDestroy ( fieldTypeRefsList );
            break;
            }
        case EValueTypePhoneNumber:
            {
            if ( CommonPhoneParser::IsValidPhoneNumber( *iValue,
                    CommonPhoneParser::EPlainPhoneNumber ) )
                {
                // Initiate find operation, returns to FindCompleteL.
                const MVPbkFieldTypeList& fieldTypeList = 
											contactManager.FieldTypes();

                CVPbkFieldTypeRefsList* fieldTypeRefsList =
                        CVPbkFieldTypeRefsList::NewL();
                CleanupStack::PushL( fieldTypeRefsList );

                // Search for all email fields.
                const TInt count(fieldTypeList.FieldTypeCount());
                for ( TInt i = 0; i < count; ++i )
                    {
                    const MVPbkFieldType& fieldType = 
											fieldTypeList.FieldTypeAt( i );
                    TArray<TVPbkFieldVersitProperty> versitProperties =
                            fieldType.VersitProperties();

                    const TInt count2(versitProperties.Count());
                    for ( TInt ii=0; ii < count2; ++ii )
                        {
                        if ( versitProperties[ii].Name() == EVPbkVersitNameTEL )
                            {
                            fieldTypeRefsList->AppendL( fieldType );
                            break;
                            }
                        }
                    }
                // Initiate find operation, returns to FindCompleteL.
                iContactOperationBase = contactManager.FindL( *iValue,
                        *fieldTypeRefsList, *this );

                CleanupStack::PopAndDestroy ( fieldTypeRefsList );
                }
            else
                {
                CreateContactL();
                }
            break;
            }
        default:
            {
            // Disable contact action handler.
            Reset();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::CreateContactL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuHandler::CreateContactL( )
    {
    FUNC_LOG;
    CVPbkContactManager& contactManager =
            iContactManagerHandler.GetContactManager();

    const MVPbkFieldTypeList& fieldTypeList = contactManager.FieldTypes();
    const MVPbkFieldType* fieldType= NULL;

    // Try to find appropriate field type.
    switch ( iValueType )
        {
        case EValueTypeEmail:
            {
            fieldType = fieldTypeList.Find ( R_VPBK_FIELD_TYPE_EMAILGEN );
            break;
            }
        case EValueTypePhoneNumber:
            {
            fieldType = fieldTypeList.Find ( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN );
            break;
            }
        default:
            {
            break;
            }
        }

    // If field type was found.
    if ( fieldType )
        {
        // Create temporary store contact.
        MVPbkContactStore& store = LocalContactStoreL();
        MVPbkStoreContact* storeContact = store.CreateNewContactLC();
        iStoreContactList.AppendL( storeContact );
        CleanupStack::Pop(); // codescanner::cleanup

        // Add link value to field.
        MVPbkStoreContactField* field =
                storeContact->CreateFieldLC ( *fieldType );
        MVPbkContactFieldTextData::Cast(field->FieldData()).SetTextL(*iValue);
        storeContact->AddFieldL( field );
        CleanupStack::Pop( field );

        // Add link value to first name too.
        // Otherwise t.ex send multimedia shows contact as 'unnamed'.
        fieldType = fieldTypeList.Find( R_VPBK_FIELD_TYPE_FIRSTNAME );
        if ( fieldType )
            {
            field = storeContact->CreateFieldLC( *fieldType );
            MVPbkContactFieldTextData::Cast( 
            		field->FieldData() ).SetTextL( *iValue );
            storeContact->AddFieldL( field );
            CleanupStack::Pop( field );
            }

        // Set contact set for contact action service.
        iContactActionService->SetContactSetL( iStoreContactList );
        // Initiate query actions, returns to QueryActionsComplete.
        iContactActionService->QueryActionsL( this );
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuHandler::LocalContactStoreL( )
// ---------------------------------------------------------------------------
//
MVPbkContactStore& CESMRContactMenuHandler::LocalContactStoreL( )
    {
    FUNC_LOG;
    CVPbkContactManager& contactManager =
            iContactManagerHandler.GetContactManager();

    // Search for local contact store.
    MVPbkContactStoreList& storeList = contactManager.ContactStoresL();
    const TInt count(storeList.Count());
    for ( TInt i = 0; i< count; ++i )
        {
        MVPbkContactStore& store = storeList.At( i );
        if ( store.StoreProperties().Local() )
            {
            return store;
            }
        }
    // Leave if not found.
    User::Leave ( KErrNotFound );
    // Disables warning.
    MVPbkContactStore* null = NULL;
    return *null;
    }

// EOF

