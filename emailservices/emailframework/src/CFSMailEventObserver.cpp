/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email Framework Client 
*
*/


#include <nmcommonheaders.h>
#include "emailtrace.h"
#include "CFSClientRequestObserver.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSClientRequestObserver::CFSClientRequestObserver
// -----------------------------------------------------------------------------
CFSClientRequestObserver::CFSClientRequestObserver()
{
    FUNC_LOG;

}

// -----------------------------------------------------------------------------
// CFSClientRequestObserver::~CFSClientRequestObserver
// -----------------------------------------------------------------------------
CFSClientRequestObserver::~CFSClientRequestObserver()
{
    FUNC_LOG;

}
// -----------------------------------------------------------------------------
// CFSClientRequestObserver::NewLC
// -----------------------------------------------------------------------------
CFSClientRequestObserver* CFSClientRequestObserver::NewLC()
{
    FUNC_LOG;
  CFSClientRequestObserver* obs = new (ELeave) CFSClientRequestObserver();
  CleanupStack:: PushL(obs);
  obs->ConstructL();
  return obs;
} 

// -----------------------------------------------------------------------------
// CFSClientRequestObserver::NewL
// -----------------------------------------------------------------------------
CFSClientRequestObserver* CFSClientRequestObserver::NewL()
{
    FUNC_LOG;
  CFSClientRequestObserver* obs =  CFSClientRequestObserver::NewLC();
  CleanupStack:: Pop(obs);
  return obs;
}

// -----------------------------------------------------------------------------
// CFSClientRequestObserver::ConstructL
// -----------------------------------------------------------------------------
void CFSClientRequestObserver::ConstructL()
{
    FUNC_LOG;

}

// -----------------------------------------------------------------------------
// CFSClientRequestObserver::ListMessagesResponse
// -----------------------------------------------------------------------------
void CFSClientRequestObserver::ListMessagesResponse( RPointerArray<CFSMailMessage>& aMessages )
{
    FUNC_LOG;
	
}


