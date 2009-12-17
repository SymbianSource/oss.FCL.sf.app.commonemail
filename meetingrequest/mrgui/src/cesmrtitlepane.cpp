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
#include <AknIconUtils.h>
#include <gulcolor.h>
#include <AknUtils.h>
#include <AknsConstants.h>
#include <aknsconstants.hrh>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>
#include <AknLayout2ScalableDef.h>

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

#ifdef _DEBUG

// Definition for panic text
_LIT( KESMRPanicTxt, "CESMRTitlePane" );

/**
 * ES MR Entry panic codes
 */
enum TESMRPanicCodes
    {
    EESMREntryTypeNotSet = 1, // Entry does not exist
    };

/**
 * Raises panic.
 * @param aPanic Panic code
 */
void Panic(TESMRPanicCodes aPanic)
    {
    User::Panic( KESMRPanicTxt, aPanic);
    }

#endif // _DEBUG

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
    delete iIcon;
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
    
    iTitle->SetAlignment(EHCenterVCenter);
    
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

    if( iIcon )
        {
        TAknWindowComponentLayout iconLayout = 
            NMRLayoutManager::GetWindowComponentLayout( 
                    NMRLayoutManager::EMRLayoutCtrlBarGraphics );
        AknLayoutUtils::LayoutImage( iIcon, rect, iconLayout );
        }
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

    if ( iIcon )
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
    	case 1:
    		{
    		return iIcon;
    		}
    	default:
    		return NULL;
    	}
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::HandleSetPriorityCmdL()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::HandleSetPriorityCmdL( TInt aPriority )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntryType != MESMRCalEntry::EESMRCalEntryNotSupported, 
    		Panic(EESMREntryTypeNotSet ) );

    switch ( aPriority )
        {
        case EESMRCmdPriorityHigh:
            {
            if ( iEntryType == MESMRCalEntry::EESMRCalEntryTodo )
                {
                SetPriorityL(EFSCalenTodoPriorityHigh);
                }
            else
                {
                SetPriorityL(EFSCalenMRPriorityHigh);
                }
            break;
            }
        case EESMRCmdPriorityLow:
            {
            if ( iEntryType == MESMRCalEntry::EESMRCalEntryTodo )
                {
                SetPriorityL(EFSCalenTodoPriorityLow);
                }
            else
                {
                SetPriorityL(EFSCalenMRPriorityLow);
                }
            break;
            }
        case EESMRCmdPriorityNormal:
            {
            if ( iEntryType == MESMRCalEntry::EESMRCalEntryTodo )
                {
                SetPriorityL(EFSCalenTodoPriorityNormal);
                }
            else
                {
                SetPriorityL(EFSCalenMRPriorityNormal);
                }
            break;
            }
        default:
        	break;
        }
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::SetPriorityL()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::SetPriorityL(TUint aPriority )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iEntryType != MESMRCalEntry::EESMRCalEntryNotSupported, 
    		Panic(EESMREntryTypeNotSet ) );

    iPriority = aPriority;

    // icon added to MR-title only if the default icon is set and
    // the priority is neither high or low (the possible high/low
    // importance icons are currently only shown in navi pane).
    delete iIcon;
    iIcon = NULL;
    if ( iDefaultIcon &&
         iEntryType != MESMRCalEntry::EESMRCalEntryTodo && 
         aPriority != EFSCalenMRPriorityHigh &&
         aPriority != EFSCalenMRPriorityLow )
        {
        CreateIconL( KAknsIIDQgnFscalIndiSubject );
        }
    
    SizeChanged();
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::CreateIconL()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::CreateIconL( TAknsItemID aIconID  )
    {
    FUNC_LOG;
    delete iIcon;
    iIcon = NULL;

    iIcon = CMRImage::NewL( aIconID );
    iIcon->SetParent( this );
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
    SetPriorityL( aEntry.GetPriorityL() );
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::SetDefaultIcon()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::SetDefaultIcon( TBool aDefault )
    {
    FUNC_LOG;
    iDefaultIcon = aDefault;
    }

// ---------------------------------------------------------------------------
// CESMRTitlePane::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRTitlePane::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest ||
         aEntry.Type() == MESMRCalEntry::EESMRCalEntryMeeting )
        {
        aEntry.SetPriorityL( iPriority );
        }
    }

// EOF
