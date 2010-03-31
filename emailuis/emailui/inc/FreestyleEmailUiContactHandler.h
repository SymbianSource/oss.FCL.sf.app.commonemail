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
* Description:  Contact handler interface
*
*/


#ifndef FREESTYLEEMAILUICONTACTHANDLER_H_
#define FREESTYLEEMAILUICONTACTHANDLER_H_

#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactStoreListObserver.h>
#include <AiwCommon.h>

// CCA Launcher view
#include <mccaconnection.h>

#include <cpbkxremotecontactlookupserviceuicontext.h>

#include "FreestyleEmailUiCLSListsObserver.h"

// Forward declarations

class CFSMailBox;
class RFs;
class CFSEmailUiClsListsHandler;
class CVPbkContactManager;
class CFSEmailUiClsItem;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class MFSEmailUiContactHandlerObserver;
class MVPbkContactLinkArray;
class CAiwServiceHandler;
class CContactItem;
class MPbkGlobalSetting;
class CEikonEnv;

/** Contact handler commands. */
enum TContactHandlerCmd
    {
    EAddToContactL = 1,
    EFindAndCallToContactByEmailL,
    ECallToContact,
    EShowContactDetailsL,
    EGetAddressesFromPhonebook,
    ESearchContacts,
    ECreateMessage,
    EIdle
    };

enum TContactUpdateType
    {
    EContactUpdateNumber = 1,
    EContactUpdateEmail
    };

enum TContactHandlerState 
	{
	EContactHandlerIdle = 1,
	EContactHandlerSearchMatches,
	EContactHandlerAddToContacts,
	EContactHandlerCallToContactByEmail,
	EContactHandlerGetAddressesFromPhonebook,
	EContactHandlerGetSmsAddressFromPhonebook,
	EContactHandlerGetMmsAddressFromPhonebook,
	EContactHandlerGetVoiceMsgAddressFromPhonebook,
	EContactHandlerShowContactDetails,
	EContactHandlerCreateMessage
	};

enum TContactHandlerRemoteLookupMode
	{
	EContactHandlerRemoteSelector = 1,
	EContactHandlerRemoteNormal,
	EContactHandlerRemoteQueryGiven
	};

enum TAddToContactsType
	{
	EContactHandlerCreateNewContact = 1,
	EContactHandlerUpdateExisting
	};

class CFSEmailUiContactHandler: 
	public CBase,
	public MFSEmailUiClsListsObserver,
	public MVPbkSingleContactOperationObserver,
	public MVPbkContactStoreListObserver,
	public MAiwNotifyCallback, 
	public MCCAObserver

{
public:
    static CFSEmailUiContactHandler* NewL( RFs& aSession );
    static CFSEmailUiContactHandler* NewLC( RFs& aSession );
    
	~CFSEmailUiContactHandler();
	
	/**
	 * Implements "Add to Contacts" funcionality. 
	 * It will launch AIW service (phonebook2) to create those views  
	 *
	 * @param aDes = phonenumber/emailaddress to be added
	 * @param aUpdatetype = EContactUpdateNumber/EContactUpdateEmail
	 * @param aAddtoContactsType = ECreateNew/EUpdateExisting
	 * @param aObserver, when AIW service is completed observer operationComplete( EAddToContactL, NULL ) is called
	 * if there's a error operationError is called. If caller isn't interested of result, this can be left as NULL
	 */
	void AddToContactL( const TDesC& aDes, TContactUpdateType aUpdateType,
						TAddToContactsType aAddtoContactsType,
	                    MFSEmailUiContactHandlerObserver* aObserver = NULL );
	/**
	 * Query whether to create a "new contact" or "update existing"
	 * @param TAddtoContactsType = ECreateNew/EUpdateExisting
	 * @return: EFalse if user choosed "cancel" instead of choosing one option
	 */
    TBool AddtoContactsQueryL( TAddToContactsType& aType );
    
	/**
	 * Implements "Calling to contact provided by email" funcionality. 
	 * It will search for the Contact and then launch AIW call service.
	 *
	 * @param aEmailAddress = emailaddress of the contact
	 * @param aCallToDefaultNumber = ETrue, call is made to default phonenumber. EFalse = User is queried of the number.
	 * @param aObserver, when AIW service is completed observer operationComplete( EFindAndCallToContactByEmailL, NULL ) 
	 * is called if there's a error operationError is called. If caller isn't interested of result, this can be left as NULL
	 */	
	void FindAndCallToContactByEmailL( const TDesC& aEmailAddress, 
									   CFSMailBox* aMailBox,
	                                   MFSEmailUiContactHandlerObserver* aObserver = NULL,
	                                   TBool aCallToDefaultNumber = EFalse );
	
	/**
	 * Implements "Sending message to contact provided by email" funcionality. 
	 * It will search for the Contact and then launch create message dialog
	 *
	 * @param aEmailAddress = emailaddress of the contact
	 * @param aCallToDefaultNumber = ETrue, call is made to default phonenumber. EFalse = User is queried of the number.
	 * is called if there's a error operationError is called. If caller isn't interested of result, this can be left as NULL
	 */	
	void FindAndCreateMsgToContactByEmailL( const TDesC& aEmailAddress,
											CFSMailBox* aMailBox );

	
	/**
	 * Implements "Show Contact details" funcionality. 
	 * It will search for the Contact and then launch AIW call service.
	 *
	 * @param aAddress = email address of the contact
	 * @param TContactUpdateType = ETrue, call is made to default phonenumber. EFalse = User is queried of the number.
	 * @param aObserver, when AIW service is completed observer operationComplete( EShowContactDetailsL, NULL ) 
	 * is called if there's a error operationError is called. If caller isn't interested of result, this can be left as NULL
	 */
	void ShowContactDetailsL( const TDesC& aAddress, TContactUpdateType aType,
	                          MFSEmailUiContactHandlerObserver* aObserver = NULL );
	
	/**
	 * Implements "Call" funcionality synchronously
	 * It launches either VoIP or Normal phone according to system setting
	 *
	 * @param aContactLink = Link for contact to be called for. If NULL aContactNumber is used
	 * @param aContactNumber = Phonenumber of the contact, this is ignored if aContactLink
	 * @param aVoipOverride = Force VoIP call 
	 */
	void MakeAiwCallL( MVPbkContactLink* aContactLink, 
					   const TDesC& aContactNumber, 
					   TBool aVoipOverride = EFalse );

	/**
	 * Search for contacts by PCS engine. This is asynchronous call, and results are in
	 * ArrayUpdated( resultsArray )
	 *
	 * @param aText = new search text.
	 * @param aClsListObserver = observer to collect results
	 * @param aMailBox = current mailbox, needed to know which MRU list to use
	 */
	void SearchMatchesL( const TDesC& aText, MFSEmailUiContactHandlerObserver* aObserver,
						 CFSMailBox* aMailBox );
	
	/**
	 * Reset the status of contact Handlers, cancel all active asynchronous calls. This should be called
	 * at least when changing view.
	 */
	void Reset();
	
	/**
	 * Check whether the language currently is use, is supported by PCS engine. This enables/disables
	 * ShowContactDetailsL, FindAndCallToContactByEmailL, SearchMatchesL functions.
	 *
	 * @return ETrue = is supported  
	 */
	TBool IsLanguageSupportedL();
	
	/**
	 * Launch remote contact lookup in selector mode, where user can select contact to be used
	 * 	 
	 * @return ETrue = Contact retrieved
	 * @param aMailbox = from which mailbox do we search for.
	 * @param aQueryString = search string
	 * @param aDisplayName = function returns selected contact's display name
	 * @param aEmailAddress = function returns selected contacts's email address
	 */
	TBool GetNameAndEmailFromRemoteLookupL( CFSMailBox& aMailBox, const TDesC& aQueryString,
							  TDes& aDisplayname, TDes& aEmailAddress );

	/**
	 * Launch remote contact lookup in selector mode, where user can select contact to be used
	 * 	 
	 * @return HBufC* = Display name of the selected contact, NULL if no contact selected
	 * @param aMailbox = from which mailbox do we search for.
	 * @param aQuery = search string
	 * @param aPhoneNumber = function returns selected contact's phonenumber, RBuf will be allocated 
	 */
	HBufC* GetNameAndNumberFromRemoteLookupL( CFSMailBox& aMailBox, const TDesC& aQuery, RBuf& aPhoneNumber );
	
	/**
	 * Launch remote contact lookup for selected mailbox
	 * 	 
	 * @param aMailbox = from which mailbox do we search for.
	 */
	void LaunchRemoteLookupL( CFSMailBox& aMailBox );
	
	/**
	 * Launch remote contact lookup for selected mailbox with given query
	 * 	 
	 * @param aMailbox = from which mailbox do we search for.
	 * @param aQuery = search string to be used
	 */
	void LaunchRemoteLookupWithQueryL( CFSMailBox& aMailBox, const TDesC& aQuery );

	/**
	 * Compare emailAddress to last searched contact's emailaddress and return 
	 * the display name if they match
	 * 	 
	 * @param aEmailAddress = email address to compare
	 * @return HBufC* = Display name of the contact if addresses match,
	 * otherwise NULL
	 */
	HBufC* GetLastSearchNameL( const TDesC& aEmailAddress );	
	
	CVPbkContactManager* GetContactManager();
	
	/**
	 * Lauches multiple item fetch dialog through AIW launcher framework.
	 * Results are returned to observer
	 *
	 * @param aObserver = observer to collect results
	 */
	void GetAddressesFromPhonebookL( MFSEmailUiContactHandlerObserver* aObserver );
	
    // <cmail> video call
    /**
     * Enables/disables video call.
     *
     * @param aState ETrue for 'enable', EFalse for 'disable'
     */
    void SetVideoCall( TBool aState );
    // </cmail>

// from base class MFSEmailUiClsListsObserver

    void ArrayUpdatedL( const RPointerArray<CFSEmailUiClsItem>& aMatchingItems );	
    void OperationErrorL( TInt aErrorCode );
    
// from base class MVPbkSingleContactOperationObserver

    void VPbkSingleContactOperationCompleteL(
    		    MVPbkContactOperationBase& aOperation,
    		    MVPbkStoreContact* aContact );
    
    
	void VPbkSingleContactOperationComplete(
	    MVPbkContactOperationBase& aOperation,
	    MVPbkStoreContact* aContact );
	
	void VPbkSingleContactOperationFailedL(
		    MVPbkContactOperationBase& aOperation, TInt aError );
	void VPbkSingleContactOperationFailed(
	    MVPbkContactOperationBase& aOperation, TInt aError );

// from base class MVPbkContactStoreListObserver

     /**
      * From MVPbkContactStoreListObserver  
      * Called when the opening process is complete, ie. all stores have been reported
      * either failed or successfully opened.
      *
      * @since S60 v3.2
      */
      void OpenComplete();  
      
     /**
      * From MVPbkContactStoreListObserver  
      * Called when a contact store is ready to use.
      * @since S60 v3.2
      */
      void StoreReady(MVPbkContactStore& aContactStore);

     /**
      * From MVPbkContactStoreListObserver  
      * Called when a contact store becomes unavailable.
      * Client may inspect the reason of the unavailability and decide whether or not
      * it will keep the store opened (ie. listen to the store events).
      *
      * @since S60 v3.2
      *
      * @param aContactStore The store that became unavailable.
      * @param aReason The reason why the store is unavailable.
      *                This is one of the system wide error codes.
      */
      void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);

     /**
      * From MVPbkContactStoreListObserver  
      * Called when changes occur in the contact store.
      * @see TVPbkContactStoreEvent
      * @param aContactStore The store the event occurred in.
      * @param aStoreEvent   Event that has occured.
      */
      void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent);

//from base class MAiwNotifyCallback
	
	TInt HandleNotifyL(
	            TInt aCmdId,
	            TInt aEventId,
	            CAiwGenericParamList& aEventParamList,
	            const CAiwGenericParamList& aInParamList);
	
	void ClearObservers();
      
// from MCCAObserver      
    void CCASimpleNotifyL( TNotifyType aType, TInt aReason );
      
private:
    
    CFSEmailUiContactHandler( RFs& aSession );
    
    void ConstructL( RFs& aSession );
    
// <cmail> call observer's MFSEmailUiContactHandlerObserver::OperationErrorL( TContactHandlerCmd aCmd, TInt aError ) 
    void ObserverOperationErrorL( TContactHandlerCmd aCmd, TInt aErrorCode );
    

private:
    
	TInt SelectBetweenCsAndVoip() const;
    void HandleCallL( const RPointerArray<CFSEmailUiClsItem>& aMatchingItems );
    
	void CreateMessageL( const RPointerArray<CFSEmailUiClsItem>& aMatchingItems );
	void SendMessageL( CAiwGenericParamList& aEventParamList, TInt aServiceType );
 	void GetSmsAddressFromPhonebookAndSendL( MVPbkContactLink* aContactLink );
	void GetMmsAddressFromPhonebookAndSendL( MVPbkContactLink* aContactLink, TBool aIsVoiceMessage = EFalse );
 
    TBool IsRemoteLookupSupported();
    
    void GetContactFieldsL( RArray<TInt>& aFieldIds, RPointerArray<HBufC>& aNumbers, 
    					   MVPbkStoreContact* aContact );
    
    TPtrC GetContactTitle( MVPbkStoreContact* aContact );
    
    void ShowDetailsL( const RPointerArray<CFSEmailUiClsItem>& aMatchingItems );
    
    void FindContactLinkL( const TDesC& aDes );
    
    void FormatFieldIds();
    void ResetFieldIds();

    /*
     * Launches remote lookup UI
     * @param aQueryString Default query string in user input field
     * @param aResult Structure containing exit reason and selected item
     * @param aLookupMode The mode in which the lookup is performed.
     */
    void DoRemoteLookupL( CFSMailBox& aMailBox, const TDesC& aQueryString, 
        CPbkxRemoteContactLookupServiceUiContext::TResult& aResult, 
        CPbkxRemoteContactLookupServiceUiContext::TMode aLookupMode =
            CPbkxRemoteContactLookupServiceUiContext::EModeContactSelector );

    // Gets the name and email address from given contact item
    void GetNameAndEmail( TDes& aName, TDes& aEmail, CContactItem& aItem ) const;
    
    // Gets the phone number from given contact item
    HBufC* GetPhoneNumberAndNameL( RBuf& aPhoneNumber, CContactItem& aItem ) const;

    // Displays call query
    TBool CallQueryL( const TDesC& aDisplayName );

    // Creates copy of given CLS items and appends them to given array.
    void CopyItemsL( const RPointerArray<CFSEmailUiClsItem>& aItems,
        RPointerArray<CFSEmailUiClsItem>& aArray );

    // Sets the address used for previous contact search
    void SetPreviousEmailAddressL( const TDesC& aAddress );
    
    void RemoteContactQueryL();
    
    TContactHandlerCmd CurrentCommand();

private: // data
    
    CFSEmailUiClsListsHandler* iClsListHandler;
    TContactHandlerState iState;
    CFSEmailUiClsItem* iSearchMatch;

    MFSEmailUiContactHandlerObserver* iHandlerObserver;
    HBufC* iPreviousEmailAddress;
    CFSEmailUiClsItem* iLastClsItem;
    TBool iOpenComplete;
    TBool iStoreReady;
    
    CFSMailBox* iMailBox;
    TBool iUseSenderText;
    
    
    CVPbkContactManager* iContactManager;
    
    MVPbkContactOperationBase* iLinkOperation;
    MVPbkContactOperationBase* iLinkOperationFetch;
    const MVPbkContactLink* iCurrentLink;
    RPointerArray<CFSEmailUiClsItem> iContactObjects;
    
    MVPbkContactLinkArray* iLinksSet;
            
    CAiwServiceHandler* iServiceHandler;
    
    RArray<TInt> iLastnameFields;
    RArray<TInt> iFirstnameFields;
    RArray<TInt> iEmailFields;
    RArray<TInt> iPhoneNumberFields;
    
    MVPbkStoreContact* iContactForMsgCreation;
    TContactHandlerState iMsgCreationHelperState;
    
    /// Own: used to query preferred telephony information
    MPbkGlobalSetting* iPersistentSetting;
    RFs& iFs;
    
    /// Flag for indicating that caching error was received  
    TBool iCachingInProgressError;
    
    // Flag for making video call
    TBool iVideoCall;

    // connection to CCMA launcher    
    MCCAConnection* iConnection;

    CEikonEnv* iEnv;
};

#endif /*FREESTYLEEMAILUICONTACTHANDLER_H_*/
