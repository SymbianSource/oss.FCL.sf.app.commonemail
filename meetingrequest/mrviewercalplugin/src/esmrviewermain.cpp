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
* Description:  Resource file for ECOM registry.
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
// From ESMRVIEWER
#include "emailtrace.h"
#include "cesmrviewersimpl.h"
#include "esmrinternaluid.h"

// From System
#include <e32base.h>
#include <ecom/implementationproxy.h>


// ==================== LOCAL FUNCTIONS ====================

// ----------------------------------------------------------------------------
// ImplementationTable
// Definition for ESMRVIEWER constructor methods
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
	{
    IMPLEMENTATION_PROXY_ENTRY( KESMRViewerCalPluginImplUid3Imap,
                                CESMRViewersImpl::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KESMRViewerCalPluginImplUid3Smtp,
                                CESMRViewersImpl::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KESMRViewerCalPluginImplUid3Fs,
                                CESMRViewersImpl::NewL )                                
	};

/**
* ImplementationGroupProxy
* @param aTableCount
* @returns "TImplementationProxy*"
*/
// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Fetches ESMRVIEWER constructor methods.
// 
// @param aTableCount On returns contains the number of constructor methods.
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
	{
	aTableCount =
	    sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

// End of file

