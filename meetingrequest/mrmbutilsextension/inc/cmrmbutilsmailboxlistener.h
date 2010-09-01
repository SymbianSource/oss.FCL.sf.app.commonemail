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
* Description:  MBUtils settings definition
*
*/

#ifndef C_MRMBUTILSMAILBOXLISTENER_H
#define C_MRMBUTILSMAILBOXLISTENER_H

#include "mfsmaileventobserver.h"
#include "cfsmailcommon.h"

#include <e32base.h>

class CMRMailBoxInfo;
class CFSMailClient;
class MMRMailboxUtilsObserver;

/**
 *  CMRMBUtilsMailboxListener is responsible for listening CMAIL mailbox 
 *  events and maintaining the extension's mailbox list.
 *  CMRMBUtilsMailboxListener also sends notifications via 
 *  MMRMailboxUtilsObserver interface.
 *  
 *  @see MFSMailEventObserver
 */
NONSHARABLE_CLASS( CMRMBUtilsMailboxListener ) : 
		public CBase,
		public MFSMailEventObserver
	{
public:
	/**
	 * Creates and initializes new CMRMBUtilsMailboxListener object.
	 * Ownership is transfered to caller.
	 * @param aMailboxes Reference to mailbox array
	 * @param aMailClient Reference to CMAIL mail client object
	 * @param aMRMailboxObserver Reference to MMRMailboxUtilsObserver
	 */
    static CMRMBUtilsMailboxListener* NewL(
			RPointerArray<CMRMailBoxInfo>& aMailboxes,
			CFSMailClient& aMailClient,
			MMRMailboxUtilsObserver& aMRMailboxObserver );
	
	/**
	 * C++ destructor
	 */
    ~CMRMBUtilsMailboxListener();

private: // From MFSMailEventObserver
	void EventL(
			TFSMailEvent aEvent, 
			TFSMailMsgId aMailbox, 
			TAny* aParam1, 
			TAny* aParam2, 
			TAny* aParam3);
	
private: // Implementation
	CMRMBUtilsMailboxListener(
			RPointerArray<CMRMailBoxInfo>& aMailboxes,
			CFSMailClient& aMailClient,
			MMRMailboxUtilsObserver& aMRMailboxObserver );
	void ConstructL();
	void ListMailboxesL();
	
private: // Data
	/// Ref: Reference to mailbox mailboxes
	RPointerArray<CMRMailBoxInfo>& iMailboxes;
	/// Ref: Reference to CMail mail client object
	CFSMailClient& iMailClient;
    /// Ref: Reference to mailbox observer
    MMRMailboxUtilsObserver& iMRMailboxObserver; 
	};

#endif // C_MRMBUTILSMAILBOXLISTENER_H

// EOF
