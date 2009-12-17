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
* Description:  Base class for Contact and RMU match observers
*
*/


// for PCS support
#include "emailtrace.h"
#include <CPsQueryItem.h>
#include <CPsRequestHandler.h>

#include <aknnotewrappers.h>
#include <AknFepInternalCRKeys.h>						// KCRUidAknFep, KAknFepPredTxtFlag
#include <VPbkEng.rsg>

#include "FreestyleEmailUiCLSListsHandler.h"			// CFSEmailUiClsListsHandler
#include "FreestyleEMailUiCLSMatchObserverInterface.h"	// CFSEmailUiClsMatchObserver
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiCLSItem.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiLayoutHandler.h"


// ================= MEMBER FUNCTIONS ==========================================

CFSEmailUiClsMatchObserver* CFSEmailUiClsMatchObserver::NewL(CRepository& aCr,  
		CFSEmailUiClsListsHandler& aListsHandler,
		CPSRequestHandler& aRequestHandler,
		CVPbkContactManager* aContactManager)
	{
    FUNC_LOG;
	CFSEmailUiClsMatchObserver* api = CFSEmailUiClsMatchObserver::NewLC( aCr, aListsHandler,
			aRequestHandler, aContactManager );
	
	CleanupStack::Pop( api );
	return api;	
	}

CFSEmailUiClsMatchObserver* CFSEmailUiClsMatchObserver::NewLC(CRepository& aCr,  
		CFSEmailUiClsListsHandler& aListsHandler,
		CPSRequestHandler& aRequestHandler,
		CVPbkContactManager* aContactManager )
	{
    FUNC_LOG;
	CFSEmailUiClsMatchObserver* api = new (ELeave) CFSEmailUiClsMatchObserver( 
			aCr, 
			aListsHandler,
			aRequestHandler,
			aContactManager );
	
	CleanupStack::PushL( api );
	api->ConstructL();
	return api;	
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsMatchObserver::CFSEmailUiClsMatchObserver
// -----------------------------------------------------------------------------
CFSEmailUiClsMatchObserver::CFSEmailUiClsMatchObserver(
 								CRepository& aCr,  
								CFSEmailUiClsListsHandler& aListsHandler,
								CPSRequestHandler& aRequestHandler,
								CVPbkContactManager* aContactManager ) :
    iRequestHandler( &aRequestHandler ),
    iAknFepCenRep( aCr ),
    iListHandler( aListsHandler ),
    iInputMode( EQwerty ),
    iContactManager( aContactManager )
	{
    FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsMatchObserver::~CFSEmailUiClsMatchObserver
// -----------------------------------------------------------------------------
CFSEmailUiClsMatchObserver::~CFSEmailUiClsMatchObserver()
    {
    FUNC_LOG;
    delete iQuery;
    if ( iRequestHandler )
        {
        iRequestHandler->RemoveObserver( this );
        }
    }

void CFSEmailUiClsMatchObserver::ConstructL()
	{
    FUNC_LOG;
	iRequestHandler->AddObserverL( this );
	iInputMode = GetInputMode();
	iQuery = CPsQuery::NewL();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiClsMatchObserver::SearchMatchesL
// -----------------------------------------------------------------------------
void CFSEmailUiClsMatchObserver::SearchMatchesL( const TDesC& aText )
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
			//	1) a character from the query string 
			//	2) the keyboard mode using which the query was entered
			
			TInt itemCount = aText.Length();
			if ( itemCount > 50 )
				{
				itemCount = 50;
				}
			for ( TInt i = 0; i < itemCount; i++ )
			{    
			   // Add a query item
			   CPsQueryItem* item = CPsQueryItem::NewL();
			   item->SetCharacter(aText[i]);   
			   item->SetMode(iInputMode);	 // EItut refers to numeric keypad
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
// CFSEmailUiClsMatchObserver::SetInputModeL
// -----------------------------------------------------------------------------
void CFSEmailUiClsMatchObserver::SetInputMode( TKeyboardModes aInputMode )
	{
    FUNC_LOG;
	iInputMode = aInputMode;
	}


// -----------------------------------------------------------------------------
// CFSEmailUiClsMatchObserver::HandleMatchComplete
// -----------------------------------------------------------------------------
void CFSEmailUiClsMatchObserver::HandlePsResultsUpdate( 
        RPointerArray<CPsClientData>& aSearchResults,
        RPointerArray<CPsPattern>& searchSeqs )
	{
    FUNC_LOG;
	TRAP_IGNORE( HandlePsResultsUpdateL( aSearchResults, searchSeqs) );
	}
// -----------------------------------------------------------------------------
// CFSEmailUiClsMatchObserver::HandleMatchComplete
// -----------------------------------------------------------------------------
void CFSEmailUiClsMatchObserver::HandlePsResultsUpdateL( 
        RPointerArray<CPsClientData>& aSearchResults,
        RPointerArray<CPsPattern>& aSearchSeqs )
	{
    FUNC_LOG;
    RPointerArray<CFSEmailUiClsItem> contacts;
    CleanupResetAndDestroyClosePushL( contacts );
    CleanupResetAndDestroyClosePushL( aSearchResults );
    CleanupResetAndDestroyClosePushL( aSearchSeqs );
    
    // Iterate through all or maximum number of search results
    CFreestyleEmailUiAppUi* appUi = 
        static_cast<CFreestyleEmailUiAppUi*>( CCoeEnv::Static()->AppUi() );
    TInt maxPcsMatches = appUi->LayoutHandler()->MaxPcsMatches();
    
	for ( TInt counter = 0 ; counter < aSearchResults.Count() && counter < maxPcsMatches ; counter++ )
		{
		// Get search result
		CPsClientData* result = aSearchResults[counter];

	    // Get the order of the data fields in result
	    RArray<TInt> dataOrder;
	    CleanupClosePushL( dataOrder );
	    iRequestHandler->GetDataOrderL( *( result->Uri() ), dataOrder );
		
		TPtrC firstname( KNullDesC );
		TPtrC lastname( KNullDesC );
		RArray<TPtrC> emailFields;
		CleanupClosePushL( emailFields );
		// Iterate through all the data fields in the result.
		// Collect the result and resolve which fields did match.
		// Email fields are collected into array
		for ( TInt dataField = 0 ; dataField < dataOrder.Count(); dataField++ )
			{
			switch ( dataOrder[dataField] )
				{
				case R_VPBK_FIELD_TYPE_FIRSTNAME:
					firstname.Set( *(result->Data(dataField)) );
					break;
					
				case R_VPBK_FIELD_TYPE_LASTNAME:
					lastname.Set( *(result->Data(dataField)) );
					break;

				case R_VPBK_FIELD_TYPE_EMAILGEN:
				case R_VPBK_FIELD_TYPE_EMAILHOME:					
				case R_VPBK_FIELD_TYPE_EMAILWORK:
					{
					if ( result->Data( dataField ) )
						{
						if ( result->Data( dataField )->Length() > 0 )
							{
							TPtrC email( *result->Data( dataField ) );
							ChopEmailEntriesList(email, emailFields);
							}
						}
					}
					break;
				}
			}


		
		TDesC& sourceUri = *( result->Uri() );
		TBool isMruItem = sourceUri.Find( KDefaultMailBoxURI ) != KErrNotFound;

		MVPbkContactLink* contactLink = NULL;
		
		if ( !isMruItem )
			{
			contactLink = iRequestHandler->ConvertToVpbkLinkLC( *result, *iContactManager );
			CleanupStack::Pop( contactLink );
			}
		
		HBufC* displayname;
		
		// Determine how many contact address to create:
		// If there's no email fields, create one item without email address
		if ( emailFields.Count() == 0 )
			{
			
			// Use Ps engine to check if the text matches the search query
			CDesCArray* matchSet = new (ELeave) CDesCArrayFlat(10);
			CleanupStack::PushL( matchSet );
			
			// Create display name, this will be used in UI.
			displayname = TFsEmailUiUtility::CreateDisplayNameLC( firstname, lastname, KNullDesC );
			
			RArray<TPsMatchLocation> matchLocation;
			CleanupClosePushL( matchLocation );
			iRequestHandler->LookupL( *iQuery, *displayname, *matchSet, matchLocation );
			
			// Create contact item and set highlighting for it
			CFSEmailUiClsItem* contactItem = CFSEmailUiClsItem::NewLC();
			contactItem->SetDisplayNameL( *displayname );
			contactItem->SetHighlights( matchLocation );
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
			for ( TInt fieldIndex = 0; fieldIndex < emailFields.Count(); fieldIndex++ )
				{
				
				// Create display name, this will be used in UI. If the
				// contact does not contain either firstname nor lastname,
				// the displayname is left empty.
				displayname = TFsEmailUiUtility::CreateDisplayNameLC(
					firstname, lastname );
				
				// Create match text field, containing display field and email address
				HBufC* matchText = HBufC::NewLC( displayname->Length() +
								KSpace().Length() +
								emailFields[fieldIndex].Length() );
				
				//This condition is for checking if there is no display name like in the case of MRU and therefore 
                //we dont need to add displayname + KSpace(unnnecessary space) which is causing problem in drawing Underline
				if (displayname->Length() > 0)
				    {

				matchText->Des().Copy( *displayname );
				matchText->Des().Append( KSpace );

				    }
				
				matchText->Des().Append( emailFields[fieldIndex] );
				

				// Use Ps engine to check if the text matches the search query
				CDesCArray* matchSet = new (ELeave) CDesCArrayFlat(10);
				CleanupStack::PushL( matchSet );
				
				RArray<TPsMatchLocation> matchLocation;
				CleanupClosePushL( matchLocation );
				iRequestHandler->LookupL( *iQuery, *matchText, *matchSet, matchLocation );
				
				if ( matchLocation.Count() > 0 )
					{
					// Create contact item and set highlighting for it
					CFSEmailUiClsItem* contactItem = CFSEmailUiClsItem::NewLC();
					contactItem->SetDisplayNameL( *displayname );
					contactItem->SetIsMruItem( isMruItem );
					contactItem->SetEmailAddressL( emailFields[fieldIndex] );
					contactItem->SetHighlights( matchLocation );
					if ( contactLink )
						{
						contactItem->SetContactLinkL( *contactLink );
						}
					if ( emailFields.Count() > 1 )
						{
						contactItem->SetMultipleEmails( ETrue );
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
		
		if ( !isMruItem )
			{
			delete contactLink;
			}
		
		// Close arrays
		CleanupStack::PopAndDestroy( &emailFields );
        CleanupStack::PopAndDestroy( &dataOrder );
		}

	iListHandler.UpdateContactMatchListsL( contacts );
	CleanupStack::PopAndDestroy( &aSearchSeqs );
	CleanupStack::PopAndDestroy( &aSearchResults );
	CleanupStack::PopAndDestroy( &contacts );
	}

void CFSEmailUiClsMatchObserver::HandlePsError(TInt aErrorCode)
	{
    FUNC_LOG;
	TRAP_IGNORE( iListHandler.OperationErrorL( aErrorCode ) );
	}

void CFSEmailUiClsMatchObserver::CachingStatus( TCachingStatus& aStatus,
    TInt& /*aError*/ )
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
// CFSEmailUiClsMatchObserver::IsCurrentInputModePredictive
// -----------------------------------------------------------------------------
TKeyboardModes CFSEmailUiClsMatchObserver::GetInputMode()
	{
    FUNC_LOG;
	TInt repVal = 0;
	// 1 = predictive, 0 = non predictive
	iAknFepCenRep.Get( KAknFepPredTxtFlag, repVal );
	
	//If true, EItut (predictive)
	if( repVal == 0 )
		{
		return EQwerty;
		}
	return EItut;
	}

TInt CFSEmailUiClsMatchObserver::CheckCacheStatusL()
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
// CFSEmailUiClsMatchObserver::ChopEmailEntriesList
// -----------------------------------------------------------------------------
void CFSEmailUiClsMatchObserver::ChopEmailEntriesList(TPtrC & aEmail, RArray<
		TPtrC> & aEmailFields)
	{
	FUNC_LOG;
	const TInt KUnitSeparator = 31;
	TInt prevSeparatorIndex = 0;
	TInt nextSeparatorIndex = aEmail.Locate( KUnitSeparator );
	// check if there are more than one email entry
	if ( nextSeparatorIndex == KErrNotFound )
		{
		aEmailFields.Append( aEmail ); //one entry only
		}
	else //more then one entry
		{
		// loop through all but last email entries 
		while ( nextSeparatorIndex != KErrNotFound )
			{
			// First entry needs a separate handling
			if ( prevSeparatorIndex == 0 )
				{
				// -1 because there is a space before a separator character
				TPtrC entry = aEmail.Mid( 0, nextSeparatorIndex - 1 ); 
				aEmailFields.Append( entry );
				}
			else //Append the next entry
				{ 
				nextSeparatorIndex = nextSeparatorIndex + 1 + prevSeparatorIndex;				
				TInt length = nextSeparatorIndex - prevSeparatorIndex - 1;
				// +2 and -2 because there are a spaces before and after a separator character
				TPtrC entry = aEmail.Mid(prevSeparatorIndex + 2, //starting pos
						length - 2); //length 
				aEmailFields.Append(entry);
				}
			// Take the part of the list skiping the entries already appended    
			TPtrC nextEntry = aEmail.Mid(nextSeparatorIndex + 1); 
			prevSeparatorIndex = nextSeparatorIndex;
			nextSeparatorIndex = nextEntry.Locate(KUnitSeparator);
			}
		// the last element does not contain the separator so it needs to be taken here
		if (nextSeparatorIndex == KErrNotFound && aEmail.Mid(prevSeparatorIndex + 1).Length() > 1)
			{
			aEmailFields.Append(aEmail.Mid(prevSeparatorIndex + 2));
			}
		}
	}
