/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MBUtils ECOM definition
*
*/
#ifndef C_MRBCPLUGINENTRYLOADER_H
#define C_MRBCPLUGINENTRYLOADER_H

#include <e32base.h>

class CCalEntry;
class MCalenServices;

/**
  * CMRBcPluginEntryLoader is responsible for loafding the correct
  * entry from the calendar database for handling. 
  */
NONSHARABLE_CLASS( CMRBCPluginEntryLoader ) : public CBase
    {
public: // Construction and destruction
    
    /**
     * Creates and initalizes new CMRBcPluginEntryLoader object.
     * @param aServices Reference to calendar services
     * @return pointer to new CMRBcPluginEntryLoader object 
     */
    static CMRBCPluginEntryLoader* NewL( MCalenServices& aServices );
    
    /**
     * C++ destructor
     */
    ~CMRBCPluginEntryLoader();

public: // Interface
    
    /**
     * Updates the entry from the database.
     */
    void UpdateEntryFromDatabaseL();
    
    /**
     * Fetches the reference to entry
     * @return Reference to entry
     */
    CCalEntry& Entry();
    
private: // Implementation
    CMRBCPluginEntryLoader( MCalenServices& aServices );
    void ConstructL();
    
private:
    /// Ref: Reference to calendar services
    MCalenServices& iServices;
    /// Own: Calendar entry
    CCalEntry* iEntry;
    };

#endif

// EOF
