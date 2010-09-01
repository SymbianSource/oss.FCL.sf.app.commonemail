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
* Description:  A controlet class for doing marquee text scrolling.
*
*/


#include "emailtrace.h"
#include <alf/alfviewportlayout.h>
#include <alf/alfevent.h>
#include <alf/alfenv.h>
#include <alf/alfcontrol.h>

#include "fsmarqueeclet.h"


// ---------------------------------------------------------------------------
//  Constructor.
// ---------------------------------------------------------------------------
//
TFsMarqueeClet::TFsMarqueeClet(CAlfControl& aControl, CAlfViewportLayout& aViewport, TScrollDirection aDir, TScrollType aType)
    : iViewport(aViewport),
      iControl(aControl),
      iDir(aDir), 
      iType(aType), 
      iTime(20),
      iRepetitions(1),
      iCycleDelay(1000)
    {
    FUNC_LOG;
    iViewport.SetVirtualSize(TAlfRealPoint(1.0, 1.0), 0);
    iViewport.SetViewportSize(TAlfRealPoint(1.0, 1.0), 0);
    iViewport.SetViewportPos(TAlfRealPoint(0, 0), 0);
    }


// ---------------------------------------------------------------------------
//  Destructor. Cancels uppdate event.
// ---------------------------------------------------------------------------
//
TFsMarqueeClet::~TFsMarqueeClet()
    {
    FUNC_LOG;
    // Cancel any previous update events.
    iControl.Env().CancelCustomCommands(this, ECustomEventScrollingFinished);
    iControl.Env().CancelCustomCommands(this, ECustomEventScrollingPauseFinished);
    iControl.Env().CancelCustomCommands(this, ECustomEventScrollingStartPauseFinished);
    }

// ---------------------------------------------------------------------------
//  Set the type of marquee.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::SetScrollType( const TScrollType aType )
    {
    FUNC_LOG;
    iType = aType;
    }


// ---------------------------------------------------------------------------
//  The number of milliseconds to complete a full scroll cycle.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::SetScrollSpeed(TInt aTime)
    {
    FUNC_LOG;
    iTime = aTime;
    }


// ---------------------------------------------------------------------------
//  Sets the number of marquee's scroll cycles.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::SetScrollRepetitions( TInt aRepetitions )
    {
    FUNC_LOG;
    iRepetitions = aRepetitions;
    }
    

// ---------------------------------------------------------------------------
//  The function sets time for a pause after each cycle.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::SetScrollCycleDelay ( TInt aCycleDelay )
    {
    FUNC_LOG;
    iCycleDelay = aCycleDelay;
    }
    
    
// ---------------------------------------------------------------------------
//  Change the direction that the text scrolls from.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::SetScrollDirection(TScrollDirection aDir)
    {
    FUNC_LOG;
    iDir = aDir;
    }


// ---------------------------------------------------------------------------
//  The area that we want to scroll.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::SetScrollableArea(TSize& aScrollableArea)
    {
    FUNC_LOG;
    iScrollableArea = aScrollableArea;
    }


// ---------------------------------------------------------------------------
//  Begin scrolling with the specified speed, direction and type.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::StartScrolling()
    {
    FUNC_LOG;
    iViewport.SetVirtualSize(iScrollableArea, 0);
    iViewport.SetViewportSize(iViewport.Size().Target(), 0);
    iViewport.SetViewportPos(TAlfRealPoint(0, 0), 0);
    
    if ( iType == EScrollBounce )
        {
        iRepetitions *= 2;
        }
//    if ( iRepetitions != 0 )
//        {
//        if ( iRepetitions > 0 )
//            {
//            --iRepetitions;
//            }
        
        switch(iType)
            {
            case EScrollForth:            
                StartScrollingForth();
                break;
                
            case EScrollBounce:
                StartScrollingBounce();
                break;
                
            case EScrollWrap:            
                StartScrollingLoop();
                break;
            } 
//        }
    }


// ---------------------------------------------------------------------------
//  Stop scrolling and reset the visual to its zero position.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::StopScrolling()
    {
    FUNC_LOG;
        
    iViewport.SetVirtualSize(iViewport.Size().Target(), 0);
    iViewport.SetViewportSize(iViewport.Size().Target(), 0);
    iViewport.SetViewportPos(TAlfRealPoint(0, 0), 0);

    // Cancel any previous update events.
    CancelScrollingCommands();
    }


// ---------------------------------------------------------------------------
//  Initiate scrolling once then stop.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::StartScrollingOnce()
    {
    FUNC_LOG;
    TReal visualWidth = iViewport.Size().Target().iX; 
    TReal widthOffScreen = (iScrollableArea.iWidth-visualWidth); 
    
    // Reset the viewport to its start position, then set the end position with a timed value.
    switch (iDir)
        {
        case EScrollFromLeft:
            // Reset the viewport to its start position, then set the end position with a timed value.
            iViewport.SetViewportPos(TAlfRealPoint(widthOffScreen, 0), 0);
            iViewport.SetViewportPos(TAlfRealPoint(0, 0), iTime);
            break;
            
        case EScrollFromRight:
            // Reset the viewport to its start position, then set the end position with a timed value.
            iViewport.SetViewportPos(TAlfRealPoint(0, 0), 0);
            iViewport.SetViewportPos(TAlfRealPoint(widthOffScreen, 0), iTime);        
            break;
        }      
    }


// ---------------------------------------------------------------------------
//  Scroll continously forth. Generates update messages to retrigger itself forever.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::StartScrollingForth()
    {
    FUNC_LOG;
    // Start scrolling in the given direction.
    StartScrollingOnce();
    
    // Cancel any previous update events.
    CancelScrollingCommands();

    // Schedule a update event for when the visual has finished scrolling.
    TInt error = iControl.Env().Send(TAlfCustomEventCommand(ECustomEventScrollingFinished, this), iTime);
    if (error != KErrNone)
        {
        }
    }


// ---------------------------------------------------------------------------
//  Scroll continuously in a loop. Generates update messages to retrigger itself forever.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::StartScrollingLoop()
    {
    FUNC_LOG;
    TReal farRight = iScrollableArea.iWidth / 2;
    
    // Reset the viewport to its start position, then set the end position with a timed value.
    switch (iDir)
        {
        case EScrollFromLeft:
            iViewport.SetViewportPos(TAlfRealPoint(farRight, 0), 0);
            iViewport.SetViewportPos(TAlfRealPoint(0, 0), iTime);
            break;
            
        case EScrollFromRight:
            iViewport.SetViewportPos(TAlfRealPoint(0, 0), 0);
            iViewport.SetViewportPos(TAlfRealPoint(farRight, 0), iTime);
            break;
        }
    
    // Cancel any previous update events.
    CancelScrollingCommands();

	// Schedule a update event for when the visual has finished scrolling.
	TInt error = iControl.Env().Send(TAlfCustomEventCommand(ECustomEventScrollingFinished, this), iTime);
	if (error != KErrNone)
		{
		}    
    } 


// ---------------------------------------------------------------------------
//  Scroll continuously in a bouncing pattern. Generates update messages to retrigger itself forever.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::StartScrollingBounce()
    {
    FUNC_LOG;
    // Start scrolling in the given direction.
    StartScrollingOnce();

    // Flip direction for next time this is called.    
    switch (iDir)
        {
        case EScrollFromLeft:
            iDir = EScrollFromRight;
            break;
            
        case EScrollFromRight:
            iDir = EScrollFromLeft;
            break;
        }    
        
    // Cancel any previous update events.
    CancelScrollingCommands();

    // Schedule a update event for when the visual has finished scrolling.
    TInt error = iControl.Env().Send(TAlfCustomEventCommand(ECustomEventScrollingFinished, this), iTime);
    if (error != KErrNone)
        {
        }             
    }


// ---------------------------------------------------------------------------
//  The function cancels custom commands sent during scrolling process.
// ---------------------------------------------------------------------------
//
void TFsMarqueeClet::CancelScrollingCommands()
    {
    FUNC_LOG;
    // Cancel any previous update events.
    iControl.Env().CancelCustomCommands(this, ECustomEventScrollingFinished);
    iControl.Env().CancelCustomCommands(this, ECustomEventScrollingPauseFinished);
    iControl.Env().CancelCustomCommands(this, ECustomEventScrollingStartPauseFinished);
    }
    
    
// ---------------------------------------------------------------------------
//  Handles the events sent through the AlfScheduler.
// ---------------------------------------------------------------------------
//
TBool TFsMarqueeClet::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool retVal (EFalse);
    
    if ( !aEvent.IsCustomEvent() )
        {
        retVal = EFalse;
        }
    else if ( aEvent.CustomParameter() == ECustomEventScrollingFinished )
	    {
	    retVal = ETrue;
	    if ( iType == EScrollWrap )
	        {
	        if ( iRepetitions != 0 )
                {
                if ( iRepetitions > 0 )
                    {
                    --iRepetitions;
                    }
                StartScrollingLoop();
                }
	        }
	    else
	        {
	        //cancel previous commands
	        CancelScrollingCommands();
	        //trigger pause if needed
            TInt error = iControl.Env().Send(TAlfCustomEventCommand(ECustomEventScrollingPauseFinished, this), iCycleDelay);
            if (error != KErrNone)
                {
                }
	        }
	    }
	else if ( aEvent.CustomParameter() == ECustomEventScrollingPauseFinished )
	    {
	    //trigger next cycle
	    if ( iRepetitions != 0 )
	        {
	        if ( iRepetitions > 0 )
	            {
	            --iRepetitions;
	            }
	        
	        // Do the event update.
	        switch(iType)
	            {
	            case EScrollForth:
	                {
	                TReal visualWidth = iViewport.Size().Target().iX; 
                    TReal widthOffScreen = (iScrollableArea.iWidth-visualWidth); 
                    
                    switch (iDir)
                        {
                        case EScrollFromLeft:
                            // Reset the viewport to its start position
                            iViewport.SetViewportPos(TAlfRealPoint(widthOffScreen, 0), 0);
                            break;
                            
                        case EScrollFromRight:
                            // Reset the viewport to its start position
                            iViewport.SetViewportPos(TAlfRealPoint(0, 0), 0);
                            break;
                        }
                    //cancel previous commands
                    CancelScrollingCommands();
                    //trigger start pause      
                    TInt error = iControl.Env().Send(TAlfCustomEventCommand(ECustomEventScrollingStartPauseFinished, this), iCycleDelay);
                    if (error != KErrNone)
                        {
                        }
                    
	                break;
	                }
	            case EScrollBounce:
	                StartScrollingBounce();
	                break;
	            
	            default:
	                // Should never get here.
	                ASSERT(EFalse);
	                break;
	            }
	        }
	    retVal = ETrue;
	    }
	else if ( aEvent.CustomParameter() == ECustomEventScrollingStartPauseFinished  )
	    {
	    StartScrollingForth();
	    }
	return retVal;
    }

//EOF

