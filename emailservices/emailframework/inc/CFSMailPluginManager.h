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


#ifndef CFSMAILPLUGINMANAGER_H
#define CFSMAILPLUGINMANAGER_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ecom.h>

//<cmail>
#include "CFSMailPlugin.h"
//</cmail>
#include "CFSMailRequestHandler.h"

/** plugin configuration at start up */
enum TFsFwConfiguration
	{
		EFSListPlugins = 1,
		EFSLoadPlugins,
		EFSLoadPluginsAndMockup,
		EFSLoadMockupOnly
	};

/**
 *  email framework plugin manager
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailPluginManager ) : public CFSMailRequestHandler
	{

public:

  
    /**
     * destructor
     */  
  	 ~CFSMailPluginManager();

    /**
     * two based constructor
     */
  	 static CFSMailPluginManager* NewL(TInt aConfiguration);
  	
    /**
     * two based constructor
     */
  	 static CFSMailPluginManager* NewLC(TInt aConfiguration);

    /**
     * adds new plugin to plugin list
     *
     */  
	 void AppendPluginL(CFSMailPlugin* aPlugin,CImplementationInformation* aPluginInfo);

    /**
     * returns plugin pointer by plugin table index
     *
     * @return plugin pointer
     */  
	 CFSMailPlugin* GetPluginByIndex(TUint aIndex);

    /**
     * get plugin id by plugin table index
     *
     */  
	 TUid GetPluginIdByIndex(TUint aIndex);
	 	 
    /**
     * returns count of existing plugins
     *
     */  
	 TUint GetPluginCount( );
	 
    /**
     * loads plugin identified by plugin id
     *
     */  
	 void LoadPluginL(TUid aPluginId);

    /**
     * list ids of existing plugins
     *
     */  
	 void ListPlugins( RArray<TUid>& aPluginList );
	 
    /**
     * lists plugin pointers & plugin info
     *
     */  
	 void ListImplementationsL( RPointerArray<CFSMailPlugin>& aImplementations,
	                            RPointerArray<CImplementationInformation>& aInfo );
									 
protected:

    /**
     * C++ default constructor.
     *
     */
 
  	 CFSMailPluginManager();

    /**
     * ConstructL
     */
	void ConstructL(TInt aConfiguration);
	
private: // data

    /** plugin information table */  
	 RPointerArray<CImplementationInformation> 	iPluginInfo;
	 
    };
    

#endif  // CFSMAILPLUGINMANAGER_H

// End of File
