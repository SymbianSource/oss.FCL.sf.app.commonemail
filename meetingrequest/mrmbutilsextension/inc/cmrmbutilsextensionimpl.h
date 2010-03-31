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
#ifndef C_MRMBUTILSEXTENSION_H
#define C_MRMBUTILSEXTENSION_H

class CFSMailClient;
class MMRMBUtilsSettings;
class CMRMBUtilsSettings;
class CMRMBUtilsMailboxListener;

#include <cmrmbutilsmbinfohandler.h>

/**
 * CMRMbUtilsExtensionImpl implements CMRMBUtilsMBInfoHandler / 
 * MMRMBUtilsMBInfoHandler ECOM interface.
 */
class CMRMbUtilsExtensionImpl : 
        public CMRMBUtilsMBInfoHandler
    {
public:
    /**
     * Creates and initializes new CMRMbUtilsExtensionImpl object.
     * Ownership is transferred to caller. 
     * Passed parameter is assumed to be reference to MMRMailboxUtilsObserver
     * object,
     * @param aMRMailboxObserver Reference to aMRMailboxObserver object
     */
    static CMRMbUtilsExtensionImpl* NewL(
            TAny* aMRMailboxObserver );

    /**
     * C++ destructor
     */
    ~CMRMbUtilsExtensionImpl();

public: // From MMRMBUtilsMBInfoHandler
    MMRMBUtilsSettings* SettingsMgr();
    const RPointerArray<CMRMailBoxInfo>& MRMailBoxes() const;
    const CMRMailBoxInfo* MRMailBoxInfoL( 
            TInt aEntryId ) const;
    void ListMRMailBoxesL(
            RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes );
    TAny* ExtensionL( TUid aExtensionUid );
    
private: // Implementation
    CMRMbUtilsExtensionImpl(
            MMRMailboxUtilsObserver& aObserver );
    void ConstructL();
    
private:
    /// Own: Pointer to mailclient object
    CFSMailClient* iMailClient;
    /// Own: List of MR capable CMail mailboxes
    RPointerArray<CMRMailBoxInfo> iMailboxes;
    /// Own: Pointer to mailbox listener object
    CMRMBUtilsMailboxListener* iMailboxListener;
    /// Own: Pointer to settings item 
    CMRMBUtilsSettings* iSettings;
    /// Ref: Reference to mailbox utilities observer
    MMRMailboxUtilsObserver& iMRMailboxObserver;
    };

#endif // C_MRMBUTILSEXTENSION_H

// EOF
