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
* Description:  Class is responsible of showing the title for current "view"
*
*/

#include "cesmrtitlepane.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <akniconutils.h>
#include <gulcolor.h>
#include <aknutils.h>
#include <aknsconstants.h>
#include <aknsconstants.hrh>
#include <aknsutils.h>
#include <data_caging_path_literals.hrh>
#include <aknlayout2scalabledef.h>

#include "emailtrace.h"

/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

/**
 * KReplaceWhitespaceChars contains some characters that should be replaced by space
 * in Calendar popup, day view etc.
 * Following characters are replaced with space
 * \x0009 horizontal tab
 * \x000a new line
 * \x000b line tabulation (vertical
 * \x000c form feed
 * \x000d carriage return
 * \x2028 line separator
 * \x2029 paragraph separator
 */
_LIT( KReplaceWhitespaceChars, "\x0009\x000A\x000B\x000C\x000D\x2028\x2029" );

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTitlePane::NewL()
// ---------------------------------------------------------------------------
//
CESMRTitlePane* CESMRTitlePane::NewL()
    {
    FUNC_LOG;
    CESMRTitlePane* self = CESMRTitlePane::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::NewLC()
// ---------------------------------------------------------------------------
//
CESMRTitlePane* CESMRTitlePane::NewLC()
    {
    FUNC_LOG;
    CESMRTitlePane* self = new (ELeave) CESMRTitlePane();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::~CESMRTitlePane()
// ---------------------------------------------------------------------------
//
CESMRTitlePane::~CESMRTitlePane()
    {
    FUNC_LOG;
    delete iTitle;
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::CESMRTitlePane()
// ---------------------------------------------------------------------------
//
CESMRTitlePane::CESMRTitlePane()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::ConstructL()
    {
    FUNC_LOG;
    iEntryType = MESMRCalEntry::EESMRCalEntryNotSupported;

    iTitle = CMRLabel::NewL();
    iTitle->SetParent( this );
    iTitle->SetTextL( KNullDesC );
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::SizeChanged()
    {
    FUNC_LOG;
    TRect rect = Rect();

    TAknTextComponentLayout labelLayout =
        NMRLayoutManager::GetTextComponentLayout(
                NMRLayoutManager::EMRLayoutCtrlBarTitle );
    AknLayoutUtils::LayoutLabel( iTitle, rect, labelLayout );

    TRgb color;
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                        color,
                                        KAknsIIDQsnTextColors,
                                        EAknsCIQsnTextColorsCG2 );
    // If this leaves we'll have to use the default color
    TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL(
                                *iTitle,
                                EColorLabelText,
                                color ) );
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRTitlePane::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );

    if ( iTitle )
        ++count;

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRTitlePane::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch( aInd )
    	{
    	case 0:
    		{
    		return iTitle;
    		}
    	default:
    		return NULL;
    	}
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::SetTextL()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::SetTextL( const TDesC& aText )
    {
    FUNC_LOG;
    // Create new string buffer for trimming the text,
    // remove all linefeed/break etc.

    HBufC* titleForTrimming = aText.AllocLC();
    TPtr ptr = titleForTrimming->Des();
    AknTextUtils::ReplaceCharacters( ptr,
                                     KReplaceWhitespaceChars,
                                     TChar(' ') );

    titleForTrimming->Des().TrimAll();

    iTitle->SetTextL( *titleForTrimming );

    CleanupStack::PopAndDestroy( titleForTrimming );
    iTitle->CropText();
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    iEntryType = aEntry.Type();
    }


// EOF
