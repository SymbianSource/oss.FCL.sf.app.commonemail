/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*  Description : ESMR tracking view dialog
*  Version     : %version: tr1sido#9 %
*
*/

#include "emailtrace.h"
#include "cesmrtrackingviewdialog.h"

// SYSTEM INCLUDE
#include <AknUtils.h>
#include <StringLoader.h>
#include <calentry.h>
#include <AiwServiceHandler.h>
#include <featmgr.h>
#include <bldvariant.hrh>
//<cmail>
#include <featmgr.h>
//</cmail>

#include "cesmrpolicy.h"
//</cmail>
#include <esmrgui.rsg>

#include "esmrgui.hrh"
#include "cesmrview.h"
#include "cesmrviewerfieldstorage.h"
#include "mesmrcalentry.h"
#include "cesmrcontactmenuhandler.h"
#include "esmrconfig.hrh"
#include "esmrinternaluid.h"

#ifdef _DEBUG
namespace // codescanner::namespace
{
// Literal for viewer dialog
_LIT( KESMRTrackinglgPanicTxt, "ESMRTrackingDlg" );

enum TESMRViewerDlgPanic
    {
    EESMRTrackingVieweDlgInvalidCommand
    };

void Panic( TESMRViewerDlgPanic aPanic )
    {
    User::Panic( KESMRTrackinglgPanicTxt, aPanic );
    }
}
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::NewL
// ---------------------------------------------------------------------------
//
CESMRTrackingViewDialog* CESMRTrackingViewDialog::NewL(
		CESMRPolicy* aPolicy,
		MESMRCalEntry& aEntry,
		MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    CESMRTrackingViewDialog* self =
	new (ELeave) CESMRTrackingViewDialog(aEntry,aCallback);
	CleanupStack::PushL( self );
	self->ConstructL( aPolicy );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::~CESMRTrackingViewDialog
// ---------------------------------------------------------------------------
//
CESMRTrackingViewDialog::~CESMRTrackingViewDialog()
    {
    FUNC_LOG;
    if ( iFeatureManagerInitialized )
        {
        FeatureManager::UnInitializeLib();
        }

    if ( iServiceHandler )
        {
        iServiceHandler->DetachMenu(R_MR_TRACKING_VIEW_MENU, R_PS_AIW_INTEREST);
        delete iServiceHandler;
        }

    iESMRStatic.Close();
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::CESMRTrackingViewDialog
// ---------------------------------------------------------------------------
//
CESMRTrackingViewDialog::CESMRTrackingViewDialog( 
		MESMRCalEntry& aEntry,
		MAgnEntryUiCallback& aCallback) :
    iEntry( aEntry ),
    iCallback(aCallback)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::ConstructL(CESMRPolicy* aPolicy)
    {
    FUNC_LOG;
    CAknDialog::ConstructL ( R_TRACKING_DIALOG_MENUBAR );
    iPolicy = aPolicy;
    iESMRStatic.ConnectL();
    iESMRStatic.ContactMenuHandlerL().Reset();

    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, clientRect );

    TBool responseReady(EFalse);

    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;

	// <cmail> remove custom feature KFeatureIdPrintingFrameworkCalendarPlugin 
	// </cmail>		

    // Sort the attendees list so that Req attendeees comes  first
    // and then opt attendees
    SortAttendeesL();

    // Set the index of builder to zero so that
    // participant field is created from index.

    iESMRStatic.SetCurrentFieldIndex(0);

    CESMRViewerFieldStorage* storage =
             CESMRViewerFieldStorage::NewL(
                     aPolicy,
                     NULL,
                     responseReady,
                     *this,
                     CountAttendeesL(CCalAttendee::EReqParticipant),
                     CountAttendeesL(CCalAttendee::EOptParticipant) );

    CleanupStack::PushL( storage );
    // storage ownership is transferred to CESMRView
    iView = CESMRView::NewL(storage, iEntry, clientRect );
    CleanupStack::Pop( storage ); 
     
    if ( iEntry.Entry().SummaryL().Length() == 0 )
		 {
		 // if no title, set unnamed text:
		 HBufC* title = StringLoader::LoadLC (R_QTN_MEET_REQ_CONFLICT_UNNAMED);
		 iView->SetTitleL( *title, ETrue );
		 CleanupStack::PopAndDestroy( title );
		 }
    else
         {
         iView->SetTitleL( iEntry.Entry().SummaryL(), ETrue );
         }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::CreateCustomControlL
// ---------------------------------------------------------------------------
//
SEikControlInfo
    CESMRTrackingViewDialog::CreateCustomControlL( TInt aType )
    {
    FUNC_LOG;
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iFlags = 0;
    controlInfo.iTrailerTextId = 0;

    if ( aType == EEsMrTrackingType )
        {
        controlInfo.iControl = iView;
        }

    return controlInfo;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRTrackingViewDialog::OfferKeyEventL
        (const TKeyEvent& aEvent, TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response = CAknDialog::OfferKeyEventL( aEvent, aType );
	
    if ( ( response == EKeyWasNotConsumed && aEvent.iCode != EKeyEscape)
		   && (!MenuShowing()) )
		{
		response = iView->OfferKeyEventL( aEvent, aType );
		}
	return response;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CESMRTrackingViewDialog::OkToExitL (TInt aButtonId)
    {
    FUNC_LOG;
    TBool res( EFalse );
    // show options
    switch ( aButtonId )
        {
        case EAknSoftkeyOptions:
            {
            CAknDialog::DisplayMenuL();
            break;
            }            
        case EESMRCmdBack:
            {
            res = ETrue;
            break;
            }            
        case EESMRCmdCall:
            {
            iESMRStatic.ContactMenuHandlerL().ExecuteOptionsMenuL( 
												EESMRCmdActionMenuFirst );
            break;
            }
        default:
            {
            res = CAknDialog::OkToExitL( aButtonId );
            break;
            }       
        }

    return res;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::CountAttendeesL
// ---------------------------------------------------------------------------
//
TInt CESMRTrackingViewDialog::CountAttendeesL( // codescanner::intleaves
        CCalAttendee::TCalRole aRole)
    {
    FUNC_LOG;
    RPointerArray<CCalAttendee>& calAttendeeList = iEntry.Entry().AttendeesL();
    
    TInt totalParticipants = calAttendeeList.Count();
    TInt totalAttendees(0);

    for (TInt i=0; i< totalParticipants; i++)
        {
        CCalAttendee::TCalRole role = CCalAttendee::ENonParticipant;
        role = calAttendeeList[i]->RoleL();
        if ( role == aRole )
        	{
        	++totalAttendees;
        	}
        }
    return totalAttendees;
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::SortAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::SortAttendeesL()
    {
    FUNC_LOG;
    RPointerArray<CCalAttendee>& attendees = iEntry.Entry().AttendeesL();
    // Sort the attendees so that REqPart come first and then optPart
    // This is required once only
     TInt i, j;
     TBool flag = ETrue;    // set flag to true to begin initial pass
     TInt arrayLength = attendees.Count();
     for(i = 1; (i <= arrayLength) && flag; i++)
         {
          flag = EFalse;
          for (j=0; j < (arrayLength -1); j++)
              {
              CCalAttendee::TCalRole lhs = CCalAttendee::ENonParticipant;
              CCalAttendee::TCalRole rhs = CCalAttendee::ENonParticipant;
              lhs = attendees[j+1]->RoleL();
              rhs = attendees[j]->RoleL();
              if ( lhs < rhs )
                  {
                  CCalAttendee* tempAttendee = attendees[j];
                  attendees[j]   = attendees[j+1];
                  attendees[j+1] = tempAttendee;
                  flag = ETrue;   // indicates that a swap occurred.
                  }
              }// Inner loop (j)
         } // Outer loop (i)
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::ProcessCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TRAPD( err, DoProcessCommandL( aCommand ) );
    if ( err != KErrNone &&
         err != KErrCancel &&
         err != KErrArgument )
        {
        User::Leave(err);
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::DoProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::DoProcessCommandL( TInt aCommand )
    {
    FUNC_LOG;
    CAknDialog::ProcessCommandL( aCommand );
    switch ( aCommand )
        {
        case EESMRCmdPrint: // Fall through
        case EESMRCmdPrint_Reserved1: // Fall through
        case EESMRCmdPrint_Reserved2: // Fall through
        case EESMRCmdPrint_Reserved3: // Fall through
        case EESMRCmdPrint_Reserved4:
            {
            this->MakeVisible(EFalse); 
            HandlePrintCommandL(aCommand);
            this->MakeVisible(ETrue); 
            break;
            }
        case EAknCmdExit:
        	{
            TryExitL( aCommand );
            break;
        	}
        case EAknCmdHelp:
            {
            iView->LaunchViewerHelpL();
            break;
            }
        default:
        	{
            if ( aCommand >= EESMRCmdActionMenuFirst && 
            	 aCommand <= EESMRCmdActionMenuLast )
                {
                iESMRStatic.ContactMenuHandlerL().ExecuteOptionsMenuL( 
																aCommand );
                }
            else
                {
                __ASSERT_DEBUG( EFalse, 
                		Panic( EESMRTrackingVieweDlgInvalidCommand ) );
                }
            break;
        	}
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::DynInitMenuPaneL(
		TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    if ( iServiceHandler )
        {
        if ( iServiceHandler->HandleSubmenuL ( *aMenuPane ) )
            {
            return;
            }
        }
    if ( aResourceId == R_ACTION_MENU )
        {
        iESMRStatic.ContactMenuHandlerL().InitOptionsMenuL( aMenuPane );
        }

    if ( aResourceId == R_MR_TRACKING_VIEW_MENU )
        {
	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
		   }
		
	    // <cmail> remove custom feature KFeatureIdPrintingFrameworkCalendarPlugin 
	    // </cmail>				
        aMenuPane->SetItemDimmed( EESMRCmdPrint, ETrue );

        aMenuPane->SetItemDimmed( EESMRCmdActionMenu, 
        		!iESMRStatic.ContactMenuHandlerL().OptionsMenuAvailable() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::ActivateL()
    {
    FUNC_LOG;
    CAknDialog::ActivateL();
    iView->InternalizeL();
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::ExecuteViewLD
// ---------------------------------------------------------------------------
//
TInt CESMRTrackingViewDialog::ExecuteViewLD()
    {
    FUNC_LOG;
    return ExecuteLD( R_MRTRACKING_DIALOG );
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::HandlePrintCommandL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::HandlePrintCommandL(TInt aCommand)
    {
    FUNC_LOG;
    CAiwGenericParamList& inParams = iServiceHandler->InParamListL();

    // Param date

    TCalTime startTime = iEntry.Entry().StartTimeL();

    TAiwGenericParam dateParam( EGenericParamDateTime );
    TTime activeDay = startTime.TimeUtcL();

    TAiwGenericParam calendarParam( EGenericParamCalendarItem );
    calendarParam.Value().Set( TUid::Uid(iEntry.Entry().LocalUidL()) );
    inParams.AppendL( calendarParam );

    // Append date param
    dateParam.Value().Set( activeDay );
    inParams.AppendL( dateParam );

    const TUid uid( TUid::Uid( KUidCalendarApplication ) );
    TAiwGenericParam uidParam( EGenericParamApplication );
    uidParam.Value().Set( uid );
    inParams.AppendL( uidParam );

    // Execute service command with given parameters
    iServiceHandler->ExecuteMenuCmdL( aCommand,
                                      inParams,
                                      iServiceHandler->OutParamListL(),
                                      0,
                                      NULL );

    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::HandleFieldEventL
// ---------------------------------------------------------------------------
//
void CESMRTrackingViewDialog::HandleFieldEventL(
        const MESMRFieldEvent& /*aEvent*/ )
    {
    FUNC_LOG;
    // From MESMRFieldEventObserver. Do nothing
    }

// End of File

