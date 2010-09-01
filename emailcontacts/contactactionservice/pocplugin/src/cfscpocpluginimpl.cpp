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
* Description:  Implementation of the class CFscPocPluginImpl.
 *
*/


#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include <bautils.h>
#include <RPbkViewResourceFile.h> 
#include <AiwPoCParameters.h>
#include <AiwPoCParameters.hrh>
#include <AiwServiceHandler.h>
#include <coemain.h>
#include <AknsConstants.h>
#include <fscpocplugin.rsg>
#include <fscpocplugin.mbg>
//<cmail>
#include "fscactionpluginactionuids.h"
#include "cfsccontactaction.h"
#include "mfsccontactset.h"
#include "mfscactionutils.h"
#include "mfsccontactactionpluginobserver.h"
//</cmail>
#include <MVPbkContactGroup.h>
#include <MVPbkContactLinkArray.h>

#include "fscactionplugincrkeys.h"
#include "cfscpocpluginimpl.h"
// <cmail>
#include "FscActionPluginUtils.h"
// </cmail>

// CONSTANTS DECLARATIONS
const TInt KMaxLenghtOfAddressData = 255;
const TInt KActionCount = 1;
const TInt KInvalidReasonId = 0;
const TInt KContactListGranularity = 2;

_LIT( KPocPluginIconPath, "\\resource\\apps\\fscpocplugin.mif" );
_LIT( KResourceFilePath, "\\resource\\apps\\fscpocplugin.rsc" );

// typedef declarations
typedef TBuf<KMaxLenghtOfAddressData> TAddressData;

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KFscPocPluginImplImpUid,
            CFscPocPluginImpl::NewL )
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::NewL
// ---------------------------------------------------------------------------
//
CFscPocPluginImpl* CFscPocPluginImpl::NewL(TAny* aParams)
    {
    FUNC_LOG;

    TFscContactActionPluginParams* params =
            reinterpret_cast< TFscContactActionPluginParams* >(aParams );

    CFscPocPluginImpl* self = new( ELeave ) CFscPocPluginImpl( *params );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::~CFscPocPluginImpl
// ---------------------------------------------------------------------------
//
CFscPocPluginImpl::~CFscPocPluginImpl()
    {
    FUNC_LOG;
    delete iSelectedPhNumArray;
    delete iContactPttAction;
    delete iActionList;
    delete iAiwServiceHandler;
    CCoeEnv::Static()->DeleteResourceFile(iResourceHandle);
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::Uid
// ---------------------------------------------------------------------------
//
TUid CFscPocPluginImpl::Uid() const
    {
    FUNC_LOG;
    TUid uid =
        {
        KFscPocPluginImplImpUid
        };
    return uid;
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::ActionList
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>* CFscPocPluginImpl::ActionList() const
    {
    return iActionList;
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::GetActionL
// ---------------------------------------------------------------------------
//
const MFscContactAction& CFscPocPluginImpl::GetActionL(TUid aActionUid) const
    {
    FUNC_LOG;

    const MFscContactAction* action= NULL;

    if (aActionUid == KFscActionUidCallPtt)
        {
        action = iContactPttAction;
        }
    else
        {
        User::Leave(KErrNotFound);
        }

    return *action;
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::PriorityForContactSetL
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::PriorityForContactSetL(TUid aActionUid,
        MFscContactSet& aContactSet,
        TFscContactActionVisibility& aActionMenuVisibility,
        TFscContactActionVisibility& aOptionsMenuVisibility,
        MFscContactActionPluginObserver* aObserver)
    {
    FUNC_LOG;

    if (iLastEvent == EActionEventIdle)
        {
        //  We are here for the first time, function called from client
        iExecute = EFalse;
        iActionUid = aActionUid;
        iContactSet = &aContactSet;
        iActionMenuVisibility = &aActionMenuVisibility;
        iOptionsMenuVisibility = &aOptionsMenuVisibility;
        iPluginObserver = aObserver;
        iLastEvent = EActionEventCanExecuteLaunched;
        CanExecuteL(aActionUid, *iContactSet);
        }
    else
        // function called by this class
        if (iLastEvent == EActionEventCanExecuteLaunched)
            {
            TInt actionPriority = KFscActionPriorityNotAvailable;
            if (iIsNmbAvailable)
                {
                actionPriority = iParams.iUtils->ActionPriority(
                        KFscCrUidCallPtt, KFscActionPriorityCallPtt);
                aActionMenuVisibility.iVisibility
                        = TFscContactActionVisibility::EFscActionVisible;
                aOptionsMenuVisibility.iVisibility
                        = TFscContactActionVisibility::EFscActionVisible;
                }
            else
                {
                aActionMenuVisibility.iVisibility
                        = TFscContactActionVisibility::EFscActionHidden;
                aActionMenuVisibility.iReasonId = KInvalidReasonId;
                aOptionsMenuVisibility.iVisibility
                        = TFscContactActionVisibility::EFscActionHidden;
                aOptionsMenuVisibility.iReasonId = KInvalidReasonId;
                }
            if (actionPriority> 0)
                {
                // ignore any leave while updating action icons
                TRAP_IGNORE( UpdateActionIconL( aActionUid ) );
                }
            ResetData();
            iLastEvent = EActionEventIdle;
            iPluginObserver->PriorityForContactSetComplete(actionPriority);
            }

    }
// ---------------------------------------------------------------------------
// CFscPocPluginImpl::CancelPriorityForContactSet 
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::CancelPriorityForContactSet()
    {
    FUNC_LOG;
    iLastEvent = EActionEventCanceled;
    ResetData();
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::ExecuteL 
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::ExecuteL(TUid aActionUid,
        MFscContactSet& aContactSet,
        MFscContactActionPluginObserver* aObserver)
    {
    FUNC_LOG;
    // No need to call CanExecuteL() 
    // some cases like phone goes to offline mode
    // after opening the action menu, user shall be indicated
    if (aActionUid != KFscActionUidCallPtt)
        {
        ResetData();
        aObserver->ExecuteFailed(KErrArgument);
        }
    else
        {
        iContactSet = &aContactSet;
        // We will switch to execution mode
        iExecute = ETrue;

        if (iLastEvent == EActionEventIdle)
            {
            iLastEvent = EActionEventExecuteLaunched;
            if (iContactSet)
                {
                iContactSet->SetToFirstContact();
                iContactSet->SetToFirstGroup();
                }
            if ( (iContactSet->ContactCount()> 0)
                    && iContactSet->HasNextContact() )
                {
                // First we will check if any contacts (not in groups) has POC nmb
                iContactSet->NextContactL(this);
                }
            else // Empty contact set is catched by CanExecuteL
                {
                iContactSet->NextGroupL(this);
                }
            }
        else
            {
            TInt executionStatus = KErrNone;
            // Checks should be in same order
            if ((iMissingCount > 0) && (iSelectedPhNumArray->Count() == 0
                    || !iParams.iUtils->ShowNumberMissingNoteL(iMissingCount,
                            iSelectedPhNumArray->Count() + iMissingCount, 
                            R_FS_NOTE_NUMBER_MISSING, 
                            R_FS_NUMBER_MISSING_QUERY_DIALOG) ))
                {
                // user cancelled dialog
                executionStatus = KErrCancel;
                }
            else
                if (iSelectedPhNumArray->Count()> 0)
                    {
                    MakeAiwCallL();
                    }
            ResetData();
            if ( executionStatus == KErrCancel ) 
            	{
            	aObserver->ExecuteFailed( executionStatus );
            	}
            else 
            	{
            	aObserver->ExecuteComplete();
            	}
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::CancelExecute
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::CancelExecute()
    {
    FUNC_LOG;
    // close popup window for selecting number if opened
    TRAPD( err, iParams.iUtils->CloseSelectDialogL() );
        
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }  
    
    iLastEvent = EActionEventCanceled;
    ResetData();
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::GetReasonL
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::GetReasonL(TUid /* aActionUid */,
        TInt /* aReasonId */, HBufC*& /* aReason */) const
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::MakeAiwCallL
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::MakeAiwCallL()
    {
    FUNC_LOG;

    TAiwPocParameterData pocParameter;
    TUid uid =
        {
        KFscPocPluginImplImpUid
        };
    pocParameter.iConsumerAppUid = uid;
    pocParameter.iConsumerWindowGroup = CCoeEnv::Static()->RootWin().Identifier();

    if (iSelectedPhNumArray->Count() == 1)
        {
        pocParameter.iCommandId = EAiwPoCCmdTalk1to1;
        }
    else
        {
        pocParameter.iCommandId = EAiwPoCCmdTalkMany;
        }

    TAiwGenericParam param = TAiwGenericParam(EGenericParamPoCData,
            TAiwVariant(TAiwPocParameterDataPckg(pocParameter) ) );

    CAiwGenericParamList* inParamList = CAiwGenericParamList::NewLC();
    inParamList->AppendL(param);

    // Address params packing as phone number
    for (TInt i = 0; i < iSelectedPhNumArray->Count(); ++i)
        {
        TAiwGenericParam phNum = TAiwGenericParam(EGenericParamPhoneNumber,
                TAiwVariant(( *iSelectedPhNumArray )[i]) );
        inParamList->AppendL(phNum);
        }

    iAiwServiceHandler->ExecuteServiceCmdL(KAiwCmdPoC, *inParamList,
            iAiwServiceHandler->OutParamListL() );

    CleanupStack::PopAndDestroy(inParamList);

    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::CanExecuteL
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::CanExecuteL(TUid aActionUid,
        MFscContactSet& aContactSet)
    {
    FUNC_LOG;
    if (aActionUid != KFscActionUidCallPtt)
        {
        ResetData();
        iPluginObserver->PriorityForContactSetFailed(KErrArgument);
        }
    else
        {
        iContactSet = &aContactSet;
        if (iParams.iUtils->IsOfflineModeL()
                || !iParams.iUtils->IsPttConfiguredL() )
            {
            // Call PriorityForContactSetL to set ActionMenuPriority and call the client
            if (iLastEvent == EActionEventCanExecuteLaunched)
                {
                PriorityForContactSetL(aActionUid, *iContactSet,
                        *iActionMenuVisibility, *iOptionsMenuVisibility,
                        iPluginObserver);
                return;
                }
            }

        if (iContactSet)
            {
            iContactSet->SetToFirstContact();
            iContactSet->SetToFirstGroup();
            }

        // Check if any contacts exists (also in groups)
        if ( (iContactSet->ContactCount() > 0)
                && iContactSet->HasNextContact() )
            {
            // First we will check if any contacts (not in groups) has POC nmb
            iContactSet->NextContactL(this);
            }
        else
            if ( (iContactSet->GroupCount()> 0)
                    && iContactSet->HasNextGroup() )
                {
                iContactSet->NextGroupL(this);
                }
            else
                {
                //no contacts, no groups => empty contact set, no actions available
                PriorityForContactSetL(aActionUid, *iContactSet,
                        *iActionMenuVisibility, *iOptionsMenuVisibility,
                        iPluginObserver);
                }
        }
    }

// ---------------------------------------------------------------------------
// void CFscPocPluginImpl::UpdateActionIconL
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::UpdateActionIconL(TUid aActionUid)
    {
    FUNC_LOG;

    TFileName dllFileName;
    Dll::FileName(dllFileName);
    TParse parse;
    User::LeaveIfError(parse.Set(KPocPluginIconPath, &dllFileName, NULL) );

    if (aActionUid == KFscActionUidCallPtt)
        {
// <cmail> icons changed
        iContactPttAction->SetIcon(iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionPtt, parse.FullName(),
                EMbmFscpocpluginQgn_prop_cmail_action_ptt,
                EMbmFscpocpluginQgn_prop_cmail_action_ptt_mask) );
// </cmail>
        }

    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::CFscPocPluginImpl
// ---------------------------------------------------------------------------
//
CFscPocPluginImpl::CFscPocPluginImpl(
        const TFscContactActionPluginParams& aParams) :
    CFscContactActionPlugin(), iParams(aParams), iResourceHandle( 0),
            iMissingCount( 0), iExecute(EFalse),
            iLastEvent(EActionEventIdle), iSCInGroup( 0),
            iIsNmbAvailable(EFalse), iGroupIndex(0)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::ConstructL()
    {
    FUNC_LOG;
    TFileName dllFileName;
    Dll::FileName(dllFileName);
    TParse parse;
    User::LeaveIfError(parse.Set(KResourceFilePath, &dllFileName, NULL) );
    TFileName resourceFileName(parse.FullName() );
    BaflUtils::NearestLanguageFile(CCoeEnv::Static()->FsSession(), resourceFileName);

    iResourceHandle = ( CCoeEnv::Static() )->AddResourceFileL(resourceFileName);

    iActionList = new( ELeave ) CArrayFixFlat<TUid>( KActionCount );
    iActionList->AppendL(KFscActionUidCallPtt);

    User::LeaveIfError(parse.Set(KPocPluginIconPath, &dllFileName, NULL) );
    iContactPttAction = iParams.iUtils->CreateActionL( *this,
            KFscActionUidCallPtt, KFscAtComCallPoc, R_FS_ACTION_PTT,
            iParams.iUtils->SkinIconL(KAknsIIDQgnFsActionPtt,
                    parse.FullName(), EMbmFscpocpluginQgn_prop_cmail_action_ptt,
                    EMbmFscpocpluginQgn_prop_cmail_action_ptt_mask) );

    iAiwServiceHandler = CAiwServiceHandler::NewL();
    iAiwServiceHandler->AttachL(R_POC_AIW_INTEREST);
    iSelectedPhNumArray = new ( ELeave ) CDesCArraySeg( KContactListGranularity );

    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::NextContactComplete
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::NextContactComplete(MVPbkStoreContact* aContact)
    {
    FUNC_LOG;
 
    TRAPD( err, ProcessContactL( aContact ) );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::NextContactFailed
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::NextContactFailed(TInt aError)
    {
    FUNC_LOG;
    ResetData();
    if (iExecute)
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::NextGroupComplete
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::NextGroupComplete(MVPbkStoreContact* aContact)
    {
    FUNC_LOG;
    
    TRAPD( err, ProcessGroupL( aContact ) );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::NextGroupFailed
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::NextGroupFailed(TInt aError)
    {
    FUNC_LOG;
    ResetData();
    if (iExecute)
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }
// ---------------------------------------------------------------------------
// CFscPocPluginImpl::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::GetGroupContactComplete(MVPbkStoreContact* aContact)
    {
    FUNC_LOG;
    TRAPD( err, ProcessGroupContactL( aContact ) );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::GetGroupContactFailed
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::GetGroupContactFailed(TInt aError)
    {
    FUNC_LOG;
    ResetData();
    if (iExecute)
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::ResetData()
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::ResetData()
    {
    FUNC_LOG;
    if (iSelectedPhNumArray->Count())
        {
        iSelectedPhNumArray->Reset();
        }
    if ( iContactSet )
        {
        iContactSet->SetToFirstContact();
        iContactSet->SetToFirstGroup();
        TRAP_IGNORE( iContactSet->CancelNextContactL() );
        TRAP_IGNORE( iContactSet->CancelNextGroupL() );
        iContactSet = NULL;
        }
    iLastEvent = EActionEventIdle;
    iMissingCount = 0;
    iSCInGroup = 0;
    iGroup = NULL;
    iIsNmbAvailable = EFalse;
    iGroupIndex = 0;
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::ProcessContactL()
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::ProcessContactL( MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    TAddressData addressData;
    addressData.Zero();
    TInt ret = KErrNone;

    if (iExecute)
        {
        MVPbkContactLinkArray* linkArray = aContact->GroupsJoinedLC();
        TBool found = EFalse;
        
        if (linkArray->Count() > 0 && iContactSet->HasNextGroup() )
            {
            while (iContactSet->HasNextGroup() && !found)
                {
                if (linkArray->Find( *iContactSet->NextGroupLinkL() )
                        != KErrNotFound)
                    {
                    found = ETrue;
                    }
                }
            
            iContactSet->SetToFirstGroup();
            }
      
        CleanupStack::PopAndDestroy(); // linkArray
     
        if (!found)
            {
            iIsNmbAvailable = iParams.iUtils->IsContactNumberAvailableL(*aContact,
                    ETypePttNumber);
            if (iIsNmbAvailable)
                {
                ret = iParams.iUtils->GetPocAddressL( *aContact, addressData); 
                if ((ret == KErrNone) && (addressData.Length()> 0 ))
                    {
                    iSelectedPhNumArray->AppendL(addressData);
                    }
                else
                    {
                    iSelectedPhNumArray->Reset();
                    ExecuteL(iActionUid, *iContactSet, iPluginObserver);
                    }
                }
            else
                {
                iMissingCount++;
                }
            }
        // Check next contact if everything went ok earlier
        if(ret == KErrNone)
            {
            if (iContactSet->HasNextContact() )
                {
                iContactSet->NextContactL(this);
                }
            else // Check if there is another group with store contacts
                {
                if (iContactSet->HasNextGroup() )
                    {
                    iContactSet->NextGroupL(this);
                    }
                else //no more contacts with POC number
                    {
                    ExecuteL(iActionUid, *iContactSet, iPluginObserver);
                    }
                }
            }
        }  
    else // iExecute == EFalse
        {
        iIsNmbAvailable = iParams.iUtils->IsContactNumberAvailableL(*aContact,
                ETypePttNumber);
        if (iIsNmbAvailable)
            {
            PriorityForContactSetL(iActionUid, *iContactSet,
                    *iActionMenuVisibility, *iOptionsMenuVisibility,
                    iPluginObserver);
            }
        else
            {
            // Check if another contact from that group has a POC address
            if (iContactSet->HasNextContact() )
                {
                iContactSet->NextContactL(this);
                }
            else // Check if there is another group with store contacts
                {
                if (iContactSet->HasNextGroup() )
                    {
                    iContactSet->NextGroupL(this);
                    }
                else //no more contacts with POC number
                    {
                    PriorityForContactSetL(iActionUid, *iContactSet,
                            *iActionMenuVisibility, *iOptionsMenuVisibility,
                            iPluginObserver);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::ProcessGroupL()
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::ProcessGroupL( MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    // just in case
    iSCInGroup = 0;
    iGroupIndex++;
    iGroup = aContact;

    if ( iContactSet->GroupContactCountL( *iGroup )> 0 )
        {
        iContactSet->GetGroupContactL( *iGroup, iSCInGroup++, this );
        }
    else
        {
        // in case that we have group without contacts
        if ( iContactSet->HasNextGroup() )
            {
            iContactSet->NextGroupL(this);
            }
        else
            {
            if ( iExecute )
                {
                ExecuteL( iActionUid, *iContactSet, iPluginObserver );
                }
            else
                {
                PriorityForContactSetL( iActionUid, *iContactSet,
                        *iActionMenuVisibility, *iOptionsMenuVisibility,
                        iPluginObserver );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CFscPocPluginImpl::ProcessGroupContactL()
// ---------------------------------------------------------------------------
//
void CFscPocPluginImpl::ProcessGroupContactL( MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    TAddressData addressData;
    aContact->PushL();
    addressData.Zero();
    TInt ret = KErrNone;

    if ( iExecute )
        {
        // Check if contact belongs to other gruops, if yes -> ignore it
        MVPbkContactLinkArray* linkArray = aContact->GroupsJoinedLC();
        TInt linkArrayCount = linkArray->Count();

        TBool found = EFalse;
        TBool isChanged = EFalse;
        if ( linkArrayCount > 0 && iContactSet->HasNextGroup() )
            {
            while ( iContactSet->HasNextGroup() && !found )
                {
                isChanged = ETrue;
                if ( linkArray->Find( *iContactSet->NextGroupLinkL() )
                        != KErrNotFound)
                    {
                    found = ETrue;
                    }
                }

            if ( isChanged )
                {
                //if iterator moved
                TInt aIndex = iGroupIndex-1;
                iContactSet->SetToFirstGroup();
                //move iterator to specified index
                while ( iContactSet->HasNextGroup() && aIndex-- > 0 )
                    {
                    iContactSet->NextGroupLinkL();
                    }
                }
            }
        CleanupStack::PopAndDestroy(); //linkArray
        if ( !found )
            {
            iIsNmbAvailable = iParams.iUtils->IsContactNumberAvailableL(
                    *aContact, ETypePttNumber );
            if ( iIsNmbAvailable )
                {
                ret = iParams.iUtils->GetPocAddressL( *aContact, addressData); 
                if ( ( ret == KErrNone ) && ( addressData.Length() > 0 ) )
                    {
                    iSelectedPhNumArray->AppendL( addressData );
                    }
                else
                    {
                    iSelectedPhNumArray->Reset();
                    ExecuteL( iActionUid, *iContactSet, iPluginObserver );
                    }
                }
            else
                {
                iMissingCount++;
                }
            }
        // Check if another contact from that group has a POC address
        if( ret == KErrNone )
            {
            if ( iContactSet->GroupContactCountL(*iGroup) > iSCInGroup )
                {
                iContactSet->GetGroupContactL( *iGroup, iSCInGroup++, this );
                }
            else
                {
                // Check if there is another group with store contacts
                if ( iContactSet->HasNextGroup() )
                    {
                    iContactSet->NextGroupL( this );
                    }
                else //no more contacts with POC number
                    {
                    ExecuteL( iActionUid, *iContactSet, iPluginObserver );
                    }
                }
            }
        }
    else // iExecute == EFalse
        {
        iIsNmbAvailable = iParams.iUtils->IsContactNumberAvailableL( 
                *aContact,
                ETypePttNumber );
        if ( iIsNmbAvailable )
            {
            PriorityForContactSetL( iActionUid, *iContactSet,
                    *iActionMenuVisibility, *iOptionsMenuVisibility,
                    iPluginObserver );
            }
        else
            {
            // Check if another contact from that group has a POC address
            if ( iContactSet->GroupContactCountL(*iGroup) > iSCInGroup )
                {
                iContactSet->GetGroupContactL( *iGroup, iSCInGroup++, this );
                }
            else // Check if there is another group with store contacts
                {
                if ( iContactSet->HasNextGroup() )
                    {
                    iContactSet->NextGroupL( this );
                    }
                else //no more contacts with POC number
                    {
                    PriorityForContactSetL( iActionUid, *iContactSet,
                            *iActionMenuVisibility, *iOptionsMenuVisibility,
                            iPluginObserver );
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(); //aContact
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

