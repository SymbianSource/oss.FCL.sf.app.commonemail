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
* Description:  Base class for most of the fields in viewer
*
*/


#include "emailtrace.h"
#include <eiklabel.h>
#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <StringLoader.h>
#include <avkon.hrh>
#include <AknUtils.h>

#include <esmrgui.rsg>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "cesmrviewerlabelfield.h"
#include "cesmreditor.h"
#include "mesmrlistobserver.h"

// ---------------------------------------------------------------------------
// CESMRViewerLabelField::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRViewerLabelField* CESMRViewerLabelField::NewL()
    {
    FUNC_LOG;
    CESMRViewerLabelField* self = new (ELeave) CESMRViewerLabelField();
    CleanupStack::PushL( self );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerLabelField::~CESMRViewerLabelField
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRViewerLabelField::~CESMRViewerLabelField()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerLabelField::CESMRViewerLabelField
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRViewerLabelField::CESMRViewerLabelField()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerLabelField::InitializeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRViewerLabelField::InitializeL()
    {
    FUNC_LOG;
    iLabel->SetFont( iLayout->Font( iCoeEnv, iFieldId ));

    if( CESMRLayoutManager::IsMirrored() )
        {
        iLabel->iMargin.iRight = iLayout->TextSideMargin();
        }
    else
        {
        iLabel->iMargin.iLeft = iLayout->TextSideMargin();
        }

    // Set the text color.
    if ( IsFocused() )
        {
        AknLayoutUtils::OverrideControlColorL( *iLabel, EColorLabelText,
                                                iLayout->ViewerListAreaHighlightedTextColor() );
        }
    else
        {
        AknLayoutUtils::OverrideControlColorL( *iLabel, EColorLabelText,
                                                iLayout->ViewerListAreaTextColor() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerLabelField::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRViewerLabelField::ConstructL( TAknsItemID aIconID )
    {
    FUNC_LOG;
    // If field id is not specified, make this field a "basic" field:
    // Basic field can't be hidden from the list.
    if( iFieldId == 0 )
        {
        SetFieldId( EESMRFieldViewerBasic );
        }

    iLabel = new (ELeave) CEikLabel();
    iLabel->SetLabelAlignment(CESMRLayoutManager::IsMirrored() ?
        ELayoutAlignRight :
        ELayoutAlignLeft );

    _LIT( KEmptyText, "" );
    iLabel->SetTextL( KEmptyText );
    CESMRIconField::ConstructL( aIconID, iLabel, EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerLabelField::FocusChanged
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRViewerLabelField::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    FUNC_LOG;
    // Focus received
    if ( IsFocused() )
        {
        // If function leaves we continue w/o changing the color
        TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( 
        		*iLabel, EColorLabelText,
                 iLayout->ViewerListAreaHighlightedTextColor() ) );
        DrawDeferred();
        }
    else
        {
        // If function leaves we continue w/o changing the color
        TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( 
        		*iLabel, EColorLabelText,
                     iLayout->ViewerListAreaTextColor() ) );
        DrawDeferred();
        }
    }

// EOF

