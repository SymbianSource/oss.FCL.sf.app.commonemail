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
* Description:  This file contains Central Repository key values.
 *
*/


#ifndef FSCENREPKEYS_H
#define FSCENREPKEYS_H

#include <e32cmn.h>  //TUid
#include <e32def.h>

// <cmail> S60 UID update
const TUid KCRUidFSGenericUI = {0x2001E280};
// </cmail> S60 UID update

const TUint32 KFSGenericUIDefScrollSpeed       = {0x00000001};
const TInt KFsListDefaultScrollSpeed           = 400;

const TUint32 KFSGenericUIFadeIn               = {0x00000002};
const TInt KFsListDefaultFadeInTime            = 1000;

const TUint32 KFSGenericUIFadeOut              = {0x00000003};
const TInt KFsListDefaultFadeOutTime           = 1000;

const TUint32 KFSGenericUISlideInTime          = {0x00000004};
const TInt KFsListDefaultSlideInTime           = 1000;

const TUint32 KFSGenericUISlideOutTime         = {0x00000005};
const TInt KFsListDefaultSlideOutTime          = 1000;

const TUint32 KFSGenericUISlideInDir           = {0x00000006};
const TInt KFsListDefaultSlideInDir            = 0;

const TUint32 KFSGenericUISlideOutDir          = {0x00000007};
const TInt KFsListDefaultSlideOutDir           = 0;

const TUint32 KFSGenericUIListExpTimeOut       = {0x00000008};
const TInt KFsListDefaultListExpTimeOut        = 400;

const TUint32 KFSGenericUIListLooping          = {0x00000009};
const TInt KFsListDefaultListLooping           = 0;

const TUint32 KFSGenericUITextViewScrollOffset = {0x0000000A};
const TInt KFsListDefaultScrollOffset          = 30;

const TUint32 KFSGenericUITextViewScrollDelay  = {0x0000000B};
const TInt KFsListDefaultScrollDelay           = 1000;

const TUint32 KFSGenericUITextViewEmbedDelay   = {0x0000000C};
const TInt KFsListDefaultEmbedDelay            = 1000;

const TUint32 KFSGenericUITextViewCursorDelay  = {0x0000000D};
const TInt KFsListDefaultCursorDelay           = 300;

#endif //FSCENREPKEYS_H
