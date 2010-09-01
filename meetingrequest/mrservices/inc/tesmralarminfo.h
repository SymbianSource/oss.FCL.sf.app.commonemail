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
* Description:  Definition for alarm information object
*
*/


#ifndef TESMRALARMINFO_H
#define TESMRALARMINFO_H

#include <e32def.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

/**
 * TESMRAlarmInfo defines ES MR alarm information object.
 *
 * Alarm information objects are defined in resource files.
 * Resource structure ESMR_ALARM_INFO defines alarm info object
 * in resource files.
 */
class TESMRAlarmInfo
    {
public: // data
    ///Own: Field id
    TESMRAlarmValue iAlarmIdValue;
    ///Own: Field type
    TInt iRelativeAlarmInSeconds;
    };

#endif // TESMRALARMINFO_H

// EOF
