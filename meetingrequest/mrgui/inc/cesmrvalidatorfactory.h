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
* Description:  ESMR ui base class
 *
*/


#ifndef CESMRVALIDATORFACTORY_H
#define CESMRVALIDATORFACTORY_H

//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32base.h>
#include "mesmrfieldvalidator.h"

NONSHARABLE_CLASS( CESMRValidatorFactory ): public CBase
    {
public:
    /**
     * Returns validator for given calendar event type.
     *
     * @param aType calendar event type
     */
    static MESMRFieldValidator* CreateValidatorL(
            TESMRCalendarEventType aType );

    };

#endif  // CESMRVALIDATORFACTORY_H

// EOF

