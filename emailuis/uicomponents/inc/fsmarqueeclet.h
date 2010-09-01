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


#ifndef MARQUEECLET_H
#define MARQUEECLET_H

#include <e32base.h>
#include <alf/alfeventhandler.h>

// Forward declarations.
class CAlfViewportLayout;
class CAlfControl;

/**
 * A controlet for doing marquee scrolling. 
 *
 * This class handles the logic and drawing for a
 * marquee scrolling visual. When you initialise the clet,
 * you pass in the type and direction of the scrolling. Also,
 * you need to tell the clet the viewport that you want to scroll.
 * To start the scrolling action, call StartScrolling.
 */
class TFsMarqueeClet: public MAlfEventHandler
    {
public:
    /**
     * The direction that the visual scrolls from.
     * ie, EScrollFromLeft means the visual will appear on the left and move off to the right.
     */
    enum TScrollDirection
        {
        EScrollFromLeft = 0,  
        EScrollFromRight,
        
        /** No scrolling direction set, do not scroll. */
        EScrollNothing = -1
        };    

    /**
     * The animation type of the scrolling.
     */    
    enum TScrollType
        {
        /** Bounce backwards and forwards continuously. */
        EScrollBounce = 0,  
        
        /** Scroll continuously. Note that the visual contents must be duplicated twice for smooth scrolling effect. */
        EScrollWrap,
        
        /** Scroll forth then stop. */
        EScrollForth
        };        
    
public:

    /* Constructors. */

    /**
     * Constructor.
     * 
     */
    TFsMarqueeClet(CAlfControl& aControl, CAlfViewportLayout& aViewport, TScrollDirection aDir = TFsMarqueeClet::EScrollFromRight, TScrollType aType = TFsMarqueeClet::EScrollWrap);

    /**
     * Destructor.
     */
    virtual ~TFsMarqueeClet();
    
    /**
     * Set the type of marquee.
     * 
     * @param aType Type of scrolling.
     */
    void SetScrollType( const TScrollType aType );
    
    /**
     * The number of milliseconds to complete a full scroll cycle.
     * 
     * The amount of time that the scrolling clet will use to scroll
     * the entire contents of the visual.
     * @param aTime The time in milliseconds that a complete scroll cycle will take.
     */
    void SetScrollSpeed(TInt aTime);
    
    /**
     * Sets the number of marquee's scroll cycles.
     * 
     * @aRepetition Number of marquee cycles. Negative value - infinite loop.
     */
    void SetScrollRepetitions( TInt aRepetitions );

    /**
     * The function sets time for a pause after each cycle.
     * 
     * @param aCycleDelay Time in miliseconds for a next cycle start delay.
     */
    void SetScrollCycleDelay ( TInt aCycleDelay );
    
    /**
     * Change the direction that the text scrolls from.
     * 
     * Set the direction that the text will scroll from. For example, setting the direction to
     * EScrollFromRight will mean that the visual will scroll in from the left and leave from the right
     * edge of the viewport.
     *
     * @param aTime The time in milliseconds that a complete scroll cycle will take.
     * @see TScrollDirection.
     */
    void SetScrollDirection(TScrollDirection aDir);
    
    /**
     * The area that we want to scroll.
     * 
     * Sets the scrolling area.
     * @param aScrollableArea The dimensions in pixels of the area to scroll.
     */    
    void SetScrollableArea(TSize& aScrollableArea);
    
    /**
     * Begin scrolling with the specified speed, direction and type.
     *
     * Call this to begin or reset the scrolling behaviour of this marquee.
     */
    void StartScrolling();

    /**
     * Stop scrolling and reset the visual to its zero position.
     */
    void StopScrolling();
    
protected:
    
    /**
     * Initiate scrolling once then stop.
     */
    void StartScrollingOnce();
    
    /**
     * Scroll continously forth. Generates update messages to retrigger itself forever.
     */
    void StartScrollingForth();
    
    /**
     * Scroll continuously in a loop. Generates update messages to retrigger itself forever.
     */    
    void StartScrollingLoop();
    
    /**
     * Scroll continuously in a bouncing pattern. Generates update messages to retrigger itself forever.
     */        
    void StartScrollingBounce(); 
    
    /**
     * The function cancels custom commands sent during scrolling process.
     */
    void CancelScrollingCommands();
    
    /**
     * Handles the events sent through the AlfScheduler. Implementation of
     * MAlfEventHandler interface. The scheduled events are needed in order
     * to update the scroll direction/position.
     * 
     * @param aEvent The event sent to this list visualiser.
     */
    TBool OfferEventL(const TAlfEvent& aEvent); 
    
protected: 

    /**
     * Custom events that drive the text wrapping/scrolling repeating.
     */
    enum TCustomEvent
        {
        ECustomEventScrollingFinished = 200,
        ECustomEventScrollingPauseFinished,
        ECustomEventScrollingStartPauseFinished
        };
   
    /** 
     * The viewport layout used by this clet for scrolling.     
     */
    CAlfViewportLayout& iViewport;
    
    /** 
     * The owning control of this clet.     
     */
    CAlfControl& iControl;
    
    /** 
     * Scroll direction of the marquee. (the direction it scrolls FROM).     
     */
    TScrollDirection iDir;
        
    /** 
     * Type of scrolling.     
     */
    TScrollType iType;
    
    /** 
     * The total area of the contents that we want to scroll. (Usually just the text extents).     
     */
    TSize iScrollableArea;
    
    /** 
     * The time for a complete scroll cycle (miliseconds).     
     */
    TInt iTime;
    
    /**
     * Number of marquee cycles.
     * Negative value - infinite loop.
     */
    TInt iRepetitions;
    
    /**
     * Cycle delay (miliseconds).
     */
    TInt iCycleDelay;
       
    };


#endif // MARQUEECLET_H
