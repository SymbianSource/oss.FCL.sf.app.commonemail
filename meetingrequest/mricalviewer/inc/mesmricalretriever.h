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
* Description:  ESMR calendar entry retriever interface definition
*
*/

#ifndef MESMRICALRETRIEVER_H
#define MESMRICALRETRIEVER_H

class CCalEntry;
class CFSMailMessage;
class CFSMailClient;
#include "tesmrinputparams.h"

/**
 * MESMRIcalRetriever defines interface for fetching calendar entry
 * information from FS email message.
 *
 * @lib esmricalviewer.lib
 */
class MESMRIcalRetriever
    {
public: // Destruction
    /*
     * Virtual C++ destructor
     */
    virtual ~MESMRIcalRetriever() {}

public: // Interface

    /**
     * Retrieves calendar entry from email message and stores it to
     * input parameters structure.
     * calendar entry object is transferred to caller.
     * @param aInputParameters Reference to ESMR startup parameters
     * @error ETrue if input parameters were filled, EFalse otherwise.
     */
    virtual TBool RetrieveCalendarEntryLC(
            TESMRInputParams& aInputParameters ) = 0;
    };

#endif // MESMRICALRETRIEVER_H
