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
*  Description : ESMR tracking viewe dialog
*  Version     : %version: tr1sido#4 %
*
*/
#ifndef CESMRTRACKINGVIEWDIALOG_H
#define CESMRTRACKINGVIEWDIALOG_H

// SYSTEM INCLUDE
#include <AknDialog.h>
#include <caluser.h>

#include "mesmruibase.h"
#include "mesmrresponseobserver.h"
#include "resmrstatic.h"
#include "mesmrfieldeventobserver.h"

// FORWARD DECLARATIONS
class CESMRPolicy;
class MAgnEntryUiCallback;
class MESMRCalEntry;
class CESMRView;
class CAiwServiceHandler;

class CESMRTrackingViewDialog : public CAknDialog,
                                public MESMRUiBase,
                                public MESMRFieldEventObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     */
    static CESMRTrackingViewDialog* NewL
            (CESMRPolicy* aPolicy,
             MESMRCalEntry& aEntry,
             MAgnEntryUiCallback& aCallback
            );

    /*
     * Destructor.
     */
    ~CESMRTrackingViewDialog();

public: // From CAknDialog
    SEikControlInfo CreateCustomControlL( TInt aType );
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    TBool OkToExitL (TInt aButtonId);
    void ProcessCommandL( TInt aCommand );
    void DynInitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    void ActivateL();

public: // From MESUIBase
    TInt ExecuteViewLD();

protected: // From MESMRFieldEventObserver
    
    void HandleFieldEventL( const MESMRFieldEvent& aEvent );
    
private: // Implementation
    void DoProcessCommandL( TInt aCommand );
    CESMRTrackingViewDialog(MESMRCalEntry& aEntry, MAgnEntryUiCallback& aCallback);
    void ConstructL( CESMRPolicy* aPolicy );
    TInt CountAttendeesL(CCalAttendee::TCalRole aRole);
    void SortAttendeesL();
    void HandlePrintCommandL(TInt aCommand);

private:

    /**
     * Own:Feature manager initialization flag
     */
    TBool iFeatureManagerInitialized;

    /**
     * Ref: Policy for building fields and menus
     */
    CESMRPolicy* iPolicy;

    /// Ref: Reference to MESRCalEntry (CCalEntry wrapper)
    MESMRCalEntry& iEntry;

    /// Ref: Agn Entry UI callback interface
    MAgnEntryUiCallback& iCallback;

    /**
     * Ref: The only control in this dialog. Fwk deletes.
     */
    CESMRView* iView;

    /// Own: AIW Service handler
    CAiwServiceHandler* iServiceHandler;

    /// Own: Static TLS data handler
    RESMRStatic iESMRStatic;
    };

#endif /*CESMRTRACKINGVIEWDIALOG_H*/

// End of File
