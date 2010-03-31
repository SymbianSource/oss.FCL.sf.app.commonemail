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
* Description:  MR Toolbar impl
*
*/

#include "cmrtoolbar.h"

#include "esmrcommands.h"
#include "cmrasynchcmd.h"
#include <esmrgui.rsg>
#include <eikenv.h>
#include <eikcolib.h>
#include <akntoolbar.h>
#include <aknbutton.h>

// LOCAL DEFINITIONS
namespace // codescanner::namespace
    {
    const TInt KFirstItemIndex( 0 );
    const TInt KSecondItemIndex( 1 );
    const TInt KThirdItemIndex( 2 );    
    }
        
// CLASS MEMBERS DEFINITIONS
// ---------------------------------------------------------------------------
// CMRToolbar::CMRToolbar
// ---------------------------------------------------------------------------
//
CMRToolbar::CMRToolbar()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRToolbar::NewL
// ---------------------------------------------------------------------------
//
CMRToolbar* CMRToolbar::NewL()
    {
    CMRToolbar* self = new ( ELeave ) CMRToolbar();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRToolbar::~CMRToolbar
// ---------------------------------------------------------------------------
//
CMRToolbar::~CMRToolbar()
    {
    if( iToolbar )
        {
        RemovePreviousItems();
        delete iToolbar;
        }
    if( iPreviousToolbar )
        {
        iPreviousToolbar->SetToolbarVisibility( iPreviousVisibility ); 
        static_cast<CEikAppUiFactory*>( 
        		 CEikonEnv::Static()->AppUiFactory() )->SetViewFixedToolbar( iPreviousToolbar );// codescanner::eikonenvstatic
        }

    iItemIdArray.Close();   
    delete iAsyncCmd;
    }

// ---------------------------------------------------------------------------
// CMRToolbar::ConstructL
// ---------------------------------------------------------------------------
//
void CMRToolbar::ConstructL()
    {
    // This does not guarantee that the iPreviousToolbar is constructed
	iPreviousToolbar = static_cast<CEikAppUiFactory*>( 
			CEikonEnv::Static()->AppUiFactory() )->CurrentFixedToolbar();// codescanner::eikonenvstatic
	if ( iPreviousToolbar )
	    {
	    iPreviousVisibility = iPreviousToolbar->IsShown();
	    iPreviousToolbar->SetToolbarVisibility( EFalse );
	    }
    }

// ---------------------------------------------------------------------------
// CMRToolbar::InitializeToolbar
// ---------------------------------------------------------------------------
//
void CMRToolbar::InitializeToolbarL( 
        TToolbarContext aCntx,
        CAknToolbar* aNativeToolbar )
    {	
    if( aNativeToolbar )
        {
        iToolbar = aNativeToolbar;
        }
    else
        {
        iToolbar = CAknToolbar::NewL( R_GLOBAL_TOOLBAR );
        }
    
    iToolbar->SetToolbarObserver( this );
    static_cast<CEikAppUiFactory*>( 
    			CEikonEnv::Static()->AppUiFactory() )->SetViewFixedToolbar( iToolbar );// codescanner::eikonenvstatic
    // Construct the correct toolbar for the context
	TInt error( KErrNone );
	TRAP( error, ConstructToolbarL( aCntx ) );
	if ( error != KErrNone )
			{
			CEikonEnv::Static()->HandleError( error );// codescanner::eikonenvstatic
			}
    }

// ---------------------------------------------------------------------------
// CMRToolbar::SetObserver
// ---------------------------------------------------------------------------
//
void CMRToolbar::ShowToolbar( TBool aShowToolbar )
    {           
    if( iToolbar )
        {
        iToolbar->SetToolbarVisibility( aShowToolbar );
        }
    }

// ---------------------------------------------------------------------------
// CMRToolbar::SetObserver
// ---------------------------------------------------------------------------
//
void CMRToolbar::SetObserver( MAknToolbarObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CMRToolbar::ConstructToolbarL
// ---------------------------------------------------------------------------
//
void CMRToolbar::ConstructToolbarL( TToolbarContext aCntx )
    {
    RemovePreviousItems();
    
    switch( aCntx )
        {
        case EMRViewerFromMailApp:
        case EMRViewerAttendee:
        	{
            iItemIdArray.Append( EESMRCmdAcceptMR );
            iItemIdArray.Append( EESMRCmdTentativeMR );
            iItemIdArray.Append( EESMRCmdDeclineMR );
            
            ConstructButtonL( 
                    R_TOOLBAR_BUTTON_ACCEPT, 
                    KFirstItemIndex );
            ConstructButtonL( 
                    R_TOOLBAR_BUTTON_TENTATIVE, 
                    KSecondItemIndex );
            ConstructButtonL( 
                    R_TOOLBAR_BUTTON_DECLINE, 
                    KThirdItemIndex );       	        	                        
        	break;
        	}
        default:
            break;
        }    
    }

// ---------------------------------------------------------------------------
// CMRToolbar::ConstructButtonL
// ---------------------------------------------------------------------------
//
void CMRToolbar::ConstructButtonL( TInt aResId, TInt aIndex )
    {    
    TInt flags( 0 );
    CAknButton* newButton = 
        CAknButton::NewL( aResId );
    // Ownership of the button is taken right away, no need for CleanupStack
    iToolbar->AddItemL( 
            newButton, EAknCtButton, iItemIdArray[aIndex], 
            flags, aIndex );
    }
        
// ---------------------------------------------------------------------------
// CMRToolbar::DynInitToolbarL
// ---------------------------------------------------------------------------
//
void CMRToolbar::DynInitToolbarL ( TInt aResourceId, CAknToolbar *aToolbar )
    {
    if( iObserver )
        {
        iObserver->DynInitToolbarL( aResourceId, aToolbar );
        }
    }

// ---------------------------------------------------------------------------
// CMRToolbar::OfferToolbarEventL
// ---------------------------------------------------------------------------
//
void CMRToolbar::OfferToolbarEventL ( TInt aCommand )
    {
    // Handle commands here
    if( iObserver )
        {
        // Lazy init
        if( !iAsyncCmd )
            {
            iAsyncCmd = CMRAsynchCmd::NewL();
            }
        // This will notify observer asynchronously
        iAsyncCmd->NotifyObserver( aCommand, iObserver );
        }    
    }

// ---------------------------------------------------------------------------
// CMRToolbar::RemovePreviousItems
// ---------------------------------------------------------------------------
//
void CMRToolbar::RemovePreviousItems()
    {
    TInt count( iItemIdArray.Count() );
    for( TInt i = 0; i < count; ++i )
        {
        iToolbar->RemoveItem( iItemIdArray[ i ] );
        }    
    iItemIdArray.Reset();
    }

// ---------------------------------------------------------------------------
// CMRToolbar::Rect
// ---------------------------------------------------------------------------
//
TRect CMRToolbar::Rect()
    {
    TRect rect( 0, 0, 0, 0 );
    
    if( iToolbar && iToolbar->IsShown() )
        {
        rect = iToolbar->Rect(); 
        }
    
    return rect;
    }

// End of file
