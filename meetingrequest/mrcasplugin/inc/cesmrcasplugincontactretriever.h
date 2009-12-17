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
* Description:  Definition of CAS plug-in contact retriever
 *
*/


#ifndef CESMRCASPLUGINCONTACTRETRIEVER_H
#define CESMRCASPLUGINCONTACTRETRIEVER_H

#include <e32base.h>
//<cmail>
#include "mfsccontactsetobserver.h"
//</cmail>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MFscContactSet;
class TFscContactActionPluginParams;
class MESMRCasPluginContactRetrieverObserver;
class CCalUser;

/**
 *  Create meeting Request plugin implementation.
 *  Contains implementation of create meeting request action plugin.
 *
 *  part contact action menu Ecom plugin based API
 */
NONSHARABLE_CLASS (CESMRCasPluginContactRetriever) :
        public CBase,
        public MFscContactSetObserver
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates and initializes
     * CESMRCasPluginContactRetriever object. Ownership transferred to caller.
     *
     * @param aParams set of contact action plugin configuration parameters
     * @return Pointer to ContactRetriever object.
     */
    static CESMRCasPluginContactRetriever* NewL(
            const TFscContactActionPluginParams& aParams );

    /**
     * C++ Destructor.
     */
    virtual ~CESMRCasPluginContactRetriever( );

public: // From Interface
    void CanExecuteL(
            MFscContactSet& aContactSet,
            MESMRCasPluginContactRetrieverObserver& aObserver );
    void GetAttendeesL(
            MFscContactSet& aContactSet,
            MESMRCasPluginContactRetrieverObserver& aObserver );
    void Cancel();

private: // from MFscContactSetObserver
    void NextContactComplete( MVPbkStoreContact* aContact );
    void NextContactFailed( TInt aError );
    void NextGroupComplete( MVPbkStoreContact* aContact );
    void NextGroupFailed( TInt aError );
    void GetGroupContactComplete( MVPbkStoreContact* aContact );
    void GetGroupContactFailed( TInt aError );

private: // Implementation
    CESMRCasPluginContactRetriever(
            const TFscContactActionPluginParams& aParams );
    void ConstructL( );

    void StartIterationL();
    void HandleNextContactCompleteL( MVPbkStoreContact& aContact );
    void HandleNextGroupCompleteL( MVPbkStoreContact* aGroup );
    void NotifyFinished();
    void NotifyError( TInt aError );
private: // data

    /** States of operations */
    enum TLastEvent
        {
        EActionEventNextContact,
        EActionEventNextGroup,
        EActionEventGetGroupContact
        };
    
    /** State current operation */
    enum TOperation
        {
        EOperationCanExecute,
        EOperationGetAttendees
        };

    /// Own: Operation in progress
    TOperation iOperation;
    /// Own: Last event
    TLastEvent iLastEvent;
    /// Own: Fetched MR addresses
    RPointerArray<CCalUser> iAttendees;
    /// Own: Contact count with email
    TInt iContactCountWithEmail;
    /// Own: Total contact count
    TInt iContactCount;
    /// Ref: Current observer
    MESMRCasPluginContactRetrieverObserver* iObserver;
    /// Ref: Plugin parameters
    const TFscContactActionPluginParams& iParams;
    /// Ref: Contact set to be handled
    MFscContactSet* iContactSet;
    /// Own: Contact index in current group
    TInt iCurrentGroupContactIndex;
    /// Own: Contact count in current group
    TInt iCurrentGroupContactCount;
    /// Ref: Current group in progress
    MVPbkStoreContact* iCurrentGroup;
    };

#endif // CESMRCASPLUGINCONTACTRETRIEVER_H
