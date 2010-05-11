/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ESMR viewer dialog
*
*/
#ifndef CESMRVIEWERDIALOG_H
#define CESMRVIEWERDIALOG_H

#include <AknDialog.h>
#include <akntoolbarobserver.h>
#include <AknServerApp.h>
#include "mesmruibase.h"
#include "mesmrresponseobserver.h"
#include "resmrstatic.h"
#include "mesmrfieldeventobserver.h"
#include "mesmrnaviarroweventobserver.h"
#include "cesmrfieldbuilderinterface.h"

class MESMRFieldStorage;
class MESMRCalEntry;
class CESMRView;
class MAgnEntryUiCallback;
class CESMRPolicy;
class CESMRSendUI;
class CCEUIPolicyResolver;
class CESMRPolicyManager;
class CESMRLocationPluginHandler;
class CESMRFeatureSettings;
class CMRToolbar;
class TDataType;
class MMRInfoProvider;
class MMRFocusStrategy;
class CESMRTitlePaneHandler;

/**
 *  CESMRViewerDialog implements the viewer dialog for meeting requests.
 *
 *  @see mesmruibase.h, mesmruifactory.h
 *  @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRViewerDialog ):
         public CAknDialog,
         public MESMRUiBase,
         public MESMRResponseObserver,
         public MESMRFieldEventObserver,
         public MAknToolbarObserver,
         public MESMRNaviArrowEventObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aInfoProvider Reference to MR / entry info provider.
     * @param aCallback Callback interface for AgnEntryUI
     * @return Pointer to created and initialized esmr editor dialog.
     */
     IMPORT_C static CESMRViewerDialog* NewL(
             MMRInfoProvider& aInfoProvider,
             MAgnEntryUiCallback& aCallback );
    /*
     * Destructor.
     */
    ~CESMRViewerDialog();

public: // From CAknDialog
    SEikControlInfo CreateCustomControlL( TInt aType );
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    TBool OkToExitL (TInt aButtonId);
    void ProcessCommandL( TInt aCommand );
    void DynInitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    void ActivateL();
    void PreLayoutDynInitL();
    
public: // From MESUIBase
    TInt ExecuteViewLD();

public: // From MESMRResponseObserver
    TBool Response( TInt aCommand );
    void ExitDialog();
    void ChangeReadyResponseL();

public: // From MAknToolbarObserver
    void DynInitToolbarL ( TInt aResourceId, CAknToolbar* aToolbar );
    void OfferToolbarEventL ( TInt aCommand );

protected: // MESMRFieldEventObserver
    void HandleFieldEventL( const MESMRFieldEvent& aEvent );
    
public: // From MESMRNaviArrowEventObserver
    void HandleNaviArrowEventL( const TInt aCommand );

private: // Implementation
    void DoProcessCommandL( TInt aCommand );
    CESMRViewerDialog(
            MMRInfoProvider& aInfoProvider,
            MAgnEntryUiCallback& aCallback );
    void ConstructL();
    void OpenInDayModeL();
    void CommandTailL(HBufC16*& aTailBuffer);
    TBool HandleCommandForEventL( TInt aCommand );
    void SendCalendarEntryL(TInt aCommandId);
    void TryInsertSendMenuL(CEikMenuPane* aMenuPane);
    void HandleEmailSubmenuL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    void HandlePolicyMenuL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    void HandleDynamicMenuItemsL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    TBool SupportsMailBoxCapabilityL( 
            MESMRBuilderExtension::TMRCFSMailBoxCapability aCapa );
    TBool HandleMRExitL(
            TInt aCommand  );
    void HandleForwardMenuL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    CESMRLocationPluginHandler& LocationPluginHandlerL();
    void SetDefaultMiddleSoftKeyL();
    TInt TryToSaveAttachmentL();
    void SetContextMenu();
    void ShowContextMenuL();
    TBool UserWantToHandleAttachmentsL();
    void InitLocationMenuL(
            CEikMenuPane* aMenuPane );
    void ConstructToolbarL();
    void ProcessCommandEventL( const MESMRFieldEvent& aEvent );
    void ProcessFieldEventL( const MESMRFieldEvent& aEvent );
    
private: // Data

     /// Ref: The only control in this dialog. Fwk deletes.
    CESMRView* iView;
    /// Ref: Agn Entry UI callback interface
    MAgnEntryUiCallback& iCallback;
    /// Own: Calendar Global data
    CESMRSendUI* iESMRSendUI;
    /// Own: Flag for marking exit
    TBool iExitTriggered;
    /// Own: Static TLS data handler
    RESMRStatic iESMRStatic;
    // Own: Location plugin handler.
    CESMRLocationPluginHandler* iLocationPluginHandler;
    // Own: Feature settings.
    CESMRFeatureSettings* iFeatures;
    /// Own: Toolbar
    CMRToolbar* iToolbar;
    /// Ref: Reference to mrinfo provider
    MMRInfoProvider& iInfoProvider;
    /// Own: Context menu resource ID.
    TInt iContextMenuResourceId;
    /// Own: Focus strategy of the viewer
    MMRFocusStrategy* iFocusStrategy;
    /// Own: Titlepane handler
    CESMRTitlePaneHandler* iTitlePane;
    };

#endif // CESMRVIEWERDIALOG_H
