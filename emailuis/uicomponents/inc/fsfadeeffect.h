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
* Description:  Fade effect for a list component.
 *
*/


#ifndef C_FSFADEEFFECT_H
#define C_FSFADEEFFECT_H
//<cmail> SF
#include <alf/alfeventhandler.h>
//</cmail>
//////TOOLKIT CLASSES
class CAlfVisual;
class CAlfControl;

class MFsFadeEffectObserver;


/**
 *  Class used for fade in/out effects.
 *
 *  @lib 
 */
NONSHARABLE_CLASS(CFsFadeEffect): public CBase, public MAlfEventHandler
    {
        
public:
    /**
     * Two-phased constructor
     * 
     * @param aRoot Root visual/layout which of the opacity is controlled
     * @param aTime Speed of fade in/out effect. Time in miliseconds.
     * 
     * @return Pointer to the new fade effect object
     */
    static CFsFadeEffect* NewL(CAlfControl* aControl, CAlfVisual* aRoot, TInt aTime);   
    
    /**
     * Two-phased constructor
     * 
     * @param aObserver Observer which will receive events about state chages
     * @param aRoot Root visual/layout which of the opacity is controlled
     * @param aTime Speed of fade in/out effect. Time in miliseconds.
     * 
     * @return Pointer to the new fade effect object
     */
    static CFsFadeEffect* NewL(CAlfControl* aControl, MFsFadeEffectObserver* aObserver, 
        CAlfVisual* aRoot, 
        TInt aTime);
    
    /**
     * Launches fade in effect
     */
    void FadeIn();
    
    /**
     * Launches fade out effect
     */
    void FadeOut();
    
    /**
     * Sets duration (in miliseconds) of the fade effects.
     * @param aTime Fade effect's duration in milisecond. 
     */
    void SetTime(const TInt aTime);
    
    /**
     * Returns speed of the fade effect
     * 
     * @return Duratin of the fade effect in miliseconds.
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
     * @param aTime Speed of fade in/out effect. Time in miliseconds.
     * 
     */
    CFsFadeEffect(CAlfControl* aControl,CAlfVisual* aRoot, TInt aTime);


    /**
     * Constructor
     * 
     * @param aObserver Observer which will receive events about state chages
     * @param aRoot Root visual/layout which of the opacity is controlled
     * @param aTime Speed of fade in/out effect. Time in miliseconds.
     * 
     * @return Pointer to the new fade effect object
     */
    CFsFadeEffect(CAlfControl* aControl,MFsFadeEffectObserver* aObserver, 
        CAlfVisual* aRoot, 
        TInt aTime);

private:

    /**
     * Custom events that informs about ending a fade effect.
     */
    enum TCustomEvent
        {
        ECustomEventFadeInFinished = 200,
        ECustomEventFadeOutFinished
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
     * Observer of fade eddects
     * Not own.
     */
    MFsFadeEffectObserver* iObserver;
    
    /**
     * Speed of fade in/out effects
     */
    TInt iTime;

    };
    
#endif //C_FSFADEEFFECT_H
