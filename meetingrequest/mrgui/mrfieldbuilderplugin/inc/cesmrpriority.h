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
* Description:  ESMR priority implementation
 *
*/


#ifndef CESMRPRIORITY_H
#define CESMRPRIORITY_H

#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

class TResourceReader;

NONSHARABLE_CLASS( CESMRPriority ): public CBase
    {
public://Construction and Destruction
    /**
     * Creates new CESMRPriority object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    CESMRPriority();

    /**
     * C++ Destructor.
     */
    ~CESMRPriority();

public://Interface
    /**
     * Loads values from resources.
     *
     * @param aReader resource reader object
     */
    void ConstructFromResourceL( TResourceReader& aReader );

    /**
     * Returns priority as text.
     *
     * @return priority as text
     */
    const TDesC& Text();

    /**
     * Returns priority id value.
     *
     * @return priority id value
     */
    TFSCalenMRPriority Id();

private://Data
    /**
     * Own. Priority text value.
     */
    HBufC* iText;

    /**
     * Priority id.
     */
    TInt iId;
    };

#endif  // CESMRPRIORITY_H

