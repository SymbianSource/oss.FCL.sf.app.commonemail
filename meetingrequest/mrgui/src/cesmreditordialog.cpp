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
* Description:  ESMR editor dialog
*
*/

#include "cesmreditordialog.h"
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
#include "esmrdef.h"
#include "esmrcommands.h"
#include "cesmrpolicy.h"
#include "cmrtoolbar.h"
#include "mmrpolicyprovider.h"
#include "mmrinfoprovider.h"
#include "cesmrcaldbmgr.h"
#include "cmrfocusstrategyeditor.h"
#include "cesmrfieldbuilderinterface.h"

#include <esmrgui.rsg>
#include <apgcli.h>
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
#include <aknnotewrappers.h>
#include <DocumentHandler.h>
#include <NpdApi.h>
#include <calentry.h>
#include <calattachment.h>
#include <CalenInterimUtils2.h>
#include <akntitle.h>
#include <StringLoader.h>

// DEBUG
#include "emailtrace.h"

// <cmail> Removed profiling. </cmail>

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG
    /** Enumeration for panic codes */
    enum TESMREditDlgPanicCode
        {
        // Recurrence series query result error
        EESMREditDlgInvalidSeriesResult = 0,
        EESMREditDlgDialogLeave
        };

    void Panic( TESMREditDlgPanicCode aPanic )
        {
        _LIT( KESMREditDlgPanicTxt, "ESMREDITORDIALOG" );
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
        aEntry.SetModifyingRuleL(
                MESMRMeetingRequestEntry::EESMRThisOnly );
        }
    }

/**
 * Creates correct type of storage for editing calendar entry.
 * @param aCommandObserver Reference to command observer
 * @param aEntry Reference to ES calendar entry
 * @param aPolicyProvider. Pointer to used policy.
 * @param aResponseObserver Pointer to response observer.
 */
CESMRFieldStorage* CreateCorrectStorageLC(
        MESMRFieldEventObserver& aEventObserver,
        MESMRCalEntry& aEntry,
        const CESMRPolicy& aPolicy,
        MESMRResponseObserver* aResponseObserver )
    {
    CESMRFieldStorage* storage = NULL;

    MESMRCalEntry::TESMRRecurrenceModifyingRule rule(
            aEntry.RecurrenceModRule() );

    TESMRViewMode mode( aPolicy.ViewMode() );

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
        MMRInfoProvider& aInfoProvider,
        MAgnEntryUiCallback& aCallback ) :
    iInfoProvider( aInfoProvider ),
    iCallback( aCallback)
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
    delete iToolbar;
    delete iFocusStrategy;

    // iView is deleted by framework because it uses the
    // custom control mechanism.
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMREditorDialog* CESMREditorDialog::NewL(
        MMRInfoProvider& aInfoProvider,
        MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    CESMREditorDialog* self =
    new (ELeave) CESMREditorDialog( aInfoProvider, aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::ConstructL()
    {
    FUNC_LOG;
    CAknDialog::ConstructL ( R_MREDITOR_MENUBAR );

    iESMRSendUI = CESMRSendUI::NewL (EESMRCmdSendAs );

    MESMRCalEntry* entry = iInfoProvider.EntryL();
    CESMRFieldStorage* storage = CreateCorrectStorageLC(
            *this,
            *entry,
            iInfoProvider.PolicyProvider().CurrentPolicy(),
            this );

    iStartTimeUtc = entry->Entry().StartTimeL().TimeUtcL();

    //clean previous toolbar if there is, and be sure restore toolbar area to EMainPane
    iToolbar = CMRToolbar::NewL();

    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane,
            clientRect );

    // Create focus strategy
    iFocusStrategy = CMRFocusStrategyEditor::NewL( *storage );

    // storage ownership is transferred to CESMRView
    iView = CESMRView::NewL( 
            storage, 
            *entry, 
            clientRect, 
            *iFocusStrategy,
            *iToolbar );
    
    iView->SetViewMode( EESMREditMR );
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

    SetTitleL();

    if(!iInterimUtils)
        {
        iInterimUtils = CCalenInterimUtils2::NewL();
        }

    iFeatures = CESMRFeatureSettings::NewL();

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
    MESMRCalEntry* entry = iInfoProvider.EntryL();
    switch ( aButtonId )
        {
        case EAknSoftkeyOptions:
            {
            // Show options menu
            CAknDialog::DisplayMenuL();
            break;
            }
        case EAknSoftkeySend:
        	{
            if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == entry->Type() )
                {
                MESMRMeetingRequestEntry* mrEntry =
                        static_cast<MESMRMeetingRequestEntry*>( 
                                iInfoProvider.EntryL() );
                        	
                const CESMRPolicy& policy = Policy();
                if ( policy.IsDefaultCommand(EESMRCmdSendMRUpdate ) &&
                        ( mrEntry->IsSentL() && mrEntry->IsStoredL() ) )
                    {
                    ProcessCommandL( EESMRCmdSendMRUpdate );
                    }
                else
                    {
                    ProcessCommandL( EESMRCmdSendMR );
                    }
                }
        	break;
            }
        case EESMRCmdSaveMR:
            {
            ProcessCommandL( EESMRCmdSaveMR );
            break;
            }
        case EESMREditorAddAttachment:
            {
            ProcessCommandL( EESMREditorAddAttachment );
            break;
            }

        case EESMRCmdLongtapDetected:
        case EAknSoftkeyContextOptions:
            {
            if( SetContextMenuL() )
                {
                ShowContextMenuL();;
                }
            break;
            }
        case EESMRCmdAlarmOn:
        case EESMRCmdAlarmOff:
        case EESMRCmdOpenPriorityQuery:
        case EESMRCmdOpenSyncQuery:
        case EESMRCmdOpenUnifiedEditorQuery:
        case EESMRCmdOpenMultiCalenSelectQuery:
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
    MESMRCalEntry* entry = iInfoProvider.EntryL();
    if ( entry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        iCallback.ProcessCommandL( EESMRCmdEditorInitializationComplete );
        }

    // initial stripe color
    MESMRCalDbMgr& dbMgr = entry->GetDBMgr();
    TPtrC calenName = dbMgr.GetCalendarNameByEntryL( *entry );
    dbMgr.SetCurCalendarByNameL( calenName );
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::IsAllowedToSaveL
// ---------------------------------------------------------------------------
//
TBool CESMREditorDialog::IsAllowedToSaveL()
    {
    FUNC_LOG;
    TBool ret(ETrue);

    MESMRCalEntry* entry = iInfoProvider.EntryL();
    if ( ( entry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest ||
            entry->Type() == MESMRCalEntry::EESMRCalEntryMeeting ) &&
         entry->IsStoredL() && entry->IsRecurrentEventL() &&
         MESMRCalEntry::EESMRAllInSeries == entry->RecurrenceModRule() )
        {
        TDateTime originalDate =
                entry->OriginalEntry().StartTimeL().TimeLocalL().DateTime();
        TDateTime modifiedDate =
                entry->Entry().StartTimeL().TimeLocalL().DateTime();

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
         err != KErrArgument )
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

    MESMRCalEntry* entry = iInfoProvider.EntryL();

    switch ( aCommand )
        {
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
        case EESMRCmdCalEntryUIAddParticipants:
            {
            TRAPD( err,  SwitchEntryTypeL( EMRCommandSwitchToMR, ETrue ) )

            if (KErrArgument == err )
            	{
            	// There was validation error --> Just show the
            	// existing editor.
            	err = KErrNone;
            	}
            else if ( err )
                {
                iCoeEnv->HandleError( err );
                TryExitL( EESMRCmdForceExit );
                }

            break;
            }
        case EAknSoftkeyClose:
            {
            TryExitL ( EAknCmdExit );
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
                    LocationPluginHandlerL().StoreLocationToHistoryL( *entry );
                    }

                if ( entry->Type() == MESMRCalEntry::EESMRCalEntryTodo )
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
        case EESMRCmdSaveCloseMR:
        	{
            TryExitL( EAknSoftkeyDone );
        	break;
        	}
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
                    LocationPluginHandlerL().StoreLocationToHistoryL( *entry );
                    }

                TInt exitCode = aCommand ==
                        EESMRCmdDeleteMR ? EESMRCmdDeleteMR : EAknSoftkeyDone;
                TryExitL ( exitCode );
                }
            }
            break;

        case EESMRCmdDeleteMR:
        case EESMRCmdCalEntryUIDelete:
            {
            TBool okToDelete = ETrue;

            if ( entry->IsRecurrentEventL() )
                {
                SetRecurrenceModRuleL(
                   *entry,
                   CESMRListQuery::EESMRDeleteThisOccurenceOrSeriesQuery );
                }
            else
                {
                if( CCalenInterimUtils2::IsMeetingRequestL( entry->Entry() ) )
                    {
                    okToDelete = CESMRConfirmationQuery::ExecuteL(
                            CESMRConfirmationQuery::EESMRDeleteMR );
                    }
                else
                    {
                    okToDelete = CESMRConfirmationQuery::ExecuteL(
                             CESMRConfirmationQuery::EESMRDeleteEntry );
                    }
                }

            if ( okToDelete )
                {
                // When deleting we do not need to externalize entry
                if( entry->IsStoredL() )
                    {
                    TInt res = iCallback.ProcessCommandWithResultL( aCommand );
                    if ( res != KErrCancel )
                        {
                        TryExitL( EESMRCmdDeleteMR );
                        }
                    }
                else
                    {
                    TryExitL( EESMRCmdDeleteMR );
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
                    HBufC* oldLocation = entry->Entry().LocationL().AllocLC();
                    iView->ExternalizeL();

                    // if old and new locations are different and old location
                    // exists, the user is updating existing location. Let's discard
                    // the existing coordinates by using command EESMRCmdUpdateFromMap
                    if ( ( entry->Entry().LocationL().Compare( *oldLocation ) != 0
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
                                                         *entry,
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
                     AddressInfoHandlerL().SearchAddressFromContactsL( *entry );
                 iView->InternalizeL();
                 iView->SetControlFocusedL( EESMRFieldLocation );
                 }
            break;
            }
        // Scenarios with viewer location field in editor dialog.
        case EESMRCmdShowOnMap:
        case EESMRCmdSearchFromMap:
            {
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                LocationPluginHandlerL().HandleCommandL(aCommand, *entry );
                }
            break;
            }
        case EAknCmdOpen:
            {
            HandleOpenCmdL();
            break;
            }
        case EESMRCmdAttendeeInsertContact:
            {
            OkToExitL(EESMRCmdAttendeeInsertContact);
            break;
            }
        /*
        * Attachment field editor context menu commands
        */
        case EESMREditorOpenAttachment:
        case EESMREditorRemoveAttachment:
        case EESMREditorRemoveAllAttachments:
        case EESMREditorAddAttachment:
           {
           iView->ProcessEditorCommandL( aCommand );
           break;
           }

        case EMRCommandMyLocations:
            {
            iView->ExternalizeL();
            iLocationModified |=
                LocationPluginHandlerL().HandleCommandL( aCommand,
                                                         *entry,
                                                         EFalse );
            iView->InternalizeL();
            iView->SetControlFocusedL( EESMRFieldLocation );
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
    MESMRCalEntry* entry = iInfoProvider.EntryL();

    TMRMenuStyle menuStyle( EMROptionsMenu );

    if ( aResourceId == R_MR_EDITOR_ORGANIZER_MENU )
        {
        const CESMRPolicy& policy = Policy();
        const TInt count(aMenu->NumberOfItemsInPane());
        for ( TInt i(0); i < count; i++ )
            {
            CEikMenuPaneItem::SData& item = aMenu->ItemDataByIndexL ( i );

            if ( item.iCommandId )
                {
                // Checking from policy for commands that has valid command id and
                aMenu->SetItemDimmed (
                            item.iCommandId,
                            !policy.IsDefaultCommand(item.iCommandId) );
                }
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

        
        TBool sendEnabled( EFalse );
        TBool sentAndStored( EFalse );
        
        if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == entry->Type() )
            {
            MESMRMeetingRequestEntry* mrEntry =
                    static_cast<MESMRMeetingRequestEntry*>( iInfoProvider.EntryL() );
        
            // if the entry is saved (stored) and sent to recipients,
            // show 'Send update', otherwise just 'Send'
            sentAndStored = mrEntry->IsSentL() && mrEntry->IsStoredL();

            // enable send only if attendee editors contain attendees and
            // entry does not occur in past.
            TBool bOccursInPast( OccursInPast( iStartTimeUtc ) );
                
            TBool sendEnabled = ( !bOccursInPast &&
                    ( mrEntry->RoleL() == EESMRRoleOrganizer ) &&            
                    ( iRequiredAttendeesEnabled | iOptionalAttendeesEnabled ) &&
                    ((( mrEntry->RoleL() == EESMRRoleOrganizer ) &&
                    ( iInfoProvider.PolicyProvider().CurrentPolicy().ViewMode() == EESMREditMR )) ||
                    ( iInfoProvider.PolicyProvider().CurrentPolicy().ViewMode() == EESMRForwardMR )
                    ));
            }
        
        if ( policy.IsDefaultCommand( EESMRCmdSendMR ) )
           {
           aMenu->SetItemDimmed ( EESMRCmdSendMR, !sendEnabled );
           }

        if ( policy.IsDefaultCommand(EESMRCmdSendMRUpdate ) )
           {
           if ( sendEnabled )
               {
               //EESMRCmdSendMRUpdate and EESMRCmdSendMR shouldn't
               //be visible at the same time
               aMenu->SetItemDimmed( EESMRCmdSendMRUpdate, !sentAndStored );
               if( policy.IsDefaultCommand( EESMRCmdSendMR ) )
                   {
                   aMenu->SetItemDimmed( EESMRCmdSendMR, sentAndStored );
                   }
               }
           else
               {
               aMenu->SetItemDimmed( EESMRCmdSendMRUpdate, ETrue );
               }
           }

        if ( policy.IsDefaultCommand( EESMRCmdAddOptAttendee ) )
            {
            // if optional attendee (recipient) is already visible, don't show
            // the menu item.
            TBool isVisible =
                    iView->IsControlVisible( EESMRFieldOptAttendee );
            aMenu->SetItemDimmed( EESMRCmdAddOptAttendee, isVisible );
            }

        if ( policy.IsDefaultCommand( EESMRCmdViewTrack ) )
            {
            //only show tracking view option when item is sent, stored
            //and when cfsmailbox actually supports attendee status

            if ( SupportsMailBoxCapabilityL(
                    MESMRBuilderExtension::EMRCFSAttendeeStatus ) )
                {
                aMenu->SetItemDimmed( EESMRCmdViewTrack, !sentAndStored );
                }
            else
                {
                aMenu->SetItemDimmed( EESMRCmdViewTrack, ETrue );
                }
            }

        // Insert send ui menu for all other entry types than meeting request
        if ( entry->Type ( )!= MESMRCalEntry::EESMRCalEntryMeetingRequest )
            {
            TryInsertSendMenuL( aMenu );
            }
		}

    if ( aResourceId == R_LOCATION_OPTIONS
         || aResourceId == R_MR_EDITOR_LOCATION_MENU
         || aResourceId == R_MR_VIEWER_LOCATION_MENU )
        {
        menuStyle = EMRContextMenu;
        HandleLocationOptionsL( aResourceId, aMenu );
        }
    if ( aResourceId == R_MR_EDITOR_ATTACHMENT_CONTEXT_MENU
         || aResourceId == R_MR_EDITOR_ATTENDEE_CONTEXT_MENU )
        {
        menuStyle = EMRContextMenu;
        }

    // Field specific context menu
    iView->DynInitMenuPaneL( menuStyle, aResourceId, aMenu );
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::SupportsAttendeeStatusL
// ---------------------------------------------------------------------------
//
TBool CESMREditorDialog::SupportsMailBoxCapabilityL(
        MESMRBuilderExtension::TMRCFSMailBoxCapability aCapa )
    {
    FUNC_LOG;
    TBool supportsCapability( EFalse );
    CESMRFieldBuilderInterface* plugin = NULL;
    TRAPD( error,
            plugin = CESMRFieldBuilderInterface::CreatePluginL(
                        TUid::Uid( KESMRUIFieldBuilderPluginImplUId ) ) );
    CleanupStack::PushL( plugin );

    if ( error == KErrNone && plugin )
        {
        TUid uid = {0};
        MESMRBuilderExtension* extension =
            static_cast<MESMRBuilderExtension*>( plugin->ExtensionL( uid ) );

        if ( extension )
            {
            supportsCapability =
                extension->CFSMailBoxCapabilityL( aCapa );
            }
        }

    CleanupStack::PopAndDestroy( plugin );

    return supportsCapability;
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

    // If no attendees, switch MR to regular meeting
    VerifyMeetingRequestL();

    // Get the entry to be confirmed for saving
    MESMRCalEntry* entry = iInfoProvider.EntryL();
    
    // if entry is stored and type changed, we will always save it
    if( entry->IsStoredL() && entry->IsEntryTypeChangedL() )
        {
        res = TryToSaveEntryL();
        }
    // If entry is new and type changed, we have to see, if something has
    // changed, before we save it.
    else if( !entry->IsStoredL() && entry->IsEntryTypeChangedL() )
        {
        if( entry->IsEntryEditedL() )
            {
            res = TryToSaveEntryL();
            }
        }
    // Entry is not type changed, we will use normal procedure to
    // see if entry will be saved or not.
    else
        {
        if( entry->IsEntryEditedL() )
            {
            res = TryToSaveEntryL();
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

    MESMRCalEntry* entry = iInfoProvider.EntryL();

    TInt res( KErrNone);

    iView->ExternalizeL ( ETrue ); // force validation should be used

    // If no attendees, switch MR to regular meeting
    VerifyMeetingRequestL();

    // The entry data may be changed by the above function VerifyMeetingRequestL()
    entry = iInfoProvider.EntryL();

    if ( entry->IsEntryEditedL() )
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
                LocationPluginHandlerL().StoreLocationToHistoryL( *entry );
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

    MESMRCalEntry* entry = iInfoProvider.EntryL();

    iView->ExternalizeL(); // no force validation
    TInt res(KErrGeneral);
    res = iCallback.ProcessCommandWithResultL( EESMRCmdSaveMR );

    if ( res == KErrNone )
        {
        if ( iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
            {
            LocationPluginHandlerL().StoreLocationToHistoryL( *entry );
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
            iESMRSendUI->SendAsVCalendarL( aCommandId, entry->Entry ( ) );
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

    MESMRCalEntry* entry = iInfoProvider.EntryL();

    if ( aResourceId == R_MR_VIEWER_LOCATION_MENU )
        {
        TBool showOnMap = EFalse;
        TBool searchFromMap = EFalse;

        if ( iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
            {
            showOnMap = LocationPluginHandlerL().IsCommandAvailableL(
                    EESMRCmdShowOnMap,
                    *entry );
            searchFromMap = !showOnMap;
            }

        aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
        aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, !searchFromMap );

        const CESMRPolicy& policy = Policy();
        aMenuPane->SetItemDimmed( EESMRCmdEdit,
                                  !policy.IsDefaultCommand( EESMRCmdEdit ) );
        aMenuPane->SetItemDimmed( EESMRCmdEditLocal,
                                  !policy.IsDefaultCommand( EESMRCmdEditLocal ) );
        aMenuPane->SetItemDimmed( EESMRCmdCalEntryUIEdit,
                                  !policy.IsDefaultCommand( EESMRCmdCalEntryUIEdit ) );
        }
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
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
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
            iContextMenuResourceId = 0;

            if ( Policy().FieldL(
                    iView->FocusedField() ).iFieldMode == EESMRFieldModeView )
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

        case EMRCommandSwitchToMR:
        case EMRCommandSwitchToMeeting:
        case EMRCommandSwitchToMemo:
        case EMRCommandSwitchToAnniversary:
        case EMRCommandSwitchToTodo:
            {
            TRAPD( err, SwitchEntryTypeL( *command, ETrue ) )

            if (KErrArgument == err )
                 {
                 // There was validation error --> Just show the
                 // existing editor.
                 err = KErrNone;
                 }
             else if ( err )
                {
                // UI and engine not consistent. Show error and exit
                iCoeEnv->HandleError( err );
                TryExitL( EESMRCmdForceExit );
                }

            break;
            }
        case EESMRCmdLongtapDetected:
            {
            OkToExitL(EESMRCmdLongtapDetected);
            break;
            }
        case EMRLaunchAttachmentContextMenu:
        case EAknSoftkeyContextOptions:
            {
            OkToExitL( EAknSoftkeyContextOptions );
            break;
            }
        case EESMRCmdCalendarChange:
            {
            DrawDeferred();
            break;
            }
        case EESMRCmdAssignFromMap:
        case EMRCommandMyLocations:
            {
            DoProcessCommandL( *command );
            break;
            }
        default:
            {
            break;
            }
        }


    MESMRCalEntry* entry = iInfoProvider.EntryL();
    if ( entry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
    	{
		MESMRMeetingRequestEntry* mrEntry =
            static_cast<MESMRMeetingRequestEntry*>( iInfoProvider.EntryL() );

		TBool bOccursInPast( OccursInPast( iStartTimeUtc ) );
		
		TBool sendEnabled = ( !bOccursInPast &&
			( mrEntry->RoleL() == EESMRRoleOrganizer || mrEntry->IsForwardedL() ) &&            
			( iRequiredAttendeesEnabled | iOptionalAttendeesEnabled ) &&
			(( ( iInfoProvider.PolicyProvider().CurrentPolicy().ViewMode() == EESMREditMR )) ||
			( iInfoProvider.PolicyProvider().CurrentPolicy().ViewMode() == EESMRForwardMR )
			));

		if( sendEnabled )
			{
			cba.SetCommandL(
                    CEikButtonGroupContainer::ERightSoftkeyPosition,
                    R_MR_SEND_SOFTKEY);
            cba.MakeCommandVisibleByPosition(
                    CEikButtonGroupContainer::ERightSoftkeyPosition,
                    ETrue );
			cba.DrawDeferred();
			}
		else
			{
			cba.SetCommandL(
                    CEikButtonGroupContainer::ERightSoftkeyPosition,
                    R_MR_DONE_SOFTKEY);
            cba.MakeCommandVisibleByPosition(
                    CEikButtonGroupContainer::ERightSoftkeyPosition,
                    ETrue );
			cba.DrawDeferred();
			}
    	}
	else
		{
		cba.SetCommandL(
                CEikButtonGroupContainer::ERightSoftkeyPosition,
                R_MR_DONE_SOFTKEY);
        cba.MakeCommandVisibleByPosition(
                CEikButtonGroupContainer::ERightSoftkeyPosition,
                ETrue );
		cba.DrawDeferred();
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
    MESMRCalEntry* entry = iInfoProvider.EntryL();

    switch ( *fieldId )
        {
        case EESMRFieldLocation:
            {
            MESMRFieldEventValue* value =
                static_cast< MESMRFieldEventValue* >( aEvent.Param( 1 ) );
            if ( entry->Entry().LocationL() != value->StringValue() )
                {
                // Show query dialog only if entry has geocoordinates.
                // It can be verified by checking if "Show on map" is available
                // for iEntry as geocoordinates are required for that.
                TBool enableQuery =
                    LocationPluginHandlerL().IsCommandAvailableL(
                            EESMRCmdShowOnMap, *entry );

                if ( enableQuery
                     && CESMRConfirmationQuery::ExecuteL(
                        CESMRConfirmationQuery::EESMRAssignUpdatedLocation ) )
                    {
                    HBufC* oldLocation = entry->Entry().LocationL().AllocLC();
                    iView->ExternalizeL();

                    // if old and new locations are different and old location
                    // exists, the user is updating existing location. Let's discard
                    // the existing coordinates by using command EESMRCmdUpdateFromMap
                    if ( entry->Entry().LocationL().Compare( *oldLocation ) != 0 &&
                            oldLocation->Length() != 0 )
                        {
                        LocationPluginHandlerL().HandleCommandL(
                                EESMRCmdUpdateFromMap,
                                *entry );
                        }
                    else
                        {
                        LocationPluginHandlerL().HandleCommandL(
                            EESMRCmdAssignFromMap,
                            *entry,
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
			// for update iStartTimeUtc when meeting & mr event changing start time
            TBool bOccursInPast( OccursInPastL( aEvent ) );
            
            if ( entry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
            	{
        		MESMRMeetingRequestEntry* mrEntry =
                    static_cast<MESMRMeetingRequestEntry*>( iInfoProvider.EntryL() );
 	            TBool bIsOrganizer( mrEntry->RoleL() == EESMRRoleOrganizer );            
				
            	ChangeRightSoftkeyL( bOccursInPast, bIsOrganizer );
            	}
            break;
            }
            
        case EESMRFieldAllDayEvent:
            {
            TBool bOccursInPast( OccursInPast( iStartTimeUtc ) );   
            TInt* bChecked = static_cast< TInt* >( aEvent.Param( 1 ) );
            if( *bChecked == 1 )
            	{
				TDateTime startTime = iStartTimeUtc.DateTime();
				startTime.SetHour( 0 );
				startTime.SetMinute( 0 );
				startTime.SetSecond( 0 );
				bOccursInPast = OccursInPast( startTime );
            	}               

            if ( entry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
            	{
        		MESMRMeetingRequestEntry* mrEntry =
                    static_cast<MESMRMeetingRequestEntry*>( iInfoProvider.EntryL() );
                TBool bIsOrganizer( mrEntry->RoleL() == EESMRRoleOrganizer );
				
            	ChangeRightSoftkeyL( bOccursInPast, bIsOrganizer );
            	}
            break;
            }              
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::SetContextMenuL
// ---------------------------------------------------------------------------
//
TBool CESMREditorDialog::SetContextMenuL()
    {
    FUNC_LOG;
    TInt ret( EFalse );

    TESMREntryFieldId fieldId = iView->ClickedOrFocusedField();

    switch( fieldId )
        {
        case EESMRFieldLocation:
            {
            ret = ETrue;

            if ( Policy().FieldL( EESMRFieldLocation ).iFieldMode
                    == EESMRFieldModeEdit )
                {
                iContextMenuResourceId =
                        R_MR_EDITOR_LOCATION_CONTEXT_MENU;
                }
            else
                {
                iContextMenuResourceId =
                        R_MR_VIEWER_CONTEXT_MENU;
                }

            if ( !iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration )
                 && iContextMenuResourceId == R_MR_EDITOR_LOCATION_CONTEXT_MENU )
                {
#ifndef RD_USE_MYLOCATIONUI
                // Maps not supported and My Locations UI disabled
                ret = EFalse;
#endif //RD_USE_MYLOCATIONUI
                }
            }
            break;

        case EESMRFieldAttachments:
            {
            if ( Policy().FieldL(
                    EESMRFieldAttachments ).iFieldMode
                    == EESMRFieldModeEdit )
                {
                iContextMenuResourceId = R_MR_EDITOR_ATTACHMENT_CONTEXT_MENU;
                ret = ETrue;
                }
            }
            break;
        case EESMRFieldAttendee:
        case EESMRFieldOptAttendee:
            {
            if ( Policy().FieldL(
                    EESMRFieldAttendee ).iFieldMode == EESMRFieldModeEdit  ||
                 Policy().FieldL(
                    EESMRFieldOptAttendee ).iFieldMode == EESMRFieldModeEdit )
                {
                iContextMenuResourceId = R_MR_EDITOR_ATTENDEE_CONTEXT_MENU;
                ret = ETrue;
                }
            }
            break;
        default:
            break;
        }

    if ( ret )
        {
        iMenuBar->SetContextMenuTitleResourceId(
                iContextMenuResourceId );
        }
    else
        {
        iContextMenuResourceId = 0;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::SwitchEntryTypeL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::SwitchEntryTypeL(
        TInt aCommand,
        TBool aAdjustView )
    {
    FUNC_LOG;
    TBool change( EFalse );
    MESMRCalEntry* entry = iInfoProvider.EntryL();

    switch ( aCommand )
        {
        case EMRCommandSwitchToMR:
            {
            TBool ret( ETrue );

            // Show confirmation note if current mailbox does not
            // support attachments and attachments exist in the entry
            if( entry->Entry().AttachmentCountL() > 0 &&
                    !SupportsMailBoxCapabilityL(
                            MESMRBuilderExtension::
                                EMRCFSSupportsAttachmentsInMR ) )
                {
                if ( !CESMRConfirmationQuery::ExecuteL(
                                   CESMRConfirmationQuery::
                                       EESMRAttachmentsNotSupported ) )
                    {
                    ret = EFalse;
                    }
                }

            if( ret )
                {
                change =
                    ( entry->Type() !=
                        MESMRCalEntry::EESMRCalEntryMeetingRequest );
                }

            break;
            }
        case EMRCommandSwitchToMeeting:
            {
            change =
                ( entry->Type() != MESMRCalEntry::EESMRCalEntryMeeting );
            break;
            }
        case EMRCommandSwitchToMemo:
            {
            change =
                ( entry->Type() != MESMRCalEntry::EESMRCalEntryMemo );
            break;
            }
        case EMRCommandSwitchToAnniversary:
            {
            change =
                ( entry->Type() != MESMRCalEntry::EESMRCalEntryAnniversary );
            break;
            }
        case EMRCommandSwitchToTodo:
            {
            change =
                ( entry->Type() != MESMRCalEntry::EESMRCalEntryTodo);
            break;
            }
        default:
            {
            break;
            }
        }

    if ( change )
        {
        iView->ExternalizeL(ETrue);
        iStartTimeUtc = entry->Entry().StartTimeL().TimeUtcL();
        iCallback.ProcessCommandL( aCommand );

        if( aAdjustView )
            {
            MESMRCalEntry* entry = iInfoProvider.EntryL();
            iView->AdjustViewL( entry, Policy() );
            SetTitleL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::Policy
// ---------------------------------------------------------------------------
//
const CESMRPolicy& CESMREditorDialog::Policy() const
    {
    FUNC_LOG;
    return iInfoProvider.PolicyProvider().CurrentPolicy();
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::DynInitToolbarL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::DynInitToolbarL ( TInt /*aResourceId*/,
        CAknToolbar* /*aToolbar*/ )
    {

    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::OfferToolbarEventL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::OfferToolbarEventL ( TInt aCommand )
    {
    DoProcessCommandL(aCommand);
    }


// ---------------------------------------------------------------------------
// CESMREditorDialog::SetTitleL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::SetTitleL ()
    {
    FUNC_LOG;

    MESMRCalEntry* entry = iInfoProvider.EntryL();

    // Setting title to status pane's title pane
    TInt titleStringId = -1;
    switch ( entry->Type() )
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
        if( !iTitlePane )
            {
            iTitlePane = CESMRTitlePaneHandler::NewL( *iEikonEnv );
            }

        HBufC* titleText = StringLoader::LoadLC( titleStringId, iCoeEnv );
        iTitlePane->SetNewTitle( titleText );
        CleanupStack::PopAndDestroy( titleText );
        }

    // If subject does not exist, let's update the title also
    // to main window
    if( entry->Entry().SummaryL().Length() <= 0 )
        {
        HBufC* title ;
        switch( entry->Type() )
           {
           case MESMRCalEntry::EESMRCalEntryTodo:
               {
               title = StringLoader::LoadLC ( R_QTN_CALENDAR_TITLE_NEW_TODO );
               break;
               }
           case MESMRCalEntry::EESMRCalEntryMemo:
               {
               title = StringLoader::LoadLC ( R_QTN_CALENDAR_TITLE_NEW_MEMO );
               break;
               }
           case MESMRCalEntry::EESMRCalEntryAnniversary:
               {
               title = StringLoader::LoadLC(R_QTN_CALENDAR_TITLE_NEW_ANNIVERSARY);
               break;
               }
           case MESMRCalEntry::EESMRCalEntryMeetingRequest: // Fall through
           case MESMRCalEntry::EESMRCalEntryMeeting: // Fall through
           case MESMRCalEntry::EESMRCalEntryReminder: // Fall through
           default:
               {
               title = StringLoader::LoadLC( R_QTN_MEET_REQ_TITLE );
               break;
               }
           }
        iView->SetTitleL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    }

// ---------------------------------------------------------------------------
// CESMREditorDialog::VerifyMeetingRequestL
// ---------------------------------------------------------------------------
//
void CESMREditorDialog::VerifyMeetingRequestL()
    {
    FUNC_LOG;

    MESMRCalEntry* entry = iInfoProvider.EntryL();

    // If entry is meeting request, but no attendees have been added,
    // entry is switched to and saved as regular meeting
    if( CCalenInterimUtils2::IsMeetingRequestL( entry->Entry() ) )
        {
        RPointerArray<CCalAttendee>& attendees(
                entry->Entry().AttendeesL() );

        TInt attendeeCount = attendees.Count();

        if( ( attendeeCount == 0 ) &&
        	(iInfoProvider.PolicyProvider().CurrentPolicy().ViewMode() != EESMRForwardMR) )
            {
            TRAPD( err, SwitchEntryTypeL( EMRCommandSwitchToMeeting, EFalse ) )

            if ( err )
               {
               // UI and engine not consistent. Show error and exit
               iCoeEnv->HandleError( err );
               TryExitL( EESMRCmdForceExit );
               }
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMREditorDialog::ShowContextMenuL
// -----------------------------------------------------------------------------
//
void CESMREditorDialog::ShowContextMenuL()
    {
    FUNC_LOG;

    if ( iMenuBar )
        {
        iMenuBar->TryDisplayContextMenuBarL();
        }
    }


// -----------------------------------------------------------------------------
// CESMREditorDialog::OccursInPast
// -----------------------------------------------------------------------------
//
TBool CESMREditorDialog::OccursInPastL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    MESMRFieldEventValue* value = static_cast< MESMRFieldEventValue* >( aEvent.Param( 1) );

    TCalTime startCalTime;
    startCalTime.SetTimeLocalL( value->TimeValue() );

    iStartTimeUtc = startCalTime.TimeUtcL();

	return OccursInPast( iStartTimeUtc );
    }

// -----------------------------------------------------------------------------
// CESMREditorDialog::OccursInPast
// -----------------------------------------------------------------------------
//
TBool CESMREditorDialog::OccursInPast( const TTime& aStartUtc )
	{
    FUNC_LOG;
	TBool bOccursInPast(EFalse);

	TTime now;
    now.UniversalTime();

    if ( aStartUtc < now )
    	{
    	bOccursInPast = ETrue;
    	}

    return bOccursInPast;
	}

// -----------------------------------------------------------------------------
// CESMREditorDialog::ChangeRightSoftkeyL
// -----------------------------------------------------------------------------
//
void CESMREditorDialog::ChangeRightSoftkeyL( TBool aOccursInPast, TBool aIsOrganizer )
	{
    FUNC_LOG;
	TBool sendEnabled = 
		( iRequiredAttendeesEnabled | iOptionalAttendeesEnabled ) && 
		!aOccursInPast && aIsOrganizer;

	CEikButtonGroupContainer& cba = ButtonGroupContainer();
	
	if( sendEnabled )
		{
		cba.SetCommandL(
                CEikButtonGroupContainer::ERightSoftkeyPosition,
                R_MR_SEND_SOFTKEY);
		cba.DrawDeferred();
		}
	else
		{
		cba.SetCommandL(
                CEikButtonGroupContainer::ERightSoftkeyPosition,
                R_MR_DONE_SOFTKEY);
		cba.DrawDeferred();   
		}	
	}

// -----------------------------------------------------------------------------
// CESMREditorDialog::TryToSaveEntryL
// -----------------------------------------------------------------------------
//
TInt CESMREditorDialog::TryToSaveEntryL()
    {
    FUNC_LOG;
    TInt res( KErrNone );
    
    MESMRCalEntry* entry = iInfoProvider.EntryL();

    if ( IsAllowedToSaveL() )
        {
        res = iCallback.ProcessCommandWithResultL( EESMRCmdSaveMR );

        if ( res == KErrNone
             && iFeatures->FeatureSupported(
                     CESMRFeatureSettings::EESMRUIPreviousLocationsList ) )
            {
            LocationPluginHandlerL().StoreLocationToHistoryL( *entry );
            }
            if ( entry->Type() == MESMRCalEntry::EESMRCalEntryTodo )
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
    else
        {
        res = KErrCancel;
        }
    
    return res;
    }

// EOF
