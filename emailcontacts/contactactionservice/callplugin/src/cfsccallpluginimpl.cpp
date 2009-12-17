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
* Description:  Implementation of the class CFscCallPluginImpl.
*
*/


#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include <AiwServiceHandler.h>
#include <bautils.h>
#include <coemain.h>
#include <AknsConstants.h>
#include <RPbkViewResourceFile.h>
#include <FscActionUtils.rsg>
#include <fsccallplugin.rsg>
#include <fsccallplugin.mbg>
//<cmail>
#include "cfsccontactaction.h"
#include "mfsccontactset.h"
#include "mfscactionutils.h"
#include "mfscreasoncallback.h"
#include "mfsccontactactionpluginobserver.h"
//</cmail>
#include <AiwCommon.hrh>
#include <MVPbkStoreContact.h>
#include <aiwdialdata.h>
#include <settingsinternalcrkeys.h>
#include <centralrepository.h>
#include <e32debug.h>

#include "fscactionpluginactionuids.h"
#include "fscactionplugincrkeys.h"
#include "cfsccallpluginimpl.h"

// CONSTANTS DECLARATIONS
const TInt KActionCount = 4;
const TInt KInvalidReasonId = 0;
const TInt KMaxLengthOfNumberString = 310;
const TInt KMaxConfIdLen = 32;
const TInt KMaxConfPinLen = 32;
const TInt KRCSECSCallPreferred = 0;
const TInt KRCSEInternetCallPreferred = 1;

_LIT( KCallPluginIconPath, "\\resource\\apps\\fsccallplugin.mif" );
_LIT( KResourceFilePath, "\\resource\\apps\\fsccallplugin.rsc");
_LIT( KDtmfPause, "p");

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
        KFscCallPluginImplImpUid,
        CFscCallPluginImpl::NewL )
    };
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::NewL
// ---------------------------------------------------------------------------
//
CFscCallPluginImpl* CFscCallPluginImpl::NewL( TAny* aParams )
    {
    FUNC_LOG;

    TFscContactActionPluginParams* params = 
        reinterpret_cast< TFscContactActionPluginParams* >( aParams );
            
    CFscCallPluginImpl* self = new( ELeave ) CFscCallPluginImpl( *params );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::~CFscCallPluginImpl
// ---------------------------------------------------------------------------
//
CFscCallPluginImpl::~CFscCallPluginImpl()
    {
    FUNC_LOG;
    delete iContactCallGsmAction;
    delete iContactCallVideoAction;
    delete iContactCallConfNumAction;
    delete iContactCallVoipAction;     
    delete iActionList;
    delete iAiwServiceHandler;
    CCoeEnv::Static()->DeleteResourceFile( iResourceHandle );
    }
    
// ---------------------------------------------------------------------------
// CFscCallPluginImpl::Uid
// ---------------------------------------------------------------------------
//
TUid CFscCallPluginImpl::Uid() const 
    {
    FUNC_LOG;
    TUid uid = { KFscCallPluginImplImpUid };
    return uid;
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::ActionList
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>* CFscCallPluginImpl::ActionList() const
    {
    return iActionList;
    }
    
// ---------------------------------------------------------------------------
// CFscCallPluginImpl::GetActionL
// ---------------------------------------------------------------------------
//
const MFscContactAction& CFscCallPluginImpl::GetActionL(
    TUid aActionUid ) const
    {
	FUNC_LOG;
    
    const MFscContactAction* action = NULL;

    if ( aActionUid == KFscActionUidCallGsm )
        {
        action = iContactCallGsmAction;
        }
    else if ( aActionUid == KFscActionUidCallVideo )
        {
        action = iContactCallVideoAction;
        }
    else if ( aActionUid == KFscActionUidCallConfNum )
        {
        action = iContactCallConfNumAction;
        }
    else if ( aActionUid == KFscActionUidCallVoip )
        {
        action = iContactCallVoipAction;
        }     
    else
        {
        User::Leave( KErrNotFound );
        }
        
    return *action;
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::PriorityForContactSetL
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::PriorityForContactSetL( 
    TUid aActionUid,
    MFscContactSet& aContactSet,
    TFscContactActionVisibility& aActionMenuVisibility,
    TFscContactActionVisibility& aOptionsMenuVisibility,
    MFscContactActionPluginObserver* aObserver )
    {
    FUNC_LOG;    
    
    switch( iLastEvent )
        {
        case EActionEventIdle:
            {
            iActionUid = aActionUid;
            iContactSet = &aContactSet;
            iActionMenuVisibility = &aActionMenuVisibility;
            iOptionsMenuVisibility = &aOptionsMenuVisibility;
            iPluginObserver = aObserver;

            iIsExecute = EFalse;
            
            iContactSet->SetToFirstContact();
            iContactSet->SetToFirstGroup();
                
            iActionPriority = KFscActionPriorityNotAvailable;
            
            iCanDisplay = KErrNone;
            if ( aActionUid == KFscActionUidCallGsm )
                {
                iCanDisplay = ( aContactSet.GroupCount() == 0 && aContactSet.ContactCount() == 1 ) ?
                    KErrNone : KErrArgument;
                iLastEvent = EActionEventCanExecuteFinished;
                
                PriorityForContactSetL( 
                            aActionUid,
                            aContactSet,
                            aActionMenuVisibility,
                            aOptionsMenuVisibility,
                            aObserver );
                //No need to check for number            
                }
            else // show other actions only if it can be executed
                {
                iLastEvent = EActionEventCanExecuteLaunched;
                CanExecuteL( aActionUid, aContactSet );
                }
            break;
            }
        case EActionEventCanExecuteFinished:
            {
            
            if ( iCanDisplay == KErrNone )
                 {
                 aActionMenuVisibility.iVisibility = 
                     TFscContactActionVisibility::EFscActionVisible;
                 aOptionsMenuVisibility.iVisibility = 
                     TFscContactActionVisibility::EFscActionVisible;
                 
                 if ( aActionUid == KFscActionUidCallGsm )
                     {
                     iActionPriority = 
                         iParams.iUtils->ActionPriority( 
                             KFscCrUidCallGsm,
                             KFscActionPriorityCallGsm );
                     }
                 else if ( aActionUid == KFscActionUidCallVideo )
                     {
                     // Video call is visible in Options menu 
                     // even if 3G n/w is not available
                     if ( iParams.iUtils->IsOfflineModeL() ||
                         !iParams.iUtils->Is3GNetworkAvailable() )
                         {
                         aOptionsMenuVisibility.iVisibility = 
                             TFscContactActionVisibility::EFscActionVisible;
                         aActionMenuVisibility.iVisibility = 
                             TFscContactActionVisibility::EFscActionHidden;
                         aActionMenuVisibility.iReasonId = KInvalidReasonId;
                         }
                     iActionPriority = 
                         iParams.iUtils->ActionPriority( 
                             KFscCrUidCallVideo,
                             KFscActionPriorityCallVideo );
                     }
                 else if ( aActionUid == KFscActionUidCallConfNum )
                     {
                     iActionPriority = 
                         iParams.iUtils->ActionPriority( 
                             KFscCrUidCallConfNum,
                             KFscActionPriorityCallConfNum );
                     }           
                 else if ( aActionUid == KFscActionUidCallVoip )
                     {
                     iActionPriority = 
                         iParams.iUtils->ActionPriority( 
                             KFscCrUidCallVoip,
                             KFscActionPriorityCallVoip );
                     if ( !iParams.iUtils->IsVoipConfiguredL() )
                         {
                         // If voip is not configured, set voip hidden
                         aActionMenuVisibility.iVisibility = 
                             TFscContactActionVisibility::EFscActionHidden;
                         aActionMenuVisibility.iReasonId = KInvalidReasonId;
                         aOptionsMenuVisibility.iVisibility = 
                             TFscContactActionVisibility::EFscActionHidden;
                         aOptionsMenuVisibility.iReasonId = KInvalidReasonId;
                         }
                     }        
                 }
             else
                 {
                 iActionPriority = KFscActionPriorityNotAvailable;
                 aActionMenuVisibility.iVisibility = 
                     TFscContactActionVisibility::EFscActionHidden;
                 aActionMenuVisibility.iReasonId = KInvalidReasonId;
                 aOptionsMenuVisibility.iVisibility = 
                     TFscContactActionVisibility::EFscActionHidden;
                 aOptionsMenuVisibility.iReasonId = KInvalidReasonId;
                 }

             if ( iActionPriority > 0 )
                 {
                 // ignore any leave while updating action icons
                 TRAP_IGNORE( UpdateActionIconL( aActionUid ) );
                 }
            ResetData();
            aObserver->PriorityForContactSetComplete(iActionPriority);
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
// CFscCallPluginImpl::ExecuteL 
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::ExecuteL( 
     TUid aActionUid,
     MFscContactSet& aContactSet,
     MFscContactActionPluginObserver* aObserver )
        {
		FUNC_LOG;

        TBuf<KMaxLengthOfNumber> vPbkPhNo;
        
        CAiwDialData::TCallType callType = CAiwDialData::EAIWVoice;
        
        switch ( iLastEvent )
            {
            case EActionEventIdle:
                {
                
                iLastEvent = EActionEventCanExecuteLaunched;
                
                iActionUid = aActionUid;
                iContactSet = &aContactSet;
                iPluginObserver = aObserver;                      
                
                iIsExecute = ETrue;
                                
                iContactSet->SetToFirstContact();
                iContactSet->SetToFirstGroup();
                
                CanExecuteL( aActionUid, aContactSet );
                
                break;
                }
                
            case EActionEventCanExecuteFinished:
                {
                if ( aActionUid == KFscActionUidCallGsm )
                    {                              
                    if( iCanDisplay == KErrNone )
                        {
                        callType = CAiwDialData::EAIWVoice;
                        iParams.iUtils->GetVoiceCallNumberL( *iRetrievedStoreContact, vPbkPhNo );
                        }
                    else //Phone number is missing
                        {
                        
                        HBufC* contactName =
                            iParams.iUtils->GetContactNameL( *iRetrievedStoreContact );
                        CleanupStack::PushL( contactName );
                        iParams.iUtils->ShowInfoNoteL(
                            R_QTN_FS_NOTE_NO_NUMBER_TO_NAME,
                            contactName );
                        CleanupStack::PopAndDestroy( contactName );
                        
                        ResetData();
                        aObserver->ExecuteComplete();
                        return;
                        }
                    }
                else if ( aActionUid == KFscActionUidCallVideo )
                    {                                
                    callType = CAiwDialData::EAIWForcedVideo;
                    TInt err = iParams.iUtils->GetVideoCallNumberL( 
                            *iRetrievedStoreContact, vPbkPhNo );
                    
                    if ( err == KErrNotFound )
                        {
                        err = iParams.iUtils->GetVoiceCallNumberL( 
                                *iRetrievedStoreContact, vPbkPhNo );
                        }
                    //Phone number is missing
                    if (err == KErrNotFound )    
                    	{
                        
                        HBufC* contactName =
                            iParams.iUtils->GetContactNameL( *iRetrievedStoreContact );
                        CleanupStack::PushL( contactName );
                        iParams.iUtils->ShowInfoNoteL(
                            R_QTN_FS_NOTE_NO_NUMBER_TO_NAME,
                            contactName );
                        CleanupStack::PopAndDestroy( contactName );
                        
                        ResetData();
                        aObserver->ExecuteComplete();
                        return;
                        }
                    }
                else if ( aActionUid == KFscActionUidCallConfNum )
                    {
                    //Fix for: TJUA-7RRG58
                    TInt value( KErrNotFound );

                    CRepository* rep = CRepository::NewL( KCRUidRichCallSettings );
                    rep->Get( KRCSEPreferredTelephony, value );
                    if ( KRCSEInternetCallPreferred == value )
                        {
                        callType = CAiwDialData::EAIWVoiP;
                        }
                    else
                        {
                        callType = CAiwDialData::EAIWVoice;
                        }
                    delete rep;
                    //end for fix
                    
                    TBuf<KMaxLengthOfNumberString> numberWithDtmf;
                    TBuf<KMaxConfIdLen> confNum;
                    TBuf<KMaxConfIdLen> confId;
                    TBuf<KMaxConfPinLen> confPin;
                    iParams.iUtils->GetConfInfoL( *iRetrievedStoreContact,
                            confNum, confId, confPin );
                
                    numberWithDtmf.Append( confNum );
                    if ( confId.Length() > 0 )
                        {
                        numberWithDtmf.Append( KDtmfPause );
                        numberWithDtmf.Append( confId );
                        if ( confPin.Length() > 0 )
                            {
                            numberWithDtmf.Append( KDtmfPause );
                            numberWithDtmf.Append( confPin );
                            }
                        }
                    vPbkPhNo.Copy( numberWithDtmf.Left( KMaxLengthOfNumber ) );
                    }
                 else if ( aActionUid == KFscActionUidCallVoip )
                       {
                       callType = CAiwDialData::EAIWVoiP;
                       TInt err = iParams.iUtils->GetVoipAddressL( 
                               *iRetrievedStoreContact, vPbkPhNo );
                       
                       if ( err == KErrNotFound )
                           {
                           err = iParams.iUtils->GetVoiceCallNumberL( 
                                   *iRetrievedStoreContact, vPbkPhNo );
                           }
                       
                       if ( err == KErrNotFound ) //Phone number is missing 
						   {
						   HBufC* contactName =
							   iParams.iUtils->GetContactNameL( *iRetrievedStoreContact );
						   CleanupStack::PushL( contactName );
						   iParams.iUtils->ShowInfoNoteL(
							   R_QTN_FS_NOTE_NO_VOIP_TO_NAME,
							   contactName );
						   CleanupStack::PopAndDestroy( contactName );
						   
						   ResetData();
						   aObserver->ExecuteComplete();
						   return;
						   }
                       }

                if ( vPbkPhNo.Length() > 0 )
                     {
                     MakeAiwCallL( vPbkPhNo, callType );
                     }      
                ResetData();
                aObserver->ExecuteComplete();
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
// CFscCallPluginImpl::GetReasonL
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::GetReasonL(
    TUid /* aActionUid */,
    TInt /* aReasonId */,
    HBufC*& /* aReason */ ) const
    {
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::MakeAiwCallL
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::MakeAiwCallL(
    const TDesC& aNumber,
    CAiwDialData::TCallType aCallType )
    {
	FUNC_LOG;
    
    CAiwDialData* dialdata = CAiwDialData::NewLC();
    CRepository* rep = CRepository::NewL( KCRUidRichCallSettings );
    TInt ret(KErrNone);
    TInt err(KErrNone);
    TInt options(0);
    options |= KAiwOptASyncronous;
    
    dialdata->SetCallType( aCallType );
    dialdata->SetPhoneNumberL( aNumber );
    dialdata->SetWindowGroup( CCoeEnv::Static()->RootWin().Identifier() );

    CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
    dialdata->FillInParamListL( paramList );

    if ( aCallType == CAiwDialData::EAIWVoice )
    	{
        TInt value( KErrNotFound );
	    // force call type to voice (call AIW interface doens't have this option,
	    // so we must change the preferred call value in cenrep for the duration
	    // of the function call)
        if ( rep )
            {
            rep->Get( KRCSEPreferredTelephony, value );
            }
	    if ( KRCSEInternetCallPreferred == value )
		    {
		    iPreferredCallValueChanged = ETrue;
		    ret = rep->Set( KRCSEPreferredTelephony, KRCSECSCallPreferred ); // must not leave before this is returned to KRCSEInternetCallPreferred
		    }
    	}
	delete rep;
    if( ( iPreferredCallValueChanged && ret == KErrNone ) || !iPreferredCallValueChanged )
    	{
	    TRAP(err, iAiwServiceHandler->ExecuteServiceCmdL
	            ( KAiwCmdCall, 
	              paramList, 
	              iAiwServiceHandler->OutParamListL(),
	              options, 
	              this ));
    	}

    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(); // this
    } 
        
// ---------------------------------------------------------------------------
// CFscCallPluginImpl::CanExecuteL
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::CanExecuteL(
    TUid aActionUid,
    MFscContactSet& aContactSet )
    {
	FUNC_LOG;
    
    switch(iLastEvent)
        {
        case EActionEventCanExecuteLaunched:
            {
            if ( aActionUid == KFscActionUidCallGsm )
                {
                iCanDisplay = ( aContactSet.GroupCount() == 0 
                        && aContactSet.ContactCount() == 1 ) ?
                    KErrNone : KErrArgument;
                if ( iCanDisplay == KErrNone )
                    {
                    iLastEvent = EActionEventContactRetrieve;
                    aContactSet.NextContactL(this);
                    }
                else 
                    {
                    iLastEvent = EActionEventCanExecuteFinished;
                    ResumeAsync(aActionUid, aContactSet, iIsExecute);
                    }
                }
            else if ( aActionUid == KFscActionUidCallVideo )
                {
                iCanDisplay = ( aContactSet.GroupCount() == 0 
                        && aContactSet.ContactCount() == 1 ) ?
                    KErrNone : KErrArgument;
                if ( iCanDisplay == KErrNone )
                    {
                    iLastEvent = EActionEventContactRetrieve;
                    aContactSet.NextContactL(this);
                    }
                else 
                    {
                    iLastEvent = EActionEventCanExecuteFinished;
                    ResumeAsync(aActionUid, aContactSet, iIsExecute);
                    }
                // 3G Network availability is checked inside 
                // PriorityForContactSetL()
                }        
            else if ( aActionUid == KFscActionUidCallConfNum )
                {
                // One Group and Zero Contacts
                iCanDisplay = ( aContactSet.GroupCount() == 1 
                        && aContactSet.ContactCount() == 0 ) ?
                        KErrNone : KErrArgument;
                if ( iCanDisplay == KErrNone )
                    {
                    iLastEvent = EActionEventGroupRetrieve;
                    aContactSet.NextGroupL(this);
                    }
                else 
                    {
                    iLastEvent = EActionEventCanExecuteFinished;
                    ResumeAsync(aActionUid, aContactSet, iIsExecute);
                    }
                }             
                else if ( aActionUid == KFscActionUidCallVoip )
                    {
                    iCanDisplay = ( aContactSet.GroupCount() == 0 
                            && aContactSet.ContactCount() == 1 ) ?
                        KErrNone : KErrArgument;
                    if ( iCanDisplay == KErrNone )
                        {
                        iLastEvent = EActionEventContactRetrieve;
                        aContactSet.NextContactL(this);
                        }
                    else 
                        {
                        iLastEvent = EActionEventCanExecuteFinished;
                        ResumeAsync(aActionUid, aContactSet, iIsExecute);
                        }
                    }        
                else
                    {
                    iLastEvent = EActionEventIdle;
                    ResetData();
                    iPluginObserver->PriorityForContactSetFailed(KErrArgument);
                    }

                break;
                }
            case EActionEventCanExecuteFinished:
                {
                if ( aActionUid == KFscActionUidCallGsm )
                    {
                    iCanDisplay = 
                        (iParams.iUtils->IsContactNumberAvailableL( 
                                *iRetrievedStoreContact, 
                                ETypeVoiceCallNumber ) ) ? KErrNone : 
                                    KErrArgument;
                    
                    ResumeAsync(aActionUid, aContactSet, iIsExecute);
                    }
                else if ( aActionUid == KFscActionUidCallVideo )
                    {
                    iCanDisplay = ( iParams.iUtils->IsContactNumberAvailableL( 
                            *iRetrievedStoreContact, ETypeVideoCallNumber ) ) 
                            ? KErrNone : KErrArgument;
                    
                    // or phone number if internet call number is missing
                    if ( iCanDisplay == KErrArgument )
                        {
                        iCanDisplay = ( 
                                iParams.iUtils->IsContactNumberAvailableL( 
                                        *iRetrievedStoreContact, 
                                        ETypeVoiceCallNumber ) ) ? KErrNone : 
                                            KErrArgument;
                        }
                    
                    ResumeAsync(aActionUid, aContactSet, iIsExecute);
                    // 3G Network availability is checked inside 
                    // PriorityForContactSetL()
                    }
                else if ( aActionUid == KFscActionUidCallConfNum )
                    {
                    iCanDisplay = ( iParams.iUtils->IsContactNumberAvailableL( 
                            *iRetrievedStoreContact, 
                        ETypeConfNumber ) ) ? KErrNone : KErrArgument;
                    
                    ResumeAsync(aActionUid, aContactSet, iIsExecute);
                    }   
                else if ( aActionUid == KFscActionUidCallVoip )
                    {
                    // Intenet call number
                    iCanDisplay = ( iParams.iUtils->IsContactNumberAvailableL( 
                            *iRetrievedStoreContact, ETypeInternetTel ) ) ? 
                                    KErrNone : KErrArgument;
                    
                    // or phone number if internet call number is missing
                    if ( iCanDisplay == KErrArgument )
                    	{
                    	iCanDisplay = ( 
                    	        iParams.iUtils->IsContactNumberAvailableL( 
                    	                *iRetrievedStoreContact, 
                    	                ETypeVoiceCallNumber ) ) ? KErrNone : 
                    	                    KErrArgument;
                    	}
                        
                    ResumeAsync( aActionUid, aContactSet, iIsExecute );
                    }     
                else
                    {
                    ResetData();
                    if (iIsExecute)
                        {
                        iPluginObserver->ExecuteFailed(KErrArgument);
                        }
                    else
                        {
                        iPluginObserver->PriorityForContactSetFailed(
                                KErrArgument);
                        }
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
// CFscCallPluginImpl::UpdateActionIconL
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::UpdateActionIconL( TUid aActionUid )
    {
	FUNC_LOG;

    TFileName dllFileName;
    Dll::FileName( dllFileName );
    TParse parse;
    User::LeaveIfError(
        parse.Set( KCallPluginIconPath, &dllFileName, NULL ) );

// <cmail> icons changed
    if ( aActionUid == KFscActionUidCallGsm )
        {
        iContactCallGsmAction->SetIcon( iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionCall,
                parse.FullName(),
                EMbmFsccallpluginQgn_prop_cmail_action_call,
                EMbmFsccallpluginQgn_prop_cmail_action_call_mask ) );
        }
    else if ( aActionUid == KFscActionUidCallVideo )
        {
        iContactCallVideoAction->SetIcon( iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionVidcall,
                parse.FullName(),
                EMbmFsccallpluginQgn_prop_cmail_action_call_video,
                EMbmFsccallpluginQgn_prop_cmail_action_call_video_mask ) );        
        }
    else if ( aActionUid == KFscActionUidCallConfNum )
        {
        iContactCallConfNumAction->SetIcon( iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionConfService,
                parse.FullName(),
                EMbmFsccallpluginQgn_prop_cmail_action_conf_service,
                EMbmFsccallpluginQgn_prop_cmail_action_conf_service_mask ) );
        }
    else if ( aActionUid == KFscActionUidCallVoip )
        {
        iContactCallVoipAction->SetIcon( iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionVoip,
                parse.FullName(),
                EMbmFsccallpluginQgn_prop_cmail_action_voip,
                EMbmFsccallpluginQgn_prop_cmail_action_voip_mask ) );        
        }
// </cmail>    
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::CFscCallPluginImpl
// ---------------------------------------------------------------------------
//
CFscCallPluginImpl::CFscCallPluginImpl(
    const TFscContactActionPluginParams& aParams )
    : CFscContactActionPlugin(),
    iParams( aParams ),
    iResourceHandle( 0 ),
    iLastEvent( EActionEventIdle ),
    iCanDisplay( KErrNone ),
    iIsExecute( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::ConstructL()
    {
	FUNC_LOG;
    TFileName dllFileName;
    Dll::FileName( dllFileName );
    TParse parse;
    User::LeaveIfError( parse.Set( KResourceFilePath, &dllFileName, NULL ) );
    TFileName resourceFileName( parse.FullName() );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), 
        resourceFileName );
    
    iResourceHandle = 
        ( CCoeEnv::Static() )->AddResourceFileL( resourceFileName );

    iActionList = new( ELeave ) CArrayFixFlat<TUid>( KActionCount );
    
    User::LeaveIfError(
        parse.Set( KCallPluginIconPath, &dllFileName, NULL ) );

// <cmail> icons changed    
    iActionList->AppendL( KFscActionUidCallGsm );
    iContactCallGsmAction = iParams.iUtils->CreateActionL(
        *this,
        KFscActionUidCallGsm,
        KFscAtComCallGSM,
        R_FS_ACTION_CALL,
        iParams.iUtils->SkinIconL(
            KAknsIIDQgnFsActionCall,
            parse.FullName(),
            EMbmFsccallpluginQgn_prop_cmail_action_call,
            EMbmFsccallpluginQgn_prop_cmail_action_call_mask ) );

    iActionList->AppendL( KFscActionUidCallVideo );
    iContactCallVideoAction = iParams.iUtils->CreateActionL(
        *this,
        KFscActionUidCallVideo,
        KFscAtComCallVideo,
        R_FS_ACTION_VIDCALL,
        iParams.iUtils->SkinIconL(
            KAknsIIDQgnFsActionVidcall,
            parse.FullName(),
            EMbmFsccallpluginQgn_prop_cmail_action_call_video,
            EMbmFsccallpluginQgn_prop_cmail_action_call_video_mask ) );

    iActionList->AppendL( KFscActionUidCallConfNum );
    iContactCallConfNumAction = iParams.iUtils->CreateActionL(
        *this,
        KFscActionUidCallConfNum,
        KFscAtComCallConfNum,
        R_FS_ACTION_CONF_SERVICE,
        iParams.iUtils->SkinIconL(
            KAknsIIDQgnFsActionConfService,
            parse.FullName(),
            EMbmFsccallpluginQgn_prop_cmail_action_conf_service,
            EMbmFsccallpluginQgn_prop_cmail_action_conf_service_mask ) );

    iActionList->AppendL( KFscActionUidCallVoip );
    iContactCallVoipAction = iParams.iUtils->CreateActionL(
        *this,
        KFscActionUidCallVoip,
        KFscAtComCallVoip,
        R_FS_ACTION_VOIP,
        iParams.iUtils->SkinIconL(
            KAknsIIDQgnFsActionVoip,
            parse.FullName(),
            EMbmFsccallpluginQgn_prop_cmail_action_voip,
            EMbmFsccallpluginQgn_prop_cmail_action_voip_mask ) );
// </cmail>

    iAiwServiceHandler = CAiwServiceHandler::NewL();
    iAiwServiceHandler->AttachL( R_CALLPLUGIN_INTEREST );
    
    iPreferredCallValueChanged = EFalse;
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::ResetData
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::ResetData()
    {
	FUNC_LOG;
    if (iContactSet)
        {
        // <cmail>
        TRAP_IGNORE( iContactSet->CancelNextContactL() );
        TRAP_IGNORE( iContactSet->CancelNextGroupL() );
        // </cmail>
        iContactSet->SetToFirstContact();
        iContactSet->SetToFirstGroup();
        iContactSet = NULL;
        }
    
    iLastEvent = EActionEventIdle;
    iActionMenuVisibility = NULL;
    iOptionsMenuVisibility = NULL;
    iCanDisplay = KErrNone;
    iRetrievedStoreContact = NULL;
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::ResumeAsync
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::ResumeAsync(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            TBool aIsExecute )
    {
	FUNC_LOG;
    TInt err( KErrNone );
    if (aIsExecute)
        {
        TRAP( err, ExecuteL( aActionUid, aContactSet, iPluginObserver ) );
        }
    else 
        {
        TRAP( err, PriorityForContactSetL( 
            aActionUid,
            aContactSet,
            *iActionMenuVisibility,
            *iOptionsMenuVisibility,
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
// CFscCallPluginImpl::NextContactComplete
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::NextContactComplete( MVPbkStoreContact* aContact )
    {
	FUNC_LOG;
    TInt err( KErrNone );
    switch(iLastEvent)
        {
        case EActionEventContactRetrieve:
            {
            iRetrievedStoreContact = aContact;
            iLastEvent = EActionEventCanExecuteFinished;
            TRAP( err, CanExecuteL( iActionUid, *iContactSet ) );
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
// CFscCallPluginImpl::NextContactFailed
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::NextContactFailed( TInt aError )
    {
	FUNC_LOG;
    ResetData();
    if (iIsExecute)
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::NextGroupComplete
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::NextGroupComplete( MVPbkStoreContact* aContact )
    {
	FUNC_LOG;
    TInt err( KErrNone );
    switch(iLastEvent)
        {
        case EActionEventGroupRetrieve:
            {
            iLastEvent = EActionEventCanExecuteFinished;
            iRetrievedStoreContact = aContact;
            TRAP( err, CanExecuteL( iActionUid, *iContactSet ) );
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
// CFscCallPluginImpl::NextGroupFailed
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::NextGroupFailed( TInt aError )
    {
	FUNC_LOG;
    ResetData();
    if (iIsExecute)
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::GetGroupContactComplete( MVPbkStoreContact* /*aContact*/ )
    {    
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::GetGroupContactFailed( TInt aError )
    {
	FUNC_LOG;
    ResetData();
    if (iIsExecute)
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::CancelPriorityForContactSet
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::CancelPriorityForContactSet()
    {
	FUNC_LOG;
    iLastEvent = EActionEventCanceled;
    ResetData();
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::CancelExecute
// ---------------------------------------------------------------------------
//
void CFscCallPluginImpl::CancelExecute()
    {
	FUNC_LOG;
    // set action
    iLastEvent = EActionEventCanceled;
    ResetData();
    
    // close popup window for selecting number if opened
    TRAPD( err, iParams.iUtils->CloseSelectDialogL() );
    
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }    
    }

// ---------------------------------------------------------------------------
// CFscCallPluginImpl::HandleNotifyL
// ---------------------------------------------------------------------------
//
TInt CFscCallPluginImpl::HandleNotifyL(
        TInt /*aCmdId*/,
        TInt /*aEventId*/,
        CAiwGenericParamList& /*aEventParamList*/,
        const CAiwGenericParamList& /*aInParamList*/)
	{
	FUNC_LOG;
	if( iPreferredCallValueChanged )
		{
		CRepository* rep = CRepository::NewL( KCRUidRichCallSettings );
		
		if ( rep )
		    {
		    rep->Set( KRCSEPreferredTelephony, KRCSEInternetCallPreferred );
		    }
		delete rep;
		iPreferredCallValueChanged = EFalse;
		}
	return KErrNone;
	}

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
	FUNC_LOG;
    aTableCount = 
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

