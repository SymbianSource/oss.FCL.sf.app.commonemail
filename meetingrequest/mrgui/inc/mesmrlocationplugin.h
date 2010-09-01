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
* Description:  Interface definition for Location integration plug-in
*
*/


#ifndef MESMRLOCATIONPLUGIN_H
#define MESMRLOCATIONPLUGIN_H


#include <e32def.h>

class CCalGeoValue;

/**
 *  MESMRLocationPlugin defines the interface for accessing location service provider
 *  from ESMR GUI.
 */

class MESMRLocationPlugin
    {

public:

    /**
     * Shows map on the screen and asks user to select a position.
     * The function returns immediately and the selection is provided
     * to the caller via MESMRLocationPluginObserver interface.
     *
     * @param aSearchQuery Text to show as default address
     * @param aLocationUrl URL containing coordinates for maps focus
     */
    virtual void SelectFromMapL(
            const TDesC& aSearchQuery,
            const TDesC& aLocationUrl ) = 0;

    /**
     * Shows map on the screen and asks user to select a position.
     * The function returns immediately and the selection is provided
     * to the caller via MESMRLocationPluginObserver interface.
     *
     * @param aSearchQuery Text to show as default address
     * @param aGeoValue coordinates for maps focus
     */
    virtual void SelectFromMapL(
            const TDesC& aSearchQuery,
            const CCalGeoValue* aGeoValue ) = 0;

    /**
     * Shows map on the screen. User may edit the search string and search
     * the position from the map, but the selection is not provided to the
     * caller
     *
     * @param aSearchQuery Text to show as default address
     */
    virtual void SearchFromMapL( const TDesC& aSearchQuery ) = 0;

    /**
     * Shows map on the screen focused on the given location.
     *
     * @param aLocationUrl URL containing coordinates for maps focus
     */
    virtual void ShowOnMapL( const TDesC& aLocationUrl ) = 0;

    /**
     * Shows map on the screen focused on the given location.
     *
     * @param aGeoValue the coordinates for maps focus
     */
    virtual void ShowOnMapL( const CCalGeoValue& aGeoValue ) = 0;

protected:

    virtual ~MESMRLocationPlugin() {}

    };

#endif // MESMRLOCATIONPLUGIN_H

