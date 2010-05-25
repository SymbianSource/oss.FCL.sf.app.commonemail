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
* Description:  Location plugin implementation
*
*/


#include "cesmrlocationpluginimpl.h"
#include "mesmrlocationpluginobserver.h"
#include "cesmrurlparserplugin.h"

#include <mnproviderfinder.h>
#include <mnprovider.h>
#include <mnmapview.h>
#include <EPos_CPosLandmark.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <ct/rcpointerarray.h>
#include <calentry.h>

#include "emailtrace.h"

// ======== LOCAL FUNCTIONS ========

namespace
    {
/**
 * Converts vCal GEO value to landmark with coordinates
 *
 * @param aGeoValue coordinates
 * @return a landmark with coordinates
 */
CPosLandmark* CreateLandmarkL( const CCalGeoValue& aGeoValue )
    {
    TReal lat;
    TReal lon;

    if( !aGeoValue.GetLatLong( lat, lon ) )
        {
        User::Leave( KErrArgument );
        }

    CPosLandmark* landmark = CPosLandmark::NewLC();
    TLocality coordinate;
    coordinate.SetCoordinate( lat, lon );
    landmark->SetPositionL( coordinate );
    CleanupStack::Pop( landmark );
    return landmark;
    }

    }
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::CESMRLocationPluginImpl
// ---------------------------------------------------------------------------
//
CESMRLocationPluginImpl::CESMRLocationPluginImpl()
    : CESMRLocationPlugin( CActive::EPriorityStandard )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::ConstructL()
    {
    FUNC_LOG;
    // Find Map & Navigation provider
    RCPointerArray<CMnProvider> providers;
    CleanupClosePushL( providers );
    MnProviderFinder::FindProvidersL( providers,
                                      CMnProvider::EServiceMapView
                                      | CMnProvider::EServiceNavigation );
    if ( providers.Count() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    iProvider = providers[ 0 ];
    providers.Remove( 0 );
    // PopAndDestroy provides from cleanupstack
    CleanupStack::PopAndDestroy( &providers );

    // Create Map & Navigation view
    CreateMapViewL();

    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::NewL
// ---------------------------------------------------------------------------
//
CESMRLocationPluginImpl* CESMRLocationPluginImpl::NewL()
    {
    FUNC_LOG;
    CESMRLocationPluginImpl* self = new( ELeave ) CESMRLocationPluginImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::SetObserver
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::SetObserver ( MESMRLocationPluginObserver* aObserver )
    {
    FUNC_LOG;
    iObserver = aObserver;
    }
// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::~CESMRLocationPluginImpl
// ---------------------------------------------------------------------------
//
CESMRLocationPluginImpl::~CESMRLocationPluginImpl()
    {
    FUNC_LOG;
    Cancel();
    if ( iMapView )
        {
        iMapView->RemoveExitObserver();
        delete iMapView;
        }
    delete iProvider;
    delete iUrlParser;

    iLandMarks.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::CreateMapViewL
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::CreateMapViewL()
    {
    FUNC_LOG;
    if ( !iMapView )
        {
        iMapView = CMnMapView::NewChainedL( *iProvider );
        iMapView->SetExitObserverL( *this );
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::SelectFromMapL
// From class MESMRLocationPlugin.
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::SelectFromMapL( const TDesC& aSearchQuery,
                                              const TDesC& aLocationUrl )
    {
    FUNC_LOG;

    // Create landmark from location URL. TRAP errors because URL is optional.
    CPosLandmark* location = NULL;
    TRAP_IGNORE( location = //codescanner::forgottoputptroncleanupstack
            UrlParserL().CreateLandmarkFromUrlL( aLocationUrl ) )

    SelectFromMapL( aSearchQuery, location );
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::SelectFromMapL
// From class MESMRLocationPlugin.
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::SelectFromMapL(
            const TDesC& aSearchQuery,
            const CCalGeoValue* aGeoValue )
    {
    FUNC_LOG;

    CPosLandmark* location = NULL;

    if ( aGeoValue )
        {
        location = CreateLandmarkL( *aGeoValue );
        }

    SelectFromMapL( aSearchQuery, location );
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::SearchFromMapL
// From class MESMRLocationPlugin.
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::SearchFromMapL( const TDesC& aSearchQuery )
    {
    FUNC_LOG;
    iLandMarks.ResetAndDestroy();

    CPosLandmark* location = CPosLandmark::NewLC();
    location->SetLandmarkNameL( aSearchQuery );

    iLandMarks.AppendL( location );
    CleanupStack::Pop( location );

    CreateMapViewL();

    iMapView->ResetLandmarksToShow();
    iMapView->AddLandmarksToShowL( iLandMarks );
    iMapView->ShowMapL();
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::ShowOnMapL
// From class MESMRLocationPlugin.
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::ShowOnMapL( const TDesC& aLocationUrl )
    {
    FUNC_LOG;

    CPosLandmark* landmark =
        UrlParserL().CreateLandmarkFromUrlL( aLocationUrl );
    ShowLandmarkL( landmark );
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::ShowOnMapL
// From class MESMRLocationPlugin.
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::ShowOnMapL( const CCalGeoValue& aGeoValue )
    {
    FUNC_LOG;

    CPosLandmark* landmark = CreateLandmarkL( aGeoValue );
    ShowLandmarkL( landmark );
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::DoCancel
// From class CActive.
// Cancels outstanding request to iMapView
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::DoCancel()
    {
    FUNC_LOG;
    if( iMapView )
        {
        iMapView->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::RunL
// From class CActive.
// Process asynchronous SelectFromMap completion from CMnMapView
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::RunL()
    {
    FUNC_LOG;
    HandleSelectFromMapCompletedL();
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::HandleSelectFromMapCompletedL
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::HandleSelectFromMapCompletedL()
    {
    FUNC_LOG;
    if ( iStatus == KErrNone )
        {
        CMnMapView::TSelectionResultType type;
        type = iMapView->SelectionResultType();

        if ( iObserver )
            {
            switch ( type )
                {
                case CMnMapView::ESelectionFreeLandmark:
                    {
                    // Read selection from map view, send result to observer
                    CPosLandmark* landmark =
                                iMapView->RetrieveSelectionResultL();
                    iObserver->SelectFromMapCompleted( iStatus.Int(),
                                                       landmark );
                    break;
                    }
                case CMnMapView::ESelectionLandmarkIndex:
                    {
                    TInt landmarkIndex = 0;
                    iMapView->RetrieveSelectionResultL( landmarkIndex );

                    if ( landmarkIndex < iLandMarks.Count() )
                        {
                        iObserver->SelectFromMapCompleted(
                            iStatus.Int(),
                            iLandMarks[landmarkIndex] );
                        iLandMarks.Remove( landmarkIndex );
                        }
                    else
                        {
                        User::Leave( KErrGeneral );
                        }
                    break;
                    }
                default:
                    {
                    User::Leave( KErrNotSupported );
                    break;
                    }
                }
            }
        }
    else if (iObserver)
        {
        // Notify error to observer
        iObserver->SelectFromMapCompleted( iStatus.Int(), NULL );
        }

    // Close Map view
    delete iMapView;
    iMapView = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::RunError
// From class CActive.
// ---------------------------------------------------------------------------
//
TInt CESMRLocationPluginImpl::RunError( TInt aError )
    {
    FUNC_LOG;
    // Notify error to observer
    if(iObserver)
        {
        iObserver->SelectFromMapCompleted( aError, NULL );
        }

    // Close Map view
    delete iMapView;
    iMapView = NULL;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::HandleServerAppExit
// From class MAknServerAppExitObserver.
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::HandleServerAppExit( TInt aReason )
    {
    FUNC_LOG;
    Cancel();

    if ( iObserver )
        {
        // Notify observer that application has been closed without selection.
        iObserver->SelectFromMapCompleted( aReason, NULL );
        }

    delete iMapView;
    iMapView = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::ShowLandmarkL
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::SelectFromMapL(
        const TDesC& aSearchQuery,
        CPosLandmark* aLandmark )
    {
    iLandMarks.ResetAndDestroy();

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }

    CreateMapViewL();

    iMapView->ResetMapArea();

    CPosLandmark* location = aLandmark;

    if ( location ) // Landmark created. Set search string as landmark name
        {
        if ( aSearchQuery.Length() > 0 )
            {
            CleanupStack::PushL( location );

            location->SetLandmarkNameL( aSearchQuery );

            iLandMarks.AppendL( location );
            CleanupStack::Pop( location );

            iMapView->ResetLandmarksToShow();
            iMapView->AddLandmarksToShowL( iLandMarks );
            }
        }
    else
        {
        // No location url provided, let's search from maps
        // with search string. It was agreed that search is done by
        // passing a landmark with search string as the name of the landmark
        // forward
         if ( aSearchQuery.Length() > 0 )
            {
            location = CPosLandmark::NewLC();
            location->SetLandmarkNameL( aSearchQuery );

            iLandMarks.AppendL( location );
            CleanupStack::Pop( location );

            iMapView->ResetLandmarksToShow();
            iMapView->AddLandmarksToShowL( iLandMarks );
            }

        // Enable current location focus on map
        iMapView->
            SetCurrentLocationOption( CMnMapView::ECurrentLocationEnabled );
        }

    SetActive();
    iMapView->SelectFromMapL( iStatus );
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::ShowLandmarkL
// ---------------------------------------------------------------------------
//
void CESMRLocationPluginImpl::ShowLandmarkL( CPosLandmark* aLandmark )
    {
    CreateMapViewL();

    iMapView->ResetLandmarksToShow();
    RCPointerArray<CPosLandmark> landmarks;
    CleanupClosePushL( landmarks );
    CleanupStack::PushL( aLandmark );
    landmarks.AppendL( aLandmark );
    CleanupStack::Pop( aLandmark );
    iMapView->AddLandmarksToShowL( landmarks );
    CleanupStack::PopAndDestroy( &landmarks );
    iMapView->ShowMapL();
    }

// ---------------------------------------------------------------------------
// CESMRLocationPluginImpl::UrlParserL
// ---------------------------------------------------------------------------
//
CESMRUrlParserPlugin& CESMRLocationPluginImpl::UrlParserL()
    {
    if ( !iUrlParser )
        {
        iUrlParser = CESMRUrlParserPlugin::NewL();
        }

    return *iUrlParser;
    }

// EOF
