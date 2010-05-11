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
* Description:  Static Tls data storage for plugin extension
 *
*/


#ifndef RESMRPLUGINEXTENSIONSTATIC_H
#define RESMRPLUGINEXTENSIONSTATIC_H

#include <e32base.h>
//<cmail>
#include "cfsmailclient.h"
//</cmail>

class TESMRPluginExtensionData;

/*
 * Proxy class to access TLS stored data
 */
NONSHARABLE_CLASS( RESMRPluginExtensionStatic )
    {
public:
    /**
     *  C++ constructor
     */
    RESMRPluginExtensionStatic();

    /**
     * C++ destructor
     */
    ~RESMRPluginExtensionStatic();

    /*
     * Retrieves stored data pointer from Tls, or creates new one
     * if called first time. Must be called before using this class.
     */
    void ConnectL();

    /*
     * Closes this instance of static data handler.
     */
    void Close();

    /**
     * Returns global instance of default CFSMailBox.
     * @return Reference to CFSMailBox object
     */
    CFSMailBox& DefaultFSMailBoxL();

    /**
     * Returns mail box instance for given email address.
     * Ownership of mail box is transferred to caller.
     * 
     * @param aEmailAddress email address of the desired mail box
     * @return CFSMailBox instance. Ownership is transferred to caller.
     */
    CFSMailBox* MailBoxL( const TDesC& aEmailAddress );

private: // Implementation

    /**
      * Returns global instance of CFSMailClient
      * @return Global reference to CFSMailClient
      */
    CFSMailClient& FSMailClientL();

private: // Data
    /*
     * Pointer to Tls stored data pointer
     */
    TESMRPluginExtensionData* iStaticData;

    /*
     * Used pointer types
     */
    TInt iUsedTypes;
    };

#endif //RESMRPLUGINEXTENSIONSTATIC_H

// EOF
