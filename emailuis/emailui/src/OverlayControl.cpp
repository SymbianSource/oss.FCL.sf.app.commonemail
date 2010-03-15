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

COverlayControl::COverlayControl( MOverlayControlObserver* aObserver )
: iObserver( aObserver )
	{
	}

COverlayControl::~COverlayControl()
	{
    delete iBitmap;
    delete iMask;
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
    SetContainerWindowL( *aParent );
	TFileName iconFileName;
	TFsEmailUiUtility::GetFullIconFileNameL( iconFileName );
	AknIconUtils::CreateIconL( iBitmap,
							   iMask,
							   iconFileName,
							   aBitmapId,
							   aMaskId );
	SetRect( aRect );
    iTouchFeedBack = MTouchFeedback::Instance();
    iTouchFeedBack->EnableFeedbackForControl(this, ETrue);
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
        }
    CCoeControl::SetRect( aRect );
    }

void COverlayControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {      
        // Give feedback to user (vibration)     
        iTouchFeedBack->InstantFeedback( this, ETouchFeedbackBasic );
        SetPointerCapture( ETrue );        
        }
    CCoeControl::HandlePointerEventL( aPointerEvent );
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        if ( Rect().Contains( aPointerEvent.iPosition ) && iObserver )
            {
            // Do not let leaves disturb the system - send event only if the poimter 
            // was pressed on this control
            TRAP_IGNORE( iObserver->HandleOverlayPointerEventL( this, aPointerEvent ) );
            }
        SetPointerCapture( EFalse );        
        }
     }

void COverlayControl::MakeVisible( TBool aVisible )
    {
    if( !aVisible )
        {
        SetPointerCapture( EFalse );
        }
    else
        {
        DrawDeferred();
        }
    CCoeControl::MakeVisible( aVisible );
    }

void COverlayControl::Draw( const TRect& ) const 
	{
	CWindowGc& gc = SystemGc();
	if( iBitmap )
	    {
        TSize size = iBitmap->SizeInPixels();
        gc.BitBltMasked(Position(),iBitmap,TRect( TPoint(0,0), size),iMask, ETrue);
        }
	}
