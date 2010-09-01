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
* Description: This file implements class TIpsSetUtilsException.
*
*/


#include "emailtrace.h"
#include <e32base.h>

#include "ipssetutilsexception.h"

_LIT( EIpsSetUiItemValuePanic, "IpsValueItem" );
_LIT( EIpsSetUiItemTextPanic, "IpsTextItem" );


// ----------------------------------------------------------------------------
// TIpsSetUtilsException::Id2Name()
// ----------------------------------------------------------------------------
//
const TDesC& TIpsSetUtilsException::Id2Name( const TInt aId )
    {
    FUNC_LOG;
    switch ( aId )
        {
        case EItemValue:    return EIpsSetUiItemValuePanic;
        case EItemText:     return EIpsSetUiItemTextPanic;
        }

    return KNullDesC;
    }

// End of File

