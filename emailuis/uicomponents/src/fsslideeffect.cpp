/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle tree node implementation.
*
*/


//////SYSTEM INCLUDES
#include "emailtrace.h"
#include <AknUtils.h>

//////TOOLKIT INCLUDES
// <cmail> SF
#include <alf/alfvisual.h>
#include <alf/alftimedvalue.h>
#include <alf/alflayout.h>
#include <alf/alfenv.h>
#include <alf/alfcommand.h>
#include <alf/alfcontrol.h>
#include <alf/alfevent.h>
// </cmail>

//////PROJECT INCLUDES
//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include "fsslideeffect.h"
#include "fsslideeffectobserver.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsSlideEffect* CFsSlideEffect::NewL( CAlfControl* aControl, CAlfVisual* aRoot, TInt aTime )
    {
    FUNC_LOG;
    CFsSlideEffect* self = new (ELeave) CFsSlideEffect( aControl, aRoot, aTime );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//    
CFsSlideEffect* CFsSlideEffect::NewL(
    CAlfControl* aControl,
    MFsSlideEffectObserver* aObserver, 
    CAlfVisual* aRoot, 
    TInt aTime)
    {
    FUNC_LOG;
    CFsSlideEffect* self = new (ELeave) CFsSlideEffect( aControl, aObserver, aRoot, aTime );
    return self;
    }

    
// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//    
CFsSlideEffect::CFsSlideEffect(CAlfControl* aControl, CAlfVisual* aRoot, TInt aTime)
    : iControl(aControl), 
      iRoot(aRoot), 
      iTime(aTime)
    {
    FUNC_LOG;
    //No implementation nedeed
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFsSlideEffect::CFsSlideEffect(
    CAlfControl* aControl,
    MFsSlideEffectObserver* aObserver, 
    CAlfVisual* aRoot, TInt aTime)
    : iControl(aControl),
      iRoot(aRoot),
      iObserver(aObserver), 
      iTime(aTime)
    {
    FUNC_LOG;
    //No implementation needed.
    }

// ---------------------------------------------------------------------------
// Launches slide in effect.
// ---------------------------------------------------------------------------
//    
void CFsSlideEffect::SlideIn(
    CFsSlideEffect::TSlideEffectDirection aDirection)
    {
    FUNC_LOG;
    
    TAlfRealPoint pos( iRoot->Layout()->Pos().ValueNow() );
    TAlfRealPoint size( iRoot->Layout()->Size().ValueNow() );

    TAlfRealRect rectFinal;
    TAlfRealRect rectStart;    
    
    //coordinates in reference to parent layout
    rectFinal.iTl.iX = 0;
    rectFinal.iTl.iY = 0;
    rectFinal.iBr.iX = size.iX;
    rectFinal.iBr.iY = size.iY;
    
    TRect sreenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, sreenRect );
    
    switch(aDirection)
        {
        case ESlideNone:
            {
            iRoot->SetRect(rectFinal,0);
            break;
            }
        case ESlideFromTop:
            {
            rectStart.iTl.iX = rectFinal.iTl.iX;
            rectStart.iTl.iY = -pos.iY - size.iY;
            rectStart.iBr.iX = rectFinal.iTl.iX + size.iX;
            rectStart.iBr.iY = -pos.iY;
            iRoot->SetRect( rectStart, 0 );
            iRoot->SetRect(rectFinal,iTime);
            break;
            }
            
        case ESlideFromBottom:
            {
            rectStart.iTl.iX = rectFinal.iTl.iX;
            rectStart.iTl.iY = sreenRect.Height() - pos.iY;
            rectStart.iBr.iX = rectFinal.iTl.iX + size.iX;
            rectStart.iBr.iY = sreenRect.Height() - pos.iY + size.iY;
            iRoot->SetRect( rectStart, 0 );
            iRoot->SetRect(rectFinal,iTime);
            break;
            }
            
        case ESlideFromLeft:
            {
            rectStart.iTl.iX = -pos.iX - size.iX;
            rectStart.iTl.iY = rectFinal.iTl.iY;
            rectStart.iBr.iX = -pos.iX;
            rectStart.iBr.iY = rectFinal.iBr.iY;
            iRoot->SetRect( rectStart, 0 );
            iRoot->SetRect(rectFinal,iTime);
            break;
            }
            
        case ESlideFromRight:
            {
            rectStart.iTl.iX = sreenRect.Width();
            rectStart.iTl.iY = rectFinal.iTl.iY;
            rectStart.iBr.iX = sreenRect.Width() + size.iX;
            rectStart.iBr.iY = rectFinal.iBr.iY;
            iRoot->SetRect( rectStart, 0 );
            iRoot->SetRect(rectFinal,iTime);
            break;
            }
        default:
            return;
        }
        
    
    if (iObserver)
        {
        iObserver->SlideEffectEvent(MFsSlideEffectObserver::ESlidingIn);
        }
    
    //cancel pending events
    if ( iControl )
        {
        iControl->Env().CancelCustomCommands(this);

        //send a command to inform about end of slide in effect
        TInt error = iControl->Env().Send(TAlfCustomEventCommand( ECustomEventSlideInFinished, this), iTime);
        if (error != KErrNone)
            {
            
            }
        }
    }
    
// ---------------------------------------------------------------------------
// Launches slide out effect.
// ---------------------------------------------------------------------------
//        
void CFsSlideEffect::SlideOut(
    CFsSlideEffect::TSlideEffectDirection aDirection)
    {
    FUNC_LOG;
    TAlfRealPoint pos = iRoot->Layout()->Pos().ValueNow();
    TAlfRealPoint size = iRoot->Layout()->Size().ValueNow();
    
    TAlfRealRect rectStart;
    TAlfRealRect rectFinal;

    rectStart.iTl.iX = 0;
    rectStart.iTl.iY = 0;
    rectStart.iBr.iX = size.iX;
    rectStart.iBr.iY = size.iY;
    
    TRect sreenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, sreenRect );
    
    switch( aDirection )
        {
        case ESlideNone:
            {
            iRoot->SetRect( rectStart, 0 );
            break;
            }
        case ESlideFromTop:
            {
            rectFinal.iTl.iX = rectStart.iTl.iX;
            rectFinal.iTl.iY = (pos.iY - sreenRect.Height()) - size.iY;
            rectFinal.iBr.iX = rectStart.iBr.iX;
            rectFinal.iBr.iY = (pos.iY - sreenRect.Height());            
            iRoot->SetRect( rectFinal ,iTime );
            break;
            }
            
        case ESlideFromBottom:
            {
            rectFinal.iTl.iX = rectStart.iTl.iX;
            rectFinal.iTl.iY = sreenRect.Height() - pos.iY;
            rectFinal.iBr.iX = rectStart.iBr.iX;
            rectFinal.iBr.iY = sreenRect.Height() - pos.iY + size.iY;
            iRoot->SetRect( rectFinal ,iTime );
            break;
            }
            
        case ESlideFromLeft:
            {
            rectFinal.iTl.iX = ( pos.iX - sreenRect.Width() ) - size.iX;
            rectFinal.iTl.iY = rectStart.iTl.iY;
            rectFinal.iBr.iX = pos.iX - sreenRect.Width();
            rectFinal.iBr.iY = rectStart.iBr.iY;
            iRoot->SetRect( rectFinal ,iTime );
            break;
            }
            
        case ESlideFromRight:
            {
            rectFinal.iTl.iX = sreenRect.Width();
            rectFinal.iTl.iY = rectStart.iTl.iY;
            rectFinal.iBr.iX = sreenRect.Width() + size.iX;
            rectFinal.iBr.iY = rectStart.iBr.iY;
            iRoot->SetRect( rectFinal ,iTime );
            break;
            }
        default:
            return;
        }
        
    
    if ( iObserver )
        {
        iObserver->SlideEffectEvent( MFsSlideEffectObserver::ESlidingOut );
        }
    
    //cancel pending events
    if ( iControl )
        {
        iControl->Env().CancelCustomCommands(this);

        //send a command to inform about end of slide out effect
        TInt error = iControl->Env().Send(TAlfCustomEventCommand( ECustomEventSlideOutFinished, this), iTime);
        if (error != KErrNone)
            {
            
            }
        }
    }
    
// ---------------------------------------------------------------------------
// Sets duration of slide in/out effect.
// ---------------------------------------------------------------------------
//        
void CFsSlideEffect::SetTime( const TInt aTime)
    {
    FUNC_LOG;
    iTime = aTime;
    }
    

// ---------------------------------------------------------------------------
// Returns duration of the slide effect
// ---------------------------------------------------------------------------
//    
TInt CFsSlideEffect::Time() const
    {
    FUNC_LOG;
    return iTime;    
    }


// ---------------------------------------------------------------------------
// From MAlfEventHandler
// Called when an input event is being offered to this object.
// ---------------------------------------------------------------------------
//
TBool CFsSlideEffect::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool result(EFalse);
    if( aEvent.IsCustomEvent() )
        {
        if ( aEvent.CustomParameter() == ECustomEventSlideInFinished )
            {
            result = ETrue;
            //inform observer
            if ( iObserver )
                {
                iObserver->SlideEffectEvent(MFsSlideEffectObserver::ESlideInFinished);
                }
            }
        else if ( aEvent.CustomParameter() == ECustomEventSlideOutFinished )
            {
            result = ETrue;
            //inform observer
            if ( iObserver )
                {
                iObserver->SlideEffectEvent(MFsSlideEffectObserver::ESlideOutFinished);
                }
            }
        }
    else
        {
        result = EFalse;
        }
    
    return result;
    }


//End Of File

