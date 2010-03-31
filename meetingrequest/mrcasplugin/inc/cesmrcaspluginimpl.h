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
* Description:  Definition of the class CESMRCasPluginImpl.
 *
*/


#ifndef CESMRCASPLUGINIMPL_H
#define CESMRCASPLUGINIMPL_H

#include <e32base.h>
//<cmail>
#include "cfsccontactactionplugin.h"
#include "mfscreasoncallback.h"
#include "cesmrcascontactaction.h"
#include <cagnentryui.h>
#include <calsession.h>
#include "mfsccontactsetobserver.h"
//</cmail>
#include "mesmrcasplugincontactretrieverobserver.h"
#include "mesmrcaspluginuilauncherobserver.h"

// FORWARD DECLARATIONS
class CGulIcon;
class CCalSession;
class CMRMailboxUtils;
class CCalenInterimUtils2;
class CESMRCasPluginUiLauncher;
class CESMRCasPluginContactRetriever;

/**
 *  Create Meeting Request contact action plugin implementation.
 *  
 *  Contains implementation to create meeting request action plugin.
 *  Adds the "Send Meeting Request" option to those contact
 *  action menu service users that request it. and lauches meeting
 *  request editors in the current process
 *
 *  Ecom plugin
 */
NONSHARABLE_CLASS (CESMRCasPluginImpl) :
        public CFscContactActionPlugin,
        public MFscReasonCallback,
        public MESMRCasPluginContactRetrieverObserver,
        public MESMRCasPluginUiLauncherObserver
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates and initializes
     * CESMRCasPluginImpl object. Ownership transferred to caller.
     *
     * @param aParams generic set of parameters from the creator of CESMRCasPluginImpl
     * @return Pointer to esmr CESMRCasPluginImpl ecom based object.
     */
    static CESMRCasPluginImpl* NewL( TAny* aParams );

    /**
     * C++ Destructor.
     */
    virtual ~CESMRCasPluginImpl( );

public: // From CFscContactActionPlugin
    TUid Uid( ) const;
    const CArrayFix<TUid>* ActionList( ) const;
    const MFscContactAction& GetActionL(
            TUid aActionUid ) const  ;
    void PriorityForContactSetL(
            TUid aActionUid,
            MFscContactSet& aContactSet,
            TFscContactActionVisibility& aActionMenuVisibility,
            TFscContactActionVisibility& aOptionsMenuVisibility,
            MFscContactActionPluginObserver* aObserver ) ;
    void CancelPriorityForContactSet();
    void ExecuteL(
        TUid aActionUid,
        MFscContactSet& aContactSet,
        MFscContactActionPluginObserver* aObserver );
    void CancelExecute();

public: // From MFscReasonCallback
    void GetReasonL( TUid aActionUid,
            TInt aReasonId,
            HBufC*& aReason ) const;

private: // from MESMRCasPluginContactRetrieverObserver
    void MRCanExecuteComplete(
            TBool aCanExecute );
    void MRCanExecuteError(
            TInt aError );
    void MRGetAttendeesComplete(
            TInt aContactCount,
            const RPointerArray<CCalUser>& aContacts );
    void MRGetAttendeesError(
            TInt aError );
    
private: // from MESMRCasPluginUiLauncherObserver
    void MREditorInitializationComplete();

private: // Implementation
    CESMRCasPluginImpl(const TFscContactActionPluginParams& aParams );
    void ConstructL( );
    CGulIcon* IconL( TInt aBitmapId, TInt aMaskId ) const;
    TBool MRViewersEnabledL();
    void MRGetAttendeesCompleteL(
            TInt aContactCount,
            const RPointerArray<CCalUser>& aContacts );

private: // data
    /// Ref: Reference to contact action params
    const TFscContactActionPluginParams& iParams;
    /// Own: Phonebook Resource handle
    TInt iResourceHandle;
    /// Own: meeting request action
    CESMRCasContactAction* iMeetingRequestContactAction;
     /// Own: Contact action array
    CArrayFixFlat<TUid>* iActionList;
    /// Ref: Plugin observer
    MFscContactActionPluginObserver* iPluginObserver;
    /// Ref: Action UID
    TUid* iActionUid;
    /// Own: Action menu visibility
    TFscContactActionVisibility* iActionMenuVisibility;
    /// Own: Option menu visibility
    TFscContactActionVisibility* iOptionsMenuVisibility;
    /// Own: Action priority
    TInt iActionPriority;
    /// Own: Calendar session
    CCalSession* iCalSession;
    /// Own: MRMailbox utils
    CMRMailboxUtils* iMailboxUtils;
    /// Own Calendar utils
    CCalenInterimUtils2* iCalUtils;
    /// Own: UI Launcher
    CESMRCasPluginUiLauncher* iUiLauncher;
    /// Own: Contact retriever
    CESMRCasPluginContactRetriever* iContactRetriever;
    };

#endif // CESMRCASPLUGINIMPL_H

/// EOF
