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
* Description:  Email plugin for s60 general settings
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <f32file.h>
#include <e32cmn.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>
#include <GSEmailSettingsPlugin.rsg>
//<cmail>
#include "cfsmailcommon.h"
#include "FreestyleEmailUiConstants.h" // email enumerations
//</cmail>
#include <freestyleemailui.mbg> // icons
#include <gsfwviewuids.h>

// LOCAL INCLUDES
#include "GSEmailSettingsView.h"
#include "GSEmailSettings.hrh"
#include "FreestyleEmailUiConstants.h"

// CONSTANTS
_LIT( KEmailSettingsAppResourceFile,"GSEmailSettingsPlugin.RSC");
_LIT( KIconPath,"FreestyleEmailUi_aif.mif" );

// -----------------------------------------------------------------------------
// CGSEmailSettingsView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSEmailSettingsView* CGSEmailSettingsView::NewL()
	{
    FUNC_LOG;
	CGSEmailSettingsView* self = CGSEmailSettingsView::NewLC();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CGSEmailSettingsView::NewLC()
// Creates a CGSEmailSettingsView object.
// -----------------------------------------------------------------------------
//
CGSEmailSettingsView* CGSEmailSettingsView::NewLC()
    {
    FUNC_LOG;
    CGSEmailSettingsView* self = new ( ELeave ) CGSEmailSettingsView;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CGSEmailSettingsView::CGSEmailSettingsView()
// Default Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGSEmailSettingsView::CGSEmailSettingsView()
	: iCaptionText( NULL )
	{
    FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CGSEmailSettingsView::~MySetFrmWrkView()
// Destroy the object and release all memory objects
// -----------------------------------------------------------------------------
//
CGSEmailSettingsView::~CGSEmailSettingsView()
	{
    FUNC_LOG;
    
    delete iCaptionText;
    iCaptionText = NULL;
    }

// -----------------------------------------------------------------------------
// CGSEmailSettingsView::ConstructL()
// Symbian OS default constructor.
// -----------------------------------------------------------------------------
//
void CGSEmailSettingsView::ConstructL()
	{
    FUNC_LOG;
	TFileName resFile;
	resFile.Append( KDC_APP_RESOURCE_DIR );
	resFile.Append( KEmailSettingsAppResourceFile );
	// Load resource file from the same drive as where our plugin dll is loaded
    TFileName dllFileName;
    Dll::FileName( dllFileName );
	TParse parse;
	            
	User::LeaveIfError( parse.Set( resFile, &dllFileName, NULL ) );
	TFileName fileName( parse.FullName() );
    // Get language of resource file
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    // Open resource file
    RConeResourceLoader resourceLoader( *iCoeEnv  );
    TRAP_IGNORE( resourceLoader.OpenL( fileName ) );
    
    BaseConstructL( R_EMAIL_GS_SETTING_LIST_VIEW );
    
    // Save caption text for later use
    iCaptionText = StringLoader::LoadL( R_FS_EMAIL_GS_APP_SHORT_NAME  );

    // Free the resource file as we don't need it anymore. This prevents
    // (un)installation failures even if the plugin is kept loaded.
    resourceLoader.Close();
	}

// -----------------------------------------------------------------------------
// CGSEmailSettingsView::GetCaptionL()
// Functionality for getting the Caption of the Plugin
// -----------------------------------------------------------------------------
//
void CGSEmailSettingsView::GetCaptionL( TDes& aCaption ) const
    {
    FUNC_LOG;

    if( iCaptionText )
        {
        aCaption.Copy( *iCaptionText );
        }
    }
    
// -----------------------------------------------------------------------------
// CGSEmailSettingsView::Visible()
// This function could be used to hide the plugin dynamically
// -----------------------------------------------------------------------------
//
TBool CGSEmailSettingsView::Visible() const
    {
    FUNC_LOG;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CGSEmailSettingsView::ItemType()
// Get the type of this GS plugin
// ---------------------------------------------------------------------------
//
TGSListboxItemTypes CGSEmailSettingsView::ItemType()
    {
    FUNC_LOG;
    return EGSItemTypeSingleLargeDialog;
    }

// ---------------------------------------------------------------------------
// CGSEmailSettingsView::HandleSelection()
// Called when user selects Email settings from GS
// ---------------------------------------------------------------------------
//
void CGSEmailSettingsView::HandleSelection( const TGSSelectionTypes /*aSelectionType*/ )
    {
    FUNC_LOG;
    TInt tmp = 1;
    const TPckgBuf<TInt> pkgBuf( tmp );
    TVwsViewId pluginViewId = TVwsViewId( KFSEmailUiUid, SettingsViewId );

    TRAP_IGNORE( ActivateViewL(
        pluginViewId, TUid::Uid(KOpenMailSettingsFromGS), pkgBuf ) );
    }

// ---------------------------------------------------------------------------
// CGSEmailSettingsView::CreateIconL()
// ---------------------------------------------------------------------------
//
CGulIcon* CGSEmailSettingsView::CreateIconL( const TUid aIconType )
	{
    FUNC_LOG;
	CGulIcon* icon;
	if ( aIconType == KGSIconTypeLbxItem )
	    {
	    TFileName resFile;
        resFile.Append( KDC_BITMAP_DIR );
        resFile.Append( KIconPath );
        // Correct icon file should be in same drive as the Email application,
        // which in turn should be same as from where our plugin dll is loaded
        TFileName dllFileName;
        Dll::FileName( dllFileName );
        TParse parse;
	                    
        User::LeaveIfError( parse.Set( resFile, &dllFileName, NULL ) );
        TFileName iconFileName( parse.FullName() );
	    
	    CFbsBitmap* bitmap(0);
	    CFbsBitmap* bitmap_mask(0);

	    TInt bitmapId = 0;
	    TInt maskId = 0;
	    AknIconUtils::ValidateLogicalAppIconId( iconFileName, bitmapId, maskId );
	    AknIconUtils::CreateIconLC( bitmap, bitmap_mask, iconFileName, bitmapId, maskId );

	    // create icon from bitmaps
	    icon = CGulIcon::NewL( bitmap, bitmap_mask );
	    CleanupStack::Pop( bitmap_mask );
	    CleanupStack::Pop( bitmap );
	    }
	else
	    {
	    icon = CGSPluginInterface::CreateIconL( aIconType );
	    }
	return icon;
	}
 
// -----------------------------------------------------------------------------
// CGSEmailSettingsView::Id()
// From CAknView, returns the views id.
// -----------------------------------------------------------------------------
//
TUid CGSEmailSettingsView::Id() const
    {
    FUNC_LOG;
    return KGSEmailSettingsPluginUid;
    }

// -----------------------------------------------------------------------------
// CGSEmailSettingsView::DoActivateL()
// -----------------------------------------------------------------------------
//
void CGSEmailSettingsView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                  TUid /*aCustomMessageId*/,
                                  const TDesC8& /*aCustomMessage*/ )
    {
    FUNC_LOG;
    // not view-type-plugin => implementation not needed
    }

// ---------------------------------------------------------------------------
// CGSEmailSettingsView::DoDeactivate()
// ---------------------------------------------------------------------------
//
void CGSEmailSettingsView::DoDeactivate()
    {
    FUNC_LOG;
    // not view-type-plugin => implementation not needed
    }

// end of file
