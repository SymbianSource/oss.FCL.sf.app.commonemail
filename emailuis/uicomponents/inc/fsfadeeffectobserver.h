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
* Description:  Observer interface for a fade effect.
 *
*/


#ifndef C_FSFADEEFFECTOBSERVER_H
#define C_FSFADEEFFECTOBSERVER_H

/**
 * Observer interface for a fade effect.
 */
NONSHARABLE_CLASS(MFsFadeEffectObserver)
    {
public:

    /**
     * 
     */
    enum TFadeEffectState 
        {
        EIdle=0,
        EFadingIn=1,
        EFadingOut,        
        EFadeInFinished,
        EFadeOutFinished
        };
    
public:
    /**
     * Function which will receive notifications about 
     * fade effect state changes.
     * @param aState Current state of fade affect
     */
    virtual void FadeEffectEvent(MFsFadeEffectObserver::TFadeEffectState aState)=0;
    };

#endif // C_FSFADEEFFECTOBSERVER_H
