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
* Description:  Utility to access MRUI feature settings repository.
*
*/

#ifndef CESMRFEATURESETTINGS_H
#define CESMRFEATURESETTINGS_H


#include <e32base.h>

/**
 *  CESMRFeatureSettings is utility to access MRUI Features repository.
 *  Usage:
 *
 *  @code
 *   // instantiate settings
 *   CESMRFeatureSettings* setting = CESMRFeatureSettings::NewL()
 *   // query feature
 *   TBool locationFeatures = setting->FeatureSupported( EESMRUILocationFeatures );
 *   if ( locationFeatures )
 *      {
 *      // Do something feature specific
 *      }
 *   // free resources
 *   delete settings;
 *  @endcode
 *
 *  @lib esmrcommon.lib
 */
NONSHARABLE_CLASS( CESMRFeatureSettings ) : public CBase
    {

public:

    /**  Feature setting flags. */
    enum TFeature
        {
        /** Map and Navigation Fw support */
        EESMRUIMnFwIntegration = 0x1,
        /** Contacts support */
        EESMRUIContactsIntegration = 0x2,
        /** Landmarks application support */
        EESMRUILandmarksIntegration = 0x4,
        /** Previous locations list */
        EESMRUIPreviousLocationsList = 0x8,
        /** Any of the previous location features */
        EESMRUILocationFeatures = 0xf,
        /** Meeting request viewer only for Cmail */
        EMRUIMeetingRequestViewerCmailOnly = 0x10,
        /** Any of the prevuious features */
        EMRUIAllFeatures
        };

    IMPORT_C static CESMRFeatureSettings* NewL();
    IMPORT_C static CESMRFeatureSettings* NewLC();

    /**
    * Destructor.
    */
    virtual ~CESMRFeatureSettings();

    /**
     * Gets feature setting. The setting must be enumerated in
     * CESMRFeatureSettings::TFeature enumeration.
     * Multiple features can be queried at the same time by
     * bitwise-oring the corresponding feature flags.
     *
     * @param aFeatures the features.
     * @return ETrue if feature is enabled
     */
    IMPORT_C TBool FeatureSupported( TUint aFeatures ) const;

private:
    CESMRFeatureSettings();

    void ConstructL();
    
    TInt MapFeature( TUint aFeature ) const;

private: // data

    /**
     * Feature index array for mapping features
     */
    enum TFeatureIndex
        {
        EESMRUIMnFwIntegrationIndex = 0,
        EESMRUIContactsIntegrationIndex,
        EESMRUILandmarksIntegrationIndex,
        EESMRUIPreviousLocationsListIndex,
        EMRUIMeetingRequestViewerCmailIndex,
        // Last item. Number of features
        EESMRUINumFeatures
        };
    
    /**
     * Feature status array
     */
    TInt iFeatures[ EESMRUINumFeatures ];

    };

#endif // CESMRFEATURESETTINGS_H
