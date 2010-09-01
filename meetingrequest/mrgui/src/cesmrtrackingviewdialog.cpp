/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Version     : %version: e002sa32#15.1.1 %
*
*/

#include "emailtrace.h"
#include "cesmrtrackingviewdialog.h"

// SYSTEM INCLUDE
#include <AknUtils.h>
#include <StringLoader.h>
#include <calentry.h>
#include <MAgnEntryUi.h>
//<cmail>
#include "cesmrpolicy.h"
//</cmail>
#include <esmrgui.rsg>

#include "esmrgui.hrh"
#include "cesmrtrackingview.h"
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
		const CESMRPolicy& aPolicy,
		MESMRCalEntry& aEntry,
		MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    CESMRTrackingViewDialog* self =
	new (ELeave) CESMRTrackingViewDialog( aPolicy, aEntry, aCallback);
	CleanupStack::PushL( self );
	self->ConstructL();
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
    
    iESMRStatic.Close();
    }

// ---------------------------------------------------------------------------
// CESMRTrackingViewDialog::CESMRTrackingViewDialog
// ---------------------------------------------------------------------------
//
CESMRTrackingViewDialog::CESMRTrackingViewDialog(
        const CESMRPolicy& aPolicy,
		MESMRCalEntry& aEntry,
		MAgnEntryUiCallback& aCallback) :
    iPolicy( aPolicy ),
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
void CESMRTrackingViewDialog::ConstructL()
    {
    FUNC_LOG;
    CAknDialog::ConstructL ( R_TRACKING_DIALOG_MENUBAR );
    iESMRStatic.ConnectL();
    iESMRStatic.ContactMenuHandlerL().Reset();

    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, clientRect );

    // Sort the attendees list so that Req attendeees comes  first
    // and then opt attendees
    SortAttendeesL();

    // Set the index of builder to zero so that
    // participant field is created from index.

    iESMRStatic.SetCurrentFieldIndex(0);

    iView = CESMRTrackingView::NewL( iEntry, clientRect,
            CountAttendeesL(CCalAttendee::EReqParticipant),
            CountAttendeesL(CCalAttendee::EOptParticipant));
    iView->SetParentDialog(this);

    if ( iEntry.Entry().SummaryL().Length() == 0 )
		 {
		 // if no title, set unnamed text:
		 HBufC* title = StringLoader::LoadLC (R_QTN_MEET_REQ_CONFLICT_UNNAMED);
		 iView->SetTitleL( *title );
		 CleanupStack::PopAndDestroy( title );
		 }
    else
         {
         iView->SetTitleL( iEntry.Entry().SummaryL() );
         }

    iMenuBar->SetContextMenuTitleResourceId( R_MR_TRACKING_VIEW_CONTEXT_MENU );
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
    if ( aResourceId == R_ACTION_MENU )
        {
        iESMRStatic.ContactMenuHandlerL().InitOptionsMenuL( aMenuPane );
        }

    if ( aResourceId == R_MR_TRACKING_VIEW_MENU )
        {
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
// CESMRTrackingViewDialog::HandleFieldEventL
// ---------------------------------------------------------------------------

void CESMRTrackingViewDialog::HandleListEventL()
    {
    FUNC_LOG;
    iMenuBar->TryDisplayContextMenuBarL();
    }

// End of File

