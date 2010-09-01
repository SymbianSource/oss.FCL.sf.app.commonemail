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

#ifndef C_MRCALENTRYPROCESSOR_H
#define C_MRCALENTRYPROCESSOR_H

#include <e32base.h>
#include "cmrentryprocessor.h"

class MESMRCalDbMgr;
class CMRCalEntry;

/**
* CESMREntryProcessor's main responsibility is to
* perform entry processing and evaluation in initialization phase.
*
* @lib esmrentry.lib
*/
NONSHARABLE_CLASS(CMRCalEntryProcessor) :
        public CMREntryProcessor
    {
public: // Construction and destruction
    /**
     * Creates new CESMREntryProcessor object. Ownership
     * is transferred to caller.
     * @param aDbMgr Reference to calendar DB manager.
     * @return Pointer to created CESMREntryProcessor object,
     */
    static CMRCalEntryProcessor* NewL(
            TAny* aDbMgr );    
    
    /**
     * C++ destructor
     */
    ~CMRCalEntryProcessor();

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

    static CMRCalEntryProcessor* CreateL(
            MESMRCalDbMgr& aDbMgr ); 
    
    CMRCalEntryProcessor(
            MESMRCalDbMgr& aDbMgr);
    void ConstructL();
    TBool IsDataOk() const;
    void CreateEntryL( CCalEntry& aEntry, TBool aSetDefaultValuesToEntry );
    void ConvertTextFieldLineFeedsL();
    void ConvertTextFieldLineFeedsL( CCalEntry& aEntry );
    
private: // Data
    /**
    * Scenario data
    * Own.
    */
    TESMRScenarioData iScenData;
    
    /**
    * Reference to calendar DB manager
    * Not own.
    */
    MESMRCalDbMgr& iDbMgr;
    
    /**
    * Calendar entry
    * Own.
    */
    CMRCalEntry* iCalEntry;
    
    /**
    * Input parameters from calling application
    * Not own.
    */
    const MAgnEntryUi::TAgnEntryUiInParams* iParams;
    
    /**
    * Entries to be processed
    * Not own.
    */
    RPointerArray<CCalEntry>* iCalEntries;    
    };

#endif // C_MRCALENTRYPROCESSOR_H
