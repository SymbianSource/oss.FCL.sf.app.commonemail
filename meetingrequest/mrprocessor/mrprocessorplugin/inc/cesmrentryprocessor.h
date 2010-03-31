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
* Description:  ESMR mr entry processor
*
*/

#ifndef C_ESMRENTRYPROCESSOR_H
#define C_ESMRENTRYPROCESSOR_H

#include <e32base.h>
#include "cmrentryprocessor.h"

class MESMRCalDbMgr;
class CMRMailboxUtils;
class CESMRFsMailboxUtils;
class CESMRMeetingRequestEntry;

/**
* CESMREntryProcessor's main responsibility is to
* perform entry processing and evaluation in initialization phase.
*
* @lib esmrentry.lib
*/
NONSHARABLE_CLASS(CESMREntryProcessor) :
        public CMREntryProcessor
    {
public: // Construction and destruction
    /**
     * Creates new CESMREntryProcessor object. Ownership
     * is transferred to caller.
     * @param aDbMgr Reference to calendar DB manager.
     * @return Pointer to created CESMREntryProcessor object,
     */
    static CESMREntryProcessor* NewL(
            TAny* aDbMgr );    
    
    /**
     * C++ destructor
     */
    ~CESMREntryProcessor();

public: // From MESMREntryProcessor
    const TESMRScenarioData& ScenarioData() const;
    TBool ContainsProcessedEntry() const;   
    void ProcessL(
            const MAgnEntryUi::TAgnEntryUiInParams* aParams,
            RPointerArray<CCalEntry>* aEntries );
    void ProcessL(
            const MAgnEntryUi::TAgnEntryUiInParams* aParams,
            CCalEntry& aEntry,
            TBool aSetDefaultValuesToEntry,
            const TBool aTypeChanging = EFalse );
    void ProcessL(
            RPointerArray<CCalEntry>* aEntries );
    void ProcessL(
            CCalInstance& aInstance );
    void SwitchProcessorToModeL(
            TMRProcessorMode aMode );
    MESMRCalEntry& ESMREntryL();
    void ResetL();
    void ProcessOutputParametersL(
            MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
            TESMRCommand aCommand );

private: // Implementaton

    static CESMREntryProcessor* CreateL(
            MESMRCalDbMgr& aDbMgr ); 
    
    CESMREntryProcessor(
            MESMRCalDbMgr& aDbMgr);
    void ConstructL();
    void SetPhoneOwnerL( CCalEntry& aEntry,
                         CCalEntry& aBaseEntry );
    void CheckViewModeL();
    TBool IsDataOk() const;
    void CreateESMREntryL( CCalEntry& aEntry, TBool aSetDefaultValuesToEntry );
    void CheckOrganizerL( CCalEntry& aEntry );
    void SetPhoneOwnerToOwnEntryL();
    void SwitchToForwardL();
    void SwitchToEditL();
    void SwitchToViewL();
    void SwitchToTrackL();
     
private: // Data
    /**
    * Scenario data
    * Own.
    */
    TESMRScenarioData iScenData;

    /**
    * ES MR Entry
    * Own.
    */    
    CESMRMeetingRequestEntry* iESMREntry;
    
    /**
    * Reference to MR Mailbox Utilities API
    * Own.
    */
    CMRMailboxUtils* iMRMailboxUtils;

    /**
    * Reference to calendar DB manager
    * Not own.
    */
    MESMRCalDbMgr& iDbMgr;

    /**
    * Entries to be processed
    * Not own.
    */
    RPointerArray<CCalEntry>* iMREntries;

    /**
     * Entry to be processed
     * Not own.
     */
    CCalEntry* iCalEntry;
    
    /**
    * Input parameters from calling application
    * Not own.
    */
    const MAgnEntryUi::TAgnEntryUiInParams* iParams;
    
    /**
     * Own: Mailbox utils class.
     */
    CESMRFsMailboxUtils* iMbUtils;
    };

#endif // CESMRENTRYPROCESSOR_H
