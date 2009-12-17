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

#ifndef CESMRENTRYPROCESSOR_H
#define CESMRENTRYPROCESSOR_H

#include <e32base.h>
#include <MAgnEntryUi.h>
//<cmail>
#include "esmrcommands.h"
#include "tesmrscenariodata.h"
//</cmail>

class MESMRCalDbMgr;
class CMRMailboxUtils;
class CCalInstance;
class MESMRMeetingRequestEntry;
class CESMRMeetingRequestEntry;
class CESMRFsMailboxUtils;

/**
* CESMREntryProcessor's main responsibility is to
* perform entry processing and evaluation in initialization phase.
*
* @lib esmrentry.lib
*/
NONSHARABLE_CLASS(CESMREntryProcessor) :
        public CBase
    {
public: // Construction and destruction
    /**
     * Creates new CESMREntryProcessor object. Ownership
     * is transferred to caller.
     * @param aMRMailboxUtils Refernce to MR Mailbox Utilities API.
     * @param aDbMgr Reference to calendar DB manager.
     * @return Pointer to created CESMREntryProcessor object,
     */
     IMPORT_C static CESMREntryProcessor* NewL(
        CMRMailboxUtils& aMRMailboxUtils,
        MESMRCalDbMgr& aDbMgr );

    /**
     * C++ destructor
     */
    IMPORT_C ~CESMREntryProcessor();

public: // Interface
    /**
     * Fetches scenario data.
     *
     * @return scenario data.
     */
    inline const TESMRScenarioData& ScenarioData() const;

    /**
     * Tests, whether processor contains entry, which it has processed
     * or not.
     *
     * @return ETrue, if contains processed entry.
     */
    inline TBool ContainsProcessedEntry() const;

    /**
     * Fetches the extension UID for ES MR entry processor.
     *
     * @return MRUI processor extension UID.
     */
    IMPORT_C static TUid ExtensionUid();

    /**
     * Process entries and constructs scenario data.
     *
     * @param aParam AGN Entry UI input parameters.
     * @param aEntries Calendar entries to be processed.
     *        Ownership is not transferred.
     */
    IMPORT_C void ProcessL(
            const MAgnEntryUi::TAgnEntryUiInParams* aParams,
            RPointerArray<CCalEntry>* aEntries );

    /**
     * Process entries. Scenario data is not constructed.
     *
     * @param aEntries Calendar entries to be processed.
     *        Ownership is not transferred.
     */
    IMPORT_C void ProcessL(
            RPointerArray<CCalEntry>* aEntries );

    /**
     * Processes calendar instance. All operations affects only the
     * processed instance.
     *
     * @param aInstance Reference to calendar instance.
     */
    IMPORT_C void ProcessL(
            CCalInstance& aInstance );

    /**
     * Switches processor for forwarding mode.
     * @error KErrNotReady no entries has been processed yet.
     */
    IMPORT_C void SwitchToForwardL();

    /**
     * Switches processor to edit mode.
     * @error KErrNotReady no entries has been processed yet.
     */
    IMPORT_C void SwitchToEditL();

    /**
     * Switches processor to view mode.
     * @error KErrNotReady no entries has been processed yet.
     */
    IMPORT_C void SwitchToViewL();

    /**
     * Switches processor to Tracking mode.
     * @error KErrNotReady no entries has been processed yet.
     */
     IMPORT_C void SwitchToTrackL();

    /**
     * Fetches ES MR Entry. Methods panics, if entries has not
     * been processed before calling this method.
     *
     * @return Reference to CESMRMeetingRequestEntry object.
     */
    IMPORT_C MESMRMeetingRequestEntry& ESMREntryL();

    /**
     * Resets the entry processor. After calling this method the
     * processor is in the same state than after creation.
     */
    IMPORT_C void ResetL();

    /**
     * Process output parameters. Processing is done according to command
     * passed as parameter.
     *
     * @param aOutputParams Reference to output parameters.
     * @param aCommand Command for which output parameters are processed.
     */
    IMPORT_C void ProcessOutputParametersL(
            MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
            TESMRCommand aCommand );

private: // Implementaton

    CESMREntryProcessor(
            CMRMailboxUtils& aMRMailboxUtils,
            MESMRCalDbMgr& aDbMgr );
    void ConstructL();
    void SetPhoneOwnerL();
    void CheckViewModeL();
    TBool IsDataOk() const;
    void CreateESMREntryL();
    void CheckOrganizerL();
    void SetPhoneOwnerToOwnEntryL();

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
    * Not own.
    */
    CMRMailboxUtils& iMRMailboxUtils;

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
    * Input parameters from calling application
    * Not own.
    */
    const MAgnEntryUi::TAgnEntryUiInParams* iParams;
    
    /**
     * Own: Mailbox utils class.
     */
    CESMRFsMailboxUtils* iMbUtils;
    };

#include "cesmrentryprocessor.inl"

#endif // CESMRENTRYPROCESSOR_H
