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
* Description: This file defines email sound state classes.
*
*/
 
#ifndef MEMAILSOUNDSTATECONTEXT_H
#define MEMAILSOUNDSTATECONTEXT_H

#include <e32std.h>

class CMdaAudioPlayerUtility;
class CEmailSoundState;

/**
 * Interface for sound states
 * @since S60 v5.1
 */
class MEmailSoundStateContext 
{
public:
    
    /**
    * Sets active state
    * @param aNewState active state
    */
    virtual void SetState( CEmailSoundState* aNewState ) = 0;
    
    /**
    * Returns audio player utility pointer, ownership is not transferred.  
    * @return audio player utility or NULL.
    */
    virtual CMdaAudioPlayerUtility* AudioPlayer() = 0;
    
    /** Creates audio player utility. Old player instance is deleted if 
     * it exists. AudioPlayer() can be called to access initialized player
     * after state receives AudioInitCompleted() event.
     */
    virtual void RecreateAudioPlayerL() = 0;
    
    /**
    * Releases audio player. After calling this method AudioPlayer() will return 
    * NULL until RecreateAudioPlayerL is succesfully called.
    */
    virtual void ReleaseAudioPlayer() = 0;

    /**
     * Returns profile engine reference
     * @return profile engine
     */
    virtual MProfileEngine& ProfileEngine() const = 0;
};


#endif // MEMAILSOUNDSTATECONTEXT_H
