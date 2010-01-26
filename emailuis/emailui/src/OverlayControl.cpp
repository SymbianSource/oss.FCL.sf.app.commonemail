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
* Description: This file implements overlay controls
*
*/

#include "OverlayControl.h"

// CONSTS
const TReal KSolidTransparencyFactor = 2;


COverlayControl::COverlayControl( MOverlayControlObserver* aObserver )
: iObserver( aObserver )
	{
	}

COverlayControl::~COverlayControl()
	{
    delete iBitmap;
    delete iMask;
    delete iSolidMask;
	}

COverlayControl* COverlayControl::NewL( CCoeControl* aParent, MOverlayControlObserver* aObserver, 
        const TRect& aRect, TInt aBitmapId, TInt aMaskId )
	{
    COverlayControl* self = new (ELeave) COverlayControl( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( aParent, aRect, aBitmapId, aMaskId );
    CleanupStack::Pop( self );
	return self;
	}

void COverlayControl::ConstructL( CCoeControl* aParent, const TRect& aRect, TInt aBitmapId, TInt aMaskId )
	{
	iSolidMask = new(ELeave) CFbsBitmap();

    CreateWindowL( aParent );
	
	TFileName iconFileName;
	TFsEmailUiUtility::GetFullIconFileNameL( iconFileName );
	AknIconUtils::CreateIconL( iBitmap,
							   iMask,
							   iconFileName,
							   aBitmapId,
							   aMaskId );

	SetRect( aRect );
    EnableDragEvents();
	
	ActivateL();
	}

void COverlayControl::SetRect( const TRect& aRect )
    {
    if( iBitmap )
        {
        AknIconUtils::SetSize( iBitmap, aRect.Size() );
        }
    if( iMask )
        {
        AknIconUtils::SetSize( iMask, aRect.Size() );
        
        // Create a solid version of the mask, too
        TRAP_IGNORE(
            CopyBitmapL( *iMask, *iSolidMask );
            AdjustAlpha( *iSolidMask, KSolidTransparencyFactor );
            );
        
        UpdateMask();
        }
    CCoeControl::SetRect( aRect );
    }

void COverlayControl::CopyBitmapL( const CFbsBitmap& aSource, CFbsBitmap& aDest )
    {
    TSize size = aSource.SizeInPixels();
    TInt dataSize = aSource.DataStride() * size.iHeight;
    User::LeaveIfError( aDest.Create( size, aSource.DisplayMode() ) );
    
    CFbsBitGc* bitCtx;
    CFbsBitmapDevice* bitDev = CFbsBitmapDevice::NewL( &aDest );
    CleanupStack::PushL( bitDev );
    User::LeaveIfError( bitDev->CreateContext( bitCtx ) );
    CleanupStack::PushL( bitCtx );
    bitCtx->BitBlt( TRect( size ).iTl, &aSource );
    CleanupStack::PopAndDestroy( 2 ); // bitCtx, bitDev
    }

void COverlayControl::AdjustAlpha( CFbsBitmap& aBitmap, TReal aFactor )
    {
    aBitmap.LockHeap();
    TUint8* data = (TUint8*)aBitmap.DataAddress();
    TSize size = aBitmap.SizeInPixels();
    TInt dataSize = aBitmap.DataStride() * size.iHeight;
    for( TInt i = 0; i<dataSize; i++ )
        {
        // multiply each pixel by aFactor
        if( data[i] ) 
            {
            TInt value = aFactor * data[ i ];
            if( value > KMaxTUint8 ) 
                {
                value = KMaxTUint8;
                }
            data[ i ] = value;
            }
        }
    aBitmap.UnlockHeap();   
    }

void COverlayControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );

    // Do hit test for the pointer event
    TBool hit = EFalse;
    
    TSize size = Size();
    if( aPointerEvent.iPosition.iX >= 0 && 
         aPointerEvent.iPosition.iY >= 0 &&
         aPointerEvent.iPosition.iX < size.iWidth && 
         aPointerEvent.iPosition.iY < size.iHeight )
        {
        hit = ETrue;
        }
    
    if( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        SetPointerCapture( EFalse );
        UpdateButtonState( EFalse );
        
        // Notify the events only if released on top of the control 
        if( iObserver && hit )
            {
            // Do not let leaves disturb the system
            TRAP_IGNORE(
                iObserver->HandleOverlayPointerEventL( this, aPointerEvent );
                );
            }
        }
    else if( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        UpdateButtonState( ETrue );
        SetGloballyCapturing( ETrue );
        SetPointerCapture( ETrue );
        }
    else if( aPointerEvent.iType == TPointerEvent::EDrag )
        {
        // Update the button UI according to hit result
        UpdateButtonState( hit );
        }
    }

void COverlayControl::UpdateButtonState( TBool aButtonDown )
    {
    if( iDownState != aButtonDown )
        {
        iDownState = aButtonDown;
        UpdateMask();
        }
    }

void COverlayControl::MakeVisible( TBool aVisible )
    {
    if( !aVisible )
        {
        UpdateButtonState( EFalse );
        SetPointerCapture( EFalse );
        }
    CCoeControl::MakeVisible( aVisible );
    }

void COverlayControl::UpdateMask()
    {
    CFbsBitmap* mask = iMask;
    if( iDownState )
        {
        mask = iSolidMask;
        }
    if( mask )
        {
        Window().SetTransparencyBitmap( *mask );
        }
    }

void COverlayControl::Draw( const TRect& ) const 
	{
	CWindowGc& gc = SystemGc();

	if( iBitmap )
	    {
        TSize size = iBitmap->SizeInPixels();
        gc.BitBlt( TPoint( 0, 0 ), iBitmap );
	    }
	}
