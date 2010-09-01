/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for Integration MTM
*
*/


#ifndef FSMTMSCONSTANTS_H
#define FSMTMSCONSTANTS_H
//<cmail>
//<cmail>
#include "fsmtmsuids.h"
//</cmail>
//</cmail>

_LIT(KFsMtmUiResourceFile, "fsuimtm.rsc");

_LIT(KFsUiDataMtmUdBitmapFile, "fsuidatamtm.mif");

_LIT(KFsUiDataMtmUdResourceFile, "fsuidatamtm.rsc");

_LIT(KFsMtmDirAndFile,"fsmtms.rsc");


const TUid KUidMsgValTypeFsMtmVal = { KUidMsgTypeFsMtmVal };
const TUint32 KCalendarAppUid  = 0x10005901;

const TInt KOneSecond = 1000000;
const TInt KMAxRetry = 5;

#define KMceAppUid            0x100058C5
#define KCalendarAppUid1      0x10005901
#define KPhoneBookUid         0x101f4CCE

const TUid KAppsToKill[]   = { KMceAppUid, KCalendarAppUid1, KPhoneBookUid };

#endif //FSMTMSCONSTANTS_H