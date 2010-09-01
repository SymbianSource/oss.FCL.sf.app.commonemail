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
#ifndef TMRBCPLUGINCOMMAND_H
#define TMRBCPLUGINCOMMAND_H

#include <e32def.h>

/**
 * Defines BC Plusgin command type
 */
class TMRBCPluginCommand
    {
public: // Construction    
    /**
     * Default constructor.
     */
    TMRBCPluginCommand();
    
    /**
     * C++ constructor.
     * @param aCommand Command type
     * @param aCheckEntryType Defines whether entry type needs to be checked 
     */
    TMRBCPluginCommand( 
            TInt aCommand, 
            TInt aCheckEntryType );

public: // Interface    
    
    /**
     * Returns command
     * @return Command info
     */
    TInt Command() const;
    
    /**
     * Checks whether entry type needs to be checked
     * @return ETrue if entry type needs to be checked
     */
    TBool CheckEntryType() const;
  
    
public:// Data
    TInt iCommand;
    TInt iCheckEntryType;
    };

#endif // TMRBCPLUGINCOMMAND_H

// EOF
