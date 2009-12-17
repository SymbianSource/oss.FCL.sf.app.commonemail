/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  URL Parser ECom plugin entry point
*
*/
#include "cmrlabel.h"

#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <aknsconstants.hrh>
#include <gdi.h>

// LOCAL DEFINITIONS
namespace // codescanner::namespace 
    { 
    #define KDefaultTextColor TRgb( 0x000000 );
    /// Get the text color from skin
    TRgb TextColor( TInt aSkinColorId )
        {
        TRgb bgColor;
        TInt err;

        err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                         bgColor,
                                         KAknsIIDQsnTextColors,
                                         aSkinColorId );
        if( err != KErrNone )
            {
            bgColor = KDefaultTextColor;
            }
        return bgColor;
        }    
        
    }

// ---------------------------------------------------------------------------
// CMRLabel::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CMRLabel* CMRLabel::NewL()
    {
    CMRLabel* self = new (ELeave) CMRLabel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRLabel::~CMRLabel
// ---------------------------------------------------------------------------
//
EXPORT_C CMRLabel::~CMRLabel()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRLabel::FocusChanged
// ---------------------------------------------------------------------------
//
void CMRLabel::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    // Focus received
    if ( IsFocused() )
        {
        // If function leaves we continue w/o changing the color
        TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( 
                *this, EColorLabelText,
                TextColor( EAknsCIQsnHighlightColorsCG2 ) ) );
        }
    else
        {
        // If function leaves we continue w/o changing the color
        TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( 
                            *this, EColorLabelText,
                            TextColor( EAknsCIQsnTextColorsCG6 ) ) );
        }
    }

// ---------------------------------------------------------------------------
// CMRLabel::CMRLabel
// ---------------------------------------------------------------------------
//
CMRLabel::CMRLabel()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRLabel::ConstructL
// ---------------------------------------------------------------------------
//
void CMRLabel::ConstructL()
    {
    AknLayoutUtils::OverrideControlColorL( 
                        *this, EColorLabelText,
                        TextColor( EAknsCIQsnTextColorsCG6 ) );
    }


// End of file
