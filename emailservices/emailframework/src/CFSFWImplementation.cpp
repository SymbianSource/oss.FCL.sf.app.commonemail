/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFSFWImplementation.
*
*/

#include <nmcommonheaders.h>
#include "emailtrace.h"
#include "CFSFWImplementation.h"
//<cmail>
#include "CFSMailPlugin.h"
//</cmail>
#include "CFSMailPluginManager.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSFWImplementation::CFSFWImplementation
// -----------------------------------------------------------------------------
CFSFWImplementation::CFSFWImplementation()
{
    FUNC_LOG;
	iPluginManager = NULL;
}

// -----------------------------------------------------------------------------
// CFSFWImplementation::~CFSFWImplementation
// -----------------------------------------------------------------------------
CFSFWImplementation::~CFSFWImplementation()
{
    FUNC_LOG;
	delete iPluginManager;
}
// -----------------------------------------------------------------------------
// CFSFWImplementation::NewLC
// -----------------------------------------------------------------------------
CFSFWImplementation* CFSFWImplementation::NewLC(TInt aConfiguration)
{
    FUNC_LOG;
  CFSFWImplementation* impl = new (ELeave) CFSFWImplementation();
  CleanupStack:: PushL(impl);
  impl->ConstructL(aConfiguration);
  return impl;
} 

// -----------------------------------------------------------------------------
// CFSFWImplementation::NewL
// -----------------------------------------------------------------------------
CFSFWImplementation* CFSFWImplementation::NewL(TInt aConfiguration)
{
    FUNC_LOG;
  CFSFWImplementation* impl =  CFSFWImplementation::NewLC(aConfiguration);
  CleanupStack:: Pop(impl);
  return impl;
}

// -----------------------------------------------------------------------------
// CFSFWImplementation::ConstructL
// -----------------------------------------------------------------------------
void CFSFWImplementation::ConstructL(TInt aConfiguration)
{
    FUNC_LOG;
	// create plugin manager
	iPluginManager = CFSMailPluginManager::NewL(aConfiguration);
}

// -----------------------------------------------------------------------------
// CFSFWImplementation::GetPluginManager
// -----------------------------------------------------------------------------
CFSMailPluginManager& CFSFWImplementation::GetPluginManager( )
{
    FUNC_LOG;
	return *iPluginManager;
}

