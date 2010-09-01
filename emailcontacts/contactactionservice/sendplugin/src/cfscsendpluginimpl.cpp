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
* Description:  Implementation of the class CFscSendPluginImpl.
*
*/


#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include <bautils.h>
#include <CMessageData.h>
#include <sendui.h>
#include <sysutil.h>
#include <SenduiMtmUids.h>
#include <TSendingCapabilities.h>
#include <AknQueryDialog.h>
#include <AknsConstants.h>
#include <RPbkViewResourceFile.h> 
//<cmail>
#include "mfsccontactset.h"
#include "mfscactionutils.h"
#include <FscActionUtils.rsg>
#include <fscsendplugin.rsg>
#include <fscsendplugin.mbg>
#include "cfsccontactaction.h"
#include <StringLoader.h>
#include <featmgr.h>
#include "fsccontactactionserviceuids.hrh"
#include "mfsccontactactionpluginobserver.h"
//</cmail>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactIdConverter.h>

#include "fscactionpluginactionuids.h"
#include "cfscsendpluginimpl.h"
#include "cfscattachmentfile.h"
#include "fscactionplugincrkeys.h"

// CONSTANTS DECLARATIONS


// <cmail> flag not needed  here

const TInt KMaxContactIdStringLength = 10;
const TInt KMsgSpaceEstimationBytes = 10240;
const TInt KMaxLenghtOfAddressData = 255;

const TUid KCRUidUniEditor = {0x102072E5};
const TUint32 KUniEditorMaxRecipientCount = 0x00000002;
const TUint32 KUniEditorSoftLimitRecipientCount = 0x00000003;

const TInt KDefaultSmsRecipients = 20;
const TInt KDefaultMmsRecipients = 100;
const TInt KDefaultEmailRecipients = KMaxTInt;

const TInt KActionCount = 4;
const TInt KInvalidReasonId = 0;
//const TInt KContactListGranularity = 2;

_LIT( KSendPluginIconPath, "\\resource\\apps\\fscsendplugin.mif" );
_LIT( KResourceFilePath, "\\resource\\apps\\fscsendplugin.rsc");

// typedef declarations
typedef TBuf<KMaxLenghtOfAddressData> TAddressData;


const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
        KFscSendPluginImplImpUid,
        CFscSendPluginImpl::NewL )
    };
	
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::NewL
// ---------------------------------------------------------------------------
//
CFscSendPluginImpl* CFscSendPluginImpl::NewL( TAny* aParams )
    {
    FUNC_LOG;

    TFscContactActionPluginParams* params = 
            reinterpret_cast< TFscContactActionPluginParams* >( aParams );
            
    CFscSendPluginImpl* self = new( ELeave ) CFscSendPluginImpl( *params );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::~CFscSendPluginImpl
// ---------------------------------------------------------------------------
//
CFscSendPluginImpl::~CFscSendPluginImpl()
    {
    FUNC_LOG;
    delete iContactMsgAction;
    delete iContactAudioMsgAction;
// <cmail> flag not needed  here
    delete iContactPostcardAction;

    delete iContactEmailAction;
    delete iActionList;
    delete iAttachmentFile;
    CCoeEnv::Static()->DeleteResourceFile( iResourceHandle );
    }
    
// ---------------------------------------------------------------------------
// CFscSendPluginImpl::Uid
// ---------------------------------------------------------------------------
//
TUid CFscSendPluginImpl::Uid() const 
    {
    FUNC_LOG;
    TUid uid = { KFscSendPluginImplImpUid };
    return uid;
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::ActionList
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>* CFscSendPluginImpl::ActionList() const
    {
	FUNC_LOG;
    return iActionList;
    }
    
// ---------------------------------------------------------------------------
// CFscSendPluginImpl::GetActionL
// ---------------------------------------------------------------------------
//
const MFscContactAction& CFscSendPluginImpl::GetActionL(
    TUid aActionUid ) const
    {
	FUNC_LOG;
    
    const MFscContactAction* action = NULL;

    if ( aActionUid == KFscActionUidCreateMsg )
        {
        action = iContactMsgAction;
        }
    else if ( aActionUid == KFscActionUidCreateAudMsg )
        {
        action = iContactAudioMsgAction;
        }
// <cmail> flag not needed  here
    else if ( aActionUid == KFscActionUidPostcard &&
              iContactPostcardAction != NULL )
        {
        action = iContactPostcardAction;
        }
    else if ( aActionUid == KFscActionUidEmail )
        {
        action = iContactEmailAction;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    return *action;
    }


// ---------------------------------------------------------------------------
// CFscSendPluginImpl::PriorityForContactSetL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::PriorityForContactSetL( 
    TUid aActionUid,
    MFscContactSet& aContactSet,
    TFscContactActionVisibility& aActionMenuVisibility,
    TFscContactActionVisibility& aOptionsMenuVisibility,
    MFscContactActionPluginObserver* aObserver )
    {
    FUNC_LOG;

    switch(iLastEvent)
        {
        case EActionEventIdle:
            {
            iActionUid = aActionUid;
            iContactSet = &aContactSet;
            iActionMenuVisibility = &aActionMenuVisibility;
            iOptionsMenuVisibility = &aOptionsMenuVisibility;
            iPluginObserver = aObserver;

            iIsExecute = EFalse;
            iActionPriority = KFscActionPriorityNotAvailable;
            
            iContactSet->SetToFirstContact();
            iContactSet->SetToFirstGroup();
            
            iLastEvent = EActionEventCanExecuteLaunched;
            CanExecuteL( aActionUid, aContactSet );
            
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
        
                if ( aActionUid == KFscActionUidCreateMsg )
                    {
                    iActionPriority = iParams.iUtils->ActionPriority( 
                        KFscCrUidCreateMsg,
                        KFscActionPrioritySendMsg );
                    }
                else if ( aActionUid == KFscActionUidCreateAudMsg )
                    {
                    iActionPriority = iParams.iUtils->ActionPriority( 
                        KFscCrUidCreateAudMsg,
                        KFscActionPrioritySendAudio );
                    }
                else if ( aActionUid == KFscActionUidPostcard )
                    {
//</cmail>
                    if ( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) )
                        {  
                        iActionPriority = iParams.iUtils->ActionPriority( 
                            KFscCrUidPostcard, KFscActionPrioritySendPostcard );
                        }
                    }
//</cmail>
                else if ( aActionUid == KFscActionUidEmail )
                    {
                    iActionPriority = iParams.iUtils->ActionPriority( 
                        KFscCrUidEmail,
                        KFscActionPrioritySendEmail );
                    }
                    // no else. already checked in CanExecuteL()
                
                CheckPriority( aActionUid );
                
                }
            else
                {
                iLastEvent = EActionEventContactAvailableLaunched;
                ContactAvailableL( aContactSet );
                }
            break;
            }
        case EActionEventContactAvailableFinished:
            {
            // Message is Visible in Options menu even if phone number isn't available
            if ( aActionUid == KFscActionUidCreateMsg &&
                    iIsContactAvailable )
                {
                aOptionsMenuVisibility.iVisibility = 
                    TFscContactActionVisibility::EFscActionVisible;
                aActionMenuVisibility.iVisibility = 
                    TFscContactActionVisibility::EFscActionHidden;
                aActionMenuVisibility.iReasonId = KInvalidReasonId;
                 
                iActionPriority = iParams.iUtils->ActionPriority( 
                        KFscCrUidCreateMsg,
                        KFscActionPrioritySendMsg );
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
             
            CheckPriority( aActionUid );
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
// CFscSendPluginImpl::ExecuteL 
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::ExecuteL( 
     TUid aActionUid,
     MFscContactSet& aContactSet,
     MFscContactActionPluginObserver* aObserver )
    {
	FUNC_LOG;

    switch ( iLastEvent )
        {
        case EActionEventIdle:
            {
            
            iActionUid = aActionUid;
            iContactSet = &aContactSet;
            iPluginObserver = aObserver;                      
                            
            iIsExecute = ETrue;
                                            
            iContactSet->SetToFirstContact();
            iContactSet->SetToFirstGroup();
                        
            iGroupIteratorPosition = 1;
            iGroupMembersCount = 0;
            iGroupMemberIndex = 0;
            
            iMaxRecipients = MaxMsgRecipientsL();
            
            iAddressList = CMessageData::NewL();                
            
            if( aContactSet.HasNextContact() )
                {
                iLastEvent = EActionEventExecuteContactRetrieve;
                aContactSet.NextContactL( this );
                }
            else 
                {
                if ( aContactSet.HasNextGroup() )
                    {
                    iLastEvent = EActionEventExecuteGroupRetrieve;
                    aContactSet.NextGroupL( this );
                    }
                else
                    {
                    iLastEvent = EActionEventExecuteFinished;
                    ExecuteL( aActionUid, aContactSet, aObserver );
                    }
                }
            
            break;
            }
        case EActionEventExecuteProcessContact:
            {            
            
            MVPbkContactLinkArray* linkArray = 
                iRetrievedStoreContact->GroupsJoinedLC();
            
            TBool found = EFalse;
            
            if ( aContactSet.GroupCount() > 0 
                    && linkArray->Count() > 0 )
                {                                 
                
                while ( aContactSet.HasNextGroup() 
                        && !found )
                    {                    
                    if ( linkArray->Find( *aContactSet.NextGroupLinkL() ) 
                            != KErrNotFound )
                        {
                        found = ETrue; 
                        }                    
                    }
                
                aContactSet.SetToFirstGroup();  
                }
            
            CleanupStack::PopAndDestroy(); //linkArray
            
            if ( !found ) 
                {
                if ( aActionUid ==  KFscActionUidPostcard )
                    {
//</cmail>
                    if ( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) )
                        {  
                        AddPostcardContactL( 
                            *iAddressList,
                            *iRetrievedStoreContact );
                        }
//</cmail>
                    }
                else
                    {
                    TInt result = AddContactL( 
                        aActionUid,
                        *iAddressList,
                        *iRetrievedStoreContact );
                                
                    if ( result == KErrCancel )
                        {
                        // Cancel whole operation                                                
                        ResetData();
                        aObserver->ExecuteComplete();
                        break;
                        }
                    else if ( result != KErrNone )
                        {
                        ++iMissingCount;
                        }
                    }
                }
  
                
            if ( aContactSet.HasNextContact() )
                {
                iLastEvent = EActionEventExecuteContactRetrieve;
                aContactSet.NextContactL( this );
                }
            else if ( aContactSet.HasNextGroup() )
                {
                iLastEvent = EActionEventExecuteGroupRetrieve;
                aContactSet.NextGroupL( this );
                }
            else 
                {
                iLastEvent = EActionEventExecuteFinished;
                ExecuteL( aActionUid, aContactSet, aObserver );
                }    
            
            break;
            }
        case EActionEventExecuteProcessGroup:
            {
            iGroupMembersCount = aContactSet.GroupContactCountL( 
                    *iRetrievedStoreGroup );
            
            if ( iGroupMembersCount > 0 )
                {
                iLastEvent = EActionEventExecuteContactRetrieve;
                aContactSet.GetGroupContactL( *iRetrievedStoreGroup, iGroupMemberIndex++, this );
                }
            else
                {
                if ( aContactSet.HasNextGroup() )
                    {
                    iGroupIteratorPosition++;
                    iLastEvent = EActionEventExecuteGroupRetrieve;
                    aContactSet.NextGroupL( this );
                    }
                else
                    {
                    iLastEvent = EActionEventExecuteFinished;
                    ExecuteL( aActionUid, aContactSet, aObserver );
                    }
                }            
            
            break;
            }
        case EActionEventExecuteProcessGroupMember:
            {
            
            if ( !IsAnyGroupMemberL( *iRetrievedGroupMember ) )
                {
                TInt result = AddContactL( 
                    aActionUid,
                    *iAddressList,
                    *iRetrievedGroupMember );
                            
                if ( result == KErrCancel )
                    {
                    // Cancel whole operation                                                
                    ResetData();
                    aObserver->ExecuteComplete();
                    break;
                    }
                else if ( result != KErrNone )
                    {
                    ++iMissingCount;
                    }
                }
            
            delete iRetrievedGroupMember;
            iRetrievedGroupMember = NULL;

            if ( iGroupMemberIndex < iGroupMembersCount )
                {
                iLastEvent = EActionEventExecuteContactRetrieve;
                aContactSet.GetGroupContactL( *iRetrievedStoreGroup, iGroupMemberIndex++, this );
                }
            else if ( aContactSet.HasNextGroup() )
                {
                iGroupMemberIndex = 0;
                iGroupMembersCount = 0;
                iGroupIteratorPosition++;
                iLastEvent = EActionEventExecuteGroupRetrieve;
                aContactSet.NextGroupL( this );
                }
            else
                {
                iLastEvent = EActionEventExecuteFinished;
                ExecuteL( aActionUid, aContactSet, aObserver );
                }
            
            break;
            }
        case EActionEventExecuteFinished:
            {
            
            if ( aContactSet.ContactCount() == 1 
                     && aContactSet.GroupCount() == 0 
                     && iMissingCount == 1 )
                 {
                 //if only one contact available and without
                 //proper contact information
                 HBufC* contactName = iParams.iUtils->GetContactNameL( *iRetrievedStoreContact );                
                 CleanupStack::PushL(contactName);
                 
                 TInt note = R_QTN_FS_NOTE_NO_MSG_TO_NAME;
                 
                 iParams.iUtils->ShowInfoNoteL(
                             note,
                             contactName );
                 
                 CleanupStack::PopAndDestroy( contactName );                
                 }            
            else if ( iMissingCount > 0 
                    && !iParams.iUtils->ShowNumberMissingNoteL(
                            iMissingCount,
                            ( iAddressList->ToAddressArray().Count() + iMissingCount ),
                             R_FS_NOTE_NUMBER_MISSING,
                             R_FS_NUMBER_MISSING_QUERY_DIALOG ) )
                {
                //Fix for: EJKA-7KAEVA
                ResetData();
                aObserver->ExecuteFailed( KErrCancel );
                break;
                }
            else
                {
                if ( iRecipientsCounter <= iMaxRecipients )
                    {
                    SendToSelectedMembersL( aActionUid, iAddressList );
                    }
                else if ( iRecipientsCounter == iMaxRecipients + 1 )
                    {
                    TInt note = R_QTN_MAX_RECIPIENTS_EXCEEDED_1; 
                    HBufC *emptyBuf = NULL;
                    iParams.iUtils->ShowInfoNoteL(
                                note,
                                emptyBuf );
                    }
                else
                    {
                    TInt note = R_QTN_MAX_RECIPIENTS_EXCEEDED_MANY;
                    iParams.iUtils->ShowInfoNoteL(
                                note,
                                iRecipientsCounter - iMaxRecipients );
                    }
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
// CFscSendPluginImpl::GetReasonL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::GetReasonL(
    TUid /* aActionUid */,
    TInt /* aReasonId */,
    HBufC*& /* aReason */) const
    {
	FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CFscSendPluginImpl::SendToSelectedMembersL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::SendToSelectedMembersL(
    TUid aActionUid,
    const CMessageData* aAddressList )
    {
	FUNC_LOG;
    // Check from sys utils that the flash drive 
    // is not on critical level before creating a message.
    TBool flashOnCL =
        SysUtil::FFSSpaceBelowCriticalLevelL(
            &CCoeEnv::Static()->FsSession(),
            KMsgSpaceEstimationBytes );
    if ( flashOnCL )
        {
        User::Leave( KErrNoMemory );
        }

    CSendUi* sendUi = CSendUi::NewLC();
    TUid messageType = KSenduiMtmUniMessageUid;
    if ( aActionUid == KFscActionUidCreateMsg )
        {
        messageType = KSenduiMtmUniMessageUid;
        }
    else if ( aActionUid == KFscActionUidCreateAudMsg )
        {
        messageType = KSenduiMtmAudioMessageUid;
        }
// <cmail> flag not needed  here
    else if ( aActionUid == KFscActionUidPostcard &&
              iContactPostcardAction != NULL  )
        {
        messageType = KSenduiMtmPostcardUid;
        }
    else if ( aActionUid == KFscActionUidEmail )
        {
        messageType = KSenduiMtmSmtpUid;
        }
    else
        {
        User::Leave( KErrArgument );
        }    
        
    sendUi->CreateAndSendMessageL(
            messageType,
            aAddressList, KNullUid, EFalse );
        
    CleanupStack::PopAndDestroy( sendUi );
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::ContactAvailableL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::ContactAvailableL( MFscContactSet& aContactSet )
    {
	FUNC_LOG;
    
    switch ( iLastEvent )
        {
        case EActionEventContactAvailableLaunched:
            {            
            if ( aContactSet.ContactCount() > 0 )
                {
                iIsContactAvailable = ETrue;
                iLastEvent = EActionEventContactAvailableFinished;
                PriorityForContactSetL( 
                    iActionUid,
                    aContactSet,
                    *iActionMenuVisibility,
                    *iOptionsMenuVisibility,
                    iPluginObserver );
                }
            else
                {
                aContactSet.SetToFirstContact();
                aContactSet.SetToFirstGroup();
                
                if ( aContactSet.HasNextGroup() 
                        && iIsContactAvailable == EFalse )
                    {
                    iLastEvent = EActionEventContactAvailableContactRetrieve;
                    aContactSet.NextGroupL( this );
                    }
                else
                    {
                    iLastEvent = EActionEventContactAvailableFinished;
                    PriorityForContactSetL( 
                        iActionUid,
                        aContactSet,
                        *iActionMenuVisibility,
                        *iOptionsMenuVisibility,
                        iPluginObserver );
                    }
                }
            break;
            }
        case EActionEventContactAvailableProcess:
            {
            if ( aContactSet.GroupContactCountL( *iRetrievedStoreGroup ) > 0 )
                {
                iIsContactAvailable = ETrue;
                iLastEvent = EActionEventContactAvailableFinished;
                PriorityForContactSetL( 
                    iActionUid,
                    aContactSet,
                    *iActionMenuVisibility,
                    *iOptionsMenuVisibility,
                    iPluginObserver );
                }
            else
                {
                if ( aContactSet.HasNextGroup() 
                        && iIsContactAvailable == EFalse )
                    {
                    iLastEvent = EActionEventContactAvailableContactRetrieve;
                    aContactSet.NextGroupL( this );
                    }
                else
                    {
                    iLastEvent = EActionEventContactAvailableFinished;
                    PriorityForContactSetL( 
                        iActionUid,
                        aContactSet,
                        *iActionMenuVisibility,
                        *iOptionsMenuVisibility,
                        iPluginObserver );
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
// CFscSendPluginImpl::CanExecuteL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::CanExecuteL(
    TUid aActionUid,
    MFscContactSet& aContactSet )
    {
	FUNC_LOG;
    
    switch( iLastEvent )
         {
         case EActionEventCanExecuteLaunched:
             {
    
             iCanDisplay = KErrArgument;
             iNumberType = ETypeInvalid;    
             if ( ( aActionUid == KFscActionUidCreateMsg ) || 
                     ( aActionUid == KFscActionUidCreateAudMsg ) )
                 {
                 iNumberType = ETypeMsgAddress;
                 }
// <cmail> flag not needed  here
             else if ( aActionUid == KFscActionUidPostcard &&
                     iContactPostcardAction )
                 {
                 iNumberType = ETypeInvalid;  
                 }
             else if ( aActionUid == KFscActionUidEmail )
                 {
                 iNumberType = ETypeEmailAddress;
                 }
             else
                 {
                 User::Leave( KErrArgument );
                 }

             if ( aContactSet.HasNextContact() 
                     && iCanDisplay != KErrNone )
                 {
                 iLastEvent = EActionEventCanExecuteContactRetrieve;
                 aContactSet.NextContactL( this );
                 }
             else
                 {
                 if ( aContactSet.HasNextGroup()
                         && iCanDisplay != KErrNone )
                     {
                     iLastEvent = EActionEventCanExecuteGroupRetrieve;
                     aContactSet.NextGroupL( this );
                     }
                 else
                     {
                     iLastEvent = EActionEventCanExecuteFinished;
                     CanExecuteL( aActionUid, aContactSet );
                     }
                 }
                          
             break;
             }
         case EActionEventCanExecuteProcessContact:
             {
             if ( aActionUid == KFscActionUidPostcard &&
                      aContactSet.ContactCount() == 1 &&
                      aContactSet.GroupCount() == 0 )
                 {
//</cmail>
                 if ( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) )
                     { 
                     //verify if contact has valid store - needed for attachement
                     MVPbkContactStore* contactStore = &iRetrievedStoreContact->ContactStore();
                     if ( contactStore )
                         {
                         iLastEvent = EActionEventCanExecuteFinished;
                         iCanDisplay = KErrNone;
                         CanExecuteL( aActionUid, aContactSet );
                         }
                      }   
                  }
//</cmail>
             if ( iParams.iUtils->IsContactNumberAvailableL(
                         *iRetrievedStoreContact, iNumberType ))
                 {
                 iLastEvent = EActionEventCanExecuteFinished;
                 iCanDisplay = KErrNone;
                 CanExecuteL( aActionUid, aContactSet );
                 }
             else if ( aContactSet.HasNextContact() 
                          && iCanDisplay != KErrNone )
                 {
                 iLastEvent = EActionEventCanExecuteContactRetrieve;
                 aContactSet.NextContactL( this );
                 }
             else if (aContactSet.HasNextGroup() )
                 {
                 iLastEvent = EActionEventCanExecuteGroupRetrieve;
                 aContactSet.NextGroupL( this);
                 }
             else
                 {
                 iLastEvent = EActionEventCanExecuteFinished;
                 CanExecuteL( aActionUid, aContactSet );
                 }
             break;
             }             
         case EActionEventCanExecuteProcessGroup:
             {
             MVPbkContactGroup* group = iRetrievedStoreGroup->Group();
             if ( group 
                     && aContactSet.GroupContactCountL( 
                             *iRetrievedStoreGroup )>0 )
                 {
                 iGroupMembersCount =  
                     aContactSet.GroupContactCountL( *iRetrievedStoreGroup );
                 iLastEvent = EActionEventCanExecuteContactRetrieve;
                 aContactSet.GetGroupContactL( 
                         *iRetrievedStoreGroup, iGroupMemberIndex++, this );
                 }
             else
                 {
                 iLastEvent = EActionEventCanExecuteFinished;
                 CanExecuteL( aActionUid, aContactSet );
                 }
             break;
             }
         case EActionEventCanExecuteProcessGroupMember:
             {
             
             if ( iParams.iUtils->IsContactNumberAvailableL(
                          *iRetrievedGroupMember, iNumberType ) 
                          && aActionUid != KFscActionUidPostcard )
                  {
                  iLastEvent = EActionEventCanExecuteFinished;
                  iCanDisplay = KErrNone; // atleast one number avaialble
                  delete iRetrievedGroupMember;
                  iRetrievedGroupMember = NULL;
                  CanExecuteL( aActionUid, aContactSet );
                  }
             else 
                 {
                 delete iRetrievedGroupMember;
                 iRetrievedGroupMember = NULL;
                 if ( iGroupMemberIndex < iGroupMembersCount )
                     {
                     iLastEvent = EActionEventCanExecuteContactRetrieve;
                     aContactSet.GetGroupContactL( *iRetrievedStoreGroup, 
                             iGroupMemberIndex++, this );
                     }
                 else 
                     {                     
                     if ( aContactSet.HasNextGroup()
                             && iCanDisplay != KErrNone )
                         {
                         iGroupMemberIndex = 0;
                         iGroupMembersCount = 0;
                         iLastEvent = EActionEventCanExecuteGroupRetrieve;
                         aContactSet.NextGroupL(this);
                         }
                     else
                         {
                         iLastEvent = EActionEventCanExecuteFinished;
                         CanExecuteL( aActionUid, aContactSet );
                         }
                     }
                 }
             
             break;
             }
         case EActionEventCanExecuteFinished:
             {
             PriorityForContactSetL( 
                 iActionUid,
                 aContactSet,
                 *iActionMenuVisibility,
                 *iOptionsMenuVisibility,
                 iPluginObserver );
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
// CFscSendPluginImpl::UpdateActionIconL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::UpdateActionIconL( TUid aActionUid )
    {
	FUNC_LOG;

    TFileName dllFileName;
    Dll::FileName( dllFileName );
    TParse parse;
    User::LeaveIfError(
        parse.Set( KSendPluginIconPath, &dllFileName, NULL ) );

// <cmail> icons changed
    if ( aActionUid == KFscActionUidCreateMsg )
        {
        iContactMsgAction->SetIcon( iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionMsg,
                parse.FullName(),
                EMbmFscsendpluginQgn_prop_cmail_action_msg,
                EMbmFscsendpluginQgn_prop_cmail_action_msg_mask ) );
        }
 
    else if ( aActionUid == KFscActionUidCreateAudMsg )
        {
        iContactAudioMsgAction->SetIcon( iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionAudio,
                parse.FullName(),
                EMbmFscsendpluginQgn_prop_cmail_action_audio,
                EMbmFscsendpluginQgn_prop_cmail_action_audio_mask ) );
        }
    else if ( aActionUid == KFscActionUidPostcard )
        {
//</cmail>
        if ( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) )
            {  
            iContactPostcardAction->SetIcon( iParams.iUtils->SkinIconL(
                   KAknsIIDQgnFsActionPostcard,
                   parse.FullName(),
                   EMbmFscsendpluginQgn_prop_cmail_action_postcard,
                   EMbmFscsendpluginQgn_prop_cmail_action_postcard_mask ) );
            }
        }
//</cmail>
    else if ( aActionUid == KFscActionUidEmail )
        {
        iContactEmailAction->SetIcon( iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionEmail,
                parse.FullName(),
                EMbmFscsendpluginQgn_prop_cmail_action_email,
                EMbmFscsendpluginQgn_prop_cmail_action_email_mask ) );
        }
// </cmail>        
    }

    
// ---------------------------------------------------------------------------
// CFscSendPluginImpl::AddContactL
// ---------------------------------------------------------------------------
//
TInt CFscSendPluginImpl::AddContactL( 
    TUid aActionUid,
    CMessageData& aAddressList,
    MVPbkStoreContact& aContact )
    {
	FUNC_LOG;
    
    TAddressData addressData;                    
    TInt ret = KErrNotFound;

    if ( aActionUid == KFscActionUidEmail )
        {
        ret = iParams.iUtils->GetEmailAddressL(
                aContact, addressData );
        }
    else
        {
        ret = iParams.iUtils->GetMessageAddressL(
                aContact, addressData );
        }
    
    if ( ret == KErrNone && addressData.Length() > 0 ) 
        {
        // no need to check for duplicate addresses
        HBufC* contactName = iParams.iUtils->GetContactNameL(
            aContact );
        CleanupStack::PushL( contactName );
        aAddressList.AppendToAddressL(
            addressData, *contactName );
        CleanupStack::PopAndDestroy( contactName );   
        iRecipientsCounter++;
        }
    
    return ret;
    }

// <cmail> ENABLE_POSTCARD_ACTION flag not needed
// ---------------------------------------------------------------------------
// CFscSendPluginImpl::AddPostcardContactL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::AddPostcardContactL( 
    CMessageData& aMessageData,
    const MVPbkStoreContact& aContact )
    {
	FUNC_LOG;
    MVPbkContactStore* store = &aContact.ContactStore();
    if ( store )
        {
        // Create attachment file object
        delete iAttachmentFile;
        iAttachmentFile = NULL;
        HBufC* attFileName = 
            StringLoader::LoadLC( R_FS_POSTCARD_WRITE_ATTACHMENT_TAG );
        iAttachmentFile = CFscAttachmentFile::NewL(
            *attFileName, 
            iParams.iUtils->ContactManager().FsSession(),
            EFileWrite | EFileShareAny );
        CleanupStack::PopAndDestroy( attFileName );
        WritePostcardAttachmentContentL( aContact, *iAttachmentFile );
        aMessageData.AppendAttachmentL(
            iAttachmentFile->FileName() );
        iAttachmentFile->Release(); 
        }

    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::WritePostcardAttachmentContentL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::WritePostcardAttachmentContentL( 
    const MVPbkStoreContact& aContact, 
    CFscAttachmentFile& aAttachmentFile )
    {
	FUNC_LOG;
    HBufC* contactId = HBufC::NewLC( KMaxContactIdStringLength );
    MVPbkContactLink* contactLink = aContact.CreateLinkLC();
    
    CVPbkContactIdConverter* cntIdCnv = 
        CVPbkContactIdConverter::NewL( aContact.ContactStore() );
    TContactItemId cntId = cntIdCnv->LinkToIdentifier( *contactLink );
    
    delete cntIdCnv;
    CleanupStack::PopAndDestroy(); //contactLink
    
    contactId->Des().Num( cntId );
    HBufC8* string8 = HBufC8::NewLC( KMaxContactIdStringLength );
    string8->Des().Append( *contactId );
    aAttachmentFile.File().Write( *string8 );
    CleanupStack::PopAndDestroy( 2, contactId );
    }    
// </cmail>

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::CFscSendPluginImpl
// ---------------------------------------------------------------------------
//
CFscSendPluginImpl::CFscSendPluginImpl( 
    const TFscContactActionPluginParams& aParams )
    : CFscContactActionPlugin(),
    iParams( aParams ),
    iResourceHandle( 0 )
    {
	FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::ConstructL()
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
        parse.Set( KSendPluginIconPath, &dllFileName, NULL ) );     

// <cmail> icons changed    
    iActionList->AppendL( KFscActionUidCreateMsg );    
    iContactMsgAction = iParams.iUtils->CreateActionL(
        *this,
        KFscActionUidCreateMsg,
        KFscAtComSendMsg,
        R_FS_ACTION_MESSAGE,
        iParams.iUtils->SkinIconL(
            KAknsIIDQgnFsActionMsg,
            parse.FullName(),
            EMbmFscsendpluginQgn_prop_cmail_action_msg,
            EMbmFscsendpluginQgn_prop_cmail_action_msg_mask ) );

    iActionList->AppendL( KFscActionUidCreateAudMsg );
    iContactAudioMsgAction = iParams.iUtils->CreateActionL(
        *this,
        KFscActionUidCreateAudMsg,
        KFscAtComSendAudio,
        R_FS_ACTION_AUDIO,
        iParams.iUtils->SkinIconL(
            KAknsIIDQgnFsActionAudio,
            parse.FullName(),
            EMbmFscsendpluginQgn_prop_cmail_action_audio,
            EMbmFscsendpluginQgn_prop_cmail_action_audio_mask ) );

// <cmail> flag not needed  here            
    if ( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) )
        {
        iActionList->AppendL( KFscActionUidPostcard );
        iContactPostcardAction = iParams.iUtils->CreateActionL(
            *this,
            KFscActionUidPostcard,
            KFscAtComSendPostcard,
            R_FS_ACTION_POSTCARD,
            iParams.iUtils->SkinIconL(
                KAknsIIDQgnFsActionPostcard,
                parse.FullName(),
                EMbmFscsendpluginQgn_prop_cmail_action_postcard,
                EMbmFscsendpluginQgn_prop_cmail_action_postcard_mask ) );
        }     
// </cmail>      
    iActionList->AppendL( KFscActionUidEmail );    
    iContactEmailAction = iParams.iUtils->CreateActionL(
        *this,
        KFscActionUidEmail,
        KFscAtComSendEmail,
        R_FS_ACTION_EMAIL,
        iParams.iUtils->SkinIconL(
            KAknsIIDQgnFsActionEmail,
            parse.FullName(),
            EMbmFscsendpluginQgn_prop_cmail_action_email,
            EMbmFscsendpluginQgn_prop_cmail_action_email_mask ) );
// </cmail>
    }


// ---------------------------------------------------------------------------
// CFscSendPluginImpl::ResetData
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::ResetData()
    {
	FUNC_LOG;
    if ( iContactSet )
        {
        // <cmail>
        TRAP_IGNORE( iContactSet->CancelNextContactL() );
        TRAP_IGNORE( iContactSet->CancelNextGroupL() );
        // </cmail>
        iContactSet->SetToFirstContact();
        iContactSet->SetToFirstGroup();
        iContactSet = NULL;
        }
    
    if ( iAddressList )
        {
        delete iAddressList;
        iAddressList = NULL;
        }
    
    if ( iRetrievedGroupMember )
        {
        delete iRetrievedGroupMember;
        iRetrievedGroupMember = NULL;
        }
    
    iLastEvent = EActionEventIdle;
    iActionMenuVisibility = NULL;
    iOptionsMenuVisibility = NULL;
    iCanDisplay = KErrNone;
    iRetrievedStoreContact = NULL;
    iRetrievedStoreGroup = NULL;
    iNumberType = ETypeInvalid;
    iGroupMembersCount = 0;
    iGroupMemberIndex = 0;
    iIsContactAvailable = EFalse;
    iGroupIteratorPosition = 0;
    iMissingCount = 0;
    iMaxRecipients = 0;
    iRecipientsCounter = 0;
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::CheckPriority
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::CheckPriority(TUid aActionUid)
    {
	FUNC_LOG;
    if ( iActionPriority > 0 )
        {
        // ignore any leave while updating action icons
        TRAP_IGNORE( UpdateActionIconL( aActionUid ) );
        }
    ResetData();
    iPluginObserver->PriorityForContactSetComplete(iActionPriority);
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::MoveGroupToIndexL
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::MoveGroupToIndexL(MFscContactSet& aContactSet, 
        TInt aIndex, TInt& aError)
    {
	FUNC_LOG;
    aError = KErrNone;
    aContactSet.SetToFirstGroup();

    if ( aIndex > aContactSet.GroupCount() )
        {
        aError = KErrArgument;
        }
    
    //move iterator to specified index
    while ( aContactSet.HasNextGroup() 
            && aIndex-- > 0 )
        {
        aContactSet.NextGroupLinkL();
        }
    
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::IsAnyGroupMember
// ---------------------------------------------------------------------------
//
TBool CFscSendPluginImpl::IsAnyGroupMemberL( MVPbkStoreContact& aContact )
    {
	FUNC_LOG;
    MVPbkContactLinkArray* linkArray = aContact.GroupsJoinedLC();
                
    TBool found = EFalse;
    TInt error = KErrNone;
    TBool isChanged = EFalse;
    
    if ( linkArray->Count() > 1
            && iContactSet->HasNextGroup() )
        {                                 
        
        while ( iContactSet->HasNextGroup() 
                && !found )
            {
            isChanged = ETrue;
            if ( linkArray->Find( *iContactSet->NextGroupLinkL() ) 
                    != KErrNotFound )
                {
                found = ETrue; 
                }                    
            }
        
        if ( isChanged )
            {
            //if iterator moved
            MoveGroupToIndexL( *iContactSet, iGroupIteratorPosition , error );
            }        
        
        User::LeaveIfError( error );            
        }
    
    CleanupStack::PopAndDestroy(); //linkArray    
    return found;
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::NextContactComplete
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::NextContactComplete( MVPbkStoreContact* aContact )
    {
	FUNC_LOG;
    TInt err = KErrNone;
    
    switch( iLastEvent )
        {
        case EActionEventCanExecuteContactRetrieve:
            {
            iRetrievedStoreContact = aContact;
            iLastEvent = EActionEventCanExecuteProcessContact; 
            TRAP( err, CanExecuteL( iActionUid, *iContactSet ) );
            break;
            }
        case EActionEventExecuteContactRetrieve:
            {
            iRetrievedStoreContact = aContact;
            iLastEvent = EActionEventExecuteProcessContact;
            TRAP( err, ExecuteL( iActionUid, *iContactSet, iPluginObserver ) );
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
// CFscSendPluginImpl::NextContactFailed
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::NextContactFailed( TInt aError )
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
// CFscSendPluginImpl::NextGroupComplete
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::NextGroupComplete( MVPbkStoreContact* aContact )
    {
	FUNC_LOG;
    TInt err = KErrNone;
    
    switch( iLastEvent )
        {
        case EActionEventCanExecuteGroupRetrieve:
            {
            iLastEvent = EActionEventCanExecuteProcessGroup;
            iRetrievedStoreGroup = aContact;
            TRAP( err, CanExecuteL( iActionUid, *iContactSet ) );
            break;
            }
        case EActionEventContactAvailableContactRetrieve:
            {
            iLastEvent = EActionEventContactAvailableProcess;
            iRetrievedStoreGroup = aContact;
            TRAP( err, ContactAvailableL( *iContactSet ) );
            break;
            }
        case EActionEventExecuteGroupRetrieve:
            {
            iLastEvent = EActionEventExecuteProcessGroup;
            iRetrievedStoreGroup = aContact;
            TRAP( err, ExecuteL( iActionUid, *iContactSet, iPluginObserver ) );
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
// CFscSendPluginImpl::NextGroupFailed
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::NextGroupFailed( TInt aError )
    {
	FUNC_LOG;
    ResetData();
    if ( iIsExecute )
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::GetGroupContactComplete( MVPbkStoreContact* aContact )
    {
	FUNC_LOG;
    TInt err = KErrNone;
    
    switch( iLastEvent )
       {
       case EActionEventCanExecuteContactRetrieve:
           {
           iLastEvent = EActionEventCanExecuteProcessGroupMember;
           iRetrievedGroupMember = aContact;
           TRAP( err, CanExecuteL( iActionUid, *iContactSet ) );
           break;
           }
       case EActionEventExecuteContactRetrieve:
           {
           iRetrievedGroupMember = aContact;
           iLastEvent = EActionEventExecuteProcessGroupMember;
           TRAP( err, ExecuteL( iActionUid, *iContactSet, iPluginObserver ) );
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
// CFscSendPluginImpl::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::GetGroupContactFailed( TInt aError )
    {
	FUNC_LOG;
    ResetData();
    if ( iIsExecute )
        {
        iPluginObserver->ExecuteFailed(aError);
        }
    else
        {
        iPluginObserver->PriorityForContactSetFailed(aError);
        }
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::CancelPriorityForContactSet
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::CancelPriorityForContactSet()
    {
	FUNC_LOG;
    iLastEvent = EActionEventCanceled;
    ResetData();
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::CancelExecute
// ---------------------------------------------------------------------------
//
void CFscSendPluginImpl::CancelExecute()
    {
	FUNC_LOG;
    // close popup window for selecting number if opened
    TRAPD( err, iParams.iUtils->CloseSelectDialogL() );
    
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    
    // set action
    iLastEvent = EActionEventCanceled;
    ResetData();
    }

// ---------------------------------------------------------------------------
// CFscSendPluginImpl::MaxMsgRecipients
// ---------------------------------------------------------------------------
//
TInt CFscSendPluginImpl::MaxMsgRecipientsL()
    {
	FUNC_LOG;
    TInt maxRecipients = 0;
    TInt smsRecipients = 0;
    TInt mmsRecipients = 0;
    
    if ( iActionUid == KFscActionUidEmail )
        {
        // There should be no limit for the amount of allowed contacts when sending email
        maxRecipients = KDefaultEmailRecipients;
        }
    else
        {
        CRepository* repository = CRepository::NewL( KCRUidUniEditor );
        CleanupStack::PushL( repository );
        
        if ( repository->Get( KUniEditorSoftLimitRecipientCount, smsRecipients ) != KErrNone || 
                smsRecipients < 1 )
            {
            // Unreasonable count, change it back to default value
            smsRecipients = KDefaultSmsRecipients;
            }
            
        if ( repository->Get( KUniEditorMaxRecipientCount, mmsRecipients ) != KErrNone || 
                mmsRecipients < 1 )
            {
            // Unreasonable count, change it back to default value
            mmsRecipients = KDefaultMmsRecipients;
            }
    
        CleanupStack::PopAndDestroy( repository );
        
        if ( smsRecipients > mmsRecipients )
            {
            maxRecipients = smsRecipients;
            }
        else
            {
            maxRecipients = mmsRecipients;
            }
        }
    
    return maxRecipients;
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

