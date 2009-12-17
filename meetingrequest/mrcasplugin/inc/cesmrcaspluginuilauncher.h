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
* Description: This file defines class CESMRCasPluginUiLauncher.
 *
*/


#ifndef CESMRCASPLUGINUILAUNCHER_H
#define CESMRCASPLUGINUILAUNCHER_H

#include <e32base.h>
#include <MAgnEntryUi.h>
#include <badesca.h>

class CCalSession;
class CCalenInterimUtils2;
class CMRMailboxUtils;
class TFscContactActionPluginParams;
class MVPbkStoreContact;
class CCalUser;
class MESMRCasPluginUiLauncherObserver;


/**
 * CESMRCasPluginUiLauncher is responsible for launching MR editor.
 */
NONSHARABLE_CLASS( CESMRCasPluginUiLauncher ) :
    public CBase,
    public MAgnEntryUiCallback
    {
public:
    /**
     * Creates and initializes new CESMRCasPluginUiLauncher object.
     * Ownership is transferred to caller.
     * @param aCalSession Reference to calendar session.
     * @param aParams Reference to contact action parameters
     * @return Pointer to CESMRCasPluginUiLauncher
     */
    static CESMRCasPluginUiLauncher* NewL(
            CCalSession& aCalSession,
            CCalenInterimUtils2& aCalUtils,
            CMRMailboxUtils& aMBUtils,
            const TFscContactActionPluginParams& aParams,
            MESMRCasPluginUiLauncherObserver& aExecuteObserver );

    /**
     * C++ destructor.
     */
    ~CESMRCasPluginUiLauncher();

public: // Interface
    /**
     * Launches UI. Contacts are added to required recipients.
     * @param aAttendees Reference to attendee email address array
     */
    void LaunchMeetingRequestViewL(
            const RPointerArray<CCalUser>& aAttendees );

private: // From MAgnEntryUiCallback
    void ProcessCommandL(TInt aCommandId);
    TInt ProcessCommandWithResultL( TInt aCommandId );
    TBool IsCommandAvailable( TInt aCommandId );

private: // Implementation
    CESMRCasPluginUiLauncher(
            CCalSession& aCalSession,
            CCalenInterimUtils2& aCalUtils,
            CMRMailboxUtils& aMBUtils,
            const TFscContactActionPluginParams& aParams,
            MESMRCasPluginUiLauncherObserver& aExecuteObserver );
    void ConstructL();
    void AddOrganizerL();
    void AddAttendeesL( const RPointerArray<CCalUser>& aAttendees );
    void SetStartAndEndTimeL();

private: // data
    /// Own calendar entry
    CCalEntry* iCalEntry;
    /// Own: MR Input parameters
    MAgnEntryUi::TAgnEntryUiInParams iInParams;
    /// Own: MR Output parameters
    MAgnEntryUi::TAgnEntryUiOutParams iOutParams;
    /// Own: MTM UID
    HBufC* iMtmUid;
    /// Ref: Calendar session
    CCalSession& iCalSession;
    /// Ref: contact action params
    const TFscContactActionPluginParams& iParams;
    /// Ref: Reference to calendar utils
    CCalenInterimUtils2& iCalUtils;
    /// Reference to mailbox utils
    CMRMailboxUtils& iMBUtils;
    /// Ref: execute observer
    MESMRCasPluginUiLauncherObserver& iExecuteObserver;
    };

#endif // CESMRCASPLUGINUILAUNCHER_H

// EOF
