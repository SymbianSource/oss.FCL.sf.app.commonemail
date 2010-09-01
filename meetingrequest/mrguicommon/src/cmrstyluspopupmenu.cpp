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
* Description: Attachment progress info
*
*/

#include "cmrstyluspopupmenu.h"

#include <aknstyluspopupmenu.h>

// ---------------------------------------------------------------------------
// CMRStylusPopupMenu::TMenuItem::TMenuItem
// ---------------------------------------------------------------------------
//
EXPORT_C CMRStylusPopupMenu::TMenuItem::TMenuItem()
: iTxt( KNullDesC() ), iCommandId( 0 )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRStylusPopupMenu::TMenuItem::TMenuItem
// ---------------------------------------------------------------------------
//
EXPORT_C CMRStylusPopupMenu::TMenuItem::TMenuItem(
        const TDesC& aTxt, TInt aCommandId )
: iTxt( aTxt ), iCommandId( aCommandId )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRStylusPopupMenu::CMRStylusPopupMenu
// ---------------------------------------------------------------------------
//
CMRStylusPopupMenu::CMRStylusPopupMenu( MEikMenuObserver& aObserver )
: iObserver( aObserver )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRStylusPopupMenu::ConstructL
// ---------------------------------------------------------------------------
//
void CMRStylusPopupMenu::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRStylusPopupMenu::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CMRStylusPopupMenu* CMRStylusPopupMenu::NewL( MEikMenuObserver& aObserver )
    {
    CMRStylusPopupMenu* self = new (ELeave) CMRStylusPopupMenu( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRStylusPopupMenu::~CMRStylusPopupMenu
// ---------------------------------------------------------------------------
//
EXPORT_C CMRStylusPopupMenu::~CMRStylusPopupMenu()
    {
    delete iPopup;
    }

// ---------------------------------------------------------------------------
// CMRStylusPopupMenu::LaunchPopupL
// ---------------------------------------------------------------------------
//
EXPORT_C void CMRStylusPopupMenu::LaunchPopupL(
        TArray<TMenuItem> aItems, const TPoint &aPosition )
    {
    if( iPopup )
        {
        delete iPopup;
        iPopup = NULL;
        }
    iPopup = CAknStylusPopUpMenu::NewL( &iObserver, aPosition );
    TInt count( aItems.Count() );
    for( TInt i = 0; i < count; ++i )
        {
        TMenuItem item = aItems[ i ];
        iPopup->AddMenuItemL( item.iTxt, item.iCommandId );
        }
    iPopup->SetPosition( aPosition );
    iPopup->ShowMenu();
    }
