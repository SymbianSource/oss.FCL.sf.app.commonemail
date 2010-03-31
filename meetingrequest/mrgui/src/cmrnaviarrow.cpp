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
* Description:  Navigation arrow (used to replace the navipane).
*
*/

#include "cmrnaviarrow.h"
#include "nmrbitmapmanager.h"
#include "cesmrviewerdialog.h"
#include "esmricalvieweropcodes.hrh"

#include <aknbutton.h>
#include <gulicon.h>
#include <w32std.h>

#include "emailtrace.h"

namespace
{
// Value table for 70% Transparency 
const TInt KTransparency[256] =
    { 
    0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7,
    8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13,
    14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19,
    19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24,
    24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29,
    30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35,
    35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40,
    41, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 46,
    46, 46, 47, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 50, 51, 51,
    51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56,
    57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62,
    62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 67, 67, 67,
    68, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 71, 72, 72, 72, 73,
    73, 73, 74, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77 
    };

}


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRNaviArrow::NewL()
// ---------------------------------------------------------------------------
//
CMRNaviArrow* CMRNaviArrow::NewL( const CCoeControl* aParent, 
		TArrowType aType,
        MESMRNaviArrowEventObserver* aObserver,
        TSize aIconSize )
    {
    FUNC_LOG;
    CMRNaviArrow* self = new (ELeave) CMRNaviArrow( aObserver, aType, aIconSize );
    CleanupStack::PushL( self );
    self->ConstructL( aParent);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::CMRNaviArrow()
// ---------------------------------------------------------------------------
//
CMRNaviArrow::CMRNaviArrow( MESMRNaviArrowEventObserver* aObserver, TArrowType aType, TSize aIconSize )
    :iObserver(aObserver),
    iArrowType(aType),
    iIconSize( aIconSize )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::~CMRNaviArrow
// ---------------------------------------------------------------------------
//
CMRNaviArrow::~CMRNaviArrow()
    {
    FUNC_LOG;
    delete iArrow;
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::ConstructL()
// ---------------------------------------------------------------------------
//
void CMRNaviArrow::ConstructL( const CCoeControl* aParent )
    {
    FUNC_LOG;
    CreateWindowL( aParent );
    EnableWindowTransparency();
    InitNaviArrowsL();
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::InitNaviArrows()
// ---------------------------------------------------------------------------
//
void CMRNaviArrow::InitNaviArrowsL()
    {
    FUNC_LOG;
    // Button
    // TODO: Icons id should be changed to correct one when available
    CFbsBitmap* bitMapL( NULL );
    CFbsBitmap* bitMapMaskL( NULL );
    NMRBitmapManager::TMRBitmapId bitmapId;
    if ( EMRArrowLeft == iArrowType )
    	{
    	bitmapId = NMRBitmapManager::EMRBitmapMailPre;
    	}
    else
    	{
    	bitmapId = NMRBitmapManager::EMRBitmapMailNext;
    	}
    NMRBitmapManager::GetSkinBasedBitmapLC(
    		bitmapId, bitMapL, bitMapMaskL, iIconSize );
    
    CGulIcon* transparentIconL = CreateSemiTransparentIconL(  bitMapL, bitMapMaskL );
    CleanupStack::Pop( 2 ); // bitMap, bitMapMask
    CleanupStack::PushL( transparentIconL );
    iArrow = CAknButton::NewL( transparentIconL, NULL, NULL, NULL, KNullDesC,
            KNullDesC, KAknButtonNoFrame | KAknButtonPressedDownFrame, 0 );
    CleanupStack::Pop( transparentIconL );

    iArrow->SetParent( this );
    iArrow->SetContainerWindowL(*this);
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRNaviArrow::SizeChanged()
    {
    FUNC_LOG;
    iArrow->SetRect( Rect() );
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CMRNaviArrow::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count(0);
    if( iArrow )
        {
        count++;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CMRNaviArrow::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    CCoeControl* control = NULL;
    switch ( aInd )
        {
        case 0:                              
            control = iArrow;
            break;
        default:
            break;
        }

    return control;
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::HandlePointerEventL()
// ---------------------------------------------------------------------------
//
void CMRNaviArrow::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    FUNC_LOG;
    CCoeControl::HandlePointerEventL(aPointerEvent);
    TESMRIcalViewerOperationType operationType;
    if ( EMRArrowLeft == iArrowType )
    	{
    	operationType = EESMRCmdMailPreviousMessage;
    	}
    else
    	{
    	operationType = EESMRCmdMailNextMessage;
    	}
    if( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        if ( iArrow->Rect().Contains( aPointerEvent.iPosition ) )
            {
            iObserver->HandleNaviArrowEventL( operationType );
            }
        else
            {
            Parent()->HandlePointerEventL(aPointerEvent);
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::SetNaviArrowStatus()
// ---------------------------------------------------------------------------
//
void CMRNaviArrow::SetNaviArrowStatus( TBool aVisible )
    {
    FUNC_LOG;
    iArrow->MakeVisible( aVisible );
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::Draw()
// ---------------------------------------------------------------------------
//
void CMRNaviArrow::Draw(const TRect& /*aRect*/) const
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRNaviArrow::CreateSemiTransparentIconL()
// ---------------------------------------------------------------------------
//
CGulIcon* CMRNaviArrow::CreateSemiTransparentIconL( CFbsBitmap* aBitMap, 
                                                CFbsBitmap* aBitMapMask )
    {
    CGulIcon* newIcon(NULL);
    
    if ( aBitMap && aBitMapMask )
        {
        newIcon = CGulIcon::NewLC();
        newIcon->SetBitmap( aBitMap );

        CFbsBitmap* newMask = new ( ELeave ) CFbsBitmap;        
        CleanupStack::PushL( newMask );
        
        User::LeaveIfError( newMask->Create( 
            newIcon->Bitmap()->SizeInPixels(), EGray256 ) );        
           
        CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( newMask );
        CleanupStack::PushL( bitmapDevice );
        
        CFbsBitGc* bitGc( NULL );
        User::LeaveIfError( bitmapDevice->CreateContext( bitGc ) );
        CleanupStack::PushL( bitGc );

        bitGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
        bitGc->BitBlt( TPoint( 0, 0 ), aBitMapMask );
        
        newMask->LockHeap();
        TInt w = aBitMapMask->SizeInPixels().iWidth; 
        TInt h = aBitMapMask->SizeInPixels().iHeight;
        TInt dataStride = aBitMapMask->DataStride() - w; 
        unsigned char* address = (unsigned char *)newMask->DataAddress();  
        
        for ( TInt i = 0; i < h; ++i )
            {
            for ( TInt j = 0; j < w; ++j )
                {
                *address = KTransparency[*address];
                ++address;
                }
            address += dataStride;         
            }

        newMask->UnlockHeap();
        newIcon->SetMask( newMask );
                           
        CleanupStack::PopAndDestroy( 2 ); // bitmapDevice, bitGc
        CleanupStack::Pop( 2 ); // newIcon, newMask
        }
    
    return newIcon;
    }

// End of file
