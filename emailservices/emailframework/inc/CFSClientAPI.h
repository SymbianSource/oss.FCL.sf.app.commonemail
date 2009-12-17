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
* Description: Utility for email client (platform) api implementation to 
*              instantiate request handler in email framework. The req
*              handler is responsible for setting TLS.
*
*/

#ifndef CFSCLIENTAPI_H_
#define CFSCLIENTAPI_H_

#include <e32base.h>

class CFSClientAPIRequestHandler;
class MEmailClientPluginManager;

/**
* Utility for creating request handler from outside the library.
*/
class CFSClientAPI : public CBase
    {
public:
    
    // destructor
    virtual ~CFSClientAPI();
    
    /**
     * Two-phased constructor.
     *
     * @param aConfiguration loaded plugins
     */
     IMPORT_C static CFSClientAPI* NewL(
        MEmailClientPluginManager* aPluginManager );

protected:
    
    // c++ constructor
    CFSClientAPI();
    
    /**
     * ConstructL
     */
     void ConstructL(MEmailClientPluginManager* aPluginManager);
    
     
private:
    
    CFSClientAPIRequestHandler* iAPIRequestHandler;         
    };

#endif /* CFSCLIENTAPI_H_ */
