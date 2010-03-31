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
#ifndef C_MRBCPLUGINCREATENEWMRCMD_H
#define C_MRBCPLUGINCREATENEWMRCMD_H

#include "mmrbcplugincommand.h"
#include <e32base.h>
#include <magnentryui.h>

class MCalenServices;
class CESMRViewerController;
class CCalEntry;

/**
 * 
 */
NONSHARABLE_CLASS( CMRBCPluginCreateNewMRCmd ) : 
		public CBase,
		public MMRBCPluginCommand,
		public MAgnEntryUiCallback
	{
public: // Construction and destruction
	
	/**
	 * Creates new CMRBCPluginCreateNewMRCmd command
	 * @param aServices Reference to calednar services
	 */
	static CMRBCPluginCreateNewMRCmd* NewL(
			MCalenServices& aServices );
	
	/**
	 * C++ destructor
	 */
	~CMRBCPluginCreateNewMRCmd();

public: // From base class

	/**
	 * Executes the command.
	 * @param aCommand Command to be executed
	 */
	void ExecuteCommandL(
			const TCalenCommand& aCommand );
	
private: // From base class MAgnEntryUiCallback
	void ProcessCommandL(TInt aCommandId);
	TInt ProcessCommandWithResultL( TInt aCommandId );
	
private: // Implementation
	CMRBCPluginCreateNewMRCmd(
			MCalenServices& aServices );
	void ConstructL();
	void LaunchMREditorL( const TCalenCommand& aCommand );
	void CreateEntryL( const TCalenCommand& aCommand );
	void AddOrganizerL();
	void IssueCommandL( MAgnEntryUi::TAgnEntryUiOutParams aOutParams );
	
private: // Data
	/// Ref: Reference to calendar services
	MCalenServices& iServices;
	/// Own: Editor component
	CESMRViewerController* iEditor;
	/// Own: Entry to be created
	CCalEntry* iEntry;
	}; 

#endif // C_MRBCPLUGINCREATENEWMRCMD_H

// EOF
