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
* Description: This file defines class CFSMailSoundHandler.
*
*/


 
#ifndef __FSMAILSOUNDHANDLER_H
#define __FSMAILSOUNDHANDLER_H

#include <e32std.h>

#include <mdaaudiosampleplayer.h>

// <cmail>
//#include <oem/MProfileEngine.h>
//#include <oem/MProfileChangeObserver.h>
#include <MProfileEngine.h>
#include <MProfileChangeObserver.h>
// </cmail>


#include "fsnotificationhandlerbase.h"
#include "fsnotificationhandlertimerobserver.h"

class CProfileChangeNotifyHandler;
class CHWRMVibra;
class CFSNotificationHandlerTimer;


/**
 *  Class to handle email sound playing.
 *  A handler for observing inputs into email framework, playing a sound
 *  if a message addition is performed on a Inbox folder.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */ 
class CFSMailSoundHandler : public CFSNotificationHandlerBase,
                            public MFSNotificationHandlerTimerObserver,
                            public MMdaAudioPlayerCallback,
                            public MProfileChangeObserver
{
public:

    /**
     * Two-phased constructor.
     * @param aMailClient Object to access email framework
     * @param aOwner Owner and manager of this handler. NULL if not
     *               specified.
     */	
    static CFSMailSoundHandler* NewL( MFSNotificationHandlerMgr& aOwner );
    virtual ~CFSMailSoundHandler();
    
    void RefreshData();

    // From base class MFSNotificationHandlerTimerObserver
    virtual void TimerCallBackL( TInt aError );

    // from base class CFSNotificationHandlerBase
    virtual void HandleEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 );

protected:

// from base class MProfileChangeObserver
    virtual void HandleActiveProfileEventL( TProfileEvent aPE, TInt id );

// from base class MMdaAudioPlayerCallback	
    virtual void MapcInitComplete( 
        TInt aError,
        const TTimeIntervalMicroSeconds& /*aInterval*/ );
    virtual void MapcPlayComplete( TInt aError );
         	
private:

    /**
     * Constructor.
     *
     * @since S60 ?S60_version
     * @param aOwner Owner and manager of this handler.
     */ 
    CFSMailSoundHandler( MFSNotificationHandlerMgr& aOwner );
    void ConstructL();

    void Init();
    void Close();
    void Sound();
    void Resume();
    TInt SetAudioPlayerL( bool def );
    /**
    * Fetches the sound and volume for received messagetype
    * @since    Series60 2.1
    * @param    aProfile Current profile.
    * @param    aSound      The variable to store the soundfile information
    * @param    aVolume     The variable to store the volume
    * @return   KErrNone when no problems found
    */
    TInt GetSoundFileAndVolumeL(
        const MProfile& aProfile, 
        TFileName& aSound, 
        TInt& aVolume ) const;
    TInt ToneVolume( const MProfile& aProfile ) const;

// from base class CFSNotificationHandlerBase         
    virtual void TurnNotificationOn();
    virtual void TurnNotificationOff();

private: // data

	enum TTasks
        {
        EInit = 1,
        ESound,
        EWait,
        EDefault,
        EPending,
        EReset
        };

    TInt iVolume;
    TTasks iNextState;    
    TTasks iPending;
    TTasks iReset;
    CMdaAudioPlayerUtility* iAudioPlayer;
    MProfileEngine* iProfileEngine;
    CProfileChangeNotifyHandler* iHandler;
    CHWRMVibra* iVibra;

    /**
     * Timer service.
     * Own.
     */        
    CFSNotificationHandlerTimer* iTimer;
};


#endif __FSMAILSOUNDHANDLER_H