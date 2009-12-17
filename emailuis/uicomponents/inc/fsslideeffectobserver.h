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
* Description:  Observer interface for a slide effect.
 *
*/


#ifndef C_FSSLIDEEFFECTOBSERVER_H
#define C_FSSLIDEEFFECTOBSERVER_H


/**
 * Observer for slide effect. 
 */
NONSHARABLE_CLASS(MFsSlideEffectObserver)
    {
public:

    /**
     * 
     */
    enum TSlideEffectState 
            {
            EIdle=0,
            ESlidingIn=1,
            ESlidingOut,        
            ESlideInFinished,
            ESlideOutFinished
            };
    
public:
    /**
     * Function which will receive notifications about 
     * slide effect state changes.
     * @param aState Current state of slide affect
     */
    virtual void SlideEffectEvent(MFsSlideEffectObserver::TSlideEffectState aState)=0;
    };


#endif // C_FSSLIDEEFFECTOBSERVER_H
