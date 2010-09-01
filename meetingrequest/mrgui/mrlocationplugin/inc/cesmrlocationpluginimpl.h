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
* Description:  Location plugin factory implementation
*
*/


#ifndef CESMRLOCATIONPLUGINIMPL_H
#define CESMRLOCATIONPLUGINIMPL_H


#include <e32base.h>
//<cmail>
#include "AknServerApp.h"
//</cmail>

#include "cesmrlocationplugin.h"

class MESMRLocationPluginObserver;
class CMnProvider;
class CMnMapView;
class CPosLandmark;
class CESMRUrlParserPlugin;

/**
 *  CESMRLocationPlugin Ecom API implementatation
 */
class CESMRLocationPluginImpl: public CESMRLocationPlugin,
                               public MAknServerAppExitObserver
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aObserver extension for the API
     */
    static CESMRLocationPluginImpl* NewL();

    /**
    * C++ Destructor.
    */
    virtual ~CESMRLocationPluginImpl();

public:// from base CESMRLocationPlugin ecom interface
    void SetObserver ( MESMRLocationPluginObserver* aObserver );

public:// from base class MESMRLocationPlugin
    void SelectFromMapL(
            const TDesC& aSearchQuery,
            const TDesC& aLocationUrl );
    void SelectFromMapL(
            const TDesC& aSearchQuery,
            const CCalGeoValue* aGeoValue );
    void SearchFromMapL( const TDesC& aSearchQuery );
    void ShowOnMapL( const TDesC& aLocationUrl );
    void ShowOnMapL( const CCalGeoValue& aGeoValue );

protected: // from base class CActive
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

protected:// from base class MAknServerAppExitObserver
    void HandleServerAppExit( TInt aReason );

private:
    CESMRLocationPluginImpl();
    void ConstructL();
    void CreateMapViewL();
    void HandleSelectFromMapCompletedL();
    void SelectFromMapL(
                const TDesC& aSearchQuery,
                CPosLandmark* aLandmark );
    void ShowLandmarkL( CPosLandmark* aLandmark );
    CESMRUrlParserPlugin& UrlParserL();

private: // data

    MESMRLocationPluginObserver* iObserver;

    /**
     * Map provider.
     * Own.
     */
    CMnProvider* iProvider;

    /**
     * Map view.
     * Own.
     */
    CMnMapView* iMapView;

    /**
     * UrlparserPlugin
     * Own.
     */
    CESMRUrlParserPlugin* iUrlParser;

    /**
     * RPointerArray for landmarks to perform for example search from maps
     * with search string functionality
     * Own.
     */
    RPointerArray<CPosLandmark> iLandMarks;
    };

#endif //  CESMRLOCATIONPLUGINIMPL_H
