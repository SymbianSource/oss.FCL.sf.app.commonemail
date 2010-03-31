/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mrviewer controller definition
*
*/


#ifndef CESMRVIEWERCTRL_H
#define CESMRVIEWERCTRL_H

//  INCLUDES
#include <magnentryui.h>
#include <cmrutils.h>
#include <e32base.h>
#include "mmrinfoprovider.h"

// FORWARD DECLARATIONS
class CCalEntry;
class TCoeHelpContext;
class CMRDialogBase;
class CESMRUtils;
class CMRMailboxUtils;
class CActiveSchedulerWait;
class CESMRUiFactory;
class CMREntryProcessor;
class MESMRTaskExtension;
class CESMRPolicyManager;
class MESMRCalDbMgr;

// CLASS DECLARATION

/**
 * ESMRVIEWER controller is responsible for control logic and UI view
 * creation and execution.
 */
NONSHARABLE_CLASS(CESMRViewerController)  :
        public CBase,
        public MAgnEntryUiCallback,
        public MMRUtilsObserver,
        public MMRInfoProvider
    {
public:  // Constructors and destructor
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CESMRViewerController * NewL(
            const TDesC8& aMtmuid,
            RPointerArray<CCalEntry>& aEntries,
            const MAgnEntryUi::TAgnEntryUiInParams& aParams,
            MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
            MAgnEntryUiCallback& aCallback );

    /**
    * Destructor.
    */
    IMPORT_C ~CESMRViewerController ();

public: // Implementation
    IMPORT_C void ExecuteL();

protected: // From MAgnEntryUICallback
    TBool IsCommandAvailable( TInt aCommandId );
    TInt ProcessCommandWithResultL( TInt aCommandId );
    void ProcessCommandL( TInt aCommandId);

protected: // From MMRUtilsObserver
    void HandleCalEngStatus( TMRUtilsCalEngStatus aStatus );
    void HandleOperation(
            TInt aType,
            TInt aPercentageCompleted,
            TInt aStatus );

protected: // From MMRInfoProvider
    MMRPolicyProvider& PolicyProvider() const;
    MESMRCalEntry* EntryL();
    
private: // Constructors and implementation
    CESMRViewerController(
            RPointerArray<CCalEntry>& aEntries,
            const MAgnEntryUi::TAgnEntryUiInParams& aParams,
            MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
            MAgnEntryUiCallback& aCallback );
    void ConstructL(const TDesC8& aMtmuid);
    TInt ProcessCommandWithResultInternalL(
            TInt aCommandId );
    void LaunchUIL();
    void LaunchCorrectOperationModeL();
    TInt ProcessDefaultCommandL( TInt aCommandId );
    TInt ProcessMeetingRequestCommandL( TInt aCommandId );
    void ChangeEntryTypeL( TInt aCommandId );
    void UpdateComparativeEntryInTypeChangeL( const CCalEntry& aOriginalEntry );
    void CancelAndDeleteMRInEntryTypeChangeL( 
            CCalEntry& entry, MESMRCalDbMgr& aDbMgr );

private: // Data
    /// Own: Operation error value
    TInt iExecutionError;
    /// Own: Pointer to ESMRUTILS handler
    CESMRUtils* iESMRUtils;
    // Ref: Oarameters from the launching application
    const MAgnEntryUi::TAgnEntryUiInParams iInParams;
    // Ref: Parameters for the launching application
    MAgnEntryUi::TAgnEntryUiOutParams& iOutParams;
    // Ref: callback for command handling
    MAgnEntryUiCallback& iCallback;
    // Ref: Calendar entries to be handled
    RPointerArray<CCalEntry>& iEntries;
    /// Own: MR UTILS syncher
    CActiveSchedulerWait* iCtrlSyncher;
    /// Own: UI factory
    CESMRUiFactory* iGuiFactory;
    /// Ref: Reference to entry processor
    CMREntryProcessor* iEntryProcessor;
    /// Ref: Reference to task extension
    MESMRTaskExtension* iTaskExt;
    /// Ref: Reference to policy manager
    CESMRPolicyManager* iPolicyMgr;
    /// Own: Current calendar entry to be handled
    CCalEntry* iCalEntry;
    };

#endif      // CMRHANDLER_H

// End of File
