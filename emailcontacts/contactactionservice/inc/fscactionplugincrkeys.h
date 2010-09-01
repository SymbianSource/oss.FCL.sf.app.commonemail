/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Cenrep keys for plugins actions
*
*/



#ifndef FSCACTIONPLUGINCRKEYS_H
#define FSCACTIONPLUGINCRKEYS_H

// Central Repository Uid
// <cmail> S60 UID update
const TUid KCRUidFsContactActionService = { 0x2001FE0E };
// </cmail> S60 UID update

// Action priority Keys
// <cmail> S60 UID update
const TUint32 KFscCrUidCallGsm              = 0x2001E288;
const TInt KFscActionPriorityCallGsm = 900;
const TUint32 KFscCrUidCallVoip             = 0x2001E289;
const TInt KFscActionPriorityCallVoip = 750;
const TUint32 KFscCrUidCallVideo            = 0x2001E28A;
const TInt KFscActionPriorityCallVideo = 400;
const TUint32 KFscCrUidCallConfNum          = 0x2001F3F5;
const TInt KFscActionPriorityCallConfNum = 900;
const TUint32 KFscCrUidConfWizard           = 0x2001F3F6;
const TUint32 KFscCrUidCallPtt              = 0x2001F3F7;
const TInt KFscActionPriorityCallPtt = 700;
const TUint32 KFscCrUidCreateMsg            = 0x2001F3F8;
const TInt KFscActionPrioritySendMsg = 600;
const TUint32 KFscCrUidCreateAudMsg         = 0x2001F3F9;
const TInt KFscActionPrioritySendAudio = 200;
const TUint32 KFscCrUidSaveAsContact        = 0x2001F3FA;
const TInt KFscActionPrioritySaveAsContact = 950;
const TUint32 KFscCrUidEmail                = 0x2001F3FC;
const TInt KFscActionPrioritySendEmail = 500;
const TUint32 KFscCrUidPostcard             = 0x2001F3FB;
const TInt KFscActionPrioritySendPostcard = 450;
const TUint32 KFscCrUidMeetingRequest       = 0x2001F3FD;

// </cmail> S60 UID update
// <cmail> 
//const TUint32 KFscCrUidShowOnMaps          = 0x2000FDDA;
// </cmail>
#endif      // FSCACTIONPLUGINCRKEYS_H
  
// End of File
