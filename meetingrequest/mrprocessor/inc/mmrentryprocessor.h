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
#ifndef M_MRENTRYPROCESSOR_H
#define M_MRENTRYPROCESSOR_H

#include <e32base.h>
#include <magnentryui.h>
#include "esmrdef.h"
#include "tesmrscenariodata.h"

class MESMRCalEntry;
class CCalEntry;
class CCalInstance;

/**
 * MESMREntryProcessor defines interface for entry processing
 */
class MESMREntryProcessor
    {
public:
    virtual ~MESMREntryProcessor() {}
        
public: // Interface
    /**
     * Fetches scenario data.
     *
     * @return scenario data.
     */
    virtual const TESMRScenarioData& ScenarioData() const = 0;

    /**
     * Tests, whether processor contains entry, which it has processed
     * or not.
     *
     * @return ETrue, if contains processed entry.
     */
    virtual TBool ContainsProcessedEntry() const = 0;

    /**
     * Process entries and constructs scenario data.
     *
     * @param aParam AGN Entry UI input parameters.
     * @param aEntries Calendar entries to be processed.
     *        Ownership is not transferred.
     */
    virtual void ProcessL(
            const MAgnEntryUi::TAgnEntryUiInParams* aParams,
            RPointerArray<CCalEntry>* aEntries ) = 0;

    /**
     * Process entry and constructs scenario data.
     *
     * @param aParam AGN Entry UI input parameters.
     * @param aEntry Calendar entry to be processed.
     * @param aSetDefaultValuesToEntry, ETrue is default values
     * should be set to entry, EFalse otherwise
     * @param aTypeChanging entry type is changing
     */
    virtual void ProcessL(
            const MAgnEntryUi::TAgnEntryUiInParams* aParams,
            CCalEntry& aEntry,
            TBool aSetDefaultValuesToEntry,
            const TBool aTypeChanging = EFalse ) = 0;
    
    /**
     * Process entries. Scenario data is not constructed.
     *
     * @param aEntries Calendar entries to be processed.
     *        Ownership is not transferred.
     */
    virtual void ProcessL(
            RPointerArray<CCalEntry>* aEntries ) = 0;

    /**
     * Processes calendar instance. All operations affects only the
     * processed instance.
     *
     * @param aInstance Reference to calendar instance.
     */
    virtual void ProcessL(
            CCalInstance& aInstance ) = 0;

    /**
     * Switches processor for specified mode mode.
     * @error KErrNotReady no entries has been processed yet.
     * @error KErrArgument Unknown mode
     */
    virtual void SwitchProcessorToModeL(
            TMRProcessorMode aMode ) = 0;

    /**
     * Fetches ES MR Entry. Methods panics, if entries has not
     * been processed before calling this method.
     *
     * @return Reference to CESMRMeetingRequestEntry object.
     */
    virtual MESMRCalEntry& ESMREntryL() = 0;

    /**
     * Resets the entry processor. After calling this method the
     * processor is in the same state than after creation.
     */
    virtual void ResetL() = 0;

    /**
     * Process output parameters. Processing is done according to command
     * passed as parameter.
     *
     * @param aOutputParams Reference to output parameters.
     * @param aCommand Command for which output parameters are processed.
     */
    virtual void ProcessOutputParametersL(
            MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
            TESMRCommand aCommand ) = 0;
    
    /**
     * Returns extension interface.
     * @param aExtensionUid Extension identifier 
     */
    virtual TAny* Extension( TUid /* aExtensionUid */ ) { return NULL; }
    };

#endif // M_MRENTRYPROCESSOR_H

// EOF
