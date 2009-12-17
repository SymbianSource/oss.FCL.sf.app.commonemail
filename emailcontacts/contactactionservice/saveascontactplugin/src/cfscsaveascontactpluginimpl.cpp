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
* Description:  Implementation of the class CfscsaveascontactpluginImpl.
 *
*/


#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include <bautils.h>
#include <aknnotewrappers.h>
#include <fscsaveascontactplugin.rsg>
#include <fscsaveascontactplugin.mbg>
#include <AknsConstants.h>
//<cmail>
#include "mfsccontactset.h"
#include "mfscactionutils.h"
#include "cfsccontactaction.h"
#include "fsccontactactionservicedefines.h"
#include <MVPbkStoreContact.h>
#include <MVPbkContactObserver.h>
#include <CPbk2StoreConfiguration.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <e32cmn.h>
#include "mfsccontactactionpluginobserver.h"
//</cmail>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactFieldData.h>
#include <VPbkContactStoreUris.h>
#include <AknWaitDialog.h>

#include "fsccontactactionserviceuids.hrh"
#include "fscactionpluginactionuids.h"
#include "fscactionplugincrkeys.h"
#include "cfscsaveascontactpluginimpl.h"
// <cmail>
#include "FscActionPluginUtils.h"
// </cmail>


// CONSTANTS DECLARATIONS
const TInt KActionCount = 1;
const TInt KInvalidReasonId = 0;

_LIT( KSaveAsContactPluginIconPath,
        "\\resource\\apps\\fscsaveascontactplugin.mif" );
_LIT( KResourceFilePath, "\\resource\\apps\\fscsaveascontactplugin.rsc" );

const TImplementationProxy ImplementationTable[] =
    {
            IMPLEMENTATION_PROXY_ENTRY(
                    KFscSaveAsContactPluginImplImpUid,
                    CFscSaveAsContactPluginImpl::NewL )
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::NewL
// ---------------------------------------------------------------------------
//
CFscSaveAsContactPluginImpl* CFscSaveAsContactPluginImpl::NewL(
        TAny* aParams)
    {
    FUNC_LOG;

    TFscContactActionPluginParams* params =
            reinterpret_cast< TFscContactActionPluginParams* >(aParams );

    CFscSaveAsContactPluginImpl* self = new (ELeave) CFscSaveAsContactPluginImpl( *params );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::~CFscSaveAsContactPluginImpl
// ---------------------------------------------------------------------------
//
CFscSaveAsContactPluginImpl::~CFscSaveAsContactPluginImpl()
    {
    FUNC_LOG;
    DismissWaitNote();
    delete iNewStoreContact;
    delete iContactSaveAsContactAction;
    delete iActionList;
    CCoeEnv::Static()->DeleteResourceFile(iResourceHandle);
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::Uid
// ---------------------------------------------------------------------------
//
TUid CFscSaveAsContactPluginImpl::Uid() const
    {
    FUNC_LOG;
    TUid uid =
        {
         KFscSaveAsContactPluginImplImpUid
        };
    return uid;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::ActionList
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>* CFscSaveAsContactPluginImpl::ActionList() const
    {
    return iActionList;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::GetActionL
// ---------------------------------------------------------------------------
//
const MFscContactAction& CFscSaveAsContactPluginImpl::GetActionL(
        TUid aActionUid) const
    {
    FUNC_LOG;

    const MFscContactAction* action= NULL;
    if (aActionUid == KFscActionUidSaveAsContact)
        {
        action = iContactSaveAsContactAction;
        }
    else
        {
        User::Leave(KErrNotFound);
        }

    return *action;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::PriorityForContactSetL
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::PriorityForContactSetL(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        TFscContactActionVisibility& aActionMenuVisibility,
        TFscContactActionVisibility& aOptionsMenuVisibility,
        MFscContactActionPluginObserver* aObserver)
    {
    FUNC_LOG;
    iActionUid = aActionUid;
    iContactSet = &aContactSet;
    iActionMenuVisibility = &aActionMenuVisibility;
    iOptionsMenuVisibility = &aOptionsMenuVisibility;

    iIsExecute = EFalse;

    iContactSet->SetToFirstContact();

    iActionPriority = KFscActionPriorityNotAvailable;

    iCanDisplay = KErrNone;

    if (iActionUid == KFscActionUidSaveAsContact)
        {
        // We save only one contact and no groups:
        iCanDisplay = (aContactSet.GroupCount() == 0
                && aContactSet.ContactCount() == 1 ) ? KErrNone : KErrArgument;

        //Fix for EJKA-7MFDHU
        // <cmail>
        if (iCanDisplay == KErrNone
        		&& IsValidStoreL() )
        // </cmail>
            {
            aActionMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionVisible;
            aOptionsMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionVisible;

            iActionPriority = iParams.iUtils->ActionPriority(
                    KFscCrUidSaveAsContact, KFscActionPrioritySaveAsContact);
            if (iActionPriority > 0)
                {
                // ignore any leave while updating action icons
                TRAP_IGNORE( UpdateActionIconL( aActionUid ) );
                }
            }
        else
            {
            iActionPriority = KFscActionPriorityNotAvailable;
            aActionMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionHidden;
            aActionMenuVisibility.iReasonId = KInvalidReasonId;
            aOptionsMenuVisibility.iVisibility
                    = TFscContactActionVisibility::EFscActionHidden;
            aOptionsMenuVisibility.iReasonId = KInvalidReasonId;
            }
        ResetData();
        aObserver->PriorityForContactSetComplete(iActionPriority);
        }
    else
        {
        ResetData();
        aObserver->PriorityForContactSetFailed(KErrArgument);
        }
    }
// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::CancelPriorityForContactSet 
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::CancelPriorityForContactSet()
    {
    FUNC_LOG;
    iLastEvent = EActionEventCanceled;
    ResetData();
    }
// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::ExecuteL 
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::ExecuteL(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        MFscContactActionPluginObserver* aObserver)
    {
    FUNC_LOG;
    switch (iLastEvent)
        {
        case EActionEventIdle:
            {

            iLastEvent = EActionEventCanExecuteLaunched;

            iActionUid = aActionUid;
            iContactSet = &aContactSet;
            iPluginObserver = aObserver;

            iIsExecute = ETrue;

            iContactSet->SetToFirstContact();

            CanExecuteL(aActionUid, aContactSet);

            break;
            }

        case EActionEventCanExecuteFinished:
            {

            iWaitDialog = new(ELeave)CAknWaitDialog(
                    (REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), EFalse);
            iWaitDialog->ExecuteLD(R_FS_WAIT_NOTE);
            
            if (iCanDisplay == KErrNone)
                {
                iLastEvent = EActionEventContactRetrieve;
                iPluginObserver = aObserver;
                aContactSet.NextContactL(this);
                }
            else
                {
                ResetData();
                iPluginObserver->ExecuteFailed(KErrGeneral);
                }
            break;
            }            
        case EActionEventCanceled:
            {
            iLastEvent = EActionEventIdle;
            break;
            }
        default:
            {
            ResetData();
            break; // <cmail>
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::CancelExecute
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::CancelExecute()
    {
    FUNC_LOG;
    DismissWaitNote();
    iLastEvent = EActionEventCanceled;
    ResetData();
    }
// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::GetReasonL
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::GetReasonL(
        TUid /* aActionUid */,
        TInt /* aReasonId */,
        HBufC*& /* aReason */) const
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::NextContactComplete
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::NextContactComplete(
        MVPbkStoreContact* aContact)
    {
    FUNC_LOG;
    TInt err( KErrNone );
    switch (iLastEvent)
        {
        case EActionEventContactRetrieve:
            {
            iRetrievedStoreContact = aContact;
            iLastEvent = EActionEventExecuteLaunched;
            TRAP (err, PrepareStoreL() );
            break;
            }
        case EActionEventCanceled:
            {
            iLastEvent = EActionEventIdle;
            break;
            }
        default:
            {
            ResetData();
            break; // <cmail>
            }
        }
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::NextContactFailed
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::NextContactFailed(
        TInt aError)
    {
    FUNC_LOG;

    ResetData();
    iPluginObserver->ExecuteFailed(aError);
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::NextGroupComplete
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::NextGroupComplete(
        MVPbkStoreContact* /*aContact*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::NextGroupFailed
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::NextGroupFailed(
        TInt /*aError*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::GetGroupContactComplete(
        MVPbkStoreContact* /*aContact*/)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::GetGroupContactFailed
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::GetGroupContactFailed(
        TInt /*aError*/)
    {
    FUNC_LOG;
    }

// <cmail>
// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::IsValidStoreL
// ---------------------------------------------------------------------------
//
TBool CFscSaveAsContactPluginImpl::IsValidStoreL()
// </cmail>	
	{
    FUNC_LOG;
	TBool isValid = EFalse;
	
	//Currently method is checking if saving store is cntdb
	//if saving store is SIM, contact can't be saved
	//(SIM store is not supported yet)
	
	CPbk2StoreConfiguration* storeCnf = CPbk2StoreConfiguration::NewL();
	CleanupStack::PushL(storeCnf);
	TVPbkContactStoreUriPtr defaultStorePtr = 
		storeCnf->DefaultSavingStoreL();
	TVPbkContactStoreUriPtr validStorePtr( 
			VPbkContactStoreUris::DefaultCntDbUri() 
			);
	
	if ( !defaultStorePtr.Compare( 
			validStorePtr.UriDes(), 
			TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) 
			)
		{
		isValid = ETrue;
		}
	
	CleanupStack::PopAndDestroy(storeCnf);
	return isValid;
	}

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::PrepareStoreL
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::PrepareStoreL()
    {
    FUNC_LOG;
    CPbk2StoreConfiguration* storeCnf = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(storeCnf);
    TVPbkContactStoreUriPtr storePtr = storeCnf->DefaultSavingStoreL();
    CVPbkContactManager& contactManager = iParams.iUtils->ContactManager();
	contactManager.LoadContactStoreL( storePtr );            
	iTargetStore = contactManager.ContactStoresL().
						Find( storePtr );
	if ( iTargetStore )
        {
		iTargetStore->OpenL( *this );
		}
	else //store not found
		{
		ResetData();
		iPluginObserver->ExecuteFailed( KErrNotFound );
		}
	CleanupStack::PopAndDestroy(storeCnf);
	}

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::SaveToContactDatabaseL
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::SaveToContactDatabaseL(
        MVPbkStoreContact* aStoreContact)
    {
    FUNC_LOG;

	iNewStoreContact = iTargetStore->CreateNewContactLC();
        CleanupStack::Pop();//iNewStoreContact
        MVPbkStoreContactFieldCollection& fields = aStoreContact->Fields();
        TInt i = 0;
	TInt a = fields.FieldCount();
        // Copy fields to the new store contact. No field matching check
        for (i = 0; i < fields.FieldCount(); i++)
            {
            MVPbkStoreContactField& oldField = fields.FieldAt(i);
            const MVPbkFieldType* fieldType =
                    oldField.BestMatchingFieldType();
            if (fieldType != NULL)
                {
                MVPbkStoreContactField* newField =
                        iNewStoreContact->CreateFieldLC(*fieldType);
                newField->FieldData().CopyL(oldField.FieldData());
                iNewStoreContact->AddFieldL(newField);
                CleanupStack::Pop();//newField
                }
            }
        // This is new contact so we don't need to lock it before commiting
        iNewStoreContact->CommitL( *this);
 

    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::StoreReady
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::StoreReady( 
        MVPbkContactStore& aContactStore )
    {
    FUNC_LOG;
    iTargetStore = &aContactStore;
    TRAPD( err, SaveToContactDatabaseL( iRetrievedStoreContact ) );
    if ( err != KErrNone )
    	{
		ResetData();
        iPluginObserver->ExecuteFailed( err );
		}
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::StoreUnavailable
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::StoreUnavailable( 
        MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
    {
    FUNC_LOG;
    ResetData();
	iPluginObserver->ExecuteFailed(KErrArgument);
    }
        
// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::HandleStoreEventL
// ---------------------------------------------------------------------------
//       
void CFscSaveAsContactPluginImpl::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::CanExecuteL
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::CanExecuteL(
        TUid aActionUid,
        MFscContactSet& aContactSet)
    {
    FUNC_LOG;

    if (aActionUid == KFscActionUidSaveAsContact)
        {
        iCanDisplay = (aContactSet.GroupCount() == 0
                && aContactSet.ContactCount() == 1 )
                                                     ? KErrNone
                                                     : KErrArgument;
        iLastEvent = EActionEventCanExecuteFinished;
        ResumeAsync(aActionUid, aContactSet, iIsExecute);
        }
    else
        {
        //Just in case
        ResetData();
        iPluginObserver->ExecuteFailed(KErrArgument);
        }

    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::UpdateActionIconL
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::UpdateActionIconL(
        TUid aActionUid)
    {
    FUNC_LOG;

    TFileName dllFileName;
    Dll::FileName(dllFileName);
    TParse parse;
    User::LeaveIfError(parse.Set(
            KSaveAsContactPluginIconPath, &dllFileName, 
            NULL) );

    if (aActionUid == KFscActionUidSaveAsContact)
// <cmail>
        {
        iContactSaveAsContactAction->SetIcon(iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsRclActionSave, parse.FullName(),
                EMbmFscsaveascontactpluginQgn_prop_cmail_action_save,
                EMbmFscsaveascontactpluginQgn_prop_cmail_action_save_mask) );
// </cmail>
        }

    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::CFscSaveAsContactPluginImpl
// ---------------------------------------------------------------------------
//
CFscSaveAsContactPluginImpl::CFscSaveAsContactPluginImpl(
        const TFscContactActionPluginParams& aParams) :
    CFscContactActionPlugin(), iParams(aParams), iResourceHandle( 0),
            iLastEvent(EActionEventIdle), iCanDisplay(KErrNone),
            iIsExecute(EFalse)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::ConstructL()
    {
    FUNC_LOG;
    
    TFileName dllFileName;
    Dll::FileName(dllFileName);
    TParse parse;
    User::LeaveIfError(parse.Set(
            KResourceFilePath, &dllFileName, NULL) );
    TFileName resourceFileName(parse.FullName() );
    BaflUtils::NearestLanguageFile(
            CCoeEnv::Static()->FsSession(), resourceFileName);

    iResourceHandle = ( CCoeEnv::Static() )->AddResourceFileL(resourceFileName);

    iActionList = new( ELeave )CArrayFixFlat<TUid>( KActionCount );
    iActionList->AppendL(KFscActionUidSaveAsContact);

    User::LeaveIfError(parse.Set(
            KSaveAsContactPluginIconPath, &dllFileName, NULL) );
// <cmail>    
    iContactSaveAsContactAction = iParams.iUtils->CreateActionL(
            *this, KFscActionUidSaveAsContact, KFscAtManSaveAs, 
            R_FS_ACTION_SAVE_AS_CONTACT, iParams.iUtils->SkinIconL(
                    KAknsIIDQgnFsRclActionSave, parse.FullName(),
                    EMbmFscsaveascontactpluginQgn_prop_cmail_action_save,
                    EMbmFscsaveascontactpluginQgn_prop_cmail_action_save_mask) );
// </cmail>
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::ResetData
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::ResetData()
    {
    FUNC_LOG;
    if (iContactSet)
        {
        iContactSet->SetToFirstContact();
        iContactSet->SetToFirstGroup();
        // <cmail>
        TRAP_IGNORE( iContactSet->CancelNextContactL() );
        TRAP_IGNORE( iContactSet->CancelNextGroupL() );
        // </cmail>        
        iContactSet = NULL;
        }
    DismissWaitNote();

    iLastEvent = EActionEventIdle;
    iActionMenuVisibility = NULL;
    iOptionsMenuVisibility = NULL;
    iCanDisplay = KErrNone;
    iRetrievedStoreContact = NULL;
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::ResumeAsync
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::ResumeAsync(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        TBool aIsExecute )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    if ( aIsExecute )
        {
        TRAP( err, ExecuteL( aActionUid, aContactSet, iPluginObserver ) );
        }
    else
        {
        TRAP( err, PriorityForContactSetL(aActionUid, aContactSet, 
                *iActionMenuVisibility, *iOptionsMenuVisibility, 
                iPluginObserver ) );
        }
    
    if ( err != KErrNone )
        {
        ResetData();
        if ( aIsExecute )
            {
            iPluginObserver->ExecuteFailed( err );
            }
        else
            {
            iPluginObserver->PriorityForContactSetFailed( err );
            }
        }
    
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::ContactOperationCompleted
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::ContactOperationCompleted(
        MVPbkContactObserver::TContactOpResult aResult)
    {
    FUNC_LOG;
    TInt err( KErrNone );
    switch (aResult.iOpCode)
        {
        case EContactLock:
            {
            TRAP( err, iNewStoreContact->CommitL( *this ) );
            break;
            }
        case EContactCommit:
            {
            DismissWaitNote();
            TRAP( err, iParams.iUtils->ShowCnfNoteL( 
                    R_FS_ACTION_SAVED_NOTE ) );
            ResetData();
            iPluginObserver->ExecuteComplete();

            if (iNewStoreContact != NULL)
                {
                delete iNewStoreContact;
                iNewStoreContact = NULL;
                }
            break;
            }
        case EActionEventCanceled:
            {
            iLastEvent = EActionEventIdle;
            break;
            }
        default:
            {
            ResetData();
            break; // <cmail>
            }            
        }
    
    if ( err != KErrNone )
        {
        ResetData();
        if ( iIsExecute )
            {
            iPluginObserver->ExecuteFailed( err );
            }
        else
            {
            iPluginObserver->PriorityForContactSetFailed( err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::ContactOperationFailed
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::ContactOperationFailed(
        MVPbkContactObserver::TContactOp /*aOpCode*/,
        TInt aErrorCode,
        TBool /*aErrorNotified*/)
    {
    FUNC_LOG;
    iLastEvent = EActionEventIdle;
    DismissWaitNote();
    ResetData();
    iPluginObserver->ExecuteFailed(aErrorCode);
    }

// ---------------------------------------------------------------------------
// CFscSaveAsContactPluginImpl::DismissWaitDialog
// ---------------------------------------------------------------------------
//
void CFscSaveAsContactPluginImpl::DismissWaitNote()
    {
    FUNC_LOG;
    if (iWaitDialog != NULL)
        {
        TRAPD( err, iWaitDialog->ProcessFinishedL() );
        if (err != KErrNone)
            {
            delete iWaitDialog;
            iWaitDialog = NULL;
            }
        }
    }
// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount )
    {
    aTableCount =
    sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

