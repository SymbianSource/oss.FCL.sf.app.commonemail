/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Static Tls data storage
*
*/


#ifndef RESMRSTATIC_H
#define RESMRSTATIC_H

#include <e32base.h>

class TESMRStaticData;
class CESMRContactMenuHandler;
class CESMRContactManagerHandler;
class CFSMailClient;
class CFSMailBox;

/*
 * Proxy class to access TLS stored data
 */
class RESMRStatic
    {
public:
    /**
     *  C++ constructor
     */
    IMPORT_C RESMRStatic();

    /**
     * C++ destructor
     */
    IMPORT_C ~RESMRStatic();

public: // interface
    /*
     * Retrieves stored data pointer from Tls, or creates new one
     * if called first time. Must be called before using this class.
     */
    IMPORT_C void ConnectL();

    /*
     * Closes this instance of static data handler.
     */
    IMPORT_C void Close();

    /*
     * Returns global instance of CESMRContactManagerHandler
     */
    IMPORT_C CESMRContactManagerHandler& ContactManagerHandlerL();

    /*
     * Returns global instance of CESMRContactMenuHandler
     */
    IMPORT_C CESMRContactMenuHandler& ContactMenuHandlerL();

    /*
     * Returns the current index for tracking  field
     */
    IMPORT_C TInt CurrentFieldIndex();

    /*
     * Sets the current index for tracking  field
     */
    IMPORT_C void SetCurrentFieldIndex(TInt aFieldIndex);
    
    IMPORT_C CFSMailBox& MailBoxL( const TDesC& aAddress );

private: // Internal definitions
    /** 
     * Enumeration for cached data types 
     */
    enum TUsedType
        {
        EContactManagerHandler = 0x01,
        EContactMenuHandler = 0x04,
        EMailBoxHandler = 0x08
        };
    
    CFSMailClient& FSMailClientL( );

private: // Data
    /*
     * Own: Pointer to Tls stored data pointer
     */
    TESMRStaticData* iStaticData;

    /*
     * Own: Used pointer types
     */
    TInt iUsedTypes;
    };

#endif //RESMRSTATIC_H

// EOF

