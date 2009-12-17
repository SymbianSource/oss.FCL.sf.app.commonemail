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
#include <MAgnEntryUi.h>
#include <CMRUtils.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CCalEntry;
class TCoeHelpContext;
class CMRDialogBase;
class CESMRUtils;
class CMRMailboxUtils;
class CActiveSchedulerWait;
class CESMRUiFactory;
class CESMREntryProcessor;
class MESMRTaskExtension;
class CESMRPolicyManager;

// CLASS DECLARATION

/**
 * ESMRVIEWER controller is responsible for control logic and UI view
 * creation and execution.
 */
NONSHARABLE_CLASS(CESMRViewerController)  :
        public CBase,
        public MAgnEntryUiCallback,
        public MMRUtilsObserver
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

private: // Constructors
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
    CESMREntryProcessor* iEntryProcessor;
    /// Ref: Reference to task extension
    MESMRTaskExtension* iTaskExt;
    /// Ref: Reference to policy manager
    CESMRPolicyManager* iPolicyMgr;
    };

#endif      // CMRHANDLER_H

// End of File
