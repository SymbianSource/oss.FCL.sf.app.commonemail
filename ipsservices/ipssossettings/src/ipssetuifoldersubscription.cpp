/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*       folder subscription dialog
*
*/



#include "emailtrace.h"
#include <mtmuibas.h>
#include <imapcmds.h>
#include <gulicon.h>
#include <eikclb.h>
#include <eikclbd.h>
#include <eiktxlbm.h>
#include <aknlists.h>
#include <AknIconArray.h>                   
#include <StringLoader.h>
#include <hlplch.h>  
#include <AknQueryDialog.h>
#include <AknWaitDialog.h> //<cmail>
#include <mtudcbas.h>
#include <data_caging_path_literals.hrh>
#include <SendUiConsts.h>
#include <muiu.mbg>
#include <ipssossettings.rsg>
//<cmail>
#include <featmgr.h>
//</cmail>
#include "CFSMailCommon.h"
//</cmail>
#include <imapset.h>
#include <akntitle.h>

#include "ipssetutilsconsts.h"
#include "ipssetutils.h"
#include "ipssetuictrlsoftkey.h"
#include "ipssetuifolderlistarray.h"
#include "ipssetutilsoperationwait.h"
#include "ipssetui.h"
#include "ipsplgconnectandrefreshfolderlist.h"

#include "ipssetuifoldersubscription.h"

#include "FreestyleEmailUiConstants.h"
#include <csxhelp/cmail.hlp.hrh>

// Correct path is added to literal when it is used.
_LIT(KIpsSetUiBitmapFile, "z:muiu.MBM");
const TInt KImumFolderArrayVisibleItems = 5;
const TInt KImumFolderIconArrayGranularity = 6;

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::NewL
// ----------------------------------------------------------------------------
//
CIpsSetUiSubscriptionDialog* CIpsSetUiSubscriptionDialog::NewL(
    TUint64& aFlags,
    TMsvId aServiceId,
    CMsvSession& aSession,
    CAknTitlePane& aTitlePane,
    const TDesC& aTitlePaneText )
    {
    FUNC_LOG;
    CIpsSetUiSubscriptionDialog* self = 
        new ( ELeave ) CIpsSetUiSubscriptionDialog(      
            aFlags,  
            aServiceId,
            aSession,
            aTitlePane );

    CleanupStack::PushL(self);
    self->ConstructL( aTitlePaneText );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::~CIpsSetUiSubscriptionDialog
// ----------------------------------------------------------------------------
//
CIpsSetUiSubscriptionDialog::~CIpsSetUiSubscriptionDialog()
    {
    FUNC_LOG;
    iSession.RemoveObserver(*this);
    delete iBaseMtm;
    iBaseMtm = NULL;
    delete iContextEntry;
    iContextEntry = NULL;
    delete iFolderListArray;
    iFolderListArray = NULL;
    delete iSoftkeys;
    iSoftkeys = NULL;
    delete iMtmRegistry;
    iMtmRegistry = NULL;
    delete iOldTitlePaneText;
    iOldTitlePaneText = NULL;
    delete iAsyncWaitNote;
    iAsyncWaitNote = NULL;
    //<cmail>
    delete iAsyncHelper;
    iAsyncHelper = NULL;
    if(iEntrySelection)
        delete iEntrySelection;
    iEntrySelection = NULL;
    //</cmail>
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::CIpsSetUiSubscriptionDialog
// ----------------------------------------------------------------------------
//
CIpsSetUiSubscriptionDialog::CIpsSetUiSubscriptionDialog(
    TUint64& aFlags,
    TMsvId aServiceId,
    CMsvSession& aSession,
    CAknTitlePane& aTitlePane )
    :
    iFlags( aFlags ),
    iServiceId( aServiceId ),
    iSession( aSession ),
    iTitlePane( aTitlePane )
    , iState(CIpsSetUiSubscriptionDialog::EIdle), //<cmail>
    iFetchDialogDismissed(EFalse), iMultipleStarted(EFalse) //<cmail>
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::ConstructL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::ConstructL( const TDesC& aTitlePaneText )
    {
    FUNC_LOG;
    CAknDialog::ConstructL( R_IPS_SET_SETTINGS_SUBSCRIBE_MENU );

    // BaseMTM interface is required for subscribing and unsubscribing folders    
    iMtmRegistry = CClientMtmRegistry::NewL( iSession );
    iBaseMtm = iMtmRegistry->NewMtmL( KSenduiMtmImap4Uid );
    iBaseMtm->SwitchCurrentEntryL(iServiceId);
    
    iSession.AddObserverL(*this);
    iOrdering = TMsvSelectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);
    iContextEntry = CMsvEntry::NewL(iSession, iServiceId, iOrdering);
    iFolderListArray = CIpsSetUiFolderListArray::NewL( iSession, iServiceId );
    iOpenedFolderId = iServiceId;
    delete iOldTitlePaneText;
    iOldTitlePaneText = NULL;
    iOldTitlePaneText = iTitlePane.Text()->Alloc();
    iTitlePane.SetTextL( aTitlePaneText );
    //<cmail>
    iAsyncHelper = new (ELeave) CSubscriptionDialogActiveHelper(this);
    //</cmail>
    }

/******************************************************************************

    Init

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::PreLayoutDynInitL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::PreLayoutDynInitL()
    {
    FUNC_LOG;
    // <cmail>
    
    /*TBool refreshed = EFalse;
    CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
    if( confDialog->ExecuteLD( R_IPS_SET_CONNECT_FOR_UPDATE ) )
        {
        refreshed = ETrue;
        CreateOperationL( EIpsSetUiRefreshFolderList );
        }
	
    if( refreshed )
        {
        iFolderListArray->RefreshFolderListArrayL();
        }*/
    // </cmail>
    CEikColumnListBox* listBox =
        static_cast< CEikColumnListBox* >( Control( EIpsSetUiIdFolderListBox ) );

    listBox->SetListBoxObserver( this );

    // Create scroll bar
    listBox->CreateScrollBarFrameL();
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( iFolderListArray );
    listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    HBufC* emptyText =
        StringLoader::LoadLC( R_FSE_SETTINGS_MAIL_NOTE_NOTUPDATED );

    listBox->View()->SetListEmptyTextL( emptyText->Des() );

    CArrayPtrFlat<CGulIcon>* icons
        = new(ELeave) CAknIconArray( KImumFolderIconArrayGranularity );

    CleanupStack::PushL( icons );

    // create icon array
    TFileName fileName;

    TParse tp;
    tp.Set( KIpsSetUiBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    fileName.Copy( tp.FullName() );

    AppendIconsL( fileName, icons );

    reinterpret_cast<CAknSingleGraphicStyleListBox*>( listBox )->
        ItemDrawer()->ColumnData()->SetIconArray( icons );


    CleanupStack::Pop( icons ); 
    CleanupStack::PopAndDestroy( emptyText );

    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::PostLayoutDynInitL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::PostLayoutDynInitL()
    {
    FUNC_LOG;
    iSoftkeys = CIpsSetUiCtrlSoftkey::NewL( ButtonGroupContainer() );
    // Set this dialog for button listening
    iSoftkeys->SetMSKLabelling( ETrue );
    iSoftkeys->UpdateMiddleSoftkeyLabelL( 
        GetResourceForMiddlekey(), EAknSoftkeyOpen, NULL );

    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    TMsvId folderId = CurrentFolder();
    if ( folderId == KErrNotFound )
        {
        if ( aResourceId == R_IPS_SET_SETTINGS_SUBSCRIBE_MENUPANE )
            {
    	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
    		   {
    		   // remove help support in pf5250
    		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
    		   }
    	    
            aMenuPane->SetItemDimmed( EIpsSetUiFolderOpenFolder, ETrue );
            aMenuPane->SetItemDimmed( EIpsSetUiFolderSubscribe, ETrue );
            aMenuPane->SetItemDimmed( EIpsSetUiFolderUnsubscribe, ETrue );
            }
        }
    else
        {
        TMsvId prevEntry = iContextEntry->Entry().Id();
        iContextEntry->SetEntryL( folderId );
        const TMsvEmailEntry emailEntry( iContextEntry->Entry() );

        if ( aResourceId == R_IPS_SET_SETTINGS_SUBSCRIBE_MENUPANE )
            {
    	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
    		   {
    		   // remove help support in pf5250
    		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
    		   }
    	    
            if ( !ContextHasChildFolders() )
                {
                aMenuPane->SetItemDimmed( EIpsSetUiFolderOpenFolder, ETrue );
                }
            if ( IpsSetUtils::IsInbox( iSession, emailEntry ) )
                {
                //inbox subscription cannot be changed
                aMenuPane->SetItemDimmed( EIpsSetUiFolderSubscribe, ETrue );
                aMenuPane->SetItemDimmed( EIpsSetUiFolderUnsubscribe, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed(
                    EIpsSetUiFolderSubscribe, emailEntry.LocalSubscription() );
                aMenuPane->SetItemDimmed(
                    EIpsSetUiFolderUnsubscribe, !emailEntry.LocalSubscription() );
                }
            }

        iContextEntry->SetEntryL( prevEntry );
        }

    }
    
/******************************************************************************

    Events

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::HandleSessionEventL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::HandleSessionEventL(TMsvSessionEvent aEvent,
                    TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    FUNC_LOG;
    if(aEvent!=EMsvEntriesChanged)
        return;

    CMsvEntrySelection& sel=*static_cast<CMsvEntrySelection*>(aArg1);
    if(sel.Find(CurrentFolder())!=KErrNotFound)
        {
        CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>(
                                            Control(EIpsSetUiIdFolderListBox));
        listBox->DrawNow();
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::OkToExitL
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiSubscriptionDialog::OkToExitL(TInt aButtonId)
    {
    FUNC_LOG;
    TBool exit=EFalse;
    CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>(
                                            Control(EIpsSetUiIdFolderListBox));
    const TInt oldCount = listBox->Model()->NumberOfItems();

    switch(aButtonId)
    	{
    	case EAknSoftkeyOptions:
    		return CAknDialog::OkToExitL( EAknSoftkeyOptions );

    	case EIpsSetUiFolderSubscribe:
    	case EIpsSetUiFolderUnsubscribe:

    		CreateOperationL( aButtonId );
    		break;

    	case EAknSoftkeyClose:
    		exit = ETrue;
    		break;

    	case EIpsSetUiFolderOpenFolder:
    		// open folder which has subfolders...
    		iContextEntry->SetEntryL( CurrentFolder() );
    		if ( ContextHasChildFolders() )
    			{
    			iOpenedFolderId = CurrentFolder();
    			iFolderListArray->ChangEIpsSetUiFolderL( CurrentFolder() );
    			if ( listBox->Model()->NumberOfItems() )
    				{
    				listBox->SetTopItemIndex( 0 );
    				listBox->SetCurrentItemIndex( 0 );
    				}
    			if ( oldCount > listBox->Model()->NumberOfItems() )
    				{
    				listBox->HandleItemRemovalL();
    				}
    			else
    				{
    				listBox->HandleItemAdditionL();
    				}
    			}
    		break;

    	case EIpsSetUiRefreshFolderList:
    		// Always after successfull update go to root view,
    		// no matter where in the folder tree we are
    		iContextEntry->SetEntryL( iServiceId );
    		iFolderListArray->ChangEIpsSetUiFolderL( iServiceId );
    		if ( listBox->Model()->NumberOfItems() )
    			{
    			listBox->SetTopItemIndex( 0 );
    			listBox->SetCurrentItemIndex( 0 );
    			}
    		if ( oldCount > listBox->Model()->NumberOfItems() )
    			{
    			listBox->HandleItemRemovalL();
    			}
    		else
    			{
    			listBox->HandleItemAdditionL();
    			}
    		break;
    	case EAknSoftkeyBack:
    		{
    		if( iContextEntry->Entry().iType == KUidMsvServiceEntry )
    			{
    			//Only exit if current folder is service
    			if ( iOldTitlePaneText )
    				{
    				iTitlePane.SetTextL( *iOldTitlePaneText );
    				}
    			exit = ETrue;
    			break;
    			}

    		TMsvId folderToBeSelected = iOpenedFolderId;
    		iContextEntry->SetEntryL( iOpenedFolderId );

    		iOpenedFolderId = iContextEntry->Entry().Parent();
    		iFolderListArray->ChangEIpsSetUiFolderL( iOpenedFolderId );
    		iContextEntry->SetEntryL( iOpenedFolderId );

    		if ( oldCount > listBox->Model()->NumberOfItems() )
    			{
    			listBox->HandleItemRemovalL();
    			}
    		else
    			{
    			listBox->HandleItemAdditionL();
    			}

    		TInt loop;
    		for ( loop = iContextEntry->Count()-1; loop >= 0; loop-- )
    			{
    			if ( folderToBeSelected == (*iContextEntry)[loop].Id() )
    				{
    				break;
    				}
    			}
    		if ( loop < 0 )
    			{
    			loop = 0;
    			}
    		listBox->SetTopItemIndex( ( loop > KImumFolderArrayVisibleItems ?
    				loop - KImumFolderArrayVisibleItems : 0 ) );
    		listBox->SetCurrentItemIndex( loop );
    		}
    		break;

    	case EEikCmdExit:
    		exit = ETrue;
    		// Issue exit to the dialog
    		iFlags |= EIpsSetUiShouldExit;
    		break;

    	default:
    		break;
    	};

    listBox->DrawNow();
    //<cmail>
    if(exit)
        {
        CancelAllAsyncRequests();
        if(iAsyncWaitNote && (!iFetchDialogDismissed))
            iAsyncWaitNote->ProcessFinishedL();
        }
    //</cmail>
    return exit;
    }  
    
// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    FUNC_LOG;
	if( aEventType == EEventItemSingleClicked ||
		aEventType == EEventEnterKeyPressed )
		{
		TMsvId prevEntry = iContextEntry->Entry().Id();

		CEikColumnListBox* listBox =
			static_cast<CEikColumnListBox*>(
				Control( EIpsSetUiIdFolderListBox ) );
		iContextEntry->SetEntryL( CurrentFolder() );

		if ( listBox->Model()->NumberOfItems() &&
			 ContextHasChildFolders() )
			{
			//if has childs, open folder
			OkToExitL( EIpsSetUiFolderOpenFolder );
			}
		else
			{
			//if not, change subs state of the folder
			const TMsvEmailEntry emailEntry( iContextEntry->Entry() );

			if ( IpsSetUtils::IsInbox( iSession, emailEntry ) )
				{
				//inbox cannot be changed
				iContextEntry->SetEntryL( prevEntry );
				return;
				}

			if ( emailEntry.LocalSubscription() )
				{

				CreateOperationL( EIpsSetUiFolderUnsubscribe );
				}
			else
				{

				CreateOperationL( EIpsSetUiFolderSubscribe );
				}
			listBox->DrawNow();
			iContextEntry->SetEntryL( prevEntry );
			}
		// Don't set prev entry here because we're opening it,
		// so it will be valid
		}
    }


// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CIpsSetUiSubscriptionDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    // For key up events, update the middle softkey label 
    if ( aType == EEventKeyDown && aKeyEvent.iScanCode == EStdKeyDevice3 )
        {
        switch( CheckMSKState() )
            {
            case EIpsSetUiFolderOpenFolder:
                ProcessCommandL( EIpsSetUiFolderOpenFolder );
                break;
            case EIpsSetUiFolderUnsubscribe:
                ProcessCommandL( EIpsSetUiFolderUnsubscribe );
                break;
            case EIpsSetUiFolderSubscribe:
                ProcessCommandL( EIpsSetUiFolderSubscribe );
                break;
            default:
                break;
            }
        }
    
    if ( aType == EEventKeyUp )
        {
        iSoftkeys->UpdateMiddleSoftkeyLabelL( 
            GetResourceForMiddlekey(), EAknSoftkeyOpen, NULL );
        }
    
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::ProcessCommandL( TInt aCommandId )
    {
    FUNC_LOG;
    if ( MenuShowing() )
        {
        HideMenu();
        }

    CAknDialog::ProcessCommandL(aCommandId);
    switch( aCommandId )
        {
        case EAknSoftkeyOk:
        case EAknSoftkeyOpen:
            HandleListBoxEventL( NULL, EEventEnterKeyPressed );
            break;
            
        case EIpsSetUiFolderOpenFolder:
            OkToExitL( EIpsSetUiFolderOpenFolder );
            break;
        case EIpsSetUiFolderSubscribe:
        case EIpsSetUiFolderUnsubscribe:
            {
            TMsvEntry tEntry;
            TMsvId service;
            iSession.GetEntry( CurrentFolder(), service, tEntry );
            const TMsvEmailEntry emailEntry( tEntry );

            CreateOperationL( aCommandId );
            }
            break;
        case EIpsSetUiRefreshFolderList:
            SynchronisEIpsSetUiFoldersL();
            iFolderListArray->RefreshFolderListArrayL();
            OkToExitL( EIpsSetUiRefreshFolderList );
            break;

        case EAknCmdHelp:
            {
            LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
            break;
            }

        case EEikCmdExit:
            TryExitL( aCommandId );
            break;
        default:
            iEikonEnv->EikAppUi()->HandleCommandL(aCommandId);
            break;
        }
    }
        
/******************************************************************************

    Tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::SynchronisEIpsSetUiFoldersL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::SynchronisEIpsSetUiFoldersL()
    {
    FUNC_LOG;
    CreateOperationL( EIpsSetUiRefreshFolderList );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::CreateOperationL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::CreateOperationL( TInt aButtonId )
    {
    FUNC_LOG;
    //<cmail>
    //only one outstanding operation at a time is allowed
    if ( iState != CIpsSetUiSubscriptionDialog::EIdle)
    	{
    	return;
    	}
    if(iEntrySelection)
        {
        delete iEntrySelection;
        iEntrySelection = NULL;
        }
    iEntrySelection = new (ELeave) CMsvEntrySelection;
    //CleanupStack::PushL(mySelection);
    TBuf8<1> dummyParam;

    MFSMailRequestObserver* dummyFSObserver = NULL;
    CIpsPlgTimerOperation* timer = NULL;
    TFSMailMsgId mailboxId;
    //CIpsSetUtilsOperationWait* wait = CIpsSetUtilsOperationWait::NewLC();
    //</cmail>
    
    switch(aButtonId)
        {
    case EIpsSetUiFolderSubscribe:
    case EIpsSetUiFolderUnsubscribe:
        {
        //check for child / parent folders for question "want to subscribe/
        //unsub whole tree branch?" eat softkey commands. If user would leave
        //while subscription process in progress, it will crash. Only for 
        //subscribe operations. We need to be able to cancel folder list update
        //operation
        iSoftkeys->SetAbsorb( ETrue );

        //<cmail>
        HandleSubscriptionL( aButtonId, *iEntrySelection );
        iCount = iEntrySelection->Count();
        if( iCount > 1 )
            {
            //if more than one folder, we need to
            //do a batch of subscribe commands
            CMsvOperation* aMsvOperation = iBaseMtm->InvokeAsyncFunctionL( KIMAP4MTMStartBatch,
            *iEntrySelection, dummyParam, iAsyncHelper->iStatus);
            iAsyncHelper->Start(aMsvOperation); //ownership transferred
            iMultipleStarted = ETrue;
            iState = aButtonId==EIpsSetUiFolderSubscribe?
                            CIpsSetUiSubscriptionDialog::EMultipleSubscribe:
                            CIpsSetUiSubscriptionDialog::EMultipleUnsubscribe;
                }
        else if (iCount) //single operation
            {
            //for only selected folder, no batch needed
            CMsvOperation* aMsvOperation = iBaseMtm->InvokeAsyncFunctionL(
                (aButtonId==EIpsSetUiFolderSubscribe?
                    KIMAP4MTMLocalSubscribe:KIMAP4MTMLocalUnsubscribe),
                *iEntrySelection, dummyParam, iAsyncHelper->iStatus);
            iAsyncHelper->Start(aMsvOperation);//ownership transferred
            iState = ESingleOperation;
            }
        ShowWaitNoteL();
        }
        break;

    case EIpsSetUiRefreshFolderList:
    	{
    	mailboxId.SetId( iServiceId );
        iEntrySelection->AppendL(iServiceId);

        CMsvOperation* aMsvOperation = CIpsPlgConnectAndRefreshFolderList::NewL( 
        		iSession, 
        		CActive::EPriorityStandard, 
        		iAsyncHelper->iStatus, 
        		iServiceId, 
        		mailboxId, 
        		*iEntrySelection, 
        		*dummyFSObserver, 
        		*timer );

        iAsyncHelper->Start(aMsvOperation);//ownership transferred
        iState = ESingleOperation;
        ShowWaitNoteL();
    	}
        break;

    default:
        break;
        };
    
    if( aButtonId == EIpsSetUiFolderSubscribe || 
        aButtonId == EIpsSetUiFolderUnsubscribe )
        {
        iSoftkeys->SetAbsorb( EFalse );
        }
    //</cmail>
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::ContextHasChildFolders
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiSubscriptionDialog::ContextHasChildFolders() const
    {
    FUNC_LOG;
    TInt index=iContextEntry->Count()-1;
    while (index>=0 && (*iContextEntry)[index].iType!=KUidMsvFolderEntry)
        index--;

    if(index!=-1)
        return ETrue;

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::DisplayFinalProgressDialog
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::DisplayFinalProgressDialog(
    CMsvOperation& /* aCompletedOperation */ ) const
    {
    FUNC_LOG;
    //iBaseMtm->DisplayProgressSummary(aCompletedOperation.FinalProgress());
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::AppendIconsL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::AppendIconsL(
    TFileName& aFileName,
    CArrayPtrFlat<CGulIcon>* aIcons)
    {
    FUNC_LOG;
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TBool found = ETrue;


    for( TInt i=EMbmMuiuQgn_prop_folder_small; 
            i<EMbmMuiuQgn_prop_folder_sub_unsubs_new+1; i++ )
        {
        CGulIcon* icon = NULL;
        found = ETrue;
        switch( i )
            {
            case EMbmMuiuQgn_prop_folder_small:
                id.Set( KAknsIIDQgnPropFolderSmall );
                break;
            case EMbmMuiuQgn_prop_folder_subs:
                id.Set( KAknsIIDQgnPropFolderSubs );
                break;
            case EMbmMuiuQgn_prop_folder_subs_new:
                id.Set( KAknsIIDQgnPropFolderSubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_sub_small:
                id.Set( KAknsIIDQgnPropFolderSubSmall );
                break;
            case EMbmMuiuQgn_prop_folder_sub_subs:
                id.Set( KAknsIIDQgnPropFolderSubSubs );
                break;
            case EMbmMuiuQgn_prop_folder_sub_subs_new:
                id.Set( KAknsIIDQgnPropFolderSubSubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_unsubs_new:
                id.Set( KAknsIIDQgnPropFolderUnsubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_sub_unsubs_new:
                id.Set( KAknsIIDQgnPropFolderSubUnsubsNew );
                break;
            default:
                found = EFalse;
                break;
            }


        if( found )
            {
            AknsUtils::CreateIconL( skins, id, bitmap,
                bitmapMask, aFileName, i, i + 1 );

            icon = CGulIcon::NewL( bitmap, bitmapMask );

            // ownership of bitmap and mask transfered to icon
            CleanupStack::PushL( icon );
            aIcons->AppendL( icon );
            CleanupStack::Pop( icon ); // icon
            }
        }

    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::CurrentFolder
// ----------------------------------------------------------------------------
//
TMsvId CIpsSetUiSubscriptionDialog::CurrentFolder()
    {
    FUNC_LOG;
    CEikColumnListBox* listBox = 
        static_cast<CEikColumnListBox*>(Control(EIpsSetUiIdFolderListBox));
    TInt curIndex = listBox->CurrentItemIndex();

    return ( curIndex == KErrNotFound ? 
                curIndex : iFolderListArray->Id( curIndex ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::LaunchHelpL
// Launch help using context
//
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::LaunchHelpL( const TDesC& aContext )
    {
    FUNC_LOG;
    CArrayFix< TCoeHelpContext >* cntx = new( ELeave ) CArrayFixFlat< TCoeHelpContext >( 1 ); 
    CleanupStack::PushL( cntx );
    cntx->AppendL( TCoeHelpContext( KFSEmailUiUid, aContext ) );
    CleanupStack::Pop( cntx ); 
    HlpLauncher::LaunchHelpApplicationL( CEikonEnv::Static()->WsSession(), cntx );
    
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::GetHelpContext
// Returns helpcontext as aContext
//
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    FUNC_LOG;
    // when this string appears in mce.hlp.hrh, remove this _LIT !!!!!
    _LIT(KMCE_HLP_SETT_MAILFOLDER_UPD,"MCE_HLP_SETT_MAILFOLDER_UPD"); 
    const TUid KUidMce ={0x100058C5};
    aContext.iMajor = KUidMce;
    aContext.iContext = KMCE_HLP_SETT_MAILFOLDER_UPD;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::HandleSubscriptionL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::HandleSubscriptionL( 
    TInt aSubType, 
    CMsvEntrySelection& aSelection )
    {
    FUNC_LOG;
    TMsvId current = CurrentFolder();

    if( iFolderListArray->ContextHasChildren( current ) )
        {
        //CMsvEntry is used this way because GetContextChildrenL is recursive.
        //Otherwise CMsvEntry objects would be created several times before the
        //first one is deleted which consumes lots of memory. Now we just set
        //new CMsvEntry over this one several times.
        CMsvEntry* entry = CMsvEntry::NewL(iSession, current, iOrdering);
        CleanupStack::PushL( entry );
        CMsvEntrySelection* sel = 
            iFolderListArray->GetContextChildrenL( current, *entry );
        CleanupStack::PopAndDestroy( entry );

        CleanupStack::PushL( sel );

        TBool allFolders = CheckAreAllFoldersIncludedL( aSubType, sel );

        CAknQueryDialog* subsQuery = new(ELeave) CAknQueryDialog;

        switch( aSubType )
            {
            case EIpsSetUiFolderSubscribe:
                {
                //subscribe also all sub folders?
                subsQuery->PrepareLC( R_IPS_SET_DO_SUBS_FOR_SUBFOLDERS );
                }
                break;
            case EIpsSetUiFolderUnsubscribe:
                {
                //unsubscribe also all sub folders?
                subsQuery->PrepareLC( R_IPS_SET_DO_UNSUBS_FOR_SUBFOLDERS );
                }
                break;
            default:
                User::Leave( KErrNotSupported );
                break;
            }//switch


        TInt ret = 1;//by default do all subfolders

        //if all subfolders already subbed / unsubbed, no need for query
        if( !allFolders )
            {
            ret = subsQuery->RunLD();
            }
        else
            {
            CleanupStack::PopAndDestroy( subsQuery );
            }

        if( ret )
            {
            //append all sub folders

            TInt count = sel->Count();

            //first append current
            aSelection.AppendL( current );

            //then the children
            TInt i;
            for( i=0; i<count; i++ )
                {
                aSelection.AppendL( sel->At(i) );
                }
            }
        else
            {
            //only selected
            aSelection.AppendL( current );
            }

        CleanupStack::PopAndDestroy( sel );
        }//if
    else
        {
        //no children, append only current
        aSelection.AppendL( current );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::CheckAreAllFoldersIncludedL
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiSubscriptionDialog::CheckAreAllFoldersIncludedL( 
    TInt aSubType, 
    CMsvEntrySelection* sel )
    {
    FUNC_LOG;
    TMsvEntry tEntry;
    TMsvId service;

    TInt count = sel->Count();
    TInt i;
    TInt result = 0;

    for( i=0; i<count; i++ )
        {
        iSession.GetEntry( sel->At(i), service, tEntry );
        TMsvEmailEntry emailEntry( tEntry );

        if( aSubType == EIpsSetUiFolderSubscribe )
            {
            //count all subscribed entries
            if( emailEntry.LocalSubscription() )
                {
                result++;
                }
            }
        else
            {
            //count all unsubscribed entries
            if( !emailEntry.LocalSubscription() )
                {
                result++;
                }
            }
        }

    if( count == result )
        {
        return ETrue;
        }

    return EFalse;

    }

// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::CheckMSKState()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiSubscriptionDialog::CheckMSKState()
    {
    FUNC_LOG;
    TInt ret(KErrNotFound);
    
    // open subfolder view if current folder have them
    if( iFolderListArray->ContextHasChildren( CurrentFolder() ) )
        {
        ret = EIpsSetUiFolderOpenFolder;
        }
    // subscribe/unsubscribe
    else
        {
        TMsvId prevEntry = iContextEntry->Entry().Id();
        TMsvId folderId = CurrentFolder();
        TRAP_IGNORE(iContextEntry->SetEntryL( folderId ) );
        const TMsvEmailEntry emailEntry( iContextEntry->Entry() );
        
        if( IpsSetUtils::IsInbox( iSession, emailEntry ) )
            {
            ret = KErrNotFound;
            }
        else if( emailEntry.LocalSubscription() )
            {
            ret = EIpsSetUiFolderUnsubscribe;
            }
        else
            {
            ret = EIpsSetUiFolderSubscribe;
            }
        TRAP_IGNORE( iContextEntry->SetEntryL( prevEntry ) );
        }
    return ret;
    }


// ----------------------------------------------------------------------------
// CIpsSetUiBaseDialog::GetResourceForMiddlekey()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiSubscriptionDialog::GetResourceForMiddlekey()
    {
    FUNC_LOG;
//#ifdef MSK
    TInt resource = KErrNotFound;
        
        switch( CheckMSKState() )
            {
            case EIpsSetUiFolderOpenFolder:
                resource = R_FSE_SETTINGS_MAIL_FOLDER_OPEN;
                break;
            case EIpsSetUiFolderUnsubscribe:
                resource = R_FSE_SETTINGS_MAIL_FOLDER_UNSUBSCRIBE;
                break;
            case EIpsSetUiFolderSubscribe:
                resource = R_FSE_SETTINGS_MAIL_FOLDER_SUBSCRIBE;
                break;
            default:
                resource = KErrNotFound;
                break;
            }
    
    return resource;
/*
#else
    return KErrNotFound;
#endif        
*/    
    }

//<cmail>
// ----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::HandleAsyncRequestCompletedL
// ----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::HandleAsyncRequestCompletedL( TInt /*aErrorId*/ )
    {
    FUNC_LOG;
    switch (iState)
        {
        case CIpsSetUiSubscriptionDialog::EIdle:
            //just ignore            
            break;
        
        case CIpsSetUiSubscriptionDialog::ESingleOperation:
            if(iAsyncWaitNote && (!iFetchDialogDismissed))
                iAsyncWaitNote->ProcessFinishedL();
            iState = CIpsSetUiSubscriptionDialog::EIdle;
            iSoftkeys->UpdateMiddleSoftkeyLabelL( 
                GetResourceForMiddlekey(), EAknSoftkeyOpen, NULL );
            break;
            
        case CIpsSetUiSubscriptionDialog::EMultipleSubscribe:
        case CIpsSetUiSubscriptionDialog::EMultipleUnsubscribe:
            if(iMultipleStarted)
                {
                iMultipleStarted = EFalse;
                }            
            else if(iCount) // dont delete in case iCount become 0
                //becausae of messagingfw does not work as specified we can't let iEntrySelection
                //empty for KIMAP4MTMEndBatch command
                iEntrySelection->Delete(0);
            TBuf8<1> dummyParam;
            if( iCount ) // if still some entries left for processing
                {
                CMsvOperation* aMsvOperation = iBaseMtm->InvokeAsyncFunctionL(
                    (iState==CIpsSetUiSubscriptionDialog::EMultipleSubscribe?
                        KIMAP4MTMLocalSubscribe:KIMAP4MTMLocalUnsubscribe),
                    *iEntrySelection, dummyParam, iAsyncHelper->iStatus);
                iAsyncHelper->Start(aMsvOperation);
                iCount--;
                }
            else //end processing
                {
                CMsvOperation* aMsvOperation = iBaseMtm->InvokeAsyncFunctionL( KIMAP4MTMEndBatch,
                *iEntrySelection, dummyParam, iAsyncHelper->iStatus);
                iState = ESingleOperation;
                iAsyncHelper->Start(aMsvOperation);
                }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::DialogDismissedL
// -----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::DialogDismissedL( TInt aButtonId )
    {
    FUNC_LOG;
    if((aButtonId == EAknSoftkeyCancel) && 
                                    (iState!=CIpsSetUiSubscriptionDialog::EIdle))
        {
        CancelAllAsyncRequests();
        }
    iFetchDialogDismissed = ETrue;
    }

// -----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::ShowWaitNoteL
// -----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::ShowWaitNoteL()
    {
    FUNC_LOG;
    if ( !iAsyncWaitNote )
        {
        iAsyncWaitNote = new(ELeave)CAknWaitDialog(
                       (REINTERPRET_CAST(CEikDialog**,&iAsyncWaitNote)), EFalse);
        iAsyncWaitNote->SetCallback(this);
        iFetchDialogDismissed = EFalse;
        iAsyncWaitNote->ExecuteLD(R_IPS_SETUI_WAIT_REFRESH_FOLDER_LIST_DIALOG);
        }
    }

// -----------------------------------------------------------------------------
// CIpsSetUiSubscriptionDialog::DialogDismissedL
// -----------------------------------------------------------------------------
//
void CIpsSetUiSubscriptionDialog::CancelAllAsyncRequests()
    {
    FUNC_LOG;
    iState = CIpsSetUiSubscriptionDialog::EIdle;
    if(iAsyncHelper)
        iAsyncHelper->Cancel();
    iMultipleStarted = EFalse;
    }

// ----------------------------------------------------------------------------
// CSubscriptionDialogActiveHelper::CSubscriptionDialogActiveHelper
// ----------------------------------------------------------------------------
//
CSubscriptionDialogActiveHelper::CSubscriptionDialogActiveHelper
                            (CIpsSetUiSubscriptionDialog* aSubscriptionDialog):
                                            CActive(CActive::EPriorityStandard),
                                            iMsvOperation(NULL)
    {
    FUNC_LOG;
    iSubscriptionDialog = aSubscriptionDialog;
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CSubscriptionDialogActiveHelper::~CSubscriptionDialogActiveHelper()
// ----------------------------------------------------------------------------
//
CSubscriptionDialogActiveHelper::~CSubscriptionDialogActiveHelper()
    {
    FUNC_LOG;
    //Cancel();
    iSubscriptionDialog = NULL;
    if(iMsvOperation)
        delete iMsvOperation;
    }
    
// ----------------------------------------------------------------------------
// CSubscriptionDialogActiveHelper::Start()
// ----------------------------------------------------------------------------
//
void CSubscriptionDialogActiveHelper::Start(CMsvOperation* aMsvOperation)
    {
    FUNC_LOG;
    if(iMsvOperation)
        delete iMsvOperation;
    iMsvOperation = aMsvOperation;
    SetActive();
    }

// ----------------------------------------------------------------------------
// CSubscriptionDialogActiveHelper::RunL()
// ----------------------------------------------------------------------------
//
void CSubscriptionDialogActiveHelper::RunL()
    {
    FUNC_LOG;
    if(iSubscriptionDialog)
        iSubscriptionDialog->HandleAsyncRequestCompletedL(KErrNone);
    }

// ----------------------------------------------------------------------------
// CSubscriptionDialogActiveHelper::DoCancel()
// ----------------------------------------------------------------------------
//
void CSubscriptionDialogActiveHelper::DoCancel()
    {
    FUNC_LOG;
    if(iMsvOperation)
        iMsvOperation->Cancel();
    }
//</cmail>    


// End of File

