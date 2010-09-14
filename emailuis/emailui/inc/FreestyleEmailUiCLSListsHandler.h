/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CLS lists handler class definition
*
*/


#ifndef FREESTYLEEMAILUICLSLISTSHANDLER_H
#define FREESTYLEEMAILUICLSLISTSHANDLER_H

//  INCLUDES
#include <CPcsDefs.h>

// FORWARD DECLARATIONS
class CPSRequestHandler;
class CRepository;
class CFSEmailUiClsMatchObserver;
class MFSEmailUiClsListsObserver;
class CFSEmailUiInputModeObserver;
class CFSMailBox;
class RFs;
class CVPbkContactManager;
class CFSEmailUiClsItem;
class MDesCArray;

_LIT(KDefaultMailBoxURI, "mailbox://");
_LIT(KDefaultMailBoxURISeparator, "/");

// CLASS DECLARATION
/**
 * Freestyle email application cls lists handler
 *
 *
 */
 
class CFSEmailUiClsListsHandler : public CBase
	{

 public:
  	  

    /**
     * Two-phased constructor.
     * Creates a new instance of class
     *
	 * @param aClsListObserver reference to lists observer object.
     * @param aMailBox reference to current mailbox item
	 * @param aFs reference to file server session
     *
     * @returns CFSEmailUiClsListsHandler instance
     */
  	 static CFSEmailUiClsListsHandler* NewL( RFs& aFs, CVPbkContactManager* aContactManager );
  	
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     *
	 * @param aClsListObserver reference to lists observer object.
     * @param aMailBox reference to current mailbox item
	 * @param aFs reference to file server session
	 *
     * @returns CFSEmailUiClsListsHandler instance
     */
  	 static CFSEmailUiClsListsHandler* NewLC( RFs& aFs, CVPbkContactManager* aContactManager  );

    /**
     * Destructor.
     *
     */  
  	 virtual ~CFSEmailUiClsListsHandler();

public:  // new methods 


   /**
    * Updates all the matching contact lists and informs the observer to update the view
	* This method is for cls contact match observer to call.
    *
    * @param aMatches ids of all the matches so far.
    */
	void UpdateContactMatchListsL( const RPointerArray<CFSEmailUiClsItem>& aMatches );


 	/**
 	 * This method is for input mode observer to call. Set new input mode to both matchers
 	 *
 	 * @param aInputMode EItut if new input mode is predictive. EQwerty if multitap
 	 *
 	 */
 	void InputModeChangedL( TKeyboardModes aInputMode );
 
 	/**
 	 * Returns the text we are currently matching
	 *
	 * @return TDesC& reference to current search string
	 */
	const TDesC& CurrentSearchText();
 
	/**
 	 * Sets new search text and either starts a search or buffers the search 
 	 * request. 
	 *
	 * @param aText new search text.
	 */
	void SearchMatchesL( const TDesC& aText );
	
	/**
	 * Check if the input language is supported by search engine
	 */
	TBool IsLanguageSupportedL();

	void SetObserver( MFSEmailUiClsListsObserver* aClsListObserver );
	void SetCurrentMailboxL( CFSMailBox* aMailBox );

	void OperationErrorL( TInt aErrorCode );
	
	TBool UseLastNameFirstOrder();
	
protected:

    /**
     * C++ default constructor.
     *
	 * @param aClsListObserver reference to lists observer object.
     * @param aMailBox reference to current mailbox item
	 * @param aFs reference to file server session
     *
     */
  	 CFSEmailUiClsListsHandler( RFs& aFs, CVPbkContactManager* aContactManager );

private:

    /**
     * ConstructL
     *
     */
	void ConstructL();
	

     /**
     * Reads display name & email address from current match item index from the aTextArray
     * and writes those informations to given clsitem
     *
	 * @param aTextArray MRU list of display names and email addresses
	 * @param aClsItem new clsitem where the information is written
	 * @param aCurrentMatchIndex index of the display name or email address that matched the search
	 * @param aPreviousMatchIndex index of the previous match. This is for checking that the item
	 *								 has not been added to the match list yet
     *
     * @return TBool ETrue if current item was new match. EFalse if matched item was the same as previous match
     */
	TBool ReadCLSInfoFromMRUListIndexL( MDesCArray& aTextArray,
										CFSEmailUiClsItem& aClsItem,
										const TInt aCurrentMatchIndex,
										const TInt aPreviousMatchIndex );

    /**
     * Searches given cls item from the given list. If duplicate is found, it is returned.
     *
	 * @param aClsItem cls item to be found from the list
	 * @param aContacts list of contacts
     *
     * @return index of the item, KErrNotFound if it doesn't exist
     */
	TInt FindDuplicate( const CFSEmailUiClsItem& aClsItem,
								  RPointerArray<CFSEmailUiClsItem>& aContacts );

    /**
     * Appends all the matches item from contact db and mru list to one list, which is given to ui to draw.
     *
	 * @param aContactMatchesWithEmail list of so far matched contacts with email address 
	 * @param aMRUMatches list of so far matched MRU items
 	 * @param aContactMatchesWithoutEmail list of so far matched contacts without email address
     *
     */
	RPointerArray<CFSEmailUiClsItem> ConstructOneListL( 
									const RPointerArray<CFSEmailUiClsItem>& aContactMatchesWithEmail,
									  const RPointerArray<CFSEmailUiClsItem>& aMRUMatches,
									  const RPointerArray<CFSEmailUiClsItem>& aContactMatchesWithoutEmail );

 	
 	/**
 	 * Creates new cls item and copies all the data from given cls item to the new item.
 	 *
	 * @param aClsItem old cls item where the data is been copied.
	 *
	 * @return CFSEmailUiClsItem* pointer to the newly created cls item. Pointer is leaved to the
	 *								cleanupstack as well.
 	 */
 	CFSEmailUiClsItem* CopyClsItemLC( const CFSEmailUiClsItem& aClsItem );
 	
 	/**
 	 * Constructs MRU datastore Uri for Mailbox
 	 **/
 	void GetMruDatastoreUriFromMailbox( CFSMailBox& aMailbox, HBufC& aUri );
    
private: //Functions
	
	void SetSearchSettingsForPcsMatchObserverL();
	
	
private: // data

	// Three pointer array's for different lists
	RPointerArray<CFSEmailUiClsItem> 	iMatchingCompleteContacts;
	RPointerArray<CFSEmailUiClsItem> 	iMatchingMRUContacts;
    RPointerArray<CFSEmailUiClsItem> 	iMatchingMissingEmailContacts;

	MDesCArray*							iAllMRUContacts;	// owned

    // Pointer to the service instance
	CPSRequestHandler* 					iRequestHandler; // owned
    
    // Pointer AknFep Central repository
    CRepository*						iAknFepCenRep; // owned
    
    CVPbkContactManager* 				iContactManager;
    // Pointer to input observer		
    CFSEmailUiInputModeObserver*		iInputObserver; // owned
    
    MFSEmailUiClsListsObserver* 		iClsListObserver;
    CFSEmailUiClsMatchObserver*			iPcsMatchObserver;// owned
    
    HBufC*								iSearchedText; // owned
    
    // Pointer to actual contact database.
    RFs&								iFs;			
    
    // Reference to current mailbox
    CFSMailBox* 						iMailBox;

    TBool                               iRemoteLookupSupported;
  
    TBool                               iUseLastNameFirstOrder;
    };

#endif  // FREESTYLEEMAILUICLSLISTSHANDLER_H

// End of File
