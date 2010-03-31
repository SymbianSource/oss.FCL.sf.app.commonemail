/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  plugin request handler
*
*/


#ifndef CFSCLIENTAPIREQUESTHANDLER_H
#define CFSCLIENTAPIREQUESTHANDLER_H

//  INCLUDES
#include "cfsmailrequesthandler.h"

// FORWARD DECLARATIONS
class CFSMailRequestObserver;
class CEmailClientApi;
class MEmailClientPluginManager;

/**
 * Client API asynchronous request handler. Used by platform api implementation
 * to access plugins
 *
 */ 
class CFSClientAPIRequestHandler : public CFSMailRequestHandler
	{

public:
  
    /**
     * destructor.
     */
     ~CFSClientAPIRequestHandler();

    /**
     * two based constructor
     *
     */
     static CFSClientAPIRequestHandler* NewL( MEmailClientPluginManager* aPluginManager);
	
    /**
     * returns plugin pointer related to plugin id
     *
     * @param aPluginId plugin id
     * @return plugin pointer
     */
      virtual CFSMailPlugin* GetPluginByUid(TFSMailMsgId aPluginId);

protected:

    /**
     * C++ default constructor.
     */
  	 CFSClientAPIRequestHandler(  MEmailClientPluginManager* aPluginManager );
  	 
    /**
     * two based constructor
     */
     void ConstructL( );


private: // data

    MEmailClientPluginManager* iPluginManager;
    };

#endif

// End of File
