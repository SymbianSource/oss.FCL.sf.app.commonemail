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
* Description:  Fade effect implementation.
*
*/



//////SYSTEM INCLUDES

//////TOOLKIT INCLUDES
// <cmail> SF
#include "emailtrace.h"
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
#include "fsfadeeffect.h"
//<cmail>
#include "fsfadeeffectobserver.h"
//</cmail>



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsFadeEffect* CFsFadeEffect::NewL(CAlfControl* aControl,CAlfVisual* aRoot, TInt aTime)
    {
    FUNC_LOG;
    CFsFadeEffect* self = new (ELeave) CFsFadeEffect(aControl, aRoot, aTime);
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//    
CFsFadeEffect* CFsFadeEffect::NewL(
    CAlfControl* aControl,
    MFsFadeEffectObserver* aObserver, 
    CAlfVisual* aRoot, 
    TInt aTime)
    {
    FUNC_LOG;
    CFsFadeEffect* self = new (ELeave) CFsFadeEffect(aControl, aObserver, aRoot, aTime);    
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//    
CFsFadeEffect::CFsFadeEffect(CAlfControl* aControl,CAlfVisual* aRoot, TInt aTime)
    : iControl( aControl ), 
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
CFsFadeEffect::CFsFadeEffect(
    CAlfControl* aControl,
    MFsFadeEffectObserver* aObserver, 
    CAlfVisual* aRoot, TInt aTime)
    : iControl( aControl ),
      iRoot(aRoot),
      iObserver(aObserver), 
      iTime(aTime)
    {
    FUNC_LOG;
    //No implementation nedeed
    }

// ---------------------------------------------------------------------------
// Launches fade in effect.
// ---------------------------------------------------------------------------
//    
void CFsFadeEffect::FadeIn()
    {
    FUNC_LOG;
    TAlfTimedValue opacity;
    opacity.SetValueNow( 0 );
    iRoot->SetOpacity( opacity );
    
    opacity.SetTarget( 1.0f, iTime );    
    iRoot->SetOpacity(opacity); 
    
    if (iObserver)
        {
        iObserver->FadeEffectEvent(MFsFadeEffectObserver::EFadingIn);
        }
    //cancel pending events
    if ( iControl )
        {
        iControl->Env().CancelCustomCommands(this);

        //send a command to inform about an end of fade out effect
        TInt error = iControl->Env().Send(TAlfCustomEventCommand(ECustomEventFadeInFinished, this), iTime);
        if (error != KErrNone)
            {
            
            }
        }
    }
    

// ---------------------------------------------------------------------------
// Launches fade out effect.
// ---------------------------------------------------------------------------
//        
void CFsFadeEffect::FadeOut()
    {
    FUNC_LOG;
    TAlfTimedValue opacity;
    opacity.SetTarget( 0.0f, iTime ); 
    iRoot->SetOpacity(opacity);
    
    if (iObserver)
        {
        iObserver->FadeEffectEvent(MFsFadeEffectObserver::EFadingOut);
        }
    
    //cancel pending events
    if ( iControl )
        {
        iControl->Env().CancelCustomCommands(this);

        //send a command to inform about an end of fade out effect
        TInt error = iControl->Env().Send(TAlfCustomEventCommand(ECustomEventFadeOutFinished, this), iTime);
        if (error != KErrNone)
            {
            
            }
        }
    }

    
// ---------------------------------------------------------------------------
// Sets speed of fade in/out effect.
// ---------------------------------------------------------------------------
//        
void CFsFadeEffect::SetTime( const TInt aTime)
    {
    FUNC_LOG;
    iTime = aTime;
    }
    

// ---------------------------------------------------------------------------
// Returns speed of fade in/out effect.
// ---------------------------------------------------------------------------
//    
TInt CFsFadeEffect::Time() const
    {
    FUNC_LOG;
    return iTime;    
    }


// ---------------------------------------------------------------------------
// From MAlfEventHandler
// Called when an input event is being offered to this object.
// ---------------------------------------------------------------------------
//
TBool CFsFadeEffect::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool result(EFalse);
    if( aEvent.IsCustomEvent() )
        {
        if ( aEvent.CustomParameter() == ECustomEventFadeInFinished )
            {
            result = ETrue;
            //inform observer
            if ( iObserver )
                {
                iObserver->FadeEffectEvent(MFsFadeEffectObserver::EFadeInFinished);
                }
            }
        else if ( aEvent.CustomParameter() == ECustomEventFadeOutFinished )
            {
            result = ETrue;
            //inform observer
            if ( iObserver )
                {
                iObserver->FadeEffectEvent(MFsFadeEffectObserver::EFadeOutFinished);
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

