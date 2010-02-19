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
* Description: 
*
*/


// INCLUDE FILES
#include "TestAppContainer.h"
#include <aknlists.h>
#include <AknsUtils.h>
#include <applayout.cdl.h>
#include <AknIconUtils.h>
#include <AknsConstants.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTestAppContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CTestAppContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL(); 

    // Get the skin instance
    iSkinInstance = AknsUtils::SkinInstance();

    // Create background control context for skinning the background
    iBackgroundSkinContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDSkinBmpMainPaneUsual, aRect, ETrue );
    
    iListBox = new (ELeave) CEikTextListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this );

    iItems = new( ELeave )CDesCArrayFlat( 4 );
    iListBox->Model()->SetItemTextArray( iItems );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iListBox->SetMopParent( this );

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CTestAppContainer::~CTestAppContainer()
    {
    delete iListBox;    
    delete iBackgroundSkinContext;
    }

// ---------------------------------------------------------
// CTestAppContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CTestAppContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CTestAppContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CTestAppContainer::CountComponentControls() const
    {
    return 1; 
    }

// ---------------------------------------------------------
// CTestAppContainer::AppendData
// ---------------------------------------------------------
//
void CTestAppContainer::AppendDataL( TDesC& buf )
    {
    iItems->AppendL( buf );
    iListBox->HandleItemAdditionL();
    TInt focus = iItems->Count()-1;
    if ( focus < 0 )
        focus=0;
    iListBox->SetCurrentItemIndexAndDraw( focus );
    }

// ---------------------------------------------------------
// CTestAppContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CTestAppContainer::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------
// CTestAppContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CTestAppContainer::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();
    
    AknsDrawUtils::Background( iSkinInstance, 
        iBackgroundSkinContext, this, gc, Rect() );
    }

// ---------------------------------------------------------
// CTestAppContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CTestAppContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// End of File  
