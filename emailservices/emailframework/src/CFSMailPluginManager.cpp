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
* Description:  common mailbox object
*
*/


#include "emailtrace.h"
#include "cfsmailpluginmanager.h"
#include "cfsmailrequestobserver.h"
#include "cfsmailrequesthandler.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailPluginManager::CFSMailPluginManager
// -----------------------------------------------------------------------------
CFSMailPluginManager::CFSMailPluginManager()
{
    FUNC_LOG;

}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::~CFSMailPluginManager
// -----------------------------------------------------------------------------
CFSMailPluginManager::~CFSMailPluginManager()
{
    FUNC_LOG;
	iPluginInfo.ResetAndDestroy();
}


// -----------------------------------------------------------------------------
// CFSMailPluginManager::NewLC
// -----------------------------------------------------------------------------
CFSMailPluginManager* CFSMailPluginManager::NewLC(TInt aConfiguration)
{
    FUNC_LOG;
  CFSMailPluginManager* pluginManager = new (ELeave) CFSMailPluginManager();
  CleanupStack:: PushL(pluginManager);
  pluginManager->ConstructL(aConfiguration);
  return pluginManager;
} 

// -----------------------------------------------------------------------------
// CFSMailPluginManager::NewL
// -----------------------------------------------------------------------------
CFSMailPluginManager* CFSMailPluginManager::NewL(TInt aConfiguration)
{
    FUNC_LOG;
  CFSMailPluginManager* pluginManager =  CFSMailPluginManager::NewLC(aConfiguration);
  CleanupStack:: Pop(pluginManager);
  return pluginManager;
}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::ConstructL
// -----------------------------------------------------------------------------
void CFSMailPluginManager::ConstructL(TInt aConfiguration)
{
    FUNC_LOG;

	// list plugin implementations

	iPluginInfo.Reset();
	CFSMailPlugin::ListImplementationsL(iPluginInfo);
	CFSMailPlugin::Close();
	
	// load plugins
	CFSMailPlugin* plugin = NULL;
    // <cmail>  	
	TInt tempCount = iPluginInfo.Count();
	for(TInt i=0;i<tempCount;i++)
	// </cmail> 	    
	{
		TUid id = iPluginInfo[i]->ImplementationUid();
		if(aConfiguration == EFSLoadPlugins)
		{
			TRAPD(err,plugin = CFSMailPlugin::NewL( id ));
			CFSMailPlugin::Close();		
			if(err == KErrNone && plugin != NULL)
			{
				AddPluginL(id,plugin);
			}
		}	
// <cmail>
		else
		    {		    
		    }
// </cmail>      		
	}

}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::LoadPluginL
// -----------------------------------------------------------------------------
void CFSMailPluginManager::LoadPluginL( TUid /*aPlugin*/ )
{
    FUNC_LOG;

}
	
// -----------------------------------------------------------------------------
// CFSMailPluginManager::GetPluginByIndex
// -----------------------------------------------------------------------------
CFSMailPlugin* CFSMailPluginManager::GetPluginByIndex(TUint aIndex)
	{
    FUNC_LOG;
	return iPluginList[aIndex]->iPlugin;
	}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::GetPluginIdByIndex
// -----------------------------------------------------------------------------
TUid CFSMailPluginManager::GetPluginIdByIndex(TUint aIndex)
	{
    FUNC_LOG;
	return iPluginList[aIndex]->iPluginId;
	}

// -----------------------------------------------------------------------------
// CFSMailPluginManager::GetPluginCount
// -----------------------------------------------------------------------------
TUint CFSMailPluginManager::GetPluginCount( )
	{
    FUNC_LOG;
	return iPluginList.Count();
	}
	


