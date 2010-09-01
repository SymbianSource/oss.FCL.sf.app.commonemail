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
#ifndef M_MRBCPLUGINCOMMAND_H
#define M_MRBCPLUGINCOMMAND_H

#include <e32base.h>
#include <calencommandhandler.h>

/**
 * MMRBCPluginCommand defines interface for BC plug-in commands
 */
class MMRBCPluginCommand
	{
public: // Destruction
	virtual ~MMRBCPluginCommand() { }
	
public: // Interface
	/**
	 * Executes the command.
	 * @param aCommand Command to be executed
	 */
	virtual void ExecuteCommandL(
			const TCalenCommand& aCommand ) = 0;	
	};

#endif // M_MRBCPLUGINCOMMAND_H

// EOF
