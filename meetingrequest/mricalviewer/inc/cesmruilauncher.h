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
* Description:  ESMR UI launcher object definition
*
*/

#ifndef CESMRUILAUNCHER_H
#define CESMRUILAUNCHER_H

#include <e32base.h>
#include "cesmricalviewerasynccommand.h"
class TESMRInputParams;
class MAgnEntryUiCallback;
class CCalSession;
class MAgnEntryUi;
class CESMRViewerController;

/**
 * CESMRUiLauncher is responsible for launching MR viewer using
 * Agn Entry UI API. Part of the ican viewer ecom plugin implementation.
 */
NONSHARABLE_CLASS( CESMRUiLauncher ) :
        public CESMRIcalViewerAsyncCommand
    {
public: // Construction and destruction

    /**
     * Creates and initialzes new CESMRUiLauncher component. Ownership
     * is transferred to caller.
     *
     * @param aCalSession Reference to calendar session.
     * @param aInputParameters Reference to input parameters structure.
     * @param aAgnCallback Reference AGN Entry UI callback parameters.
     * @return Pointer to CESMRUiLauncher object
     */
    static CESMRUiLauncher* NewL(
            CCalSession& aCalSession,
            TESMRInputParams& aInputParameters,
            MAgnEntryUiCallback& aAgnCallback );

    /**
     * Creates and initialzes new CESMRUiLauncher component. Ownership
     * is transferred to caller. Created object is left to cleanup stack.
     *
     * @param aCalSession Reference to calendar session.
     * @param aInputParameters Reference to input parameters structure.
     * @param aAgnCallback Reference AGN Entry UI callback parameters
     * @return Pointer to CESMRUiLauncher object
     */
    static CESMRUiLauncher* NewLC(
            CCalSession& aCalSession,
            TESMRInputParams& aInputParameters,
            MAgnEntryUiCallback& aAgnCallback );

    /**
     *  C++ destructor.
     */
    ~CESMRUiLauncher();


public: // From CESMRIcalViewerAsyncCommand
    void ExecuteAsyncCommandL();
    void CancelAsyncCommand();

private: // Implementation
    CESMRUiLauncher(
            CCalSession& aCalSession,
            TESMRInputParams& aInputParameters,
            MAgnEntryUiCallback& aAgnCallback );
    void ConstructL();
    void LaunchMRViewerL();
    void LaunchEmailViewerUiL();
    void ExecuteAsyncCommandInternalL();

private: // Data
    /// Owm: ESMRViewer controller
    CESMRViewerController* iViewerController;
    /// Ref: Reference to input parameters
    TESMRInputParams& iInputParameters;
    // Ref: Reference to agn entryui callback
    MAgnEntryUiCallback& iAgnCallback;
    /// Own: Command result structure
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    };

#endif // CESMRUILAUNCHER_H
