/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements RECAL customisation api
*
*/

// INCLUDES
#include "cmrbcplugin.h"
#include "cmrbcpluginentryloader.h"
#include "cmrbcpluginresourcereader.h"
#include "cmrbceventplugin.h"
#include "esmrhelper.h"
#include "esmrdef.h"
#include "esmrinternaluid.h"

#include <calendar.rsg>
#include <calenservices.h>
#include <eikmenup.h>
#include <bautils.h>
#include <ecom/ecom.h>
#include <MeetingRequestUids.hrh>
#include <ct/rcpointerarray.h>
#include <data_caging_path_literals.hrh>
#include <CalenInterimUtils2.h>
#include <calencommands.hrh>
#include <featdiscovery.h>
#include <bldvariant.hrh>

#include "emailtrace.h"

namespace { // codescanner::namespace

#ifdef _DEBUG

// Panic literal
_LIT( KMRBCPlugin, "MRBCPlugin" );

enum TMRBCPluginPanic
    {
    ENoBCPluginsFound // No MRUi MR Viewer Impl found
    };

void Panic( TMRBCPluginPanic aPanic )
    {
    User::Panic( KMRBCPlugin, aPanic );
    }

#endif // _DEBUG

TBCPluginEventType EventTypeL(
        MCalenServices& aServices,
        CCalEntry& aEntry )
    {
    TBCPluginEventType eventType( EMRTypeNone );

    CCalenInterimUtils2& utils = aServices.InterimUtilsL();
    if ( utils.IsMeetingRequestL(aEntry) )
        {
        eventType = EMRTypeMeetingRequest;
        }
    else
        {
        switch( aEntry.EntryTypeL() )
            {
            case CCalEntry::EAppt:
                eventType = EMRTypeMeeting;
                break;

            case CCalEntry::ETodo:
                eventType = EMRTypeToDo;
                break;

            case CCalEntry::EEvent:
                eventType = EMRTypeMemo;
                break;

            case CCalEntry::EAnniv:
                eventType = EMRTypeAnniversary;
                break;
            }
        }

    return eventType;
    }

/**
 * Checks if features for handling commands are enabled
 * @param aCommand command to check
 * @return ETrue if feature is enabled for handling the given command
 */
TBool IsCommandHandlerEnabledL( TInt aCommand )
    {
    FUNC_LOG;

    TBool enabled( ETrue );

    switch ( aCommand )
        {
        case ECalenNewMeetingRequest:
            {
            if ( !CFeatureDiscovery::IsFeatureSupportedL(
                    TUid::Uid( KFeatureIdFfCalMeetingRequestUi ) ) )
                {
                // KFeatureIdFfCalMeetingRequestUi is not supported.
                // Disable ECalenNewMeetingRequest command handling.
                enabled = EFalse;
                }
            break;
            }

        case ECalenEventView:
            {
            break;
            }

        case ECalenNewMeeting:
        case ECalenNewTodo:
        case ECalenNewAnniv:
        case ECalenNewDayNote:
            {
            if ( !CFeatureDiscovery::IsFeatureSupportedL(
                    TUid::Uid( KFeatureIdFfEnhancedCalendarEventUi ) ) )
                {
                // KFeatureIdFfEnhancedCalendarEventUi is not supported.
                // Disable enhanced event editors.
                enabled = EFalse;
                }
            break;
            }

        case ECalenNewMeetingTimeSpan:
            {
            if ( !CFeatureDiscovery::IsFeatureSupportedL(
                    TUid::Uid( KFeatureIdFfCalMeetingRequestUi ) )
                 && !CFeatureDiscovery::IsFeatureSupportedL(
                         TUid::Uid( KFeatureIdFfEnhancedCalendarEventUi ) ) )
                {
                enabled = EFalse;
                }
            break;
            }

        default:
            {
            enabled = EFalse;
            break;
            }
        }

    return enabled;
    }

/**
 * Checks is feature for handling a command is enabled
 * @param aCommand the command to handle
 * @param aType the event type
 * @return ETrue if command handling is enabled
 */
TBool IsCommandSupportedForEntryTypeL(
        TInt aCommand,
        TBCPluginEventType aType )
    {
    FUNC_LOG;

    TBool supported = ETrue;

    if ( aCommand == ECalenEventView )
        {
        // Check that corresponding viewer feature is enabled
        switch ( aType )
            {
            case EMRTypeMeetingRequest:
                {
                if ( !CFeatureDiscovery::IsFeatureSupportedL(
                        TUid::Uid( KFeatureIdFfCalMeetingRequestUi ) ) )
                    {
                    // Meeting request viewer is not supported
                    supported = EFalse;
                    }
                break;
                }

            case EMRTypeMeeting:
            case EMRTypeMemo:
            case EMRTypeAnniversary:
            case EMRTypeToDo:
                {
                if ( !CFeatureDiscovery::IsFeatureSupportedL(
                        TUid::Uid( KFeatureIdFfEnhancedCalendarEventUi ) ) )
                    {
                    // Enhanced event viewers are not supported
                    supported = EFalse;
                    }
                break;
                }
            default:
                {
                supported = EFalse;
                break;
                }

            }
        }

    return supported;
    }

} // namespace

// -----------------------------------------------------------------------------
// CMRBCPlugin::CMRBCPlugin
// C++ constructor for this class
// -----------------------------------------------------------------------------
//
CMRBCPlugin::CMRBCPlugin( MCalenServices* aServices )
:   iServices( aServices )
    {
    FUNC_LOG;
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::~CMRBCPlugin
// Destructor for this class
// -----------------------------------------------------------------------------
//
CMRBCPlugin::~CMRBCPlugin()
    {
    FUNC_LOG;

    if ( iServices )
        {
        iServices->Release();
        }

    iPluginResources.ResetAndDestroy();
    delete iEntryLoader;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::NewL
// Static constructor for this class
// -----------------------------------------------------------------------------
//
CMRBCPlugin* CMRBCPlugin::NewL(
        MCalenServices* aServices )
    {
    FUNC_LOG;

    CMRBCPlugin* self = new ( ELeave ) CMRBCPlugin( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::ConstructL
// 2nd phase constructor for this class.
// -----------------------------------------------------------------------------
//
void CMRBCPlugin::ConstructL()
    {
    FUNC_LOG;

    RCPointerArray<CImplementationInformation> bcPlugins;
    CleanupClosePushL( bcPlugins );

    REComSession::ListImplementationsL(
            TUid::Uid(KMRBCEventPluginInterfaceUID),
            bcPlugins );

    TInt pluginCount(  bcPlugins.Count() );
    __ASSERT_DEBUG( pluginCount , Panic( ENoBCPluginsFound) );

    if ( !pluginCount )
        {
        ERROR( KErrNotFound, "No BC Plugins found" );
        User::Leave( KErrNotFound );
        }

    for ( TInt i(0); i < pluginCount; ++i )
        {
        HBufC* resourceFileName =
                    HBufC::NewLC( bcPlugins[i]->OpaqueData().Length() );
        resourceFileName->Des().Copy( bcPlugins[i]->OpaqueData() );

        CMRBCPluginResourceReader* pluginResource =
                CMRBCPluginResourceReader::NewL( *iServices );
        CleanupStack::PushL( pluginResource );

        TFileName resource;
        TInt err = ESMRHelper::LocateResourceFile(
                        *resourceFileName,
                        KDC_RESOURCE_FILES_DIR,
                        resource,
                        NULL );

        User::LeaveIfError( err );

        pluginResource->ReadFromResourceFileL(
                bcPlugins[i]->ImplementationUid(),
                resource );

        User::LeaveIfError(
                iPluginResources.Append( pluginResource ) );

        CleanupStack::Pop( pluginResource );
        CleanupStack::PopAndDestroy( resourceFileName );
        }

    // bcPlugins
    CleanupStack::PopAndDestroy();  // codescanner::cleanup

    iEntryLoader = CMRBCPluginEntryLoader::NewL( *iServices );
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::GetCustomViewsL
// -----------------------------------------------------------------------------
//
void CMRBCPlugin::GetCustomViewsL(
        RPointerArray<CCalenView>& /*aCustomViewArray*/ )
    {
    FUNC_LOG;

    // Do nothing
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::GetCustomSettingsL
// No implementation needed
// -----------------------------------------------------------------------------
//
void CMRBCPlugin::GetCustomSettingsL( RPointerArray<CAknSettingItem>& /*aCustomSettingArray*/ )
    {
    FUNC_LOG;

    // Do nothing
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::InfobarL
// No implementation needed
// -----------------------------------------------------------------------------
//
CCoeControl* CMRBCPlugin::InfobarL( const TRect& /*aRect*/ )
    {
    FUNC_LOG;

    // Do nothing
    return NULL;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::InfobarL
// No implementation needed
// -----------------------------------------------------------------------------
//
const TDesC& CMRBCPlugin::InfobarL()
    {
    FUNC_LOG;

    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::CustomPreviewPaneL
// No implementation needed
// -----------------------------------------------------------------------------
//
MCalenPreview* CMRBCPlugin::CustomPreviewPaneL( TRect& /*aRect*/ )
    {
    FUNC_LOG;

    // Do nothing
    return NULL;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::PreviewPaneL
// No implementation needed
// -----------------------------------------------------------------------------
//
CCoeControl* CMRBCPlugin::PreviewPaneL( TRect& /*aRect*/ )
    {
    FUNC_LOG;

    // Do nothing
    return NULL;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::CommandHandlerL
// Decides what command handler is returned
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CMRBCPlugin::CommandHandlerL(
        TInt aCommand )
    {
    FUNC_LOG;

    MCalenCommandHandler* cmdHandler = NULL;

    if ( IsCommandHandlerEnabledL( aCommand ) )
        {
        cmdHandler = ResolveCommandHandlerL( aCommand );
        }

    return cmdHandler;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::RemoveViewsFromCycle
// -----------------------------------------------------------------------------
//
void CMRBCPlugin::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
    {
    FUNC_LOG;

    // Do nothing
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::CustomiseMenuPaneL
// No implementation needed
// -----------------------------------------------------------------------------
//
TBool CMRBCPlugin::CustomiseMenuPaneL( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    FUNC_LOG;

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::CanBeEnabledDisabled
// No implementation needed
// -----------------------------------------------------------------------------
//
TBool CMRBCPlugin::CanBeEnabledDisabled()
    {
    FUNC_LOG;

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::CalenCustomisationExtensionL
// No implementation needed
// -----------------------------------------------------------------------------
//
TAny* CMRBCPlugin::CalenCustomisationExtensionL( TUid /*aExtensionUid*/ )
    {
    FUNC_LOG;

    return NULL;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::ResolveCommandHandlerL
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CMRBCPlugin::ResolveCommandHandlerL( TInt aCommand )
    {
    MCalenCommandHandler* cmdHandler = NULL;

    //Open meetingview instead of mr while can't resolve phone owner.
    //This situation command ECalenOpenMeetingView will be received.
    if( EMRLaunchMeetingViewer == aCommand )
        {
        CMRBCEventPlugin* eventPlugin = PluginByUidL(
                TUid::Uid( KMRBCMREventPluginImplementationUID ) );

        if ( eventPlugin )
            {
            cmdHandler = eventPlugin->CommandHandler();
            }
        }
    else
        {
        TInt pluginCount( iPluginResources.Count() );
        for ( TInt i(0); i < pluginCount && !cmdHandler; ++i )
            {
            TMRBCPluginCommand pluginCommand;

            TInt err = iPluginResources[i]->Command( aCommand, pluginCommand );
            if ( KErrNone == err )
                {
                // Command was found from the plugin
                if ( pluginCommand.CheckEntryType() )
                    {
                    iEntryLoader->UpdateEntryFromDatabaseL();

                    TBCPluginEventType eventType(
                            EventTypeL( *iServices, iEntryLoader->Entry() ) );

                    if ( iPluginResources[i]->SupportsType( eventType)
                         && IsCommandSupportedForEntryTypeL( aCommand, eventType ) )
                        {
                        cmdHandler = iPluginResources[i]->PluginL().CommandHandler();
                        }
                    }
                else
                    {
                    cmdHandler = iPluginResources[i]->PluginL().CommandHandler();
                    }
                }
            }
        }
    return cmdHandler;
    }

// -----------------------------------------------------------------------------
// CMRBCPlugin::PluginByUid
// -----------------------------------------------------------------------------
//
CMRBCEventPlugin* CMRBCPlugin::PluginByUidL( TUid aUid )
    {
    CMRBCEventPlugin* plugin = NULL;

    for ( TInt i = 0; i < iPluginResources.Count() && !plugin; ++i )
        {
        if ( iPluginResources[ i ]->PluginImplUid() == aUid )
            {
            plugin = &( iPluginResources[ i ]->PluginL() );
            }
        }

    return plugin;
    }

// End of file
