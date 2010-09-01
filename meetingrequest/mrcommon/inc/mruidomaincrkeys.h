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
* Description:  Central repository key definitions for project MRUI
*
*/

#ifndef MRUIDOMAINCRKEYS_H
#define MRUIDOMAINCRKEYS_H

#include "mruiuids.h"

/**
 * MRUI Feature local variation repository uid.
 * The repository contains read-only keys for supported features.
 * Values are on/off settings stored as integer data.
 */
const TUid KCRUidESMRUIFeatures = { KESMRUIFeaturesCRUid };

/**
 * CenRep key for Map and Navigation framework integration feature setting.
 * 
 * Default value: 1 (on)
 */
const TUint32 KESMRUIFeatureMnFwIntegration = 0x1;

/**
 * CenRep key for Contacts integration feature setting.
 * 
 * Default value: 1 (on)
 */
const TUint32 KESMRUIFeatureContactsIntegration = 0x2;

/**
 * CenRep key for Landmakrs application integration feature setting.
 * 
 * Default value: 1 (on)
 */
const TUint32 KESMRUIFeatureLandmarksIntegration = 0x3;

/**
 * CenRep key for Previous locations list feature setting.
 * 
 * Default value: 1 (on)
 */
const TUint32 KESMRUIFeaturePreviousLocationsList = 0x4;

/**
 * CenRep key for disabling all features except meeting request
 * viewer in Cmail application.
 * 
 * Default value: 0 (off) 
 */
const TUint32 KMRUIFeatureMeetingRequestViewerCmailOnly = 0x5;

/**
 * CenRep key for storing default calendar database index for 
 * meeting request entries.
 */
const TUint32 KMRUIDefaultCalDbIndex = 0x6;

#endif // MRUIDOMAINCRKEYS_H
