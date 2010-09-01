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
* Description: Color manager for MRGUI usage.
*
*/

#ifndef NMRGLOBALFEATURESETTINGS_H
#define NMRGLOBALFEATURESETTINGS_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATIONS

// NAMESPACE DECLARATIONS
namespace NMRGlobalFeatureSettings
    {
    enum TKeyboardLayout
        {
        // Values taken from
        // http://developer.symbian.org/main/documentation/reference/s%5E2/
        //  doc_source/reference/9.4-PP/AVKON_Settings_API/
        //  AvkonInternalCRKeys.hGlobals.html
        EUninitialized = -1,
        ENoKeyboard = 0,
        EKeyboardW12Keys = 1,
        EQwerty4x12 = 2,
        EQwerty4x10 = 3,
        EQwerty3x11 = 4,
        EHalfQwerty = 5,
        ECustomQwerty = 6
        };

    /**
     * Checks the device's keyboard layout
     *
     * @return The keyboard layout, ENoKeyboard returned from the touch
     *          only devices.
     */
    IMPORT_C TKeyboardLayout KeyboardType();
    }

#endif // NMRGLOBALFEATURESETTINGS_H
// End of file
