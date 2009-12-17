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
* Description:  MRUI Feature settings utility implementation
*
*/

#include "emailtrace.h"
#include "cesmrfeaturesettings.h"

#include <featmgr.h>
#include <centralrepository.h>
#include <bldvariant.hrh>

#include "esmrconfig.hrh"
#include "mruiprivatecrkeys.h"

namespace { // codescanner::namespace

#ifdef _DEBUG

_LIT( KPanicCategory, "CESMRFeatureSettings" );

enum TPanicCode
    {
    EUnknownFeature = 0
    };

#endif  // _DEBUG

// <cmail> Removed unneeded cleanup function.
// </cmail>

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRFeatureSettings::CESMRFeatureSettings
// ---------------------------------------------------------------------------
//
CESMRFeatureSettings::CESMRFeatureSettings()
    {
    FUNC_LOG;
    // Do nothing
    }


// ---------------------------------------------------------------------------
// CESMRFeatureSettings::ConstructL
// Reads settings from CenRep
// ---------------------------------------------------------------------------
//
void CESMRFeatureSettings::ConstructL()
    {
    FUNC_LOG;
    // <cmail> Removed unneeded cleanup function.
    FeatureManager::InitializeLibL();
    
    TBool locationIntegrationSupported = FeatureManager::FeatureSupported(
        KFeatureIdFfFsCalendarLocation );

    FeatureManager::UnInitializeLib();

    if ( locationIntegrationSupported )
        {
        // Read settings from central repository. These are read-only settings,
        // so these must be read only once at this object lifetime.
        CRepository* settings = NULL;
        TRAPD( err, settings = CRepository::NewL( KCRUidESMRUIFeatures ) );
        
        if ( err == KErrNone )
			{
	        settings->Get( KESMRUIFeatureMnFwIntegration,
    	                   iLocationFeatures[ EESMRUIMnFwIntegrationIndex ] );
                
        	settings->Get( KESMRUIFeatureContactsIntegration,
            	           iLocationFeatures[ EESMRUIContactsIntegrationIndex ] );
        
	        settings->Get( KESMRUIFeatureLandmarksIntegration,
    	                   iLocationFeatures[ EESMRUILandmarksIntegrationIndex ] );
        
        	settings->Get( KESMRUIFeaturePreviousLocationsList,
            	           iLocationFeatures[ EESMRUIPreviousLocationsListIndex ] );
        	
	        delete settings;
			}		
        }
    // </cmail>
    }


// ---------------------------------------------------------------------------
// CESMRFeatureSettings* CESMRFeatureSettings::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFeatureSettings* CESMRFeatureSettings::NewL()
    {
    FUNC_LOG;
    CESMRFeatureSettings* self = CESMRFeatureSettings::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRFeatureSettings* CESMRFeatureSettings::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFeatureSettings* CESMRFeatureSettings::NewLC()
    {
    FUNC_LOG;
    CESMRFeatureSettings* self = new( ELeave ) CESMRFeatureSettings;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRFeatureSettings::~CESMRFeatureSettings
// ---------------------------------------------------------------------------
//
CESMRFeatureSettings::~CESMRFeatureSettings()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRFeatureSettings::FeatureSupported
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRFeatureSettings::FeatureSupported( TUint aFeatures ) const
    {
    FUNC_LOG;
    TBool featureSupported( EFalse );
    switch ( aFeatures )
        {
        // Single flags
        case EESMRUIMnFwIntegration:
        case EESMRUIContactsIntegration:
        case EESMRUILandmarksIntegration:
        case EESMRUIPreviousLocationsList:
            {
            TInt index = MapFeature( aFeatures );
            if ( index > KErrNotFound
                 && iLocationFeatures[ index ] ) // codescanner::accessArrayElementWithoutCheck2
                {
                featureSupported = ETrue;
                }
            break;
            }
        
        // All flag
        case EESMRUILocationFeatures:
            {
            for ( TInt i = 0; i < EESMRUINumLocationFeatures; ++i )
                {
                if ( iLocationFeatures[ i ] ) // codescanner::accessArrayElementWithoutCheck2
                    {
                    featureSupported = ETrue;
                    break; // for-loop
                    }
                }
            break;
            }
            
        // Combined flags
        default:
            {
            TUint mask = 0x1;
            for ( TInt i = 0; i < EESMRUINumLocationFeatures; ++i )
                {
                TUint feature = aFeatures & mask;
                TInt index = MapFeature( feature );
                if ( ( index > KErrNotFound )
                     && ( feature < EESMRUILocationFeatures )
                     && iLocationFeatures[ index ] ) // codescanner::accessArrayElementWithoutCheck2
                    {
                    featureSupported = ETrue;
                    break;
                    }
                else if ( feature > EESMRUILocationFeatures )
                    {
                    __ASSERT_DEBUG( EFalse, User::Panic( KPanicCategory, EUnknownFeature) );
                    }
                
                // Test next feature
                mask = mask << 1;
                }
            
            break;
            }
        }
    return featureSupported;
    }

// ---------------------------------------------------------------------------
// CESMRFeatureSettings::MapFeature
// Converts single feature bitmask to feature array index
// ---------------------------------------------------------------------------
//
TInt CESMRFeatureSettings::MapFeature( TUint aFeature ) const
    {
    FUNC_LOG;
    TInt index = KErrNotFound;
    switch ( aFeature )
        {
        case 0: // None feature
            {
            break;
            }
        case EESMRUIMnFwIntegration:
            {
            index = EESMRUIMnFwIntegrationIndex;
            break;
            }
        case EESMRUIContactsIntegration:
            {
            index = EESMRUIContactsIntegrationIndex;
            break;
            }
        case EESMRUILandmarksIntegration:
            {
            index = EESMRUILandmarksIntegrationIndex;
            break;
            }
        case EESMRUIPreviousLocationsList:
            {
            index = EESMRUIPreviousLocationsListIndex;
            break;
            }
        default: // Illegal flags
            {
            __ASSERT_DEBUG( EFalse, User::Panic( KPanicCategory, EUnknownFeature) );
            break;
            }
        }
    
    return index;
    }

