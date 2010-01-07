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
* Description:  ESMR editor dialog
*
*/

#include "emailtrace.h"
#include "cesmreditordialog.h"

//<cmail>
#include "esmrdef.h"
#include <esmrgui.rsg>
#include "cesmrpolicy.h"
//</cmail>
#include <eikenv.h>
#include <eikappui.h>
#include <avkon.hrh>
#include <eikmenup.h>
#include <MAgnEntryUi.h>
#include <layoutmetadata.cdl.h>
#include <AknDef.h>
#include <AknUtils.h>

#include <AknIndicatorContainer.h>
#include <aknenv.h>
#include <aknEditStateIndicator.h>
#include <calentry.h>
#include <CalenInterimUtils2.h>

#include <akntitle.h>
#include <StringLoader.h>
//<cmail>
#include <featmgr.h>
//</cmail>
#include "esmrcommands.h"
//</cmail>

#include <AiwServiceHandler.h>
#include <featmgr.h>
#include <bldvariant.hrh>

#include "esmrgui.hrh"
#include "cesmrview.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrconfirmationquery.h"
#include "cesmrlistquery.h"
#include "cesmreditorfieldstorage.h"
#include "cesmrmixedfieldstorage.h"
#include "cesmrsendui.h"
#include "cesmrfield.h"
#include "cesmrfieldbuilderinterface.h"
#include "mesmrresponseobserver.h"
#include "cesmrglobalnote.h"
#include "cesmrtitlepanehandler.h"
#include "esmrinternaluid.h"
#include "cesmrlocationpluginhandler.h"
#include "cesmraddressinfohandler.h"
#include "cesmrfeaturesettings.h"
#include "mesmrfieldevent.h"

#include "esmrconfig.hrh"

// <cmail> Removed profiling. </cmail>

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG
    _LIT( KESMREditDlgPanicTxt, "ESMREDITORDIALOG" );
    /** Enumeration for panic codes */
    enum TESMREditDlgPanicCode
        {
        // Recurrence series query result error
        EESMREditDlgInvalidSeriesResult = 0,
        EESMREditDlgDialogLeave
        };

    void Panic( TESMREditDlgPanicCode aPanic )
        {
        User::Panic ( KESMREditDlgPanicTxt, aPanic );
        }
#endif

/**
 * Sets recurrence modification rule to entry. Rule is queried
 * from the user. Invalid result will cause panic.
 * @param aEntry Reference to meeting request.
 */
void SetRecurrenceModRuleL(
        MESMRCalEntry& aEntry,
        CESMRListQuery::TESMRListQueryType aQueryType )
    {
    if ( aEntry.IsRecurrentEventL() &&
         aEntry.IsStoredL() )
        {
        TInt result =
            CESMRListQuery::ExecuteL( aQueryType );

        if( KErrCancel == result )
            {
            // User has cancelled selecting opening the instance
            User::Leave( KErrCancel );
            }

        TESMRThisOccurenceOrSeriesQuery recurrenceModRule =
            static_cast<TESMRThisOccurenceOrSeriesQuery>( result );

        switch( recurrenceModRule )
            {
            case EESMRThisOccurence:
                {
                aEntry.SetModifyingRuleL(
                        MESMRMeetingRequestEntry::EESMRThisOnly );
                }
                break;
            case EESMRSeries:
                {
                aEntry.SetModifyingRuleL(
                        MESMRMeetingRequestEntry::EESMRAllInSeries );
                }
                break;
            default:
                __ASSERT_DEBUG( EFalse, Panic(EESMREditDlgInvalidSeriesResult));
                break;
            }
        }
    else
        {
        TInt result =
                    CESMRListQuery::ExecuteL( aQueryType );

                if( KErrCancel == result )
                    {
                    // User has cancelled selecting opening the instance
                    User::Leave( KErrCancel );
                    }

                TESMRThisOccurenceOrSeriesQuery recurrenceModRule =
                    static_cast<TESMRThisOccurenceOrSeriesQuery>( result );

                switch( recurrenceModRule )
                    {
                    case EESMRThisOccurence:
                        {
                        aEntry.SetModifyingRuleL(
                                MESMRMeetingRequestEntry::EESMRThisOnly );
                        }
                        break;
                    case EESMRSeries:
                        {
                        aEntry.SetModifyingRuleL(
                                MESMRMeetingRequestEntry::EESMRAllInSeries );
                        }
                        break;
                    default:
                        __ASSERT_DEBUG( EFalse, Panic(EESMREditDlgInvalidSeriesResult));
                        break;
                    }
        }
    }

/**
 * Creates correct type of storage for editing calendar entry.
 * @param aCommandObserver Reference to command observer
 * @param aEntry Reference to ES calendar entry
 * @param aPolicy. Pointer to used policy.
 * @param aResponseObserver Pointer to response observer.
 */
CESMRFieldStorage* CreateCorrectStorageLC(
        MESMRFieldEventObserver& aEventObserver,
        MESMRCalEntry& aEntry,
        CESMRPolicy* aPolicy,
        MESMRResponseObserver* aResponseObserver )
    {
    CESMRFieldStorage* storage = NULL;

    MESMRCalEntry::TESMRRecurrenceModifyingRule rule(
            aEntry.RecurrenceModRule() );

    TESMRViewMode mode(
            aPolicy->ViewMode() );

    if ( ( aEntry.IsRecurrentEventL() &&
           rule == MESMRCalEntry::EESMRThisOnly ) ||
           mode == EESMRForwardMR )
        {
        // instance editing or forwarding entry
        storage = CESMRMixedFieldStorage::NewL(
                aEventObserver,
                aPolicy,
                aResponseObserver,
                aEntry );
        CleanupStack::PushL ( storage );
        }
    else if ( aEntry.IsRecurrentEventL() &&
              rule == MESMRCalEntry::EESMRAllInSeries  )
        {
        // Editing all in series
        storage = CESMRMixedFieldStorage::NewL(
                aEventObserver,
                aPolicy,
                aResponseObserver,
                aEntry );
        CleanupStack::PushL ( storage );
        }
    else
        {
        storage = CESMREditorFieldStorage::NewL ( aPolicy, aEventObserver );
        CleanupStack::PushL ( storage );
        }

    return storage;
    }

/**
 * Tests if two datetime objects has same day or not.
 * @param aLhs Reference to left hand side datetime object.
 * @param aRhs Reference to right hand side datetime object.
 */
TBool IsSameDay(
        TDateTime& aLhs,
        TDateTime& aRhs )
    {
    TBool retValue( ETrue );

    if ( aLhs.Day()     != aRhs.Day() ||
         aLhs.Month() != aRhs.Month() ||
         aLhs.Year()  != aRhs.Year() )
        {
        retValue = EFalse;
        }

    return retValue;
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMREditorDialog::CESMREditorDialog
// ---------------------------------------------------------------------------
//
CESMREditorDialog::CESMREditorDialog(
        MESMRCalEntry& aEntry,
        MAgnEntryUiCallback& aCallback ) :
    iEntry( aEntry), iCallback( aCallback)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::~CESMREditorDialog
// ---------------------------------------------------------------------------
//
CESMREditorDialog::~CESMREditorDialog( )
    {
    FUNC_LOG;
    if ( iFeatureManagerInitialized )
        {
        FeatureManager::UnInitializeLib();
        }

    if ( iServiceHandler )
        {
        iServiceHandler->DetachMenu ( 
        		R_MR_EDITOR_ORGANIZER_MENU, 
        		R_PS_AIW_INTEREST );
        delete iServiceHandler;
        }

    delete iESMRSendUI;
    if ( iTitlePane )
        {
        // Returns the previous title back to titlepane
        iTitlePane->Rollback();
        delete iTitlePane;
        }

    delete iInterimUtils;

    delete iLocationPluginHandler;
    
    delete iAddressInfoHandler;
    
    delete iFeatures;
    // iView is deleted by framework because it uses the
    // custom control mechanism.
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMREditorDialog* CESMREditorDialog::NewL(
        CESMRPolicy* aPolicy,
        MESMRCalEntry& aEntry,
        MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    CESMREditorDialog* self =
    new (ELeave) CESMREditorDialog( aEntry, aCallback );
    CleanupStack::PushL( self );
    self->ConstructL( aPolicy );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::ConstructL(
        CESMRPolicy* aPolicy )
    {
    FUNC_LOG;
    CAknDialog::ConstructL ( R_MREDITOR_MENUBAR );
    iPolicy = aPolicy;

    iESMRSendUI = CESMRSendUI::NewL (EESMRCmdSendAs );

    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;

    // <cmail> remove custom feature KFeatureIdPrintingFrameworkCalendarPlugin 
    // </cmail>	

    CESMRFieldStorage* storage =
            CreateCorrectStorageLC( *this, iEntry, aPolicy, this );

    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane,
            clientRect );

    // storage ownership is transferred to CESMRView
    iView = CESMRView::NewL( storage, iEntry, clientRect );
    CleanupStack::Pop( storage );

    // try to find subject field and set title pane observer
    CESMRField* field = NULL;
    TInt fieldCount( storage->Count() );
    for ( TInt i(0); i < fieldCount; ++i )
        {
        field = storage->Field( i );
        TESMREntryFieldId fieldId( field->FieldId() );

        switch (fieldId)
            {
            case EESMRFieldSubject: //Fall through
            case EESMRFieldOccasion://Fall through
            case EESMRFieldPriority://Fall through
            case EESMRFieldDetailedSubject:
                {
                field->SetTitlePaneObserver( iView );
                break;
                }            
            default:
            	break;
            }
        }

    TInt titleStringId = -1;
    switch ( iEntry.Type() )
        {
        case MESMRCalEntry::EESMRCalEntryMeetingRequest:
            titleStringId = R_QTN_MEET_REQ_TITLE_MEETING;
            break;
        case MESMRCalEntry::EESMRCalEntryMeeting:
            titleStringId = R_QTN_CALENDAR_TITLE_MEETING;
            break;
        case MESMRCalEntry::EESMRCalEntryTodo:
            titleStringId = R_QTN_CALENDAR_TITLE_TODO;
            break;
        case MESMRCalEntry::EESMRCalEntryMemo:
            titleStringId = R_QTN_CALENDAR_TITLE_MEMO;
            break;
        case MESMRCalEntry::EESMRCalEntryReminder:
            break;
        case MESMRCalEntry::EESMRCalEntryAnniversary:
            titleStringId = R_QTN_CALENDAR_TITLE_ANNIVERSARY;
            break;
        default:
            break;
        }
    if ( titleStringId != -1 )
        {
        iTitlePane = CESMRTitlePaneHandler::NewL( *iEikonEnv );
        HBufC* titleText = StringLoader::LoadLC( titleStringId, iCoeEnv );
        iTitlePane->SetNewTitle( titleText );
        CleanupStack::PopAndDestroy( titleText );
        }
    
    if(!iInterimUtils)
        {
        iInterimUtils = CCalenInterimUtils2::NewL();
        }
    
    iFeatures = CESMRFeatureSettings::NewL();
    
    if ( iFeatures->FeatureSupported(
            CESMRFeatureSettings::EESMRUILocationFeatures ) )
        {
        if ( iPolicy->FieldL( EESMRFieldLocation ).iFieldMode == EESMRFieldModeEdit )
            {
            iMenuBar->SetContextMenuTitleResourceId( R_MR_EDITOR_CONTEXT_MENU );
            }
        else
            {
            iMenuBar->SetContextMenuTitleResourceId( R_MR_VIEWER_CONTEXT_MENU );
            }        
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::CreateCustomControlL
// ---------------------------------------------------------------------------
//
SEikControlInfo CESMREditorDialog::CreateCustomControlL(
        TInt aType )
    {
    FUNC_LOG;
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iFlags = 0;
    controlInfo.iTrailerTextId = 0;

    if ( aType == EEsMrEditorType )
        {
        controlInfo.iControl = iView;
        }

    return controlInfo;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMREditorDialog::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;

    TKeyResponse response( EKeyWasNotConsumed);
    if ( aEvent.iCode != EKeyEscape && !MenuShowing() && aType == EEventKey)
        {
        response = iView->OfferKeyEventL ( aEvent, aType );
        }

    if ( response == EKeyWasNotConsumed  )
        {
        response = CAknDialog::OfferKeyEventL ( aEvent, aType );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CESMREditorDialog::OkToExitL( TInt aButtonId )
    {
    FUNC_LOG;
    TBool res( EFalse );
    
    switch ( aButtonId )
        {
        case EAknSoftkeyOptions:
            {
            // Show options menu
            CAknDialog::DisplayMenuL();
            break;
            }
        case EESMRCmdSaveMR:
            {
            ProcessCommandL( EESMRCmdSaveMR );
            break;
            }
        case EAknSoftkeyContextOptions:
            {
            // Show context menu
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUILocationFeatures ) )
                {
                iMenuBar->TryDisplayContextMenuBarL();
                }
            break;            
            }
        case EESMRCmdAlarmOn:
        case EESMRCmdAlarmOff:
        case EESMRCmdOpenPriorityQuery:
        case EESMRCmdOpenSyncQuery:
        case EESMRCmdOpenRecurrenceQuery:
        case EESMRCmdOpenAlarmQuery:
        case EESMRCmdCheckEvent:
        case EESMRCmdAttendeeSoftkeySelect:
        case EESMRCmdAttendeeSoftkeyCancel:
        case EESMRCmdAttendeeInsertContact:
            {
            // Forward to editor
            TRAPD( err, iView->ProcessEditorCommandL( aButtonId ) )
            if ( err != KErrNone &&
                 err != KErrCancel &&
                 err != KErrArgument )
                {
                User::Leave(err);
                }
            break;
            }
        case EESMRCmdForceExit:
        case EESMRCmdCalEntryUIDelete:
        case EESMRCmdDeleteMR:
            {
            res = ETrue;
            break;
            }
        case EAknSoftkeyDone:
            {
            // Exit dialog
            TRAPD( err, HandleExitL() )
            
            switch ( err )
                {
                case KErrNone:
                    {
                    res = ETrue;
                    break;
                    }
                case KErrArgument:
                    {
                    res = EFalse;
                    break;
                    }
                default:
                    {
                    User::Leave( err );
                    break;
                    }
                }
            break;
            }
        case EAknCmdExit:
        case EEikBidCancel:
            {
            // Force exit. Don't care about exceptions
            // just close the dialog
            TRAP_IGNORE( HandleForcedExitL( aButtonId == EAknCmdExit ) )
            res = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }

    return res;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::ActivateL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::ActivateL( )
    {
    FUNC_LOG;
    CAknDialog::ActivateL();
    iView->InternalizeL();
    
    // Notification that MR editor is ready to be shown on screen.
    if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        TTime start = iEntry.Entry().StartTimeL().TimeUtcL();
        TTime now;
        now.UniversalTime();
        if ( start < now )
            {
            iOccursInPast = ETrue;
            }
        iCallback.ProcessCommandL( EESMRCmdEditorInitializationComplete );
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::IsAllowedToSaveL
// ---------------------------------------------------------------------------
//
TBool CESMREditorDialog::IsAllowedToSaveL()
    {
    FUNC_LOG;
    TBool ret(ETrue);


    if ( ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest ||
            iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeeting ) &&
         iEntry.IsStoredL() && iEntry.IsRecurrentEventL() &&
         MESMRCalEntry::EESMRAllInSeries == iEntry.RecurrenceModRule() )
        {
        TDateTime originalDate =
                iEntry.OriginalEntry().StartTimeL().TimeLocalL().DateTime();
        TDateTime modifiedDate =
                iEntry.Entry().StartTimeL().TimeLocalL().DateTime();

        if ( !IsSameDay( originalDate, modifiedDate ) )
        {
            if ( !CESMRConfirmationQuery::ExecuteL(
                   CESMRConfirmationQuery::EESMRSaveMeetingChangedStartDay ) )
                {
                ret = EFalse;
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::ProcessCommandL(
        TInt aCommand )
    {
    FUNC_LOG;
    TRAPD( err, DoProcessCommandL( aCommand ) );
    if ( err != KErrNone &&
         err != KErrCancel &&
         err != KErrArgument&&
         err !=EESMRCmdSendMR)
        {
        User::Leave(err);
        }           
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::DoProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::DoProcessCommandL(
        TInt aCommand )
    {
    FUNC_LOG;
    CAknDialog::ProcessCommandL ( aCommand );

    switch ( aCommand )
        {
        case EESMRCmdPrint: //Fall through
        case EESMRCmdPrint_Reserved1: //Fall through
        case EESMRCmdPrint_Reserved2: //Fall through
        case EESMRCmdPrint_Reserved3: //Fall through
        case EESMRCmdPrint_Reserved4:
            {
            iView->ExternalizeL(); // no force validation
            TInt res(KErrGeneral);
            res = iCallback.ProcessCommandWithResultL ( EESMRCmdSaveMR );

            if (res == KErrNone )
                {
                this->MakeVisible(EFalse); 
                HandlePrintCommandL (aCommand );
                this->MakeVisible(ETrue); 
                }
            break;
            }

        case EESMRCmdSendAs:
            {
            SendCalendarEntryL (aCommand );
            break;
            }
            // pass-through all calentryui related command to call back
        case EESMRCmdCalEntryUISend:
            {
            User::LeaveIfError(
            		iCallback.ProcessCommandWithResultL ( aCommand ));
            break;
            }
        case EESMRCmdCalEntryUIDelete:
            {
            iView->ExternalizeL(); // no force validation
            if (!(iCallback.ProcessCommandWithResultL(aCommand) == KErrCancel))
            	{
                TryExitL( EESMRCmdCalEntryUIDelete );
            	}
            break;
            }
        case EESMRCmdCalEntryUIAddParticipants:
            {
            iView->ExternalizeL (); // no force validation
            TInt result;
            TRAPD(err,result=iCallback.ProcessCommandWithResultL ( aCommand ))
			if (err != EESMRCmdSendMR&&err!=KErrNone)
				{
				User::Leave(err);
				}
			User::LeaveIfError(result);
            TryExitL ( EESMRCmdForceExit );
            break;
            }
        case EEikBidCancel: //Fall through
        case EAknCmdExit:
        	{
            TryExitL ( aCommand );
            break;
        	}
        case EAknCmdHelp:
            {
            iView->LaunchEditorHelpL ( );
            break;
            }
        case EESMRCmdViewTrack:
            {
            TInt res = iCallback.ProcessCommandWithResultL( aCommand );
            break;
            }
        case EESMRCmdAddOptAttendee: //Fall through
        case EESMRCmdPriorityHigh: //Fall through
        case EESMRCmdPriorityNormal: //Fall through
        case EESMRCmdPriorityLow:
            {
            iView->ProcessEditorCommandL ( aCommand );
            }
            break;
        case EESMRCmdSaveMR:
            {
            TInt ret( KErrGeneral );
            iView->ExternalizeL (); // no force validation
            if ( IsAllowedToSaveL() )
                {
                ret = iCallback.ProcessCommandWithResultL ( aCommand );
                }
            if ( ret == KErrNone )
                {
                if ( iFeatures->FeatureSupported(
                        CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
                    {
                    LocationPluginHandlerL().StoreLocationToHistoryL( iEntry );
                    }
                
                if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryTodo )
                    {
                    CESMRGlobalNote* note =
                        CESMRGlobalNote::NewL( 
                        		CESMRGlobalNote::EESMRTodoEntrySaved );
                    note->ExecuteLD();
                    }
                else
                    {
                    CESMRGlobalNote* note =
                        CESMRGlobalNote::NewL( 
                        		CESMRGlobalNote::EESMREntrySaved );
                    note->ExecuteLD();
                    }
                }
            }
            break;

        case EESMRCmdSendMR: //Fall through
        case EESMRCmdSendMRUpdate:
            {
            TInt ret( KErrGeneral );
            iView->ExternalizeL();
            if ( IsAllowedToSaveL() )
                {
                ret = iCallback.ProcessCommandWithResultL ( aCommand );
                }

            if ( ret == KErrNone )
                {
                if ( iFeatures->FeatureSupported(
                             CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
                    {
                    LocationPluginHandlerL().StoreLocationToHistoryL( iEntry );
                    }
                
                TInt exitCode = aCommand ==
                        EESMRCmdDeleteMR ? EESMRCmdDeleteMR : EAknSoftkeyDone;
                TryExitL ( exitCode );
                }
            }
            break;

        case EESMRCmdDeleteMR:
            {
            if ( iEntry.Type () == MESMRCalEntry::EESMRCalEntryMeetingRequest )
                {
                TBool okToDelete = ETrue;

                if ( iEntry.IsRecurrentEventL() )
                    {
                    SetRecurrenceModRuleL(
                       iEntry,
                       CESMRListQuery::EESMRDeleteThisOccurenceOrSeriesQuery );
                    }
                else
                    {
                    okToDelete = CESMRConfirmationQuery::ExecuteL(
                            CESMRConfirmationQuery::EESMRDeleteMR );

                    }

                if ( okToDelete )
                    {
                    // When deleting we do not need to externalize entry
                    TInt res = iCallback.ProcessCommandWithResultL( aCommand );
                    if ( res != KErrCancel )
                        {
                        TInt exitCode = aCommand == EESMRCmdDeleteMR ?
                                            EESMRCmdDeleteMR : EAknSoftkeyDone;
                        TryExitL ( exitCode );
                        }
                    }
                }
            }
            break;
        
        case EESMRCmdAssignFromMap:
        case EESMRCmdLandmarks:
        case EESMRCmdPreviousLocations:
        	{
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration
                    | CESMRFeatureSettings::EESMRUILandmarksIntegration
                    | CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
                {
                TBool ignoreQuery = EFalse;
                
                if ( aCommand == EESMRCmdAssignFromMap )
                    {
                    HBufC* oldLocation = iEntry.Entry().LocationL().AllocLC();
                    iView->ExternalizeL();
                    
                    // if old and new locations are different and old location
                    // exists, the user is updating existing location. Let's discard
                    // the existing coordinates by using command EESMRCmdUpdateFromMap
                    if ( ( iEntry.Entry().LocationL().Compare( *oldLocation ) != 0
                           && oldLocation->Length() != 0 )
                         || iLocationModified )
                        {
                        aCommand = EESMRCmdUpdateFromMap;
                        }
                    else if ( oldLocation->Length() == 0 )
                        {
                        // Assigning location from Maps for first time.
                        ignoreQuery = EFalse;                                            
                        }
                    CleanupStack::PopAndDestroy( oldLocation );
                    oldLocation = NULL;
                    }
                else
                    {
                    iView->ExternalizeL();
                    }
                
                iLocationModified |=
                    LocationPluginHandlerL().HandleCommandL( aCommand,
                                                         iEntry,
                                                         ignoreQuery ); 
                iView->InternalizeL();
                iView->SetControlFocusedL( EESMRFieldLocation );
                }
			break;
        	}
        case EESMRCmdSearchFromContacts:
        	{
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIContactsIntegration ) )
                 {
                 iView->ExternalizeL();
                 iLocationModified |=
                     AddressInfoHandlerL().SearchAddressFromContactsL( iEntry );
                 iView->InternalizeL();
                 iView->SetControlFocusedL( EESMRFieldLocation );
                 }
        	break;
        	}
        // Scenarios with viewer location field in editor dialog.
        case EESMRCmdShowOnMap:
        case EESMRCmdSearchFromMap:
            {
            LocationPluginHandlerL().HandleCommandL(aCommand, iEntry );
            break;
            }        	
        case EAknCmdOpen:
        	{
        	HandleOpenCmdL();
        	break;
        	}
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenu )
    {
    FUNC_LOG;

    if ( iServiceHandler && iServiceHandler->HandleSubmenuL ( *aMenu ) )
        {
        return;
        }

    if ( aResourceId == R_MR_EDITOR_ORGANIZER_MENU )
        {
        
	    if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
		   {
		   // remove help support in pf5250
		   aMenu->SetItemDimmed( EAknCmdHelp, ETrue);      
		   }        
        
        const TInt count(aMenu->NumberOfItemsInPane());
		for (TInt i(0); i < count; i++ )
			{
			CEikMenuPaneItem::SData& item = aMenu->ItemDataByIndexL ( i );
			aMenu->SetItemDimmed (
						item.iCommandId, 
						!iPolicy->IsDefaultCommand(item.iCommandId) );
			}
        
		//Open command		
		if ( iView->IsComponentFocused( EESMRFieldSync ) ||
			 iView->IsComponentFocused( EESMRFieldRecurrence ) ||
			 iView->IsComponentFocused( EESMRFieldAlarm ) ||
			 iView->IsComponentFocused( EESMRFieldPriority ))			
			{
			aMenu->SetItemDimmed( EAknCmdOpen, EFalse );
			}		
		
		if( iInterimUtils && !(iInterimUtils->MRViewersEnabledL()))
			{ 
			aMenu->SetItemDimmed( EESMRCmdCalEntryUIAddParticipants, ETrue );
			}
        
        // if the entry is saved (stored) and sent to recipients,
        // show 'Send update', otherwise just 'Send'
        TBool sentAndStored = iEntry.IsSentL() && iEntry.IsStoredL();

        // enable send only if attendee editors contain attendees and
        // entry does not occur in past (unless if this is forwarded
        // MR in which case the "in past" sending is also allowed).
        TBool occurredInPastCheckPassed( !iOccursInPast );
        if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry.Type() )
            {
            // This is meeting request
            MESMRMeetingRequestEntry* mrEntry = 
                static_cast<MESMRMeetingRequestEntry*>( &iEntry );
            occurredInPastCheckPassed = mrEntry->IsForwardedL();
            }

        TBool sendEnabled = 
				( iRequiredAttendeesEnabled | iOptionalAttendeesEnabled ) &&
				occurredInPastCheckPassed;

        if ( iPolicy->IsDefaultCommand( EESMRCmdSendMR ) )
           {
           aMenu->SetItemDimmed ( EESMRCmdSendMR, !sendEnabled );
           }

        if ( iPolicy->IsDefaultCommand(EESMRCmdSendMRUpdate ) )
           {
           if ( sendEnabled )
               {
               //EESMRCmdSendMRUpdate and EESMRCmdSendMR shouldn't 
               //be visible at the same time
               aMenu->SetItemDimmed( EESMRCmdSendMRUpdate, !sentAndStored );
               if(iPolicy->IsDefaultCommand( EESMRCmdSendMR ) )
                   {
                   aMenu->SetItemDimmed( EESMRCmdSendMR, sentAndStored );
                   }
               }
           else
               {
               aMenu->SetItemDimmed( EESMRCmdSendMRUpdate, ETrue );
               }
           }

        if(iPolicy->IsDefaultCommand( EESMRCmdViewTrack ) )
            {
            //only show tracking view option when item is sent, stored
            //and when cfsmailbox actually supports attendee status

            if (SupportsAttendeeStatusL())
                {
                aMenu->SetItemDimmed( EESMRCmdViewTrack, !sentAndStored );
                }
            else
                {
                aMenu->SetItemDimmed( EESMRCmdViewTrack, ETrue );
                }

            }

        if ( iPolicy->IsDefaultCommand( EESMRCmdAddOptAttendee ) )
            {
            // if optional attendee (recipient) is already visible, don't show
            // the menu item.
            TBool isVisible =
                    iView->IsComponentVisible( EESMRFieldOptAttendee );
            aMenu->SetItemDimmed( EESMRCmdAddOptAttendee, isVisible );
            }

        // Insert send ui menu for all other entry types than meeting request
        if ( iEntry.Type ( )!= MESMRCalEntry::EESMRCalEntryMeetingRequest )
            {
            TryInsertSendMenuL( aMenu );
            }

        if ( FeatureManager::FeatureSupported( 
        		KFeatureIdPrintingFrameworkCalendarPlugin ) )
            {
            // Initiliaze menu
            iServiceHandler->InitializeMenuPaneL(
                    *aMenu,
                    aResourceId,
                    EESMRCmdPrint,
                    iServiceHandler->InParamListL() );
            }
        else
            {
            aMenu->SetItemDimmed( EESMRCmdPrint, ETrue );            
            }
        
        HandleLocationOptionsL( aResourceId, aMenu );
        }
    
    if ( aResourceId == R_LOCATION_OPTIONS
         || aResourceId == R_MR_EDITOR_LOCATION_MENU
         || aResourceId == R_MR_VIEWER_LOCATION_MENU )
        {
        HandleLocationOptionsL( aResourceId, aMenu );
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::SupportsAttendeeStatusL
// ---------------------------------------------------------------------------
//
TBool CESMREditorDialog::SupportsAttendeeStatusL( )
    {
    FUNC_LOG;
    TBool supportsAttendeeStatus(EFalse);
    CESMRFieldBuilderInterface* plugin = NULL;
    TRAPD( error, 
    		plugin = CESMRFieldBuilderInterface::CreatePluginL( 
						TUid::Uid(KESMRUIFieldBuilderPluginImplUId) ) );
    CleanupStack::PushL( plugin );
    
    if (error == KErrNone && plugin)
        {
        TUid uid = {0};
        MESMRBuilderExtension* extension = 
			static_cast<MESMRBuilderExtension*>( plugin->ExtensionL(uid) );

        if (extension)
            {
            supportsAttendeeStatus = 
				extension->CFSMailBoxCapabilityL(
						MESMRBuilderExtension::EMRCFSAttendeeStatus);
            }
        }

    CleanupStack::PopAndDestroy( plugin );

    return supportsAttendeeStatus;
    }


// ---------------------------------------------------------------------------
// CESMREditorDialog::ExecuteViewLD
// ---------------------------------------------------------------------------
//
TInt CESMREditorDialog::ExecuteViewLD( )
    {
    return ExecuteLD( R_MREDITOR_DIALOG );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::Response
// ---------------------------------------------------------------------------
//
TBool CESMREditorDialog::Response(
        TInt /*aCommand*/)
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ExitDialog
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::ExitDialog()
    {
    // This is for mixed field storage's viewer fields.
    // Not actual used in editor dialog
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ChangeReadyResponseL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::ChangeReadyResponseL() // codescanner::LFunctionCantLeave
    {
    // This is for mixed field storage's viewer fields.
    // Not actual used in editor dialog
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::HandleExitL
// ---------------------------------------------------------------------------
//
TInt CESMREditorDialog::HandleExitL() // codescanner::intleaves
    {
    FUNC_LOG;
    TInt res( KErrNone);

    iView->ExternalizeL(); // no force validation
    TBool isStored = iEntry.IsStoredL();

    if ( iEntry.IsEntryEditedL() )
        {
        if ( IsAllowedToSaveL() )
            {
            res = iCallback.ProcessCommandWithResultL( EESMRCmdSaveMR );
            
            if ( res == KErrNone
                 && iFeatures->FeatureSupported(
                         CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
                {
                LocationPluginHandlerL().StoreLocationToHistoryL( iEntry );
                }
            }
        else
            {
            res = KErrCancel;
            }
        }
    return res;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::HandleForcedExitL
// ---------------------------------------------------------------------------
//
TInt CESMREditorDialog::HandleForcedExitL( TBool aShowConfirmationQuery ) // codescanner::intleaves
    {
    FUNC_LOG;
    TInt res( KErrNone);

    iView->ExternalizeL ( ETrue ); // force validation should be used

    if ( iEntry.IsEntryEditedL() && 
    	 iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest)
        {
        if ( !aShowConfirmationQuery ||
             CESMRConfirmationQuery::ExecuteL(
        		CESMRConfirmationQuery::EESMRSaveChanges ))
            {
            res = iCallback.ProcessCommandWithResultL( EESMRCmdSaveMR );
            
            if ( res == KErrNone
                 && iFeatures->FeatureSupported(
                         CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
                {
                LocationPluginHandlerL().StoreLocationToHistoryL( iEntry );
                }
            }
        }
    return res;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::SendCalendarEntryL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::SendCalendarEntryL(
        TInt aCommandId )
    {
    FUNC_LOG;
    iView->ExternalizeL(); // no force validation
    TInt res(KErrGeneral);
    res = iCallback.ProcessCommandWithResultL( EESMRCmdSaveMR );

    if ( res == KErrNone )
        {
        if ( iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
            {
            LocationPluginHandlerL().StoreLocationToHistoryL( iEntry );
            }
        
        // we don't need MenuPane in CCalSend but we need it for the API
        CEikMenuPane* pane = NULL;

        // Show menu to user
        // CCalenSend handles selection internally, 
        // so we don't get anything in return
        iESMRSendUI->DisplaySendCascadeMenuL( *pane );

        // Try to send
        if (iESMRSendUI->CanSendL( aCommandId ) )
            {
            iESMRSendUI->SendAsVCalendarL( aCommandId, iEntry.Entry ( ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::TryInsertSendMenuL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::TryInsertSendMenuL(
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    // Insert send menu to the next position from "delete"
    TInt index(KErrNotFound);
    aMenuPane->ItemAndPos(EESMRCmdCalEntryUIDelete, index);
    if (index == KErrNotFound)
        {
        aMenuPane->ItemAndPos(EESMRCmdDeleteMR, index);
        }
    index++;
    iESMRSendUI->DisplaySendMenuItemL(*aMenuPane, index);
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::HandlePrintCommandL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::HandlePrintCommandL(
        TInt aCommand )
    {
    FUNC_LOG;
    CAiwGenericParamList& inParams = iServiceHandler->InParamListL ( );

    // Param date

    TCalTime startTime = iEntry.Entry().StartTimeL ( );

    TAiwGenericParam dateParam( EGenericParamDateTime);
    TTime activeDay = startTime.TimeUtcL ( );

    TAiwGenericParam calendarParam( EGenericParamCalendarItem);
    calendarParam.Value().Set ( TUid::Uid (iEntry.Entry().LocalUidL ( ) ) );
    inParams.AppendL ( calendarParam );

    // Append date param
    dateParam.Value().Set ( activeDay );
    inParams.AppendL ( dateParam );

    const TUid uid( TUid::Uid( KUidCalendarApplication ) );
    TAiwGenericParam uidParam( EGenericParamApplication);
    uidParam.Value().Set ( uid );
    inParams.AppendL ( uidParam );

    // Execute service command with given parameters
    iServiceHandler->ExecuteMenuCmdL ( aCommand,
            inParams, iServiceHandler->OutParamListL ( ), 0,
            NULL );
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::HandleOpenCmdL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::HandleOpenCmdL()
	{
    FUNC_LOG;
	iView->ProcessEditorCommandL( EAknCmdOpen );
	}

// ---------------------------------------------------------------------------
// CESMREditorDialog::HandleLocationOptionsL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::HandleLocationOptionsL( TInt aResourceId,
                                                CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    TInt menuPos = KErrNotFound;
    
    if ( aResourceId == R_MR_EDITOR_ORGANIZER_MENU )
        {
        
	    if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
		   }        
                
        if ( iPolicy->FieldL( EESMRFieldLocation ).iFieldMode == EESMRFieldModeView )
            {
            // Dim Location cascade menu with viewer field
            aMenuPane->SetItemDimmed( EESMRCmdAddLocation, ETrue );
            
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                TBool showOnMap =
                    LocationPluginHandlerL().IsCommandAvailableL( EESMRCmdShowOnMap,
                                                                  iEntry );
                aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
                aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, showOnMap );
                }
            }
        else if ( aMenuPane->MenuItemExists( EESMRCmdAddLocation, menuPos ) )
            {
            TInt numOptions = 0; // number of sub menu items
            TInt commandId = 0; // command id if only one sub menu item
            
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                ++numOptions;
                commandId = EESMRCmdAssignFromMap;
                }
            
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIContactsIntegration ) )
                {
                ++numOptions;
                commandId = EESMRCmdSearchFromContacts;
                }
            
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUILandmarksIntegration) )
                {
                ++numOptions;
                commandId = EESMRCmdLandmarks;
                }
            
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
                {
                ++numOptions;
                commandId = EESMRCmdPreviousLocations;
                }
                        
                        
            
            if ( numOptions == 0 )
                {
                // Delete whole sub menu
                aMenuPane->DeleteMenuItem( EESMRCmdAddLocation );
                }
            else if ( numOptions == 1 )
                {
                // Replace submenu with available command
                aMenuPane->DeleteMenuItem( EESMRCmdAddLocation );
                
                // Insert new menu item
                HBufC* text = LoadOptionsMenuTextLC( commandId );
                CEikMenuPaneItem::SData menuItem;
                menuItem.iCommandId = commandId;
                menuItem.iCascadeId = 0;
                menuItem.iFlags = 0;
                menuItem.iText = *text;
                CleanupStack::PopAndDestroy( text );
                aMenuPane->InsertMenuItemL( menuItem, menuPos );
                }
            }
        else if ( aMenuPane->MenuItemExists( EESMRCmdPreviousLocations, menuPos )
                  && !LocationPluginHandlerL().IsCommandAvailableL(
                          EESMRCmdPreviousLocations, iEntry ) )
            {
            // No items in location history. Dim item.
            aMenuPane->SetItemDimmed( EESMRCmdPreviousLocations, ETrue );
            }
        }
    
    if ( aResourceId == R_LOCATION_OPTIONS
         || aResourceId == R_MR_EDITOR_LOCATION_MENU )
        {
        TBool assignFromMap = iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration );
        aMenuPane->SetItemDimmed( EESMRCmdAssignFromMap, !assignFromMap );
        
        TBool searchFromContacts = iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIContactsIntegration );        
        aMenuPane->SetItemDimmed( EESMRCmdSearchFromContacts,
                                  !searchFromContacts );
        
        TBool landmarks = iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUILandmarksIntegration );        
        aMenuPane->SetItemDimmed( EESMRCmdLandmarks,
                                  !landmarks );
        
        TBool previousLocations = iFeatures->FeatureSupported(
                        CESMRFeatureSettings::EESMRUIPreviousLocationsList );
        if ( previousLocations
             && !LocationPluginHandlerL().IsCommandAvailableL(
                     EESMRCmdPreviousLocations, iEntry ) )
            {
            // No items in history. Dim option.
            previousLocations = EFalse;
            }
        aMenuPane->SetItemDimmed( EESMRCmdPreviousLocations,
                                  !previousLocations );
        }
    
    if ( aResourceId == R_MR_VIEWER_LOCATION_MENU )
        {
        TBool showOnMap =
            LocationPluginHandlerL().IsCommandAvailableL( EESMRCmdShowOnMap,
                                                          iEntry );
        
        aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
        aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, showOnMap );
        
        aMenuPane->SetItemDimmed( EESMRCmdEdit,
                                  !iPolicy->IsDefaultCommand( EESMRCmdEdit ) );
        aMenuPane->SetItemDimmed( EESMRCmdEditLocal,
                                  !iPolicy->IsDefaultCommand( EESMRCmdEditLocal ) );
        aMenuPane->SetItemDimmed( EESMRCmdCalEntryUIEdit,
                                  !iPolicy->IsDefaultCommand( EESMRCmdCalEntryUIEdit ) );
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::LoadOptionsMenuTextLC
// ---------------------------------------------------------------------------
//
HBufC* CESMREditorDialog::LoadOptionsMenuTextLC( TInt aCommandId )
    {
    FUNC_LOG;
    TInt resourceId = 0;
    
    switch ( aCommandId )
        {
        case EESMRCmdAssignFromMap:
            {
            resourceId = R_MEET_REQ_OPTIONS_ASSIGN_FROM_MAP;
            break;
            }
        case EESMRCmdSearchFromContacts:
            {
            resourceId = R_MEET_REQ_OPTIONS_SEARCH_FROM_CONTACTS;
            break;
            }
        case EESMRCmdPreviousLocations:
            {
            resourceId = R_MEET_REQ_OPTIONS_PREVIOUS_LOCATIONS;
            break;
            }
        case EESMRCmdLandmarks:
            {
            resourceId = R_MEET_REQ_OPTIONS_LANDMARKS;
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    
    HBufC* text = StringLoader::LoadLC( resourceId, iCoeEnv );
    return text;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::LocationPluginHandlerL
// ---------------------------------------------------------------------------
//
CESMRLocationPluginHandler& CESMREditorDialog::LocationPluginHandlerL()
    {
    FUNC_LOG;
    if ( !iLocationPluginHandler )
        {
        iLocationPluginHandler = CESMRLocationPluginHandler::NewL( *this );
        }
    
    return *iLocationPluginHandler;
    }
    
// ---------------------------------------------------------------------------
// CESMREditorDialog::AddressInfoHandlerL
// ---------------------------------------------------------------------------
//
CESMRAddressInfoHandler& CESMREditorDialog::AddressInfoHandlerL()
    {
    FUNC_LOG;
    if ( !iAddressInfoHandler )
        {
        iAddressInfoHandler = CESMRAddressInfoHandler::NewL();
        }
    
    return *iAddressInfoHandler;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::HandleFieldEventL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::HandleFieldEventL( // codescanner::LFunctionCantLeave
        const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    switch ( aEvent.Type() )
        {
        case MESMRFieldEvent::EESMRFieldCommandEvent:
            {
            HandleFieldCommandEventL( aEvent );
            break;
            }
        case MESMRFieldEvent::EESMRFieldChangeEvent:
            {
            HandleFieldChangeEventL( aEvent );
            break;    
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::HandleFieldCommandEventL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::HandleFieldCommandEventL(
        const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    TInt* command = static_cast< TInt* >( aEvent.Param( 0 ) );
    switch ( *command )
        {
        case EESMRCmdAttendeeRequiredEnabled:
            {
            iRequiredAttendeesEnabled = ETrue;
            break;
            }            
        case EESMRCmdAttendeeRequiredDisabled:
            {
            iRequiredAttendeesEnabled = EFalse;
            break;
            }            
        case EESMRCmdAttendeeOptionalEnabled:
            {
            iOptionalAttendeesEnabled = ETrue;
            break;
            }
        case EESMRCmdAttendeeOptionalDisabled:
            {
            iOptionalAttendeesEnabled = EFalse;
            break;
            }
        case EESMRCmdRestoreMiddleSoftKey:
            {
            CEikButtonGroupContainer& cba = ButtonGroupContainer();
            
            if ( iPolicy->FieldL( iView->FocusedField() ).iFieldMode
                 == EESMRFieldModeView )
                {
                // Dim default MSK if editor contains viewer fields
                cba.MakeCommandVisibleByPosition(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                        EFalse );
                }
            else
                {
                cba.SetCommandL(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
                    R_MR_SELECT_SOFTKEY );
                            
                cba.MakeCommandVisibleByPosition(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                        ETrue );
                cba.DrawDeferred();
                }
            }
            break;
        case EESMRCmdSizeExceeded:
        	iView->ProcessEditorCommandL( *command );
        	break;
        default:
            {
            break;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CESMREditorDialog::HandleFieldChangeEventL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::HandleFieldChangeEventL(
        const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    TInt* fieldId = static_cast< TInt* >( aEvent.Param( 0 ) );
    switch ( *fieldId )
        {
        case EESMRFieldLocation:
            {
            MESMRFieldEventValue* value =
                static_cast< MESMRFieldEventValue* >( aEvent.Param( 1 ) );
            if ( iEntry.Entry().LocationL() != value->StringValue() )
                {
                // Show query dialog only if entry has geocoordinates.
                // It can be verified by checking if "Show on map" is available
                // for iEntry as geocoordinates are required for that.
                TBool enableQuery =
                    LocationPluginHandlerL().IsCommandAvailableL(
                            EESMRCmdShowOnMap, iEntry );
                
                if ( enableQuery
                     && CESMRConfirmationQuery::ExecuteL(
                        CESMRConfirmationQuery::EESMRAssignUpdatedLocation ) )
                    {
                    HBufC* oldLocation = iEntry.Entry().LocationL().AllocLC();
                    iView->ExternalizeL();
                                       
                    // if old and new locations are different and old location
                    // exists, the user is updating existing location. Let's discard
                    // the existing coordinates by using command EESMRCmdUpdateFromMap
                    if ( iEntry.Entry().LocationL().Compare( *oldLocation ) != 0 && 
                            oldLocation->Length() != 0 )
                        {
                        LocationPluginHandlerL().HandleCommandL( 
                                EESMRCmdUpdateFromMap, 
                                iEntry );
                        }
                    else
                        {
                        LocationPluginHandlerL().HandleCommandL(
                            EESMRCmdAssignFromMap,
                            iEntry,
							   EFalse );
                        }

                    iView->InternalizeL();
                    iView->SetControlFocusedL( EESMRFieldLocation );
                    
                    CleanupStack::PopAndDestroy( oldLocation );
                    oldLocation = NULL;
                    }
                else if ( enableQuery )
                    {
                    // Externalize entry, so query won't be displayed
                    // again for the same text. 
                    iView->ExternalizeL();
                    }
                }
            break;
            }
        case EESMRFieldStartDate:
            {
            if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
                {
                MESMRFieldEventValue* value =
                    static_cast< MESMRFieldEventValue* >( aEvent.Param( 1 ) );
                TTime now;
                now.UniversalTime();
                if ( value->TimeValue() < now )
                    {
                    // disable send option
                    iOccursInPast = ETrue;
                    }
                else
                    {
                    iOccursInPast = EFalse;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// EOF

