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
* Description:  ESMR Location plugin handler implementation
*
*/
#include "emailtrace.h"
#include "cesmrlocationpluginhandler.h"

#include <calentry.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <esmrgui.rsg>
//<cmail>
#include "cesmrurlparserplugin.h"
//</cmail>
#include <CLmkLandmarkSelectorDlg.h>
#include <TLmkItemIdDbCombiInfo.h>
#include <EPos_CPosLandmark.h>
#include <AknUtils.h>
#include <ct/rcpointerarray.h>

#include "cesmrlocationplugin.h"
#include "esmrcommands.h"
#include "mesmrcalentry.h"

#include "cesmrdynamicitemselectionlist.h"
#include "cesmrlocationhistorymanager.h"
#include "mesmrlocationhistoryitem.h"
#include "cesmrconfirmationquery.h"

// Unnamed namespace for local definitions
namespace { //codescanner::namespace

_LIT(KSpace, " ");

// Maximum length of the text in location field
const TInt KMaxLocationTextLength = 255;

}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::CESMRLocationPluginHandler
// -----------------------------------------------------------------------------
//
CESMRLocationPluginHandler::CESMRLocationPluginHandler( MObjectProvider& aParent )
: iParent(&aParent)
	{
    FUNC_LOG;
	
	}

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::~CESMRLocationPluginHandler
// -----------------------------------------------------------------------------
//
CESMRLocationPluginHandler::~CESMRLocationPluginHandler()
	{
    FUNC_LOG;
    delete iWait;
    delete iLocationPlugin;
    delete iLandmark;
    delete iUrlParser;
	delete iLocationHistoryManager;
	}

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::NewL
// -----------------------------------------------------------------------------
//
CESMRLocationPluginHandler* CESMRLocationPluginHandler::NewL( 
        MObjectProvider& aParent )
	{
    FUNC_LOG;
	CESMRLocationPluginHandler* self = 
		new (ELeave) CESMRLocationPluginHandler( aParent );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
     
// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::ConstructL()
	{
    FUNC_LOG;
	iWait = new( ELeave ) CActiveSchedulerWait;
	}

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CESMRLocationPluginHandler::HandleCommandL( TInt aCommand,
												 MESMRCalEntry& aEntry,
												 TBool aIgnoreQuery )
	{
    FUNC_LOG;
	iIgnoreQuery = aIgnoreQuery;
	iEntryUpdated = EFalse;
	
	switch ( aCommand )
		{
		case EESMRCmdAssignFromMap:
			{
			if( !(iWait->IsStarted()) )
			    {
			    const TDesC& location = aEntry.Entry().LocationL();
			    
			    // Extract location URL from description
			    TPtrC locationUrl;
			    UrlParserL().FindLocationUrl( aEntry.Entry().DescriptionL(),
			                                  locationUrl );
			    
			    // Launch Maps
			    LocationPluginL().SelectFromMapL( location, locationUrl );
			    iWait->Start(); // codescanner::callActiveObjectWithoutCheckingOrStopping
			    
			    // iLandmark is set in async callback SelectFromMapCompleted
			    // from Maps application.
			    if ( iLandmark )
			        {
			        UpdateEntryFromLandmarkL( aEntry, *iLandmark );
			        delete iLandmark;
			        iLandmark = NULL;
			        }
			    }
			}
			break;
        case EESMRCmdUpdateFromMap:
            {
            if ( !(iWait->IsStarted()) )
                {
                // Launch Maps and ignore existing coordinates, 
                // because location has been updated
                LocationPluginL().SelectFromMapL( aEntry.Entry().LocationL(), 
                                                  KNullDesC );
                iWait->Start(); // codescanner::callActiveObjectWithoutCheckingOrStopping
                               
                // iLandmark is set in async callback SelectFromMapCompleted
                // from Maps application.
                if ( iLandmark )
                    {
                    UpdateEntryFromLandmarkL( aEntry, *iLandmark );
                    delete iLandmark;
                    iLandmark = NULL;
                    }                
                }
            break;
            }
		case EESMRCmdSearchFromMap:
		    {
		    const TDesC& location = aEntry.Entry().LocationL();
		    LocationPluginL().SearchFromMapL( location );
		    }		    
		    break;
		case EESMRCmdShowOnMap:
		    {
		    const TDesC& description = aEntry.Entry().DescriptionL();
		    TPtrC locationUrl;
		    TInt position = iUrlParser->FindLocationUrl( description,
		                                                 locationUrl );
		    const TDesC& location = aEntry.Entry().LocationL();
		    if ( position >= 0 )
		        {
		        LocationPluginL().ShowOnMapL( location, locationUrl );
		        }
		    break;
		    }
		case EESMRCmdLandmarks:
		    {
            SearchFromLandmarksL( iParent, aEntry );
		    break;
		    }
		case EESMRCmdPreviousLocations:
		    {
		    ShowSelectPreviousLocationQueryL( aEntry );
		    break;
		    }
		default:
			User::Leave( KErrGeneral );
		}
	
	return iEntryUpdated;
	}

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::IsCommandAvailableL
// -----------------------------------------------------------------------------
//
TBool
CESMRLocationPluginHandler::IsCommandAvailableL( TInt aCommand,
                                                 const MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TBool res( EFalse );
    
    switch ( aCommand )
        {
        case EESMRCmdAssignFromMap: //fallthrough
        case EESMRCmdUpdateFromMap:
        case EESMRCmdSearchFromMap:
        case EESMRCmdLandmarks:
            {
            res = ETrue;
            break;
            }
        case EESMRCmdPreviousLocations:
            {
            if ( LocationHistoryManagerL().ItemCount() > 0 )
                {
                res = ETrue;
                }
            break;
            }
        case EESMRCmdShowOnMap:
            {
            const TDesC& description = aEntry.Entry().DescriptionL();
            TPtrC locationUrl;
            TInt position = UrlParserL().FindLocationUrl( description,
                                                          locationUrl );
            if ( position >= 0 )
                {
                res = ETrue;
                }
            break;
            }
        default:
            {
            break;
            }
        }
    
    return res;
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::StoreLocationToHistoryL
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::StoreLocationToHistoryL(
        const MESMRCalEntry& aEntry  )
    {
    FUNC_LOG;
    const CCalEntry& entry = aEntry.Entry();
    const TDesC& entryLocation = entry.LocationL();
    
    // Store location only if location field contains text.
    if ( entryLocation.Length() > 0 )
        {
        const TDesC& entryDescription = entry.DescriptionL();
        
        // Search for location url from description.
        TPtrC locationUrl;
        TInt position = UrlParserL().FindLocationUrl( entryDescription,
                                                      locationUrl );
        
        // If url was not found.
        if ( position < 0 )
            {
            locationUrl.Set( KNullDesC );
            }
        
        // Check if history item with same location and url
        // is already stored.
        TInt existingHistoryItemIndex = KErrNotFound;
        TInt itemCount = LocationHistoryManagerL().ItemCount();
        for ( TInt i = 0; i < itemCount; ++i )
            {
            const MESMRLocationHistoryItem& item =
                iLocationHistoryManager->LocationHistoryItemL( i );
            
            if ( item.Address().Compare( entryLocation ) == 0 &&
                 item.Url().Compare( locationUrl ) == 0 )
                {
                existingHistoryItemIndex = i;
                break;
                }
            }
        
        // If history item with same location and url existed
        // update it's position in history array only.
        if ( existingHistoryItemIndex >= 0 )
            {
            // Ownership is not transferred,
            // existing item is already owned by location manager.
            const MESMRLocationHistoryItem& item =
                iLocationHistoryManager->LocationHistoryItemL( existingHistoryItemIndex );
            
            iLocationHistoryManager->UpdateLocationHistoryL( &item );
            }
        else
            {
            // Create new history item.
            MESMRLocationHistoryItem* historyItem =
                iLocationHistoryManager->CreateLocationHistoryItemL(
                            entryLocation,
                            locationUrl );
            
            // Store new item to history.
            CleanupDeletePushL( historyItem );
            iLocationHistoryManager->UpdateLocationHistoryL( historyItem );
            CleanupStack::Pop( historyItem );
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::UpdateEntryLocationL
// -----------------------------------------------------------------------------
//
TBool CESMRLocationPluginHandler::UpdateEntryLocationL( MESMRCalEntry& aEntry,
                                                        const TDesC& aLocation,
                                                        TBool aIgnoreQuery )
    {
    FUNC_LOG;
    TBool entryUpdated = EFalse;
    
    if ( aLocation.Length() > 0 )
        {
        CCalEntry& entry = aEntry.Entry();
        const TDesC& location = entry.LocationL();
        
        // Replace old location field content.
        TBool replace = ETrue;
        
        if ( !aIgnoreQuery
             && location.Length() > 0
             && location != aLocation )
            {
            // Query for replacing old location field content 
            replace = CESMRConfirmationQuery::ExecuteL( location ); 
            }
        
        // If previous location was empty or user selects to
        // replace previous location with new one.
        if (  location.Length() == 0 || replace )
            {
            entry.SetLocationL( aLocation.Left( KMaxLocationTextLength ) );
            entryUpdated = ETrue;                
            }
        else
            {
            // Concatenate new address to existing one
            // Create CPtrCArray for existing and new location strings
            CPtrCArray* strings = new( ELeave ) CPtrCArray( 2 );
            CleanupStack::PushL( strings );
            strings->AppendL( TPtrC( location ) );
            strings->AppendL( TPtrC( aLocation ) );
            HBufC* newLocation =
                StringLoader::LoadLC( R_MEET_REQ_NEW_LOCATION_FORMAT,
                                      *strings );
            entry.SetLocationL( newLocation->Left( KMaxLocationTextLength ) );
            entryUpdated = ETrue;
            CleanupStack::PopAndDestroy( 2, strings );
            }
        }
    
    return entryUpdated;
    }

// -----------------------------------------------------------------------------
// From MESMRLocationPluginObserver
// CESMRLocationPluginHandler::SelectFromMapCompleted
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::SelectFromMapCompleted( TInt aError,
                                                         CPosLandmark* aLandmark )
	{
    FUNC_LOG;
	// Remove previous landmark if one is set.
    if ( iLandmark )
        {
        delete iLandmark;
        iLandmark = NULL;
        }
	
	if ( aError == KErrNone )
		{
		// pointer to the result of landmark fetch
		iLandmark = aLandmark;
		}
	
	if ( iWait->IsStarted() )
		{
		iWait->AsyncStop();
		}
	}
// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::LocationPluginL
// -----------------------------------------------------------------------------
//
CESMRLocationPlugin& CESMRLocationPluginHandler::LocationPluginL()
	{
    FUNC_LOG;
	if ( !iLocationPlugin )
		{
		// Lazy construction of Location ECom plugin
		iLocationPlugin = CESMRLocationPlugin::NewL();
		iLocationPlugin->SetObserver(this);
		}
	
	return *iLocationPlugin;
	}

// ---------------------------------------------------------------------------
// Creates adress descriptor from a  landmark object
// ---------------------------------------------------------------------------
//
HBufC* CESMRLocationPluginHandler::CreateAddressFromLandmarkL( 
                                               const CPosLandmark& aLandmark )
    {
    FUNC_LOG;
    CPtrCArray* addressStrings = new( ELeave ) CPtrCArray( 2 );
    CleanupStack::PushL( addressStrings );
    
    TBool streetAvailable = aLandmark.IsPositionFieldAvailable( EPositionFieldStreet );
    TBool cityAvailable = aLandmark.IsPositionFieldAvailable( EPositionFieldCity );
    
    if ( streetAvailable )
        {
        TPtrC streetPtr( KNullDesC );
        User::LeaveIfError( aLandmark.GetPositionField( EPositionFieldStreet,
                                                         streetPtr ) );
        if ( streetPtr.Length() > 0 )
            {
            addressStrings->AppendL( streetPtr );
            }
        }

    if ( cityAvailable )
        {
        TPtrC cityPtr( KNullDesC );
        User::LeaveIfError( aLandmark.GetPositionField( EPositionFieldCity,
                                                         cityPtr ) );
        if ( cityPtr.Length() > 0 )
            {
            addressStrings->AppendL( cityPtr );
            }
        }
    
    HBufC* address(NULL);
    
    if ( addressStrings->Count() == 2 )
        {
        //format street and city to buffer
        address = StringLoader::LoadL( R_MEET_REQ_ADDRESS_STREET_CITY, 
                                       *addressStrings );
        }
    else if ( addressStrings->Count() == 1 )
        {
        // alloc street or city to buffer
        address = addressStrings->At( 0 ).AllocL();
        }
    else if ( addressStrings->Count() == 0 )
        {
        TPtrC namePtr( KNullDesC );
        User::LeaveIfError( aLandmark.GetLandmarkName( namePtr ));
        if ( namePtr.Length() > 0)
            {
            address = namePtr.AllocL();
            }
        }
    
    CleanupStack::PopAndDestroy( addressStrings );                       
    
    //Transfer ownership of address
    return address;
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::UpdateEntryFromLandmarkL
// Updates location information into calendar entry from aLandmark.
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::UpdateEntryFromLandmarkL( MESMRCalEntry& aEntry, const CPosLandmark& aLandmark )
	{
    FUNC_LOG;
    //parse and add address to calendar entry
    HBufC* streetAddress = CreateAddressFromLandmarkL( aLandmark );
    if ( streetAddress )
        {
        CleanupDeletePushL( streetAddress );
        UpdateEntryLocationL( aEntry, *streetAddress );
        CleanupStack::PopAndDestroy( streetAddress );
        }
    
    //parse and add location url to calendar entry
    HBufC* locationUrl = NULL;
    TRAP_IGNORE( locationUrl = UrlParserL().CreateUrlFromLandmarkL( aLandmark ) )
    if ( locationUrl )
        {
        CleanupDeletePushL( locationUrl );
        UpdateEntryDescriptionL( aEntry, *locationUrl );
        CleanupStack::PopAndDestroy( locationUrl );
        }
    
	}

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::SearchFromLandmarksL
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::SearchFromLandmarksL( MObjectProvider* aParent, MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CLmkLandmarkSelectorDlg* landmarkDlg = CLmkLandmarkSelectorDlg::NewL();
    CleanupStack::PushL( landmarkDlg );
    landmarkDlg->SetMopParent( aParent );
    TLmkItemIdDbCombiInfo selectedItem;
    
    // Execute landmark selection dialog
    TInt isAccepted = landmarkDlg->ExecuteLD( selectedItem );
    CleanupStack::Pop( landmarkDlg );
    if ( isAccepted != 0 )
        {
        TPosLmItemId itemId = selectedItem.GetItemId();
        CPosLandmarkDatabase* lmDataBase = selectedItem.GetLmDb();
        CleanupStack::PushL( lmDataBase );
        
        // Read selected landmark from landmarks database
        CPosLandmark* landmark = lmDataBase->ReadLandmarkLC( itemId );
        UpdateEntryFromLandmarkL( aEntry, *landmark );
        CleanupStack::PopAndDestroy( landmark );
        CleanupStack::PopAndDestroy( lmDataBase ); 
        ReleaseLandmarkResources();
        }
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::ShowSelectPreviousLocationQueryL
// Shows "Previous locations" query and updates aEntry
// according to selection.
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::ShowSelectPreviousLocationQueryL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    // Array for listbox items.
    RCPointerArray<HBufC> addressArray;
    CleanupClosePushL( addressArray );
    
    // Populate address array.
    TInt itemCount = LocationHistoryManagerL().ItemCount();
    for ( TInt i = 0; i < itemCount; ++i )
        {
        const MESMRLocationHistoryItem& item =
            iLocationHistoryManager->LocationHistoryItemL( i );
        HBufC* address = item.Address().AllocLC();
        addressArray.AppendL( address );
        CleanupStack::Pop( address );
        }
    
    CESMRDynamicItemSelectionList* selectionQuery =
            CESMRDynamicItemSelectionList::NewL();
    CleanupStack::PushL( selectionQuery );
    
    // Execute selection query.
    TInt idx = selectionQuery->ExecuteL( addressArray,
            CESMRDynamicItemSelectionList::EESMRRecentLocationList );
    
    CleanupStack::PopAndDestroy( selectionQuery );
    
    if ( idx >= 0 && idx < addressArray.Count() )
        {
        const MESMRLocationHistoryItem& item =
            iLocationHistoryManager->LocationHistoryItemL( idx );
        
        // Update entry location.
        const TDesC& selectedLocation = item.Address();
        UpdateEntryLocationL( aEntry, selectedLocation );
        
        // Update entry description if url available.
        const TDesC& selectedLocationUrl = item.Url();
        if ( selectedLocationUrl.Length() > 0 )
            {
            UpdateEntryDescriptionL( aEntry, selectedLocationUrl );
            }
        }
    
    CleanupStack::PopAndDestroy( &addressArray );
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::UpdateEntryLocationL
// Updates aEntry location with aLocation.
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::UpdateEntryLocationL(
        MESMRCalEntry& aEntry, const TDesC& aLocation )
    {
    FUNC_LOG;
    iEntryUpdated |= UpdateEntryLocationL( aEntry, aLocation, iIgnoreQuery );
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::UpdateEntryDescriptionL
// Updates aEntry description with aLocationUrl.
// -----------------------------------------------------------------------------
//
void CESMRLocationPluginHandler::UpdateEntryDescriptionL(
        MESMRCalEntry& aEntry, const TDesC& aLocationUrl )
    {
    FUNC_LOG;
    if ( aLocationUrl.Length() > 0 )
        {
        CCalEntry& entry = aEntry.Entry();
        HBufC* description = HBufC::NewL( entry.DescriptionL().Length() + 
                             aLocationUrl.Length() + 1);
        CleanupStack::PushL( description );
        TPtr descriptionPointer( description->Des() );
        TPtrC urlPointer;
        TInt position;
        position = UrlParserL().FindLocationUrl( entry.DescriptionL(), urlPointer);
        
        if ( position >= KErrNone )
            {
            descriptionPointer.Copy( entry.DescriptionL() );
            if ( ( position > 0 ) && ( entry.DescriptionL().Length() > 
                                        (position + aLocationUrl.Length() ) ) )
                {
                descriptionPointer.Delete( position, ( urlPointer.Length() + 1 ) );
                }
            else
                {
                descriptionPointer.Delete( position, urlPointer.Length() );
                }
            descriptionPointer.Insert( 0, aLocationUrl );
            if ( position > 0 )
                {
                descriptionPointer.Insert( aLocationUrl.Length(), KSpace );
                }
            entry.SetDescriptionL( *description );
            iEntryUpdated = ETrue;
            }
        else
            {
            descriptionPointer.Copy( entry.DescriptionL() );
            descriptionPointer.Insert( 0, aLocationUrl );
            if ( entry.DescriptionL().Length() != 0 )
                {
                descriptionPointer.Insert( aLocationUrl.Length(), KSpace );
                }
            entry.SetDescriptionL( *description );
            iEntryUpdated = ETrue;
            }
        CleanupStack::PopAndDestroy( description );
        }
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::UrlParserL
// Creates and returns location URL parser.
// -----------------------------------------------------------------------------
//
CESMRUrlParserPlugin& CESMRLocationPluginHandler::UrlParserL()
    {
    FUNC_LOG;
    if ( !iUrlParser )
        {
        iUrlParser = CESMRUrlParserPlugin::NewL();
        }
    
    return *iUrlParser;
    }

// -----------------------------------------------------------------------------
// CESMRLocationPluginHandler::LocationHistoryManagerL
// Creates and returns location history manager.
// -----------------------------------------------------------------------------
//
CESMRLocationHistoryManager& CESMRLocationPluginHandler::LocationHistoryManagerL()
    {
    FUNC_LOG;
    if ( !iLocationHistoryManager )
        {
        iLocationHistoryManager = CESMRLocationHistoryManager::NewL();
        }
    
    return *iLocationHistoryManager;
    }

// EOF

