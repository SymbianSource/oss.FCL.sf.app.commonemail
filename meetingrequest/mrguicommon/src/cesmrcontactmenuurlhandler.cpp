/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CESMRContactMenuUrlHandler.
*
*/


#include "cesmrcontactmenuurlhandler.h"
#include "esmrhelper.h"
//#include "cesmriconfield.h"
#include "nmrbitmapmanager.h"
//<cmail>
#include "cfsccontactactionmenu.h"
#include "cfsccontactactionmenuitem.h"
#include "mfsccontactaction.h"
#include "mfsccontactactionmenumodel.h"
#include "esmrcommands.h"
#include <esmrgui.rsg>
//</cmail>
#include <eikmenup.h>
#include <coemain.h>
#include <gulicon.h>
#include <aknsutils.h>
#include <favouritesdb.h>
#include <apgtask.h>
#include <apgcli.h>
#include <aknquerydialog.h>
#include <aknnotewrappers.h>
// for intranet application opening
#include <aiwservicehandler.h>
#include <data_caging_path_literals.hrh>
#include <e32cmn.h>

// DEBUG
#include "emailtrace.h"

/// Unnamed namespace for local definitions
namespace // codescanner::namespace
	{
// URL actionmenu uids
const TUid KUidOpenInBrowser =
    {
    1
    };

const TUid KUidOpenInIntranet =
    {
    2
    };

const TUid KUidBookmark =
    {
    3 // codescanner::magicnumbers
    };

// Browser app uid
const TUid KUidBrowser =
    {
    0x10008D39
    };

// Maximum length of Browser bookmark
const TInt KMaxBookmarkNameLength = 50;
// Browser protocol
// 4 is number used for opening the browser
_LIT( KBrowserProtocol, "4 " );

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::CESMRContactMenuUrlHandler
// ---------------------------------------------------------------------------
//
CESMRContactMenuUrlHandler::CESMRContactMenuUrlHandler( 
		CFscContactActionMenu& aContactActionMenu ) :
    iContactActionMenu( aContactActionMenu )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::NewL
// ---------------------------------------------------------------------------
//
CESMRContactMenuUrlHandler* CESMRContactMenuUrlHandler::NewL(
        CFscContactActionMenu& aContactActionMenu )
    {
    FUNC_LOG;
    CESMRContactMenuUrlHandler* self = 
		new (ELeave) CESMRContactMenuUrlHandler( aContactActionMenu );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::~CESMRContactMenuUrlHandler( )
// ---------------------------------------------------------------------------
//
CESMRContactMenuUrlHandler::~CESMRContactMenuUrlHandler( )
    {
    FUNC_LOG;
    delete iUrl;
    delete iServiceHandler;
    iIconArray.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::Reset( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::Reset( )
    {
    FUNC_LOG;
    delete iUrl;
    iUrl = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::SetUrlL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::SetUrlL( const TDesC& aUrl )
    {
    FUNC_LOG;
    HBufC* temp = aUrl.AllocL();
    delete iUrl;
    iUrl = temp;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::CreateMenuItemL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::CreateMenuItemL( 
		TInt aResourceId, 
		TUid aCommandUid )
    {
    FUNC_LOG;
    CGulIcon* itemIcon = CreateIconL(aCommandUid);
    //ownership transferred
    CleanupStack::PushL( itemIcon );
    iIconArray.AppendL( itemIcon );
    CleanupStack::Pop( itemIcon );

    HBufC* text = CCoeEnv::Static()->AllocReadResourceLC( aResourceId );
    CFscContactActionMenuItem* newItem = 
		iContactActionMenu.Model().NewMenuItemL( *text,
												 itemIcon,
                                                 1,
                                                 EFalse,
                                                 aCommandUid );

    CleanupStack::PopAndDestroy( text );
    CleanupStack::PushL( newItem );
    iContactActionMenu.Model().AddItemL( newItem );
    CleanupStack::Pop( newItem );
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::CreateActionMenuIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CESMRContactMenuUrlHandler::CreateIconL(TUid aCommandUid)
    {
    FUNC_LOG;
    // Define default icon object
    CFbsBitmap* iconBitmap( NULL );
    CFbsBitmap* iconMaskBitmap( NULL );
    NMRBitmapManager::TMRBitmapId iconID( NMRBitmapManager::EMRBitmapNotSet );

    //select correct icon for item
    if( aCommandUid == KUidOpenInBrowser )
        {
        //<cmail> icon is not available in Cmail
        iconID = NMRBitmapManager::EMRBitmapBrowser;
        //</cmail>
        }
    else if( aCommandUid == KUidOpenInIntranet)
        {
        //<cmail> icon is not available in Cmail
        iconID = NMRBitmapManager::EMRBitmapIntranet;
        //</cmail>
        }
    else
        {
        //<cmail> icon is not available in Cmail
        iconID = NMRBitmapManager::EMRBitmapBookmark;
        //</cmail>
        }

    // TODO: correct to use XML layout
    NMRBitmapManager::GetSkinBasedBitmapLC( 
                    iconID, iconBitmap, iconMaskBitmap, TSize(20,20) );
    
    CGulIcon* icon = CGulIcon::NewL(iconBitmap, iconMaskBitmap);
    
    CleanupStack::Pop( 2 ); // iconBitmap, iconMaskBitmap
    
    return icon;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::OptionsMenuAvailable( )
// ---------------------------------------------------------------------------
//
TBool CESMRContactMenuUrlHandler::OptionsMenuAvailable( )
    {
    FUNC_LOG;
    return iUrl != NULL;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::InitOptionsMenuL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::InitOptionsMenuL( 
		CEikMenuPane* aActionMenuPane )
    {
    FUNC_LOG;
    CEikMenuPaneItem::SData data;
    data.iFlags = 0;
    data.iCascadeId = 0;

    //action menu is dynamic so commands are assigned dynamically here
    TInt command = EESMRCmdActionMenuFirst;

    data.iCommandId = command;
    command++;//dynamic command ids
    CCoeEnv::Static()->ReadResourceL( 
    		data.iText, R_QTN_MEET_REQ_VIEWER_OPTIONS_OPEN_IN_WEB );
    aActionMenuPane->AddMenuItemL( data );

    if(IsServiceAvailableL(R_INTRANET_AIW_INTEREST))
        {
        data.iCommandId = command;
        command++;//dynamic command ids
        CCoeEnv::Static()->ReadResourceL( 
        		data.iText, R_QTN_MEET_REQ_FSOPTIONS_OPEN_IN_INTRA );
        aActionMenuPane->AddMenuItemL( data );
        }

    data.iCommandId = command;
    CCoeEnv::Static()->ReadResourceL( 
    		data.iText, R_QTN_MEET_REQ_VIEWER_OPTIONS_BOOKMARK );
    aActionMenuPane->AddMenuItemL( data );
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::IsServiceAvailableL
// ---------------------------------------------------------------------------
//
TBool CESMRContactMenuUrlHandler::IsServiceAvailableL(TInt aResourceId)
    {
    FUNC_LOG;
    //read resource and check if that service is available
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );
    // Read first the count of the Criteria Items in resource.
    // After this the reader is in correct position at the start
    // of the first criteria.
    const TInt count = reader.ReadInt16();
    CAiwCriteriaItem* criteriaItem = CAiwCriteriaItem::NewLC();
    criteriaItem->ReadFromResoureL( reader );
    TInt serviceAvailable = ServiceHandlerL().NbrOfProviders(criteriaItem);
    CleanupStack::PopAndDestroy( criteriaItem );
    // destroy -> resource reader
    CleanupStack::PopAndDestroy( ); // codescanner::cleanup  
    return serviceAvailable;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::ExecuteOptionsMenuL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::ExecuteOptionsMenuL( TInt aCommand )
    {
    FUNC_LOG;
    TInt command = EESMRCmdActionMenuFirst;

    if( (aCommand == KUidOpenInBrowser.iUid) || (aCommand == command) )
        {
        OpenUrlInBrowserL();
        return;
        }
    command++;//next item in the dynamic list

    if(IsServiceAvailableL(R_INTRANET_AIW_INTEREST))
        {
        if( (aCommand == KUidOpenInIntranet.iUid) || (aCommand == command))
            {
            OpenUrlInIntranetL();
            }
        command++;//next item in the dynamic list
        }

    if( (aCommand == KUidBookmark.iUid) || (aCommand == command) )
        {
        BookmarkUrlL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::ShowActionMenuL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::ShowActionMenuL( )
    {
    FUNC_LOG;
    //reset
    iIconArray.ResetAndDestroy();
    iContactActionMenu.Model().RemoveAll();

    //fill list
    CreateMenuItemL( R_QTN_MEET_REQ_VIEWER_OPTIONS_OPEN_IN_WEB, 
					 KUidOpenInBrowser);

    if(IsServiceAvailableL(R_INTRANET_AIW_INTEREST))
        {
        CreateMenuItemL( R_QTN_MEET_REQ_FSOPTIONS_OPEN_IN_INTRA, 
						 KUidOpenInIntranet);
        }

    CreateMenuItemL(R_QTN_MEET_REQ_VIEWER_OPTIONS_BOOKMARK, KUidBookmark);

    //show list
    if ( iContactActionMenu.ExecuteL() == EFscCustomItemSelected )
        {
        TUid uid = iContactActionMenu.FocusedItem().ImplementationUid();

        //handle command
        ExecuteOptionsMenuL(uid.iUid);
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::OpenUrlInBrowserL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::OpenUrlInBrowserL( )
    {
    FUNC_LOG;
    HBufC* param = HBufC::NewLC( KFavouritesMaxUrl );
    param->Des().Copy( KBrowserProtocol() );
    param->Des().Append( *iUrl );
    TApaTaskList taskList(
            CEikonEnv::Static()->WsSession() ); // codescanner::eikonenvstatic
    TApaTask task = taskList.FindApp( KUidBrowser );
    if ( task.Exists() )
        {
        HBufC8* param8 = HBufC8::NewLC( param->Length() );
        param8->Des().Append( *param );
        task.SendMessage( TUid::Uid ( 0 ), *param8 ); // Uid is not used
        CleanupStack::PopAndDestroy( param8 );
        }
    else
        {
        RApaLsSession appArcSession;
        CleanupClosePushL( appArcSession );
        // connect to AppArc server
        User::LeaveIfError( appArcSession.Connect() ); 
        TThreadId id;
        User::LeaveIfError( appArcSession.StartDocument ( 
        		*param, KUidBrowser, id ) );
        CleanupStack::PopAndDestroy( &appArcSession );
        }
    CleanupStack::PopAndDestroy( param );
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::OpenUrlInIntranetL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::OpenUrlInIntranetL( )
    {
    FUNC_LOG;
    HBufC8* eightBitUrl = HBufC8::NewLC( iUrl->Length() );
    eightBitUrl->Des().Copy( *iUrl );

    TAiwGenericParam inParam(EGenericParamURL, TAiwVariant(*eightBitUrl));
    CAiwGenericParamList& inParamList = ServiceHandlerL().InParamListL();
    inParamList.AppendL( inParam );

    ServiceHandlerL().ExecuteServiceCmdL( 
							KAiwCmdView, 
							inParamList, 
							ServiceHandlerL().OutParamListL() );
    CleanupStack::PopAndDestroy( eightBitUrl );
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::BookmarkUrlL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuUrlHandler::BookmarkUrlL( )
    {
    FUNC_LOG;
    // Create an item
    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetParentFolder( KFavouritesRootUid );
    item->SetType( CFavouritesItem::EItem );

    // Read default name from resources
    HBufC* defaultName =CCoeEnv::Static()->AllocReadResourceLC ( 
									R_QTN_CALENDAR_DEFAULT_BOOKMARK );

    TBuf<KMaxBookmarkNameLength> retName;
    retName.Copy( defaultName->Des() );

    // Query bookmark name from user
    CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL( retName );
    dlg->SetMaxLength( KMaxBookmarkNameLength );
    if ( !dlg->ExecuteLD( R_ADD_BOOKMARK_QUERY_DIALOG ) )
        {
        // User press cancel - do not add bookmark
        CleanupStack::PopAndDestroy ( 2, item ); // item, defaultName
        }
    else
    	{
    	item->SetNameL( retName );
		item->SetUrlL( *iUrl );

		RFavouritesSession sess; // codescanner::resourcenotoncleanupstack
		RFavouritesDb db; // codescanner::resourcenotoncleanupstack

		User::LeaveIfError( sess.Connect() );
		CleanupClosePushL<RFavouritesSession> ( sess );
		User::LeaveIfError( db.Open( sess, KBrowserBookmarks ) );
		CleanupClosePushL<RFavouritesDb> ( db );

		// add item
		db.Add( *item, ETrue );
		// Close the database.
		db.Close();

		HBufC * msgBuffer = CCoeEnv::Static()->AllocReadResourceLC( 
										R_QTN_CALENDAR_BOOKMARK_SAVED );
		CAknConfirmationNote* note = new(ELeave)CAknConfirmationNote( ETrue );
		note->ExecuteLD ( *msgBuffer );

		// item, db, sess, defaultName, msgBuffer
		CleanupStack::PopAndDestroy ( 5, item ); // codescanner::magicnumbers
    	}
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuUrlHandler::ServiceHandlerL
// ---------------------------------------------------------------------------
//
CAiwServiceHandler& CESMRContactMenuUrlHandler::ServiceHandlerL()
    {
    FUNC_LOG;
    if ( !iServiceHandler )
        {
        iServiceHandler = CAiwServiceHandler::NewL();
        iServiceHandler->AttachL( R_INTRANET_AIW_INTEREST );
        }
    return *iServiceHandler;
    }
// End of file

