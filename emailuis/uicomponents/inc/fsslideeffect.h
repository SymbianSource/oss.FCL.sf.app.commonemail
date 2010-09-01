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
* Description:  Data structure class for tree list component
 *
*/


#ifndef C_FSSLIDEEFFECT_H
#define C_FSSLIDEEFFECT_H

//<cmail> SF
#include <alf/alfeventhandler.h>
//</cmail>
//////TOOLKIT CLASSES
class CAlfVisual;
class CAlfControl;

//////PROJECT CLASSES
class MFsSlideEffectObserver;


/**
 *  Class used for slide in/out effects.
 *
 *  @lib 
 */
NONSHARABLE_CLASS(CFsSlideEffect) : public CBase, public MAlfEventHandler
    {
public:    
        
    enum TSlideEffectDirection
        {
        ESlideNone = 0,
        ESlideFromTop,
        ESlideFromBottom,
        ESlideFromRight,
        ESlideFromLeft
        };
        
public:
    /**
     * Two-phased constructor
     * 
     * @param aRoot Root visual/layout which of the opacity is controlled
     * @param aTime Speed of slide in/out effect. Time in miliseconds.
     * 
     * @return Pointer to the new slide effect object
     */
    static CFsSlideEffect* NewL(CAlfControl* aControl, CAlfVisual* aRoot, TInt aTime);
    
    /**
     * Two-phased constructor
     * 
     * @param aObserver Observer which will receive events about state chages
     * @param aRoot Root visual/layout which of the opacity is controlled
     * @param aTime Speed of slide in/out effect. Time in miliseconds.
     * 
     * @return Pointer to the new slide effect object
     */
    static CFsSlideEffect* NewL(CAlfControl* aControl, MFsSlideEffectObserver* aObserver, 
                                CAlfVisual* aRoot, 
                                TInt aTime);
    

    /**
     * Launches slide in effect
     */
    void SlideIn(TSlideEffectDirection aDirection);
    
    /**
     * Launches slide out effect
     */
    void SlideOut(TSlideEffectDirection aDirection);
    
    /**
     * Sets duration of the slide effects.
     * @param aTime Slide's effect duration in miliseconds.
     */
    void SetTime(const TInt aTime);
    
    /**
     * Returns duration of the slide effect
     * 
     * @return Duration (in miliseconds) of the slide effect.
     */
    TInt Time() const;
    
public: //From MAlfEventHandler

    /**
     * From MAlfEventHandler
     * Called when an input event is being offered to this object.
     * 
     * The implementation must ensure that the function returns EFalse if 
     * it does not do anything in response to the event, otherwise, other objects
     * may be prevented from receiving the event. If it is able to process the 
     * event it should return ETrue.
     *
     * @param aEvent  Event to be handled.
     *
     * @return  <code>ETrue</code>, if the event was handled.
     *          Otherwise <code>EFalse</code>.
     */
    virtual TBool OfferEventL(const TAlfEvent& aEvent);
    
private:
    /**
     * Constructor
     * 
     * @param aRoot Root visual/layout which of the opacity is controlled
     * @param aTime Speed of slide in/out effect. Time in miliseconds.
     * 
     */
    CFsSlideEffect(CAlfControl* aControl, CAlfVisual* aRoot, TInt aTime);
    
    /**
     * Constructor
     * 
     * @param aObserver Observer which will receive events about state chages
     * @param aRoot Root visual/layout which of the opacity is controlled
     * @param aTime Speed of slide in/out effect. Time in miliseconds.
     * 
     * @return Pointer to the new slide effect object
     */
    CFsSlideEffect(CAlfControl* aControl, MFsSlideEffectObserver* aObserver, 
                   CAlfVisual* aRoot, 
                   TInt aTime);

private:    
    /**
     * Custom events that informs about ending a slide effect.
     */
    enum TCustomEvent
        {
        ECustomEventSlideInFinished = 300,
        ECustomEventSlideOutFinished
        };    
    
private:

    /**
     * 
     */
    CAlfControl* iControl;
    
    /**
     * Root visual/layout which of the opacity is controlled
     * Not own.
     */
    CAlfVisual* iRoot;
    
    /**
     * Observer of slide eddects
     * Not own.
     */
    MFsSlideEffectObserver* iObserver;
    
    /**
     * Duration of slide in/out effects
     */
    TInt iTime;
    
    };
    
#endif //C_FSSLIDEEFFECT_H
