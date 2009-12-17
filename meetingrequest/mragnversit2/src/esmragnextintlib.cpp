/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the import functionality of CESMRAgnVersit2
*     
*
*/


#include "emailtrace.h"
#include "cesmragnversit2.h"			//CESMRAgnVersit2
#include <ecom/implementationproxy.h>	//TImplementationProxy
//<cmail>
#include "mruiuids.h"

//debug
//</cmail>

/**
 * Defines the pairing required by the ECOM framework to correctly identify the
 * instantiation method pointer for client's request.
 * @publishedAll
 */
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
            KESMRIcalConversionImplementationUid,
            CESMRAgnVersit2::NewL )
    };

/**
 * Returns the TImplementationProxy for AgnVersit2
 * @publishedAll
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    FUNC_LOG;

    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

// End of file

