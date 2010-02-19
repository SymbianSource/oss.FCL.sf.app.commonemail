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
* Description:  ESMR FS mailbox utilities definition
*
*/

#ifndef CESMRFSMAILBOXUTILS_H
#define CESMRFSMAILBOXUTILS_H

#include <e32base.h>
#include <cmrmailboxutils.h>
#include "cfsmailcommon.h"
#include "esmrdef.h"

class CFSMailClient;
class CFSMailMessage;
class CFSMailBox;
class CCalUser;


/**
 * Utility class for FS mailbox
 */
NONSHARABLE_CLASS( CESMRFsMailboxUtils ): public CBase
    {

public: // Constructors and destructors

    /**
    * Constructs CESMRFsMailboxUtils object.
    *
    * @param aMailboxUtils Reference to S60 mailbox utils
    * @return instantiated object, ownership transferred
    */
    static CESMRFsMailboxUtils* NewL(
            CMRMailboxUtils& aMailboxUtils );

    /**
    * C++ Destructor.
    */
    ~CESMRFsMailboxUtils();

public: // New functions

    /**
    * If phone owner field hasn't been set so far, this method compares
    * organizer field to the mailboxes in the device, and if no match
    * was found compares each attendee to the mailboxes. If match was found,
    * phone owner field is set, otherwise KErrNotFound is returned.
    * If aPrimaryBox is given, then that is used in matching before any
    * other mailbox. If not given, then default MR mailbox will have
    * the highest presedence followed by other MR-capable mailboxes.
    *
    * @param aCalEntry entry to modify
    * @param aPrimaryBox primary mailbox for matching, if applicable
    * @return KErrNone or a system wide error code
    */
    TInt SetPhoneOwnerL(
        CCalEntry& aCalEntry,
        TMsvId aPrimaryBox = KMsvNullIndexEntryId );

    /**
    * If phone owner field hasn't been set so far, this method compares
    * mailboxowner's email address to organizer field and if no match
    * was found compares each attendee to the mailboxowner's email address. 
    * If match was found, phone owner field is set, otherwise KErrNotFound 
    * is returned.
    *
    * @param aCalEntry entry to modify
    * @param aPrimaryBox primary mailbox for matching, if applicable
    * @return KErrNone or a system wide error code
    */
    TInt SetPhoneOwnerL(
        CCalEntry& aCalEntry,
        CFSMailClient& aMailClient,
        CFSMailMessage& aMailMessage );    
    
    /**
     * Resolves the plugin to be used to current calendar entry.
     * Method retrieves phone owner from calendar entry and loops all
     * FS Email mailboxes. 
     * @param aEntry Reference to calendar entry.
     * @return Plugin ID for FS Email mailbox
     */
    TESMRMailPlugin FSEmailPluginForEntryL( const CCalEntry& aEntry );

    /**
     * Resolves the mail box for current calendar entry.
     * Method retrieves phone owner from calendar entry and loops all
     * FS Email mailboxes. 
     * @param aEntry Reference to calendar entry.
     * @return ID of the FS Email mailbox
     */
    TFSMailMsgId FSEmailMailBoxForEntryL( const CCalEntry& aEntry );
    
private: // Implementation
    CESMRFsMailboxUtils(
            CMRMailboxUtils& aMailboxUtils );
    void ConstructL();
    CCalUser* PhoneOwnerL(
            const CCalEntry& aCalEntry,
            const RPointerArray<CFSMailBox>& aMailBoxes,
            TInt& aMatchIndex );
    TBool IsPhoneOwnerL(
            const CCalUser& aUser,
            const RPointerArray<CFSMailBox>& aMailBoxes,
            TInt& aMatchIndex );
    
    CFSMailClient& MailClientL();

private: // Data
    /// Ref: Reference to S60 mailbox utilities
    CMRMailboxUtils& iMRMailboxUtils;
    /// Own: Mail client object
    CFSMailClient* iMailClient;
    };

#endif // CESMRFSMAILBOXUTILS_H

// EOF

