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
* Description:  Definitions for MR output parameters
*
*/


#ifndef TMROUTPUTPARAMS_H
#define TMROUTPUTPARAMS_h

//<cmail>
#include "esmrdef.h"
//</cmail>

/**
 *  TMROutputParams defines MR UI output parameters.
 *
 *  @lib mrcommon.lib
 */
class TMROutputParams
    {
public: // Data

    /**
    * Command to be executed
    */
    TESMRCommand iCommand;

    /**
    * Reserved for future use
    */
    TInt iSpare;
    };

#endif // TMROUTPUTPARAMS_H
