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
* Description:  Holds the tracking status for MR attendee
 *
*/

// CLASS HEADER

#include "cesmrtrackstatus.h"

#include "cesmrfield.h"
#include "esmrdef.h"
#include "mesmrlistobserver.h"
#include "cesmrcontactmenuhandler.h"
#include "esmrhelper.h"
#include "nmrbitmapmanager.h"
#include "cmrimage.h"

#include <eiklabel.h>
#include <eikimage.h>
#include <calentry.h>
#include <caluser.h>
#include <eikenv.h>
#include <stringloader.h>
#include <aknsutils.h>
#include <aknutils.h>
#include <aknsconstants.h>
#include <aknbiditextutils.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>
#include <e32cmn.h>


// DEBUG
#include "emailtrace.h"

// TODO: THIS WHOLE CLASS NEEDS TO BE REIMPLEMENTED USING XML LAYOUT DATA
// IF THIS IS TAKEN INTO USE


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
    SetFieldId( EESMRTrackStatus );
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::ConstructL()
    {
    FUNC_LOG;
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapTrackingNone );
    iIcon->SetParent( this );

    iESMRStatic.ConnectL();
    iMenuHandler = &iESMRStatic.ContactMenuHandlerL();
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

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

    // TODO: set font with XML data
    //const CFont* font = iLayout->Font (iCoeEnv, iFieldId );        
    //TInt maxLineWidth = iLabel->Size().iWidth;        
    //maxLineWidth -= KIconSize.iWidth;
    
    //HBufC* clippedTextHBufC = ClipTextLC( text, *font, maxLineWidth );
    //TPtr clippedText = clippedTextHBufC->Des();
    //clippedText.Trim();
    //iLabel->SetTextL( clippedText );
    //CleanupStack::PopAndDestroy( clippedTextHBufC );
    //clippedTextHBufC = NULL;
    
    NMRBitmapManager::TMRBitmapId bitmapId;
    
    switch( status )
        {
        case CCalAttendee::ENeedsAction:
            {
            bitmapId = NMRBitmapManager::EMRBitmapTrackingNone;
            break;
            }
        case CCalAttendee::ETentative:
            {
            bitmapId = NMRBitmapManager::EMRBitmapTrackingTentative;
            break;
            }
        case CCalAttendee::EAccepted:
        case CCalAttendee::EConfirmed:
            {
            bitmapId = NMRBitmapManager::EMRBitmapTrackingAccept;
            break;
            }
        case CCalAttendee::EDeclined:
            {
            bitmapId = NMRBitmapManager::EMRBitmapTrackingReject;
            break;
            }
        default:
            {
            bitmapId = NMRBitmapManager::EMRBitmapTrackingNone;
            break;
            }
        }

    iESMRStatic.SetCurrentFieldIndex(++currentIndex); // Move to next index

    delete iIcon;
    iIcon = NULL;
    iIcon = CMRImage::NewL( bitmapId );
    iIcon->SetParent( this );
    
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
TKeyResponse CESMRTrackStatus::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode /*aType*/ )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    
    if ( aEvent.iScanCode == EStdKeyRightArrow )
        {
        // Show right click menu (action menu)

        iMenuHandler->ShowActionMenuL();
        response = EKeyWasConsumed;
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::SetOutlineFocusL(TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL (aFocus );
    
    SetActionMenuIconL(aFocus);
    TRgb fontColor( 0, 0, 0 );
    if ( aFocus )
        {
        if( iAddress )
            {
            //no longer needed
            delete iDefaultAddress;
            iDefaultAddress = NULL;
            //give contact to actionmenuhandler
            iMenuHandler->SetValueL(*iAddress, CESMRContactMenuHandler::EValueTypeEmail);
            }
        }
    else
        {
        iMenuHandler->Reset();
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRTrackStatus::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
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
    
    CCoeControl* control = NULL;
    
    if ( aInd == 2 )
        {
        control = iIcon;
        }
    
    ASSERT( control );
    
    return control;
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
        // TODO: correct this
        User::LeaveIfError( 
                NMRBitmapManager::GetSkinBasedBitmap( 
                        NMRBitmapManager::EMRBitmapRightClickArrow, 
                            iActionMenuIcon, iActionMenuIconMask, TSize(20, 20) ) );

        // TODO: USE XML LAYOUT DATA AND CMRIMAGE
        // Even if creating mask failed, bitmap can be used (background is just not displayed correctly)
        /*if( iActionMenuIcon )
            {
            iIcon = new (ELeave) CEikImage;
            iIcon->SetPictureOwnedExternally(ETrue);
            iIcon->SetPicture( iActionMenuIcon, iActionMenuIconMask );

            TRect rect = Rect ( );*/
            
            // TODO: use layout data 
            /*TInt iconTopMargin = ( rect.Height() - KIconSize.iHeight ) / 2;
            TPoint iconPos( rect.iBr.iX - KIconSize.iWidth - KIconBorderMargin,
                                rect.iBr.iY - iconTopMargin - KIconSize.iHeight);
            iIcon->SetPosition ( iconPos );
            iIcon->SetSize ( KIconSize );*/           
            //}
        }
    }

// ---------------------------------------------------------------------------
// CESMRTrackStatus::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRTrackStatus::SizeChanged( )
    {
    FUNC_LOG;

    if ( iIcon )
        {
        TRect rect = Rect ( );
        // TODO: use XML layout
        /*
        TInt iconTopMargin = ( rect.Height() - KIconSize.iHeight ) / 2;
        TPoint iconPos( rect.iBr.iX - KIconSize.iWidth - KIconBorderMargin,
                            rect.iBr.iY - iconTopMargin - KIconSize.iHeight);

        iIcon->SetPosition ( iconPos );
        iIcon->SetSize ( KIconSize );*/
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

