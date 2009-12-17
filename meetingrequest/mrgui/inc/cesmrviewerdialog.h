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
* Description:  ESMR viewer dialog
*
*/
#ifndef CESMRVIEWERDIALOG_H
#define CESMRVIEWERDIALOG_H

#include <AknDialog.h>
#include <akntoolbarobserver.h>
#include <AknNaviDecoratorObserver.h>
#include <msvapi.h>

#include "mesmruibase.h"
#include "mesmrresponseobserver.h"
#include "resmrstatic.h"
#include "mesmrfieldeventobserver.h"

class MESMRFieldStorage;
class MESMRCalEntry;
class CESMRView;
class MAgnEntryUiCallback;
class CESMRPolicy;
class CESMRSendUI;
class CAiwServiceHandler;
class CCEUIPolicyResolver;
class CESMRPolicyManager;
class CESMRLocationPluginHandler;
class CESMRFeatureSettings;
class CAknNavigationDecorator;
class MESMRMeetingRequestEntry;
class TFSMailMsgId;

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
         public MAknNaviDecoratorObserver,
         public MMsvSessionObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aStorage Field storage contains all the fields for list component.
     * @param aEntry ESMR calendar entry (wraps the CCalEntry object)
     * @param aCallback Callback interface for AgnEntryUI
     * @return Pointer to created and initialized esmr editor dialog.
     */
     IMPORT_C static CESMRViewerDialog* NewL(
        CESMRPolicy* aPolicy,
        MESMRCalEntry& aEntry,
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
    virtual void OfferToolbarEventL( TInt aCommand );

public: // from MAknNaviDecoratorObserver
    void HandleNaviDecoratorEventL( TInt aEventID );

public: // from MMsvSessionObserver
    void HandleSessionEventL( 
        TMsvSessionEvent aEvent, TAny* aArg1,
        TAny* aArg2, TAny* aArg3 );
    
public:
    /**
     *  Static method that is given as callback to CIdle object
     *  handling the navigation decorator right arrow event.
     *  
     *  @param aObjPtr TAny-casted pointer to an instance of this class.
     *  @return Always KErrNone.
     */
    static TInt RunCmdMailNextMessageIdle( TAny* aObjPtr );
    
    /**
     *  Static method that is given as callback to CIdle object
     *  handling the navigation decorator left arrow event.
     *  
     *  @param aObjPtr TAny-casted pointer to an instance of this class.
     *  @return Always KErrNone.
     */
    static TInt RunCmdMailPreviousMessageIdle( TAny* aObjPtr );
    
protected: // MESMRFieldEventObserver
    void HandleFieldEventL( const MESMRFieldEvent& aEvent );

private: // Implementation
    void DoProcessCommandL( TInt aCommand );
    CESMRViewerDialog( MESMRCalEntry& aEntry, MAgnEntryUiCallback& aCallback );
    void ConstructL( CESMRPolicy* aPolicy );
    void OpenInDayModeL();
    void CommandTailL(HBufC16*& aTailBuffer);
    TBool HandleCommandForRecurrentEventL( TInt aCommand );
    void SendCalendarEntryL(TInt aCommandId);
    void TryInsertSendMenuL(CEikMenuPane* aMenuPane);
    void HandlePrintCommandL(TInt aCommand);
    void HandleEmailSubmenuL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    void HandlePolicyMenuL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    void HandleDynamicMenuItemsL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );
    TBool SupportsAttendeeStatusL();
    TBool HandleMRExitL(
            TInt aCommand  );
    TBool SupportsForwardingAsMeetingL( 
            MESMRMeetingRequestEntry* aEntry,
            TBool aForceResetDefaultMRMailbox );
    CESMRLocationPluginHandler& LocationPluginHandlerL();
    void SetDefaultMiddleSoftKeyL();
    TPtrC DigMailboxAndRemovePrefixL();
    void MakeMrGuiToolbarButtonsL();
    void ClearToolbarL();
    TInt AccountIdL( const TFSMailMsgId& aMailboxId );
    
private: // Data

    /// Own: Feature manager initialization flag
    TBool iFeatureManagerInitialized;
    /// Ref: The only control in this dialog. Fwk deletes.
    CESMRView* iView;
    /// Ref: Reference to MESRCalEntry (CCalEntry wrapper)
    MESMRCalEntry& iEntry;
    /// Ref: Agn Entry UI callback interface
    MAgnEntryUiCallback& iCallback;
    /// Ref: Pointer to policy
    CESMRPolicy* iPolicy;
    /// Own: Calendar Global data
    CESMRSendUI* iESMRSendUI;
    /// Own: AIW Service handler
    CAiwServiceHandler* iServiceHandler;
    /// Own: Flag for marking exit
    TBool iExitTriggered;
    /// Own: Static TLS data handler
    RESMRStatic iESMRStatic;
    // Own: Location plugin handler.    
    CESMRLocationPluginHandler* iLocationPluginHandler;
    // Own: Feature settings.
    CESMRFeatureSettings* iFeatures;
    // Not own: Pointer to previous toolbar observer.
    MAknToolbarObserver* iOldObserver;
    // flag to indicate toolbar can be cleared
    TBool iClearToolbar;
    // Own: CIdle object through which navi button events are handled.
    //      CIdle with callback is used because the navi buttons events
    //      cause deletion of this dialog, but the actual button event 
    //      handling needs to be completed before closing the dialog.
    CIdle* iIdleNaviEventRunner;
    // Not own: Pointer to navigation decorator
    CAknNavigationDecorator* iESMRNaviDecorator;
    };

#endif // CESMRVIEWERDIALOG_H
