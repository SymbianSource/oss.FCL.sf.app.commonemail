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
* Description:  Implementation of the class CFscContactActionMenuListBox.
*
*/

//<cmail>

// INCUDES
#include "emailtrace.h"
#include <e32std.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <eikclbd.h>
#include <AknsFrameBackgroundControlContext.h>

#include "cfsccontactactionmenulistbox.h"
#include "cfsccontactactionmenulistboxitemdrawer.h"
#include "fsccontactactionmenuuidefines.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CFscContactActionMenuListBox::CFscContactActionMenuListBox
// ---------------------------------------------------------------------------
//
CFscContactActionMenuListBox::CFscContactActionMenuListBox()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuListBox::~CFscContactActionMenuListBox
// ---------------------------------------------------------------------------
//
CFscContactActionMenuListBox::~CFscContactActionMenuListBox()
    {
    FUNC_LOG;
    if ( iBgContext )
        {
        delete iBgContext;
        iBgContext = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// CFscContactActionMenuListBox::ConstructL
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuListBox::ConstructL( 
    const CCoeControl* aParent,TInt aFlags )
    {
    FUNC_LOG;
    
    // Construct own item drawer
    CreateModelL();
    ( ( CTextListBoxModel* )iModel )->ConstructL();

    CColumnListBoxData* columnData = 
        CColumnListBoxData::NewL();
    
    CleanupStack::PushL( columnData );
    iItemDrawer=new ( ELeave ) 
        CFscContactActionMenuListBoxItemDrawer(
            Model(), iEikonEnv->NormalFont(), columnData );
    CleanupStack::Pop( columnData );
    iItemDrawer->SetDrawMark(EFalse);

    EnableExtendedDrawingL();
    CEikListBox::ConstructL(aParent,aFlags);
        
    columnData->SetSkinEnabledL( ETrue );

    //Create background control for skinning
    if (!iBgContext)
       {
       iBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrPopup, iBackgroundRect, iBackgroundRect, EFalse );
       }
    
    }   
    
// ---------------------------------------------------------------------------
// CFscContactActionMenuListBox::Draw
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuListBox::Draw( const TRect& aRect ) const
    {
    FUNC_LOG;

    if ( iBgContext )
        {    
        //Set frame borders
        iBgContext->SetFrameRects( iBackgroundRect, Rect() );    
        //Draw background
        AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, this, SystemGc(), iBackgroundRect );
        }
            
    CFSCCONTACTACTIONMENULISTBOXBASE::Draw( aRect );        
    }

// -----------------------------------------------------------------------------
// CFscContactActionMenuListBox::MopSupplyObject()
// -----------------------------------------------------------------------------
//

TTypeUid::Ptr CFscContactActionMenuListBox::MopSupplyObject(TTypeUid aId)
    {
    FUNC_LOG;
    // For skinning
    if ( iBgContext && aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject( aId );
    }


// ---------------------------------------------------------------------------
// CFscContactActionMenuListBox::SetVisibleItemCount
// ---------------------------------------------------------------------------
//
void CFscContactActionMenuListBox::SetBackgroundRect( const TRect& aRect )
    {
    FUNC_LOG;
    iBackgroundRect = aRect;
    iBgContext->SetRect(aRect);
    }    
//</cmail>

