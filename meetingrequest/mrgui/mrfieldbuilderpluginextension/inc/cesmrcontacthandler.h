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
* Description:  ESMR Contact Handler for fetching contacts for attendee field
*
*/

#ifndef CESMRCONTACTHANDLER_H
#define CESMRCONTACTHANDLER_H

// Includes
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <AiwCommon.h>
//<cmail>
#include "cpbkxremotecontactlookupserviceuicontext.h"
//</cmail>
#include "resmrstatic.h"
#include "mesmrclslistsobserver.h"
#include "cesmrcontactmanagerhandler.h"
#include "resmrpluginextensionstatic.h"

// Forward declarations
class CFSMailBox;
class CESMRClsListsHandler;
class CVPbkContactManager;
class CESMRClsItem;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class MESMRContactHandlerObserver;
class MVPbkContactOperationBase;
class MVPbkContactLinkArray;
class CAiwServiceHandler;
class CESMRClsItem;
class CContactItem;

/**
 * Command to execute by contact handler
 */
enum TContactHandlerCmd
    {
    EGetAddressesFromPhonebook = 1,
    ESearchContacts
    };

/**
 * Implements remote look up and contact search related functionality.
 * It will launch AIW service (virtual phonebook provided) to create those views
 */
class CESMRContactHandler:
    public CBase,
    public MESMRClsListsObserver,
    public MVPbkSingleContactOperationObserver,
    public MAiwNotifyCallback,
    public MESMRContactManagerObserver
{
public: // Construction & Deconstruction
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRContactHandler object.
     * @return Pointer to object.
     */
    static CESMRContactHandler* NewL();
    
    /**
     * Two-phased constructor. Creates and initializes
     * CESMRContactHandler object, which is left on 
     * the cleanupstack
     * @return Pointer to object.
     */
    static CESMRContactHandler* NewLC();

    /**
     * Shows the popup on the screen.
     */
    ~CESMRContactHandler();

public: // Interface
    /**
     * Search for contacts by PCS engine. This is asynchronous call, and results are in
     * ArrayUpdated( resultsArray )
     *
     * @param aText = new search text.
     * @param aClsListObserver = observer to collect results
     */
    void SearchMatchesL( const TDesC& aText, 
                         MESMRContactHandlerObserver* aObserver );

    /**
     * Launches remote lookup query window (remote lookup UI)
     * @param aQueryString search string for search engines
     * @param aDisplayname displayed name of contact
     * @param aEmailAddress email address of contact
     * @return not used
     */
    TBool LaunchRemoteLookupL( const TDesC& aQueryString,
                               TDes& aDisplayname, 
                               TDes& aEmailAddress );

    /**
     * get last name found with search
     * aEmailAddress email address of contact
     * @return display name of contact
     */
    HBufC* GetLastSearchNameLC( const TDesC& aEmailAddress );

    /**
     * Get pointer to contact manager (handles phone book stores)
     * @return pointer to contact manager
     */
    CVPbkContactManager* GetContactManager();

    /**
     * Lauches multiple item fetch dialog through AIW launcher framework.
     * Results are returned to observer
     * @param aObserver = observer to collect results
     */
    void GetAddressesFromPhonebookL( MESMRContactHandlerObserver* aObserver );

    TBool RemoteLookupSupportedL();
    
public: // from MESMRClsListsObserver
    void ArrayUpdatedL( RPointerArray<CESMRClsItem>& aMatchingItems );
    void OperationErrorL( TInt aErrorCode );

public: // from MVPbkSingleContactOperationObserver
    void VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                             MVPbkStoreContact* aContact );

    void VPbkSingleContactOperationFailed( MVPbkContactOperationBase& aOperation, 
                                           TInt aError );

public: // from MAiwNotifyCallback
    TInt HandleNotifyL( TInt aCmdId,
                        TInt aEventId,
                        CAiwGenericParamList& aEventParamList,
                        const CAiwGenericParamList& aInParamList);

public: // from MESMRContactManagerObserver
    void ContactManagerReady();

private: // Implementation
    CESMRContactHandler();
    void ConstructL();
    TBool IsRemoteLookupSupported();
    void GetNameAndEmail( TDes& aName, TDes& aEmail, CContactItem& aItem );
    void VPbkSingleContactOperationFailedL( MVPbkContactOperationBase& aOperation, TInt aError );
    void VPbkSingleContactOperationCompleteL( MVPbkContactOperationBase& aOperation,
                                              MVPbkStoreContact* aContact );
    void GetContactFieldsL( RArray<TInt>& aFieldIds, 
                            RPointerArray<HBufC>& aNumbers,
                            MVPbkStoreContact* aContact );
    void DoRemoteLookupL( const TDesC& aQueryString,
                          CPbkxRemoteContactLookupServiceUiContext::TResult& aResult,
                          CPbkxRemoteContactLookupServiceUiContext::TMode aContext = 
                          CPbkxRemoteContactLookupServiceUiContext::EModeContactSelector );
    CFSMailBox& DelayedMailBoxL();
    
private: // data
    RESMRStatic iESMRStatic;//not own, reference counted
    CESMRClsListsHandler* iClsListHandler;//own
    CESMRClsItem* iSearchMatch;//own
    MESMRContactHandlerObserver* iHandlerObserver;//not own
    CFSMailBox* iMailBox;//not own
    CVPbkContactManager* iContactManager; //not own
    MVPbkContactOperationBase* iLinkOperationFetch;//own
    const MVPbkContactLink* iCurrentLink;//own
    MVPbkContactLinkArray* iLinksSet;//own
    CAiwServiceHandler* iServiceHandler;//own
    /**
     * Static TLS stored pointer to fsmailbox.
     */
    RESMRPluginExtensionStatic          iESMRMailBoxStatic;
    TBool                               iESMRMailBoxStaticCalled;
    TBool                               iRemoteLookupSupported;
    
    /**
     * State of contact handlers asynchronous processing
     */
    enum TContactHandlerState
        {
        EContactHandlerIdle = 1,
        EContactHandlerSearchMatches,
        EContactHandlerGetAddressesFromPhonebook
        };
    TContactHandlerState iState;

    //internal arrays on the stack
    RPointerArray<CESMRClsItem> iContactObjects;
    RArray<TInt> iLastnameFields;
    RArray<TInt> iFirstnameFields;
    RArray<TInt> iEmailFields;
};

#endif
