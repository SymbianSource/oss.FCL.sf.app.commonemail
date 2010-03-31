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


#include "emailtrace.h"
#include "cesmrclsmatchobserver.h"
#include "cesmrclslistshandler.h"
#include "esmrfieldbuilderdef.h"

// for PCS support
#include <cpsrequesthandler.h>
#include <cpssettings.h>
#include <cpsquery.h>
#include <cpsqueryitem.h>
#include <cpcsdefs.h>
#include <mvpbkcontactlink.h>
#include <cvpbkcontactmanager.h>
#include <aknnotewrappers.h>
#include <centralrepository.h>                          // CRepository
#include <aknfepinternalcrkeys.h>                       // KCRUidAknFep, KAknFepPredTxtFlag
#include <vpbkeng.rsg>

//Constants

// Unnamed namespace for local definitions
namespace{ // codescanner::namespace

const TInt KMatchLen = 10;

template<typename T> class CleanupResetAndDestroyClose
    {
public:
    inline static void PushL( T& aRef );
private:
    static void Close( TAny *aPtr );
    };

template<typename T> inline void CleanupResetAndDestroyClosePushL( T& aRef );

template<typename T> inline void CleanupResetAndDestroyClose<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &Close, &aRef ) );
    }

template<typename T> void CleanupResetAndDestroyClose<T>::Close( TAny *aPtr )
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    static_cast<T*>(aPtr)->Close();
    }

template<typename T> inline void CleanupResetAndDestroyClosePushL( T& aRef )
    {
    CleanupResetAndDestroyClose<T>::PushL( aRef );
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::NewL
// -----------------------------------------------------------------------------
//
CESMRClsMatchObserver* CESMRClsMatchObserver::NewL(CRepository& aCr,
        CESMRClsListsHandler& aListsHandler,
        CPSRequestHandler& aRequestHandler,
        CVPbkContactManager* aContactManager)
    {
    FUNC_LOG;
    CESMRClsMatchObserver* api = CESMRClsMatchObserver::NewLC( aCr, aListsHandler,
            aRequestHandler, aContactManager );

    CleanupStack::Pop( api );
    return api;
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::NewLC
// -----------------------------------------------------------------------------
//
CESMRClsMatchObserver* CESMRClsMatchObserver::NewLC(CRepository& aCr,
        CESMRClsListsHandler& aListsHandler,
        CPSRequestHandler& aRequestHandler,
        CVPbkContactManager* aContactManager )
    {
    FUNC_LOG;
    CESMRClsMatchObserver* api = new (ELeave) CESMRClsMatchObserver(
            aCr,
            aListsHandler,
            aRequestHandler,
            aContactManager );

    CleanupStack::PushL( api );
    api->ConstructL();
    return api;
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::CESMRClsMatchObserver
// -----------------------------------------------------------------------------
//
CESMRClsMatchObserver::CESMRClsMatchObserver(
                                CRepository& aCr,
                                CESMRClsListsHandler& aListsHandler,
                                CPSRequestHandler& aRequestHandler,
                                CVPbkContactManager* aContactManager ) :
    iRequestHandler( &aRequestHandler ),
    iAknFepCenRep( aCr ),
    iListHandler( aListsHandler ),
    iInputMode(EQwerty),
    iContactManager( aContactManager )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::~CESMRClsMatchObserver
// -----------------------------------------------------------------------------
//
CESMRClsMatchObserver::~CESMRClsMatchObserver()
    {
    FUNC_LOG;
    delete iQuery;
    if(iRequestHandler)
        {
        iRequestHandler->RemoveObserver(this);
        }
    }

void CESMRClsMatchObserver::ConstructL()
    {
    FUNC_LOG;
    iRequestHandler->AddObserverL( this );
    iInputMode = GetInputMode();
    iQuery = CPsQuery::NewL();
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::SearchMatchesL
// -----------------------------------------------------------------------------
//
void CESMRClsMatchObserver::SearchMatchesL( const TDesC& aText )
    {
    FUNC_LOG;
    if( iRequestHandler )
        {
        TInt cacheStatus = CheckCacheStatusL();
        if ( !cacheStatus )
            {
            // Reset query
            iQuery->Reset();

            // Create one queryitem for every character in the query string and append it to the query object
            // Every queryitem is filled with two fields :
            //  1) a character from the query string
            //  2) the keyboard mode using which the query was entered

            TInt itemCount = aText.Length();
            
            for ( TInt i = 0; i < itemCount; i++ )
            {
               // Add a query item
               CPsQueryItem* item = CPsQueryItem::NewL();
               item->SetCharacter(aText[i]);
               item->SetMode(iInputMode);    // EItut refers to numeric keypad
                                     // Use EQwerty if QWERTY keyboard is used
               iQuery->AppendL(*item);
            }

            // Initiate a search request
            iRequestHandler->SearchL(*iQuery);
            }
        else //Cache isn't ready
            {
            iListHandler.OperationErrorL( cacheStatus );
            }

        }
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::SetInputModeL
// -----------------------------------------------------------------------------
//
void CESMRClsMatchObserver::SetInputMode( TKeyboardModes aInputMode )
    {
    FUNC_LOG;
    iInputMode = aInputMode;
    }


// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::HandleMatchComplete
// -----------------------------------------------------------------------------
//
void CESMRClsMatchObserver::HandlePsResultsUpdate(
        RPointerArray<CPsClientData>& searchResults,
        RPointerArray<CPsPattern>& searchSeqs)
    {
    FUNC_LOG;
    TRAP_IGNORE( HandlePsResultsUpdateL( searchResults, searchSeqs) );
    }

void CESMRClsMatchObserver::HandlePsError(TInt aErrorCode)
    {
    FUNC_LOG;
    TRAP_IGNORE( iListHandler.OperationErrorL( aErrorCode ) );
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::CachingStatus
// -----------------------------------------------------------------------------
//
void CESMRClsMatchObserver::CachingStatus(
        TCachingStatus& aStatus, TInt& /*aError*/)
    {
    FUNC_LOG;
    TInt err = KErrNone;
    switch ( aStatus )
        {
        case ECachingComplete:
            err = KErrNone;
            break;
        case ECachingNotStarted:
            err = KErrNotFound;
            break;
        case ECachingInProgress:
            err = KErrNotReady;
            break;
        case ECachingCompleteWithErrors:
            err = KErrCorrupt;
            break;
        default:
            break;
        }

    TRAP_IGNORE( iListHandler.OperationErrorL( err ) );
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::IsCurrentInputModePredictive
// -----------------------------------------------------------------------------
//
TKeyboardModes CESMRClsMatchObserver::GetInputMode()
    {
    FUNC_LOG;
    TInt repVal = 0;
    // 1 = predictive, 0 = non predictive
    iAknFepCenRep.Get( KAknFepPredTxtFlag, repVal );

    //If true, EItut (predictive)
    if( repVal == 1 )
        {
        return EItut;
        }
    return EQwerty;
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::CheckCacheStatusL
// -----------------------------------------------------------------------------
//
TBool CESMRClsMatchObserver::CheckCacheStatusL()
    {
    FUNC_LOG;
    // Get the caching status synchronously
    // 'status' has the caching status
    // 'err' has KErrNone or any caching errors
    TCachingStatus status;
    TInt err = iRequestHandler->GetCachingStatusL(status);

    switch (status)
        {
        case ECachingComplete:
            return KErrNone;
        case ECachingNotStarted:
            return KErrNotFound;
        case ECachingInProgress:
            return KErrNotReady;
        case ECachingCompleteWithErrors:
            return KErrCorrupt;
        default:
            break;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::CreateDisplayNameLC
// -----------------------------------------------------------------------------
//
HBufC* CESMRClsMatchObserver::CreateDisplayNameLC( const TDesC& aFirstname, const TDesC& aLastname, const TDesC& aEmailField )
    {
    FUNC_LOG;
    HBufC* displayname;
    if ( aFirstname.Length() == 0)
        {
        if ( aLastname.Length() == 0 )
            {
            //There's just the email address for Displayname
            displayname = HBufC::NewLC( aEmailField.Length() );
            displayname->Des().Copy( aEmailField );
            }
        else
            {
            //Lastname = displayname
            displayname = HBufC::NewLC( aLastname.Length() );
            displayname->Des().Copy( aLastname );
            }
        }

    else if ( aLastname.Length() == 0 )
        {
        //Firstname = displayname
        displayname = HBufC::NewLC( aFirstname.Length() );
        displayname->Des().Copy( aFirstname );
        }

    else
        {
        //Displayname = firstname + " " + lastname
        displayname = HBufC::NewLC( aFirstname.Length() +
                                    1 +
                                    aLastname.Length() );
        displayname->Des().Copy( aFirstname );
        displayname->Des().Append( KEmptySpace );
        displayname->Des().Append( aLastname );
        }
    return displayname;
    }

// -----------------------------------------------------------------------------
// CESMRClsMatchObserver::HandlePsResultsUpdateL
// -----------------------------------------------------------------------------
//
void CESMRClsMatchObserver::HandlePsResultsUpdateL(
        RPointerArray<CPsClientData>& searchResults,
        RPointerArray<CPsPattern>& searchSeqs)
    {
    FUNC_LOG;
    RPointerArray<CESMRClsItem> contacts;
    CleanupResetAndDestroyClosePushL( contacts );
    CleanupResetAndDestroyClosePushL( searchResults );
    CleanupResetAndDestroyClosePushL( searchSeqs );

    // Iterate through all the search results
    for( TInt counter = 0 ; counter < searchResults.Count(); counter++ )
        {
        // Get search result
        CPsClientData* result = searchResults[counter];

        // Get the order of the data fields in result
        RArray<TInt> dataOrder;
        CleanupClosePushL( dataOrder );
        iRequestHandler->GetDataOrderL( *( result->Uri() ), dataOrder );

        HBufC* firstname = HBufC::NewLC( KMaxTextLength );
        HBufC* lastname = HBufC::NewLC( KMaxTextLength );
        RPointerArray<HBufC> emailFields;
        CleanupResetAndDestroyClosePushL( emailFields );
        // Iterate through all the data fields in the result.
        // Collect the result and resolve which fields did match.
        // Email fields are collected into array
        for( TInt dataField = 0 ; dataField < dataOrder.Count(); dataField++ )
            {
            switch( dataOrder[dataField] )
                {
                case R_VPBK_FIELD_TYPE_FIRSTNAME:
                    firstname->Des().Append( *(result->Data(dataField)) );
                    break;

                case R_VPBK_FIELD_TYPE_LASTNAME:
                    lastname->Des().Append( *(result->Data(dataField)) );
                    break;

                case R_VPBK_FIELD_TYPE_EMAILGEN:
                case R_VPBK_FIELD_TYPE_EMAILHOME:
                case R_VPBK_FIELD_TYPE_EMAILWORK:
                    {
                    if( result->Data( dataField ) )
                        {
                        if( (*(result->Data( dataField ))).Length() > 0 )
                            {
                            HBufC* email = HBufC::NewLC( KMaxTextLength );
                            email->Des().Append( *(result->Data( dataField )) );
                            emailFields.Append( email );
                            CleanupStack::Pop( email );
                            }
                        }
                    }
                    break;
                }
            }



        TDesC& sourceUri = *( result->Uri() );
        TBool isMruItem = sourceUri.Find( KDefaultMailBoxURI ) != KErrNotFound;

        MVPbkContactLink* contactLink = NULL;

        if( !isMruItem )
            {
            contactLink = iRequestHandler->ConvertToVpbkLinkLC( *result, *iContactManager );
            CleanupStack::Pop();
            // Push M-pointer to contactLink into CleanupStack for proper cleanup
            CleanupDeletePushL( contactLink );
            }

        // Determine how many contact address to create:
        // If there's no email fields, create one item without email address
        if ( emailFields.Count() == 0 )
            {

            // Use Ps engine to check if the text matches the search query
            CDesCArray* matchSet = new (ELeave) CDesCArrayFlat(KMatchLen);
            CleanupStack::PushL( matchSet );

            // Create display name, this will be used in UI.
            HBufC* displayname = CreateDisplayNameLC( *firstname, *lastname, KNullDesC );

            RArray<TPsMatchLocation> matchLocation;
            CleanupClosePushL( matchLocation );
            iRequestHandler->LookupL( *iQuery, *displayname, *matchSet, matchLocation );

            // Create contact item and set highlighting for it
            CESMRClsItem* contactItem = CESMRClsItem::NewLC();
            contactItem->SetDisplayNameL( *displayname );
            contactItem->SetHighlightsL( matchLocation );
            if ( contactLink )
                {
                contactItem->SetContactLinkL( *contactLink );
                }
            contactItem->SetIsMruItem( isMruItem );
            contacts.AppendL( contactItem );
            CleanupStack::Pop( contactItem );

            CleanupStack::Pop( &matchLocation );
            matchLocation.Close();
            CleanupStack::PopAndDestroy( displayname );
            CleanupStack::PopAndDestroy( matchSet );

            }
        else
            {
            // Iterate through all the email fields, check if they match the search criteria and
            // create new contact item for each one that matches
            for(TInt fieldIndex = 0; fieldIndex < emailFields.Count(); ++fieldIndex )
                {

                // Create display name, this will be used in UI.
                HBufC* displayname = CreateDisplayNameLC( *firstname,
                                                          *lastname,
                                                          *emailFields[fieldIndex] );

                // Create match text field, containing display field and email address
                HBufC* matchText = HBufC::NewLC( displayname->Des().Length() +
                                1 +
                                emailFields[fieldIndex]->Des().Length() );

                matchText->Des().Copy( *displayname );
                matchText->Des().Append( KEmptySpace );
                matchText->Des().Append( *emailFields[fieldIndex] );

                // Use Ps engine to check if the text matches the search query
                CDesCArray* matchSet = new (ELeave) CDesCArrayFlat(KMatchLen);
                CleanupStack::PushL( matchSet );

                RArray<TPsMatchLocation> matchLocation;
                iRequestHandler->LookupL( *iQuery, *matchText, *matchSet, matchLocation );
                CleanupClosePushL( matchLocation );

                if( matchLocation.Count() > 0 )
                    {
                    // Create contact item and set highlighting for it
                    CESMRClsItem* contactItem = CESMRClsItem::NewLC();
                    contactItem->SetDisplayNameL( *displayname );
                    contactItem->SetIsMruItem( isMruItem );
                    contactItem->SetEmailAddressL( *emailFields[fieldIndex] );
                    contactItem->SetHighlightsL(matchLocation);
                    if ( contactLink )
                        {
                        contactItem->SetContactLinkL( *contactLink );
                        }
                    if ( emailFields.Count() > 1 )
                        {
                        contactItem->SetMultipleEmails( );
                        }
                    contacts.AppendL( contactItem );
                    CleanupStack::Pop( contactItem );
                    }

                CleanupStack::PopAndDestroy( &matchLocation );
                CleanupStack::PopAndDestroy( matchSet );
                CleanupStack::PopAndDestroy( matchText );
                CleanupStack::PopAndDestroy( displayname );
                }
            }
        
        if ( contactLink )
            {
            CleanupStack::PopAndDestroy(); //codescanner::cleanup
            }

        // De-allocate name fields, first & last name
        CleanupStack::PopAndDestroy( &emailFields );
        CleanupStack::PopAndDestroy( lastname );
        CleanupStack::PopAndDestroy( firstname );
        CleanupStack::PopAndDestroy( &dataOrder );
        }

    iListHandler.UpdateContactMatchListsL(contacts);
    CleanupStack::PopAndDestroy( &searchSeqs );
    CleanupStack::PopAndDestroy( &searchResults );
    CleanupStack::PopAndDestroy( &contacts );
    }

//EOF

