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
* Description:  ESMR text item implementation, used to map text resource to id
*
*/


#ifndef CESMRTEXTITEM_H
#define CESMRTEXTITEM_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32std.h>

class TResourceReader;

/**
 * Wrapper class for descriptors for easier usage in fields
 */
NONSHARABLE_CLASS( CESMRTextItem ) : public CBase
    {
public:
    /**
     * Constructor.
     */
    CESMRTextItem();

    /**
     * Destructor.
     */
    ~CESMRTextItem();

    /**
     * Loads values from resources.
     * @param aReader resource reader
     */
    void ConstructFromResourceL( TResourceReader& aReader );

public:
    /**
     * Returns text.
     * @return text
     */
    TDesC& TextL();

    /**
     * Returns id value.
     * @return id value
     */
    TInt Id();

private:
    /**
     * Own. Text.
     */
    HBufC* iText;

    /**
     * Id value.
     */
    TInt iId;
    };

#endif  // CESMRTEXTITEM_H

