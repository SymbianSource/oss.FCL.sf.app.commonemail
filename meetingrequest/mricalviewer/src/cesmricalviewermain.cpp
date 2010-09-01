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
* Description:  DLL entry point and ECOM implementation proxy.
*
*/


//INCLUDE FILES

// From ESMURUtils
//<cmail>
#include "emailtrace.h"
#include "cesmricalviewer.h"
#include "mruiuids.h"
//</cmail>
#include "cesmricalviewerimpl.h"

// From System
#include <e32base.h>
#include <ecom/implementationproxy.h>

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// ImplementationTable
// Defines instantation methods for ESMRIcalViewer module.
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
            KESMRIcalViewerImplementationUid,
            CESMRIcalViewerImpl::NewL )
    };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns instantation methods for ES MR Utils module.
// @param aTableCount On return contains the number of instantation methods.
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount )
    {
    aTableCount =
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// EOF

