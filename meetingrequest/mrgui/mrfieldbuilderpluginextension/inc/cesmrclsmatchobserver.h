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
* Description:  Base class for Contact and RMU match observers
 *
*/


#ifndef CESMRCLSMATCHOBSERVER_H
#define CESMRCLSMATCHOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>

#include <mpsresultsobserver.h>
#include <cpsrequesthandler.h>
#include <cpcsdefs.h>

// FORWARD DECLARATIONS
class CRepository;
class CPSRequestHandler;
class CESMRClsListsHandler;
class CVPbkContactManager;

// CLASS DECLARATION
/**
 * Base class for Contact and RMU match observers
 *
 *
 */

NONSHARABLE_CLASS( CESMRClsMatchObserver ) : public CBase, public MPsResultsObserver
    {

public:
    static CESMRClsMatchObserver* NewL( CRepository& aCr,
                CESMRClsListsHandler& aListsHandler,
                CPSRequestHandler& aRequestHandler,
                CVPbkContactManager* aContactManager );

    static CESMRClsMatchObserver* NewLC( CRepository& aCr,
                CESMRClsListsHandler& aListsHandler,
                CPSRequestHandler& aRequestHandler,
                CVPbkContactManager* aContactManager);

    /**
     * Destructor.
     *
     */
     virtual ~CESMRClsMatchObserver();

 public: // New methods

   /**
    * Sets new search text and either starts a search or buffers the search
    * request.
    *
    * @param aText new search text.
    */
    virtual void SearchMatchesL( const TDesC& aText );

   /**
    * Sets given input mode to current matcher.
    *
    * @param aInputMode weather the predictive input mode is to be set on/off.
    *               If EQwerty, multitap mode is set. If EItut predictive mode is set.
    */
    virtual void SetInputMode( TKeyboardModes aInputMode );

 public: // From MPsResultsObserver

     virtual void HandlePsResultsUpdate(RPointerArray<CPsClientData>& searchResults,
                                                RPointerArray<CPsPattern>& searchSeqs);
     virtual void HandlePsError(TInt aErrorCode);
     virtual void CachingStatus(TCachingStatus& aStatus, TInt& aError);
 protected:

    /**
     * C++ default constructor.
     *
     * @param aCR refenrence to aknfep cenrep
     * @param aListsHandler reference to listshandler object.
     * @param aContactListingService reference to contact listing service object who provides
     *                                  matcher object for the MRU list
     *
     */
     CESMRClsMatchObserver( CRepository& aCr,
                                CESMRClsListsHandler& aListsHandler,
                                CPSRequestHandler& aRequestHandler,
                                CVPbkContactManager* aContactManager );

    /**
     * Returns wheather current input mode is predictive (EItut) or QWERTY.
     *
     * @return TKeyboardModes EItut if predictive. EQwerty if anything else.
     */
    virtual TKeyboardModes GetInputMode();


    void ConstructL();

 protected: // Functions from base classes

 private:

    /**
     * Chech the status of PCS cache
     * ETrue, if Cache is OK. Otherwise EFalse
     */
     TBool CheckCacheStatusL();

     HBufC* CreateDisplayNameLC( const TDesC& aFirstname, const TDesC& aLastname, const TDesC& aEmailField );

     void HandlePsResultsUpdateL(
             RPointerArray<CPsClientData>& searchResults,
             RPointerArray<CPsPattern>& searchSeqs );

 protected: // data

    // Pointer to Predictive contact search engine client
    CPSRequestHandler* iRequestHandler; // owned

    // Pointer to Predictive contact search query object
    CPsQuery* iQuery;

    // Refenrence to AknFepCenRep
    CRepository&    iAknFepCenRep;

    // Reference to lists handler
    CESMRClsListsHandler& iListHandler;

    // What input mode is currently in use. EItut or QWERTY
    TKeyboardModes iInputMode;

    CVPbkContactManager* iContactManager;

    };


#endif  // CESMRCLSMATCHOBSERVER_H

// End of File
