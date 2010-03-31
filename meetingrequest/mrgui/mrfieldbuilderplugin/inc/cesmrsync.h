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
* Description:  ESMR synchronization implementation
 *
*/


#ifndef CESMRSYNC_H
#define CESMRSYNC_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32std.h>

class TResourceReader;

/**
 * Wrapper class for sync related descriptors for easier usage in fields
 */
NONSHARABLE_CLASS( CESMRSync ) : public CBase
    {
public:
    /**
     * Constructor.
     */
    CESMRSync();

    /**
     * Destructor.
     */
    ~CESMRSync();

    /**
     * Loads values from resources.
     * @param aReader resource reader
     */
    void ConstructFromResourceL( TResourceReader& aReader );

public:
    /**
     * Returns synchronization text.
     * @return synchronization text
     */
    TDesC& TextL();

    /**
     * Returns synchronization value.
     * @return synchronization value
     */
    TESMRSyncValue Id();

private:
    /**
     * Own. Synchronization text.
     */
    HBufC* iText;

    /**
     * Synchronization value.
     */
    TInt iId;
    };

#endif  // CESMRSYNC_H

