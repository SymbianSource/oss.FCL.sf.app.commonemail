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
* Description: Interface class for controlling absolute alarm
*
*/

#ifndef MMRABSOLUTEALARMCONTROLLER_H
#define MMRABSOLUTEALARMCONTROLLER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATIONS
class MMRAbsoluteAlarmController
    {
    public: // Interface
        
        /**
         * Validator sets absolute alarm 'On' in CMRAlarmOnOffField
         * by calling this function.
         */
        virtual void SetAbsoluteAlarmOn() = 0;
        
        /**
         * Validator sets absolute alarm 'Off' in CMRAlarmOnOffField
         * by calling this function.
         */
        virtual void SetAbsoluteAlarmOff() = 0;

    };
#endif // MMRABSOLUTEALARMCONTROLLER_H

// End of file
