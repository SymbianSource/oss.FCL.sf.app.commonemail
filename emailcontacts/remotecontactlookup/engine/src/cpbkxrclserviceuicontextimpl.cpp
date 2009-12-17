/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CPbkxRclServiceUiContextImpl.
*
*/


#include "emailtrace.h"
#include <pbkxrclengine.rsg>
#include <cntfldst.h>
#include <StringLoader.h>
#include <CPbkContactEngine.h>
#include <bautils.h>
#include <textresolver.h>
//<cmail>
#include "fsccontactactionservicedefines.h"
//</cmail>
#include <data_caging_path_literals.hrh>
#include <AknsUtils.h>
#include <hlplch.h>

#include "cpbkxrclserviceuicontextimpl.h"
#include "pbkxremotecontactlookuppanic.h"
#include "cpbkxremotecontactlookupprotocolsession.h"
#include "cpbkxrclprotocolenvimpl.h"
#include "cpbkxremotecontactlookupprotocoladapter.h"
#include "cpbkxremotecontactlookupprotocolaccount.h"

#include "cpbkxrclsearchresultdlg.h"
#include "cpbkxrclresultinfodlg.h"
#include "cpbkxrclqueryeditor.h"
#include "cpbkxrclactionservicewrapper.h"

#include "pbkxrclutils.h"
#include "pbkxrclengineconstants.h"

#include "engine.hrh"
#include <aknnotewrappers.h>

// own macro to handle errors in traps
// <cmail> Do not use, produces Codescanner warnings
//#define EXIT_IF_ERROR( xxx ) { TRAPD( err, xxx ); if ( err != KErrNone ) HandleError( err ); }
// </cmail>

/// Unnamed namespace for local definitions
namespace {
const TInt KTwoMinutesTimerInterval(120000000);
} /// namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclServiceUiContextImpl* CPbkxRclServiceUiContextImpl::NewL(
    TPbkxRemoteContactLookupProtocolAccountId aId,
    TMode aMode )
    {
    FUNC_LOG;
    CPbkxRclServiceUiContextImpl* context = 
        CPbkxRclServiceUiContextImpl::NewLC( aId, aMode );
    CleanupStack::Pop( context );
    return context;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::NewLC
// ---------------------------------------------------------------------------
//
CPbkxRclServiceUiContextImpl* CPbkxRclServiceUiContextImpl::NewLC(
    TPbkxRemoteContactLookupProtocolAccountId aId,
    TMode aMode )
    {
    FUNC_LOG;
    CPbkxRclServiceUiContextImpl* context = 
        new ( ELeave ) CPbkxRclServiceUiContextImpl( aId, aMode );
    CleanupStack::PushL( context );
    context->ConstructL();
    return context;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::CPbkxRclServiceUiContextImpl
// ---------------------------------------------------------------------------
//
CPbkxRclServiceUiContextImpl::CPbkxRclServiceUiContextImpl(
    TPbkxRemoteContactLookupProtocolAccountId aId,
    TMode aMode ) : 
    iAccountId( aId ), iMode( aMode ), iSelectedIndex( KErrNotFound )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::~CPbkxRclServiceUiContextImpl
// ---------------------------------------------------------------------------
//
CPbkxRclServiceUiContextImpl::~CPbkxRclServiceUiContextImpl()
    {
    FUNC_LOG;

    if( iTimer )
        {
        iTimer->Cancel();
        }
    delete iTimer;

    iQueryText.Close();
    iQueryCriteria.Close();

    CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );

    iContactCards.Reset();
    iSearchResults.ResetAndDestroy();
    iWaitObjects.ResetAndDestroy();
    delete iContactEngine;

    delete iActionServiceWrapper;

    delete iAccount;
    delete iSession;
    delete iAdapter;
    delete iEnvImpl;

    delete iEventScheduler;
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::ConstructL()
    {
    FUNC_LOG;

    CCoeEnv* coeEnv = CCoeEnv::Static();

    TFileName dllFileName;
    Dll::FileName( dllFileName );
      
    TParse parse;
    parse.Set( KResourceFile, &KDC_APP_RESOURCE_DIR, &dllFileName );
    TFileName resourceFile = parse.FullName();
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

    // create default contact engine to use to create CPbkContactItems
    iContactEngine = CPbkContactEngine::NewL();

    // create actions service wrapper used by UI views
    iActionServiceWrapper = CPbkxRclActionServiceWrapper::NewL( *iContactEngine );

    iActionServiceWrapper->SetContactSelectorMode( 
        iMode == EModeContactSelector );

    // Create objects needed for executing the search
    iEnvImpl = CPbkxRclProtocolEnvImpl::NewL();
    
    iAdapter = CPbkxRemoteContactLookupProtocolAdapter::NewL( 
        iAccountId.iProtocolUid, 
        *iEnvImpl );

    iAccount = iAdapter->NewProtocolAccountL( iAccountId );

    if ( iAccount == NULL )
        {
        User::Leave( KErrNotFound );
        }
    
    iSession = iAdapter->NewSessionL();
    iSession->InitializeL( *this, iAccountId );

    iEventScheduler = CPbkxRclEventScheduler::NewL( *this );
    
    iTimer = CPeriodic::NewL( CActive::EPriorityIdle );

    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::GetSelectedContactL
// ---------------------------------------------------------------------------
//
CContactItem* CPbkxRclServiceUiContextImpl::GetSelectedContactL()
    {
    FUNC_LOG;
    __ASSERT_ALWAYS( iSelectedIndex >= 0 && iSelectedIndex < iContactCards.Count(),
                     PbkxRclPanic( EPbkxRclPanicGeneral ) );
    CContactCard* selected = iContactCards[iSelectedIndex];
    CContactCard* copy = CContactCard::NewL( selected );
    return copy;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::GetResultByIndex
// ---------------------------------------------------------------------------
//
CPbkxRemoteContactLookupProtocolResult* 
CPbkxRclServiceUiContextImpl::GetResultByIndex( TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iContactCards.Count(), 
                     PbkxRclPanic( EPbkxRclPanicGeneral ) );
  
    CContactCard* card = iContactCards[aIndex];
    CPbkxRemoteContactLookupProtocolResult* result = NULL;
    for ( TInt i = 0; i < iSearchResults.Count(); i++ )
        {
        if ( card == &( iSearchResults[i]->ContactItem() ) )
            {
            result = iSearchResults[i];
            break;
            }
        }

    __ASSERT_ALWAYS( result != NULL, PbkxRclPanic( EPbkxRclPanicGeneral ) );

    return result;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::ExecuteL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::ExecuteL( const TDesC& aQueryText, TResult& aResult )
    {
    FUNC_LOG;

    // set state as initial
    ResetState();
    SetState( EInitial );
    iExitCode = KErrNone;

    iSearchResults.ResetAndDestroy();

    iQueryCriteria.Close();
    iQueryCriteria.CreateL( KRclQueryTextMaxLength );
    iQueryCriteria = aQueryText.Left( KRclQueryTextMaxLength );

    iResult = &aResult;

    TOperation op = ( iMode != EModeExistingCriteria ) ? 
        EOpenSearchQueryDefault : EExecuteSearchWithNoQuery;
    
    AddOperation( op );
        
    StartActiveWaitL( EMainWait );

    
    HandleContextExitL();

    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::LooseSearchCompleted
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::LooseSearchCompleted(
    TInt aStatus,
    RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults )
    {
    FUNC_LOG;

    // <cmail>
    TRAPD( err, DoLooseSearchCompletedL( aStatus, aResults ) );
    if ( err != KErrNone )
        {
        HandleError( err );
        }
    // </cmail>
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::ContactFieldsRetrievalCompleted
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::ContactFieldsRetrievalCompleted( TInt aStatus )
    {
    FUNC_LOG;

    // <cmail>
    TRAPD( err, DoContactFieldsRetrievalCompletedL( aStatus ) );
    if ( err != KErrNone )
        {
        HandleError( err );
        }
    // </cmail>

    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::RetrieveDetailsL
// ---------------------------------------------------------------------------
//
CContactCard* CPbkxRclServiceUiContextImpl::RetrieveDetailsL( 
    TInt aContactIndex,
    const TDesC& aWaitNoteText )
    {
    FUNC_LOG;

    CPbkxRemoteContactLookupProtocolResult* result = GetResultByIndex( aContactIndex );

    CContactCard* card = static_cast<CContactCard*>( &result->ContactItem() );

    if ( !result->IsComplete() )
        {
        
        RPointerArray<CPbkxRemoteContactLookupProtocolResult> array;
        CleanupClosePushL( array );
        array.AppendL( result );

        iSession->RetrieveContactFieldsL( array );

        // Start time for time out event
        iTimer->Start(
        		KTwoMinutesTimerInterval, KTwoMinutesTimerInterval, 
            TCallBack(TimerCallBack, this));
        
        DisplayWaitDialogL( R_RCL_SEARCH_WAIT_DIALOG_CANCEL, aWaitNoteText );

        // Start active wait. When contact retrieval is completed, 
        // ContactFieldsRetrievalCompleted is called. If user cancels
        // retrieval, DialogDismissedL is called. TimeOut is called when time is out.
        StartActiveWaitL( EContactRetrievalWait );
        
        CleanupStack::PopAndDestroy( &array );
        

        // if contact retrieval was canceledor retrieval failed, 
        // result is not yet complete
        if ( !result->IsComplete() )
            {
            card = NULL;
            }
        
        }

    return card;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::ContactCount
// ---------------------------------------------------------------------------
//
TInt CPbkxRclServiceUiContextImpl::ContactCount() const
    {
    FUNC_LOG;
    return iContactCards.Count();
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::SetSelectedContact
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::SetSelectedContactL( TInt aIndex )
    {
    FUNC_LOG;
    iSearchResultDialog->SetCurrentItemIndexL( aIndex );
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::EventTriggered
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::EventTriggered()
    {
    FUNC_LOG;

    // <cmail>
    TRAPD( err, DoHandleOperationL() );
    if ( err != KErrNone )
        {
        HandleError( err );
        }
    // </cmail>

    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::ProcessCommandL( TInt aCommandId )
    {
    FUNC_LOG;
   
    // some commands are handled the same way no matter which dialog is on top
    TBool handled = ETrue;

    switch ( aCommandId )
        {
        case ERclCmdVoiceCall:
            iActionServiceWrapper->ExecuteActionL( 
                KFscAtComCallGSM );
            break;
        case ERclCmdVideoCall:
            iActionServiceWrapper->ExecuteActionL( 
                KFscAtComCallVideo );
            break;
        case ERclCmdVoip:
            iActionServiceWrapper->ExecuteActionL(
                KFscAtComCallVoip );
            break;
        case ERclCmdSendMsg:
            iActionServiceWrapper->ExecuteActionL( 
                KFscAtComSendMsg );
            break;
          
        case ERclCmdSendEmail:
            iActionServiceWrapper->ExecuteActionL( 
                KFscAtComSendEmail );
            break;
            
        case ERclCmdSendMeetingReq:
            iActionServiceWrapper->ExecuteActionL( 
                KFscAtComSendCalReq );
            break;
        case ERclCmdSendAudioMsg:
            iActionServiceWrapper->ExecuteActionL( 
                KFscAtComSendAudio );
            break;
        case ERclCmdToContact:
            iActionServiceWrapper->ExecuteActionL(
                KFscAtComCallPoc );
            break;
        case ERclCmdHelp:
            {
            CCoeAppUi* appUi = CCoeEnv::Static()->AppUi();
            CArrayFix<TCoeHelpContext>* contexts = appUi->AppHelpContextL();
            HlpLauncher::LaunchHelpApplicationL( 
                CCoeEnv::Static()->WsSession(), 
                contexts );
            }
            break;
        default:
            handled = EFalse;
            break;
        }
    
    if ( !handled )
        {
        if ( IsStateSet( EResultDlgOnTop ) )
            {
            switch ( aCommandId )
                {
                case ERclCmdAddAsRecipient:
                    // close the dialog, selected contact is stored in iSelectedContact
                    AddOperation( ECloseSearchResultDlg );
                    SetState( EReturnToCaller );
                    SetState( EResultSelected );
                    break;
                case ERclCmdViewDetails:
                    AddOperation( EOpenResultInfoDlg );
                    break;
                case ERclCmdNewSearch:
                    // open empty search query
                    AddOperation( EOpenSearchQueryEmpty );
                    break;
                case ERclCmdSendBusinessCard:
                    {
                    // first fetch details if they are not already
                    // fetched
                    HBufC* waitNoteText = StringLoader::LoadLC(
                        R_QTN_RCL_RETRIEVAL_WAIT_NOTE );
                    if ( RetrieveDetailsL(
                             iSearchResultDialog->CurrentItemIndex(),
                             *waitNoteText ) != NULL )
                        {
                        iSearchResultDialog->SendBusinessCardL();
                        }
                    CleanupStack::PopAndDestroy( waitNoteText );
                    }
                    break;
                case ERclCmdSaveToContacts:
                    {
                    // first retrieve all the details
                    HBufC* waitNoteText = StringLoader::LoadLC(
                        R_QTN_RCL_SAVING_WAIT_NOTE );
                    if ( RetrieveDetailsL( 
                             iSearchResultDialog->CurrentItemIndex(),
                             *waitNoteText ) != NULL )
                        {
                        
                        iActionServiceWrapper->ExecuteActionL( 
                            KFscAtManSaveAs );
                        
                        // in contact selector mode, return to the calling 
                        // application
                        if ( iMode == EModeContactSelector )
                            {
                            AddOperation( ECloseSearchResultDlg );
                            SetState( EReturnToCaller );
                            SetState( EResultSelected );
                            }
                        }
                    CleanupStack::PopAndDestroy( waitNoteText );
                    }
                    break;
                case EAknCmdExit:
                    iResult->iExitReason = TResult::EExitApplication;
                    AddOperation( ECloseSearchResultDlg );
                    SetState( EReturnToCaller );
                    break;
                default:
                    break;
                }
            }
        else if ( IsStateSet( EInfoDlgOnTop ) )
            {
            switch ( aCommandId )
                {
                case ERclCmdAddAsRecipient:
                    // close both dialogs, selected contact is in iSelectedContact
                    AddOperation( ECloseResultInfoDlg );
                    AddOperation( ECloseSearchResultDlg );
                    SetState( EResultSelected );
                    SetState( EReturnToCaller );
                    break;
                case ERclCmdSendCallbackReq:
                    iResultInfoDialog->SendCallbackRequestL();
                    break;
                case ERclCmdCreateNew:
                    iActionServiceWrapper->ExecuteActionL(
                        KFscAtManSaveAs );
                    // in contact selector mode, return to the calling
                    // application
                    if ( iMode == EModeContactSelector )
                        {
                        AddOperation( ECloseResultInfoDlg );
                        AddOperation( ECloseSearchResultDlg );
                        SetState( EReturnToCaller );
                        SetState( EResultSelected );
                        }
                    break;
                case ERclCmdUpdateExisting:
                    {
                    HBufC* resultText = 
                        iResultInfoDialog->UpdateExistingContactL();
                    if ( resultText != NULL )
                        {
                        CleanupStack::PushL( resultText );
                        DisplayNoteDialogL(
                            R_RCL_CONFIRMATION_NOTE,
                            *resultText,
                            ETrue );
                        CleanupStack::PopAndDestroy( resultText );
                        }
                    }
                    break;
                case ERclCmdSendBusinessCard:
                    iResultInfoDialog->SendBusinessCardL();
                    break;
                case EAknCmdExit:
                    // again close both dialogs
                    AddOperation( ECloseResultInfoDlg );
                    AddOperation( ECloseSearchResultDlg );
                    SetState( EReturnToCaller );
                    iResult->iExitReason = TResult::EExitApplication;
                    break;
                default:
                    break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::DialogDismissedL( TInt aButtonId )
    {
    FUNC_LOG;

    if ( aButtonId == EAknSoftkeyCancel )
        {
        
        // Action was canceled. Don't need to inform about errors
        iExitCode = KErrNone;
    	// cancel search or detail retrieval
    	if (iSession)
    		{
    		iSession->Cancel();
    		}
    	
    	// cancel time out timer
        if (iTimer)
    		{
    		iTimer->Cancel();
    		}
        
        // do actions based on state
        if ( IsStateSet( EInitial ) )
            {
            if ( iMode != EModeExistingCriteria )
                {
                // show new search query dialog
                AddOperation( EOpenSearchQueryDefault );
                }
            else
                {
                // with existing criteria, exit
                AddOperation( EExit );
                }
            }
        else if ( IsStateSet( EResultDlgOnTop ) )
            {
            if ( iWaitObjects.Count() == EContactRetrievalWait + 1 )
                {
                // contact retrieval was in progress
                StopActiveWait( EContactRetrievalWait );
                } // else new search was in progress, no actions needed
            }
        else if ( IsStateSet( EInfoDlgOnTop ) )
            {
            // details retrieval was in progress, stop active wait 
            StopActiveWait( EContactRetrievalWait );
            }
        
        }
    else
        {
        // wait dialog closed by itself. exit now if exiting
        if ( IsStateSet( EReturnToCaller ) &&
                ( IsStateSet( EResultSelected ) || IsStateSet( EInitial ) ) )
            {
            AddOperation( EExit );
            }
        
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::ExecuteSearchL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::ExecuteSearchL( 
    TBool aShowQueryDialog,
    const TDesC& aQueryText )
    {
    FUNC_LOG;


    iQueryText.Close();
    iQueryText.CreateMaxL( KRclQueryTextMaxLength );
    iQueryText = aQueryText;

    TBool result = EFalse;
    if ( aShowQueryDialog )
        {
        CPbkxRclQueryEditor* queryEditor = CPbkxRclQueryEditor::NewL( 
            iAccount->Name(), 
            iQueryText );
        result = queryEditor->ExecuteDialogLD();
        
        // save initial query text given by user
        if ( !IsStateSet( EResultDlgOnTop ) )
            {
            iQueryCriteria = iQueryText;
            }
        }
    else
        {
        result = ETrue;
        }

    if ( result )
        {
        
        // Start time for time out event
        iTimer->Start(
            KTwoMinutesTimerInterval, KTwoMinutesTimerInterval, 
            TCallBack(TimerCallBack, this));
        
        // execute search based on query text
        // use KMaxMatches + 1 to be able to catch "too many results error"
        iSession->LooseSearchL( iQueryText, KMaxMatches + 1 );
        HBufC* text = StringLoader::LoadLC( R_QTN_RCL_SEARCH_WAIT_NOTE, iQueryText );
        DisplayWaitDialogL( R_RCL_SEARCH_WAIT_DIALOG_CANCEL, *text );
        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        if ( !IsStateSet( EResultDlgOnTop ) )
            {
            AddOperation( EExit );
            } // otherwise just return to search result dlg
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DisplayWaitDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::DisplayWaitDialogL(
    TInt aDialogResourceId,
    const TDesC& aText )
    {
    FUNC_LOG;

    iWaitDialog = new ( ELeave ) CAknWaitDialog( NULL, ETrue );
    iWaitDialog->SetTextL( aText );
    iWaitDialog->SetCallback( this );
    iWaitDialog->ExecuteLD( aDialogResourceId );

    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DisplayNoteDialogL
// ---------------------------------------------------------------------------
//
TBool CPbkxRclServiceUiContextImpl::DisplayNoteDialogL(
    TInt aDialogResourceId, 
    const TDesC& aText,
    TBool aTimeout )
    {
    FUNC_LOG;
    CAknNoteDialog* dialog = NULL;
    
    if ( aTimeout )
        {
        dialog = new ( ELeave ) CAknNoteDialog( 
            CAknNoteDialog::ENoTone, 
            CAknNoteDialog::ELongTimeout );
        }
    else
        {
        dialog = new ( ELeave ) CAknNoteDialog();
        }

    dialog->PrepareLC( aDialogResourceId );
    dialog->SetTextL( aText );
        
    TBool ret = ( TBool )dialog->RunLD();
    

    return ret;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DisplayQueryDialogL
// ---------------------------------------------------------------------------
//
//Fix for: EASV-7KFGG3
TBool CPbkxRclServiceUiContextImpl::DisplayQueryDialogL(
    TInt aDialogResourceId, 
    const TDesC& aText )
    {
    FUNC_LOG;
    CAknQueryDialog* dialog = NULL;
    
    dialog = new ( ELeave ) CAknQueryDialog();
    dialog->PrepareLC( aDialogResourceId );
    dialog->SetPromptL( aText );
        
    TBool ret = ( TBool )dialog->RunLD();
    

    return ret;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::CloseWaitDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::CloseWaitDialogL()
    {
    FUNC_LOG;
    iWaitDialog->ProcessFinishedL();
    iWaitDialog = NULL;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DisplaySearchResultDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::DisplaySearchResultDialogL()
    {
    FUNC_LOG;

    if ( iSearchResultDialog == NULL )
        {
        // no search result dialog on screen, show new one

        // result array holds list box items and it is filled inside search
        // result dialog
        CDesCArrayFlat* resultArray = 
            new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
        CleanupStack::PushL( resultArray );
        
        TBool contactSelectorEnabled = iMode == EModeContactSelector;
        
        iSearchResultDialog = CPbkxRclSearchResultDlg::NewL(
            iSelectedIndex,
            resultArray,
            this,
            iContactCards,
            *iContactEngine,
            *iActionServiceWrapper,
            contactSelectorEnabled );
        
        UnsetState( EInitial );
        SetState( EResultDlgOnTop );
    
        // set action service mode
        iActionServiceWrapper->SetActionMenuMode( ETrue );
    
        iSearchResultDialog->ExecuteLD();
        
        iActionServiceWrapper->SetActionMenuMode( EFalse );


        iSearchResultDialog = NULL;
        
        CleanupStack::PopAndDestroy( resultArray );

        HandleSearchResultDialogExitL();

        UnsetState( EResultDlgOnTop );
        SetState( EInitial );
        }
    else
        { // iSearchResultDialog != NULL
        
        
        iSearchResultDialog->UpdateDialogL();
        }
    }
// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DisplayResultInfoDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::DisplayResultInfoDialogL()
    {
    FUNC_LOG;
    
    // items array holds the list box items in result info dialog and it is filled
    // result info dialog
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( items );
    
    TInt index = 0;
    
    TBool contactSelectorEnabled = iMode == EModeContactSelector;

    TInt itemIndex = iSearchResultDialog->CurrentItemIndex();

    // creation fails if item details cannot be retrieved or user cancels
    // retrieval
    iResultInfoDialog = CPbkxRclResultInfoDlg::NewL(
        index,
        items,
        this,
        this,
        *iContactEngine,
        *iActionServiceWrapper,
        contactSelectorEnabled,
        itemIndex );
    
    UnsetState( EResultDlgOnTop );
    SetState( EInfoDlgOnTop );
    
     // set action service mode
    iActionServiceWrapper->SetActionMenuMode( EFalse );
    
    //Fix for: PKAO-7NNCJ2
    if ( iSearchResultDialog )
        {
        iSearchResultDialog->InfoDlgVisible( ETrue );
        }
    
    iResultInfoDialog->ExecuteLD();
    
    // update action service since now search result dialog is on top
    iActionServiceWrapper->SetActionMenuMode( ETrue );

    if ( !IsStateSet( EReturnToCaller ) && iResult->iExitReason != TResult::EExitApplication )
    	{
    	iSearchResultDialog->SetCurrentContactToActionServiceL();
    	}
    
    //Fix for: PKAO-7NNCJ2
    if ( iSearchResultDialog )
        {
        iSearchResultDialog->InfoDlgVisible( EFalse );
        }
    
    iResultInfoDialog = NULL;
    
    UnsetState( EInfoDlgOnTop );
    SetState( EResultDlgOnTop );
        
    CleanupStack::PopAndDestroy( items );
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::CreateContactCardArray
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::CreateContactCardArray()
    {
    FUNC_LOG;

    // remove possible old contact cards
    iContactCards.Reset();
    

    for ( TInt i = 0; i < iSearchResults.Count(); i++ )
        {
        CContactItem& item = iSearchResults[i]->ContactItem();
        CContactCard* card = dynamic_cast<CContactCard*>( &item );
        if ( card != NULL )
            {
            iContactCards.Append( card );
            }
        }


    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DoLooseSearchCompletedL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::DoLooseSearchCompletedL( 
    TInt aStatus,
    RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults )
    {
    FUNC_LOG;
	
    // cancel time out timer
    if (iTimer)
		{
		iTimer->Cancel();
		}
    
    CloseWaitDialogL();

    if ( aStatus == KErrNone )
        {
        if ( aResults.Count() > KMaxMatches )
            {
            
            HBufC* text = StringLoader::LoadLC( 
                R_QTN_RCL_TOO_MANY_RESULTS_NOTE, 
                KMaxMatches );

            TBool ret = DisplayQueryDialogL(
                R_RCL_CONFIRMATION_NOTE_OK_REFINE,
                *text );
            
            CleanupStack::PopAndDestroy( text );
            
            if ( !ret )
                {
                // user wants new search
                AddOperation( EOpenSearchQueryDefault );

                // reset the old results
                aResults.ResetAndDestroy();
                return;
                }
            }
        
        // delete old results
        iSearchResults.ResetAndDestroy();
        TInt count = aResults.Count() <= KMaxMatches ? aResults.Count() : KMaxMatches;
        for ( TInt i = 0; i < count; i++ )
            {
            iSearchResults.AppendL( aResults[i] );
            }

        // delete the extra results
        for ( TInt j = aResults.Count() - 1; j >= KMaxMatches; j-- )
            {
            delete aResults[j];
            aResults.Remove( j );
            }
        
        CreateContactCardArray();
        TLinearOrder<CContactCard> sort( CPbkxRclServiceUiContextImpl::Sort );
        iContactCards.Sort( sort );
        
        AddOperation( EOpenSearchResultDlg );
        }
    else
        {

        // reset the array just in case
        aResults.ResetAndDestroy();

        // remote contact lookup failed, exit
        HandleError( aStatus );
        
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DoContactFieldsRetrievalCompletedL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::DoContactFieldsRetrievalCompletedL( TInt aStatus )
    {
    FUNC_LOG;

	// cancel time out timer
    if (iTimer)
		{
		iTimer->Cancel();
		}
    
    StopActiveWait( EContactRetrievalWait );

    CloseWaitDialogL();

    if ( aStatus != KErrNone )
        {
        // error occurred, so contact is not complete. this is checked
        // in RetrieveContactDetailsL method and it returns NULL
        // to the caller

        // exit application
        HandleError( aStatus );
        }
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::DoHandleOperationL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::DoHandleOperationL()
    {
    FUNC_LOG;

    if ( iOperations.Count() > 0 )
        {
        TOperation operation = iOperations.Pop();
        
        
        switch ( operation )
            {
            case EOpenSearchResultDlg:
                DisplaySearchResultDialogL();
                break;
            case EOpenResultInfoDlg:
                DisplayResultInfoDialogL();
                break;
            case ECloseSearchResultDlg:
                if ( iSearchResultDialog != NULL )
                    {
                    iSearchResultDialog->Close();
                    }
                break;
            case ECloseResultInfoDlg:
                if ( iResultInfoDialog != NULL )
                    {
                    iResultInfoDialog->Close();
                    }
                break;
            case EOpenSearchQueryDefault:
                ExecuteSearchL( ETrue, iQueryCriteria );
                break;
            case EOpenSearchQueryEmpty:
                ExecuteSearchL( ETrue, KNullDesC );
                break;
            case EExecuteSearchWithNoQuery:
                ExecuteSearchL( EFalse, iQueryCriteria );
                break;
            case EExit:
                StopActiveWait( EMainWait );
                break;
            default:
                PbkxRclPanic( EPbkxRclPanicGeneral );
                break;
            }
        
        if ( iOperations.Count() > 0 )
            {
            iEventScheduler->TriggerEvent();
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::HandleSearchResultDialogExitL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::HandleSearchResultDialogExitL()
    {
    FUNC_LOG;

    // this flag is set if wait dialog on the screen and we cannot exit before
    // wait dialog is closed itself
    TBool waitDlgOnScreen = EFalse;
    
    if ( IsStateSet( EResultSelected ) )
        {
        // if contact details are not retrieved, we must do that before exiting
        CPbkxRemoteContactLookupProtocolResult* result = GetResultByIndex( iSelectedIndex );
        if ( !result->IsComplete() )
            {
            HBufC* text = StringLoader::LoadLC( R_QTN_RCL_RETRIEVAL_WAIT_NOTE );
            RetrieveDetailsL( iSelectedIndex, *text );
            CleanupStack::PopAndDestroy( text );
            
            // if wait dialog is NULL, it is closed by CloseWaitDialogL method
            // and not by user. so it is still on the screen
            waitDlgOnScreen = iWaitDialog == NULL;
            
            // make sure we succeeded in detail retrieving. if not, return nothing.
            if ( result->IsComplete() )
                {
                iResult->iSelectedContactItem = GetSelectedContactL();
                }
            }
        else
            {
            iResult->iSelectedContactItem = GetSelectedContactL();
            }
        }
    
    if ( !waitDlgOnScreen )
        {
        // exit

        AddOperation( EExit );
        }

    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::HandleContextExitL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::HandleContextExitL()
    {
    FUNC_LOG;

    // set exit reason
    if ( iResult->iExitReason == TResult::EExitUndefined )
        {
        if ( iExitCode != KErrNone )
            {
            iResult->iExitReason = TResult::EExitError;
            }
        else if ( iResult->iSelectedContactItem != NULL )
            {
            iResult->iExitReason = TResult::EExitContactSelected;
            }
        else
            {
            iResult->iExitReason = TResult::EExitUserClosed;
            }
        }

    if ( iExitCode != KErrNone )
        {
        // show error note
        CTextResolver* resolver = CTextResolver::NewLC( *( CCoeEnv::Static() ) );
    
        // on return, contains information about the loaded error text
        TUint textFlags;
        TInt textId;

        const TDesC& errorText = resolver->ResolveErrorString( 
            iExitCode,
            textId,
            textFlags,
            CTextResolver::ECtxNoCtxNoSeparator );
        
        if ( textFlags & EErrorResBlankErrorFlag ||
             textFlags & ETextResolverUnknownErrorFlag )
            {
            // show general error message, because there was no good error 
            // message available
            HBufC* text = StringLoader::LoadLC( R_QTN_RCL_ERROR_NOTE );
            DisplayNoteDialogL( R_RCL_WARNING_NOTE, *text, ETrue );
            CleanupStack::PopAndDestroy( text );
            }
        else
            {
            DisplayNoteDialogL( R_RCL_WARNING_NOTE, errorText, ETrue );
            }
        CleanupStack::PopAndDestroy( resolver );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::StartActiveWaitL
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::StartActiveWaitL( TWaitObjectIndex aIndex )
    {
    FUNC_LOG;

    if ( aIndex != iWaitObjects.Count() )
        {
        // wait object index invalid, panic
        PbkxRclPanic( EPbkxRclPanicGeneral );
        }
    
    // starts new active wait and appends it in the list of wait objects
    CActiveSchedulerWait* wait = new ( ELeave ) CActiveSchedulerWait();
    CleanupStack::PushL( wait );
    iWaitObjects.AppendL( wait );
    CleanupStack::Pop( wait );
    wait->Start();
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::StopActiveWait
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::StopActiveWait( TWaitObjectIndex aIndex )
    {
    FUNC_LOG;
    if ( iWaitObjects.Count() != aIndex + 1 )
        {
        // invalid wait object count, panic
        PbkxRclPanic( EPbkxRclPanicGeneral );
        }
    
    // stops active wait of the most recently started wait
    CActiveSchedulerWait* wait = iWaitObjects[aIndex];
    iWaitObjects.Remove( aIndex );
    wait->AsyncStop();
    delete wait;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::SetState
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::SetState( TState aState )
    {
    FUNC_LOG;
    iState |= aState;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::UnsetState
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::UnsetState( TState aState )
    {
    FUNC_LOG;
    iState &= ~aState;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::IsStateSet
// ---------------------------------------------------------------------------
//
TBool CPbkxRclServiceUiContextImpl::IsStateSet( TState aState )
    {
    FUNC_LOG;
    return ( iState & aState ) != 0;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::ResetState
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::ResetState()
    {
    FUNC_LOG;
    iState = 0;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::AddOperation
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::AddOperation( TOperation aOperation )
    {
    FUNC_LOG;
    iOperations.Add( aOperation );
    iEventScheduler->TriggerEvent();
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::HandleError
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::HandleError( TInt aError )
    {
    FUNC_LOG;
    iExitCode = aError;
    SetState( EReturnToCaller );
    if ( IsStateSet( EInfoDlgOnTop ) )
        {
        AddOperation( ECloseResultInfoDlg );
        AddOperation( ECloseSearchResultDlg );
        }
    else if ( IsStateSet( EResultDlgOnTop ) )
        {
        AddOperation( ECloseSearchResultDlg );
        }
    
	// If dialog is NULL and state is EInitial & EReturnToCaller
	// EExit operation is added in DialogDismissedL callback
	if ( !iWaitDialog && IsStateSet( EInitial ) && IsStateSet( EReturnToCaller ) )
        { 
        return;
        }
	else 
	    {
	    AddOperation( EExit );
	    }
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::Sort
// ---------------------------------------------------------------------------
//
TInt CPbkxRclServiceUiContextImpl::Sort( 
    const CContactCard& aFirst, 
    const CContactCard& aSecond )
    {
    FUNC_LOG;
    
    TPtrC lastName1 = PbkxRclUtils::FieldText( &aFirst, KUidContactFieldFamilyName );
    TPtrC lastName2 = PbkxRclUtils::FieldText( &aSecond, KUidContactFieldFamilyName );

    TInt ret = 0;
    if ( lastName1 < lastName2 )
        {
        ret = -1;
        }
    else if ( lastName1 > lastName2 )
        {
        ret = 1;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::TimerCallBack
// ----------------------------------------------------------------------------
//
TInt CPbkxRclServiceUiContextImpl::TimerCallBack(TAny* aAny)
	{
    FUNC_LOG;
	CPbkxRclServiceUiContextImpl* self = static_cast<CPbkxRclServiceUiContextImpl*>( aAny );
	self->TimeOut();
	return KErrNone;
	}

// ----------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::TimeOut
// ----------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::TimeOut()
	{
    FUNC_LOG;
	iTimer->Cancel();
	
	if (iSession)
		{
		iSession->Cancel();
		}
	
	if (IsStateSet( EInfoDlgOnTop ) || IsStateSet( EResultDlgOnTop ))
		{
		StopActiveWait( EContactRetrievalWait );
		}
	
	TRAP_IGNORE( CloseWaitDialogL() );	
	HandleError( KErrTimedOut );
	}

//////////////////////////////////////////////////////////////////////////////
// TOperationQueue class definition
//////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::TOperationQueue::TOperationQueue
// ---------------------------------------------------------------------------
//
CPbkxRclServiceUiContextImpl::TOperationQueue::TOperationQueue() : 
    iCurrent( 0 ), iCount( 0 )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::TOperationQueue::Add
// ---------------------------------------------------------------------------
//
void CPbkxRclServiceUiContextImpl::TOperationQueue::Add( TOperation aOperation )
    {
    FUNC_LOG;
    if ( iCount == KMaxOperations )
        {
        PbkxRclPanic( EPbkxRclPanicGeneral );
        }
    
    TBool exists = EFalse;
    for ( TInt i = iCurrent, count = 0; 
          count < iCount; 
          ( ++i ) % KMaxOperations, count++ )
        {
        if ( iOperations[i] == aOperation )
            {
            exists = ETrue;
            break;
            }
        }

    if ( !exists )
        {
        TInt newIndex = ( iCurrent + iCount ) % KMaxOperations;
        iOperations[newIndex] = aOperation;
        iCount++;
        }
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclServiceUiContextImpl::TOperationQueue::Pop
// ---------------------------------------------------------------------------
//
CPbkxRclServiceUiContextImpl::TOperation 
CPbkxRclServiceUiContextImpl::TOperationQueue::Pop()
    {
    if ( iCount > 0 )
        {
        TOperation op = iOperations[iCurrent];
        iOperations[iCurrent] = ENoOperation;
        iCount--;
        
        if ( iCount > 0 )
            {
            iCurrent = ( ++iCurrent ) % KMaxOperations;
            }
        else
            {
            iCurrent = 0;
            }
        return op;
        }
    else
        {
        return ENoOperation;
        }
    }

TInt CPbkxRclServiceUiContextImpl::TOperationQueue::Count() const
    {
    FUNC_LOG;
    return iCount;
    }


