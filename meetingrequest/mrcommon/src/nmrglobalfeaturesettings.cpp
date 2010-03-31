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

#include "nmrglobalfeaturesettings.h"

#include <avkoninternalcrkeys.h>
#include <centralrepository.h>

using namespace NMRGlobalFeatureSettings;

// ---------------------------------------------------------------------------
// NMRGlobalFeatureSettings::KeyboardType
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyboardLayout NMRGlobalFeatureSettings::KeyboardType()
    {
    CRepository* settings = NULL;
    TRAPD( err, settings = CRepository::NewL( KCRUidAvkon ) );
    TInt keyboardLayout( EUninitialized );

    if ( err == KErrNone )
        {
        settings->Get( KAknKeyBoardLayout, keyboardLayout );
        delete settings;
        }

    return static_cast<NMRGlobalFeatureSettings::TKeyboardLayout>
                ( keyboardLayout );
    }
// End of file
