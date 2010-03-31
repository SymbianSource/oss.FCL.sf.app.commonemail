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
* Description:  Definitions for ES MR input parameters
*
*/


#ifndef TESMRINPUTPARAMS_H
#define TESMRINPUTPARAMS_H

//<cmail>
#include "esmrdef.h"
//</cmail>

class CCalEntry;
class MMRInfoObject;
class CFSMailMessage;
class CFSMailClient;
class CESMRAttachmentInfo;

/**
 *  TESMRInputParams defines ES MR UI input parameters.
 *
 *  @lib esmricalviewer.lib
 */
class TESMRInputParams
    {
public: // Data
    /**
    * Calendar entry reference
    * Not own.
    */
    CCalEntry* iCalEntry;

    /**
    * MRINFO object reference
    * Not own
    */
    MMRInfoObject* iMRInfoObject;

    /**
    * Reference to FS Email message object
    * Not own.
    */
    CFSMailMessage* iMailMessage;

    /**
    * Reference to FS Mail client object
    * Not own.
    */
    CFSMailClient* iMailClient;

    /**
    * Command to be executed
    */
    TESMRCommand iCommand;

    /**
    * Reserved for future use
    */
    TInt iSpare;
    };

#endif // TESMRINPUTPARAMS_H
