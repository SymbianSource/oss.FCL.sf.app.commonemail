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
* Description:  Common defines for Contact Action Service
*
*/


#ifndef FSCCONTACTACTIONSERVICEDEFINES_H
#define FSCCONTACTACTIONSERVICEDEFINES_H

#include <e32base.h>
//<cmail>
#include "tfsccontactactionqueryresult.h"
//</cmail>

// FORWARD DECLARATIONS
class MFscContactGroup;
class MVPbkStoreContact;

// TYPEDEFS
typedef RPointerArray< MVPbkStoreContact > RFscStoreContactList;
typedef CArrayFixFlat< TFscContactActionQueryResult > CFscContactActionList;

// CONSTS
// Action types
const TUint64 KFscAtAll = 0xFFFFFFFFFFFFFFFF; 
const TUint64 KFscAtCom = 0xFFFFFFFF;
const TUint64 KFscAtComCall = 0x0FFF;
const TUint64 KFscAtComCallGSM = 0x1;
const TUint64 KFscAtComCallVoip = 0x2; 
const TUint64 KFscAtComCallVideo = 0x4; 
const TUint64 KFscAtComCallConfNum = 0x8; 
const TUint64 KFscAtComCallConfWiz = 0x10; 
const TUint64 KFscAtComCallPoc = 0x20; 
const TUint64 KFscAtComSend = 0xFFF000; 
const TUint64 KFscAtComSendMsg = 0x1000;
const TUint64 KFscAtComSendPostcard = 0x20000;
const TUint64 KFscAtComSendEmail = 0x4000; 
const TUint64 KFscAtComSendAudio = 0x8000; 
const TUint64 KFscAtComSendCalReq = 0x10000; 
const TUint64 KFscAtMan = 0xFFFFFFFF00000000; 
const TUint64 KFscAtManSaveAs = 0x100000000; 

const TInt KFscActionPriorityNotAvailable = -1;
const TInt KFscActionPriorityLow = 250;
const TInt KFscActionPriorityMedium = 500;
const TInt KFscActionPriorityHigh = 750;
const TInt KFscActionPriorityVeryHigh = 1000;

/** Default actions' set is without "Save Contact As" */
const TUint64 KFscAtDefault = 
    KFscAtAll & ~KFscAtManSaveAs; 

/** Default contacts' actions' set doesn't contain "Save Contact As" action 
 *  Left from api compatibility reason */
const TUint64 KFscAtContactsDefault = 
    KFscAtAll & ~KFscAtManSaveAs; 

#endif // FSCCONTACTACTIONSERVICEDEFINES_H
