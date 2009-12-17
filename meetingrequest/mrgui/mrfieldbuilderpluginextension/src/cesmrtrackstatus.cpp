/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Holds the tracking status for MR attendee
 *
*/

// CLASS HEADER
#include "cesmrtrackstatus.h"
#include "cesmrfield.h"
#include "cesmriconfield.h"
#include "cesmrborderlayer.h"
#include "esmrdef.h"
#include "mesmrlistobserver.h"
#include "cesmrcontactmenuhandler.h"
#include "esmrhelper.h"
#include "nmrbitmapmanager.h"
#include <eiklabel.h>
#include <eikimage.h>
#include <calentry.h>
#include <caluser.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <AknsConstants.h>
#include <AknBidiTextUtils.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>

// DEBUG
#include "emailtrace.h"

using namespace ESMRLayout;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTrackStatus::NewL()
// ---------------------------------------------------------------------------
//
CESMRTrackStatus* CESMRTrackStatus::NewL()
    {
    FUNC_LOG;
    CESMRTrackStatus* self = new (ELeave) CESMRTrackStatus();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::~CESMRTrackStatus()
// ---------------------------------------------------------------------------
//
CESMRTrackStatus::~CESMRTrackStatus()
    {
    FUNC_LOG;
    if ( iMenuHandler )
        {
        iMenuHandler->Reset();
        }
    iESMRStatic.Close();
    delete iDefaultAddress;
    delete iAddress;
    delete iCommonName;
    delete iIcon;
    delete iActionMenuIcon;
    delete iActionMenuIconMask;
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::CESMRTrackStatus()
// ---------------------------------------------------------------------------
//
CESMRTrackStatus::CESMRTrackStatus()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::ConstructL()
    {
    FUNC_LOG;
    // As a default set icon and text as it should be in status Needs action
    SetFieldId( EESMRTrackStatus );
    CESMRViewerLabelField::ConstructL( KAknsIIDQgnFsIndiCaleTrackingNone );
    iESMRStatic.ConnectL();
    iMenuHandler = &iESMRStatic.ContactMenuHandlerL();

    iIcon = new (ELeave) CEikImage;
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TAknsItemID statusIconId = KAknsIIDQgnFsIndiCaleTrackingNone;
    RPointerArray<CCalAttendee> attendee = aEntry.Entry().AttendeesL();
    // The array is already in sorted order with ReqAttendee coming first and
    // OptAttendee  coming later.
    TInt currentIndex = iESMRStatic.CurrentFieldIndex();
    
    if(currentIndex >= attendee.Count())
        {
        User::Leave( KErrArgument );
        }
        
    TPtrC text;
    CCalAttendee::TCalStatus status = attendee[currentIndex]->StatusL();    
    if (attendee[currentIndex]->Address().Length() >0 )
        {
        if ( attendee[currentIndex]->CommonName().Length() > 0 )
            {
            text.Set( attendee[currentIndex]->CommonName() );
            iCommonName = text.AllocL();
            }
        else
            {
            text.Set( attendee[currentIndex]->Address() );
            }
        iAddress = attendee[currentIndex]->Address().AllocL();
        }

    const CFont* font = iLayout->Font (iCoeEnv, iFieldId );        
    TInt maxLineWidth = iLabel->Size().iWidth;        
    maxLineWidth -= KIconSize.iWidth;
    
    HBufC* clippedTextHBufC = ClipTextLC( text, *font, maxLineWidth );
    TPtr clippedText = clippedTextHBufC->Des();
    clippedText.Trim();
    iLabel->SetTextL( clippedText );
    CleanupStack::PopAndDestroy( clippedTextHBufC );
    clippedTextHBufC = NULL;
    
    switch( status )
        {
        case CCalAttendee::ENeedsAction:
            {
            statusIconId = KAknsIIDQgnFsIndiCaleTrackingNone;
            break;
            }
        case CCalAttendee::ETentative:
            {
            statusIconId = KAknsIIDQgnFsIndiCaleTrackingTentative;
            break;
            }
        case CCalAttendee::EAccepted:
        case CCalAttendee::EConfirmed:
            {
            statusIconId = KAknsIIDQgnFsIndiCaleTrackingAccept;
            break;
            }
        case CCalAttendee::EDeclined:
            {
            statusIconId = KAknsIIDQgnFsIndiCaleTrackingReject;
            break;
            }
        default:
            {
            statusIconId = KAknsIIDQgnFsIndiCaleTrackingNone;
            break;
            }
        }

        iESMRStatic.SetCurrentFieldIndex(++currentIndex); // Move to next index

        IconL( statusIconId );
        
        //store default list selection to contactmenuhandler
        iDefaultAddress = attendee[0]->Address().AllocL();
        iMenuHandler->SetValueL(*iDefaultAddress, CESMRContactMenuHandler::EValueTypeEmail);

        // This needs to be called so icon will be redrawn
        SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRTrackStatus::OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType )
    {
    FUNC_LOG;
    if ( aEvent.iScanCode == EStdKeyRightArrow )
        {
        // Show right click menu (action menu)

        iMenuHandler->ShowActionMenuL();
        return EKeyWasConsumed;
        }

    return CESMRViewerLabelField::OfferKeyEventL(aEvent,aType);
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::SetOutlineFocusL(TBool aFocus )
    {
    FUNC_LOG;
    CESMRViewerLabelField::SetOutlineFocusL (aFocus );
    SetActionMenuIconL(aFocus);
    TRgb fontColor( 0, 0, 0 );
    if ( aFocus )
        {
        if(iAddress)
            {
            //no longer needed
            delete iDefaultAddress;
            iDefaultAddress = NULL;
            //give contact to actionmenuhandler
            iMenuHandler->SetValueL(*iAddress, CESMRContactMenuHandler::EValueTypeEmail);
            }
        fontColor = iLayout->ViewerListAreaHighlightedTextColor();
        }
    else
        {
        iMenuHandler->Reset();
        fontColor = iLayout->ViewerListAreaTextColor();
        }

    TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL(
                                      *iLabel,
                                      EColorLabelText,
                                      fontColor ) );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRTrackStatus::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = CESMRViewerLabelField::CountComponentControls ( );
    if ( iIcon )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRTrackStatus::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    if ( aInd == 2 )
        {
        return iIcon;
        }
    return CESMRViewerLabelField::ComponentControl ( aInd );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SetIconL
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::SetActionMenuIconL( TBool aFocused )
    {
    FUNC_LOG;
    delete iIcon;
    iIcon = NULL;
    delete iActionMenuIcon;
    iActionMenuIcon = NULL;
    delete iActionMenuIconMask;
    iActionMenuIconMask = NULL;

    if( aFocused )
        {
        User::LeaveIfError( 
                NMRBitmapManager::GetSkinBasedBitmap( 
                        NMRBitmapManager::EMRBitmapRightClickArrow, 
                            iActionMenuIcon, iActionMenuIconMask, KIconSize ) );

        // Even if creating mask failed, bitmap can be used (background is just not displayed correctly)
        if( iActionMenuIcon )
            {
            iIcon = new (ELeave) CEikImage;
            iIcon->SetPictureOwnedExternally(ETrue);
            iIcon->SetPicture( iActionMenuIcon, iActionMenuIconMask );

            TRect rect = Rect ( );
            TInt iconTopMargin = ( rect.Height() - KIconSize.iHeight ) / 2;
            TPoint iconPos( rect.iBr.iX - KIconSize.iWidth - KIconBorderMargin,
                                rect.iBr.iY - iconTopMargin - KIconSize.iHeight);
            iIcon->SetPosition ( iconPos );
            iIcon->SetSize ( KIconSize );           
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::SizeChanged( )
    {
    FUNC_LOG;
    CESMRViewerLabelField::SizeChanged();

    if ( iIcon )
        {
        TRect rect = Rect ( );
        TInt iconTopMargin = ( rect.Height() - KIconSize.iHeight ) / 2;
        TPoint iconPos( rect.iBr.iX - KIconSize.iWidth - KIconBorderMargin,
                            rect.iBr.iY - iconTopMargin - KIconSize.iHeight);

        iIcon->SetPosition ( iconPos );
        iIcon->SetSize ( KIconSize );
        }
    }

// -----------------------------------------------------------------------------
// CESMRTrackStatus::ClipTextLC
// -----------------------------------------------------------------------------
//
HBufC* CESMRTrackStatus::ClipTextLC( 
        const TDesC& aText, 
        const CFont& aFont, 
        TInt aWidth )
    {
    FUNC_LOG;
    HBufC* text = HBufC::NewLC( aText.Length() + KAknBidiExtraSpacePerLine );
    TPtr textPtr = text->Des();
    AknBidiTextUtils::ConvertToVisualAndClip( aText, textPtr, aFont, aWidth, aWidth );
    return text;
    }

// End of file

