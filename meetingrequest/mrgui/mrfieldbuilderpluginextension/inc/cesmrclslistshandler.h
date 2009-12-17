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
* Description:  CLS lists handler class definition
 *
*/

#ifndef CESMRCLSLISTSHANDLER_H
#define CESMRCLSLISTSHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>                     // TContactItemId
#include <bamdesca.h>                   // MDesCArray
#include <CPbkContactItem.h>
#include <CPcsDefs.h>

#include "cesmrclsitem.h"
#include "resmrstatic.h"

// FORWARD DECLARATIONS
class CPSRequestHandler;
class CRepository;
class CESMRClsMatchObserver;
class CESMRInputModeObserver;
class MESMRClsListsObserver;
class CFSMailBox;
class RFs;
class CPsQuery;
class CVPbkContactManager;

// CLASS DECLARATION
/**
 * Handler for cls lists. Heavy array operations are wrapped here.
 */
NONSHARABLE_CLASS( CESMRClsListsHandler ) : public CBase
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
     * @return CESMRClsListsHandler instance
     */
    static CESMRClsListsHandler* NewL( RFs& aFs, CVPbkContactManager* aContactManager );

    /**
     * Two-phased constructor.
     * Creates a new instance of class and leaves it on the cleanupstack
     *
     * @param aClsListObserver reference to lists observer object.
     * @param aMailBox reference to current mailbox item
     * @param aFs reference to file server session
     *
     * @return CESMRClsListsHandler instance
     */
    static CESMRClsListsHandler* NewLC( RFs& aFs, CVPbkContactManager* aContactManager  );

    /**
     * Destructor.
     */
    virtual ~CESMRClsListsHandler();

public: // new methods
    /**
     * Updates all the matching contact lists and informs the observer to update the view
     * This method is for cls contact match observer to call.
     *
     * @param aMatches ids of all the matches so far.
     */
    void UpdateContactMatchListsL( const RPointerArray<CESMRClsItem>& aMatches );

    /**
     * This method is for input mode observer to call. Set new input mode to both matchers
     *
     * @param aInputMode EItut if new input mode is predictive. EQwerty if multitap
     */
    void InputModeChangedL( TKeyboardModes aNewInputMode );

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

    /**
     * Sets a MESMRClsListsObserver
     */
    void SetObserver( MESMRClsListsObserver* aClsListObserver );
    
    /**
     * Sets mailbox to be used
     */
    void SetCurrentMailboxL( CFSMailBox* aMailBox );
    
    /**
     * handle operation errors
     */
    void OperationErrorL( TInt aErrorCode );
    
protected:
    /**
     * C++ default constructor.
     *
     * @param aClsListObserver reference to lists observer object.
     * @param aMailBox reference to current mailbox item
     * @param aFs reference to file server session
     */
    CESMRClsListsHandler( RFs& aFs, CVPbkContactManager* aContactManager );

private:
    /**
     * ConstructL
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
     *                            has not been added to the match list yet
     *
     * @return TBool ETrue if current item was new match. EFalse if matched item was the same as previous match
     */
    TBool ReadCLSInfoFromMRUListIndexL( MDesCArray& aTextArray,
            CESMRClsItem& aClsItem,
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
    TInt FindDuplicate( const CESMRClsItem& aClsItem,
                                  RPointerArray<CESMRClsItem>& aContacts );

    /**
     * Appends all the matches item from contact db and mru list to one list, which is given to ui to draw.
     *
     * @param aContactMatchesWithEmail list of so far matched contacts with email address
     * @param aMRUMatches list of so far matched MRU items
     * @param aContactMatchesWithoutEmail list of so far matched contacts without email address
     */
    RPointerArray<CESMRClsItem> ConstructOneListL(
            const RPointerArray<CESMRClsItem>& aContactMatchesWithEmail,
            const RPointerArray<CESMRClsItem>& aMRUMatches,
            const RPointerArray<CESMRClsItem>& aContactMatchesWithoutEmail );

    /**
     * Creates new cls item and copies all the data from given cls item to the new item.
     *
     * @param aClsItem old cls item where the data is been copied.
     *
     * @return CESMRClsItem* pointer to the newly created cls item. Pointer is leaved to the
     *                       cleanupstack as well.
     */
    CESMRClsItem* CopyClsItemLC( const CESMRClsItem& aClsItem );

    /**
     * Constructs MRU datastore Uri for Mailbox
     **/
    void GetMruDatastoreUriFromMailbox( CFSMailBox& aMailbox, HBufC& aUri );

private: // Implementation
    void SetSearchSettingsForPcsMatchObserverL();

private: // data
    // Three pointer array's for different lists
    RPointerArray<CESMRClsItem>         iMatchingCompleteContacts;
    RPointerArray<CESMRClsItem>         iMatchingMRUContacts;
    RPointerArray<CESMRClsItem>         iMatchingMissingEmailContacts;

    // owned
    MDesCArray*                         iAllMRUContacts;    

    // Pointer to the service instance, owned
    CPSRequestHandler*                  iRequestHandler; 

    // Pointer AknFep Central repository,  owned
    CRepository*                        iAknFepCenRep; 

    //not owned
    CVPbkContactManager*                iContactManager;
    
    // Pointer to input observer,  owned
    CESMRInputModeObserver*             iInputObserver; 

    MESMRClsListsObserver*              iClsListObserver;
    //owned
    CESMRClsMatchObserver*              iPcsMatchObserver;

    // owned
    HBufC*                              iSearchedText; 

    // Pointer to actual contact database.
    RFs&                                iFs;

    // Reference to current mailbox
    CFSMailBox*                         iMailBox; 

    TBool                               iRemoteLookupSupported;
    };

#endif  // CESMRCLSLISTSHANDLER_H

// End of File
