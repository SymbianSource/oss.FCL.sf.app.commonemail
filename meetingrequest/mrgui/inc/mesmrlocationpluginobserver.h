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
* Description:  Interface definition for Location integration plug-in observer
*
*/


#ifndef MESMRLOCATIONPLUGINOBSERVER_H
#define MESMRLOCATIONPLUGINOBSERVER_H

#include <e32std.h>

class CPosLandmark;

/**
 *  MESMRLocationPluginObserver defines the interface for location plugin observer
 *  from ESMR GUI.
 *
 */
class MESMRLocationPluginObserver
    {
public:

    /**
     * Notifies the observer that user has selected a location from map.
     *
     * @param aError Result of the selection. If KErrNone aAddress and 
     * 				 aLocationUrl contain valid selection information.
     * @param aLandmark landmark object with information on the location.
     *               Ownership of aLandmark is transferred to observer.
     */
    virtual void SelectFromMapCompleted( TInt aError,
                                         CPosLandmark* aLandmark ) = 0;

protected:

	virtual ~MESMRLocationPluginObserver() {}
    };


#endif // MESMRLOCATIONPLUGINOBSERVER_H
