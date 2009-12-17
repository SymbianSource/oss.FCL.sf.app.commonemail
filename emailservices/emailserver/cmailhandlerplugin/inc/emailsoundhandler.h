/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __EMAILSOUNDHANDLER_H
#define __EMAILSOUNDHANDLER_H

#include <e32std.h>
#include <mdaaudiosampleplayer.h>
#include <MProfileChangeObserver.h>
#include "fsnotificationhandlerbase.h"
#include "emailsoundstates.h"
#include "memailsoundstatecontext.h"
#include "cmaildriveobserver.h"
#include "PSSubscriber.h"
#include "PSSubscribeHandler.h"

class CProfileChangeNotifyHandler;
class MProfileEngine;

/**
 *  Class to handle email sound playing.
 *  A handler for observing inputs into email framework, playing a sound
 *  if a message addition is performed on a Inbox folder.
 *
 *  @lib cmailhandlerplugin
 *  @since S60 v5.2
 */
class CFSMailSoundHandler : public CFSNotificationHandlerBase,
                            public MMdaAudioPlayerCallback,
                            public MProfileChangeObserver,
                            public MEmailSoundStateContext,
                            public MDriveObserver,
                            public MPSSubscribeHandler
{
public:

public: //from MStoreDriveStateObserver

    /**
     * called when observer drive ( memory card )
     * changes state.
     * */
    void DriveStateChangedL( TBool aState );

    /**
     * Callback from PSSubscriber about a PS key event
     * @param aCategory defines the key category
     * @param aKey defines the changed key
     */
    void HandlePropertyChangedL( const TUid& aCategory, TInt aKey );
    
    /**
     * Two-phased constructor.
     * @param aMailClient Object to access email framework
     * @param aOwner Owner and manager of this handler. NULL if not
     *               specified.
     */
    static CFSMailSoundHandler* NewL( MFSNotificationHandlerMgr& aOwner );

    /**
    * destructor
    */
    virtual ~CFSMailSoundHandler();

    // from base class CFSNotificationHandlerBase
    virtual void HandleEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 );

public: // from MEmailSoundStateContext

    /** @see MEmailSoundStateContext */
    virtual void SetState( CEmailSoundState* aNewState );

    /** @see MEmailSoundStateContext */
    virtual CMdaAudioPlayerUtility* AudioPlayer();

    /** @see MEmailSoundStateContext */
    virtual void RecreateAudioPlayerL();

    /** @see MEmailSoundStateContext */
    virtual void ReleaseAudioPlayer();

    /** @see MEmailSoundStateContext */
    virtual MProfileEngine& ProfileEngine() const;

protected:

    // from base class MProfileChangeObserver
    virtual void HandleActiveProfileEventL( TProfileEvent aPE, TInt id );

    // from base class MMdaAudioPlayerCallback
    virtual void MapcInitComplete(
        TInt aError,
        const TTimeIntervalMicroSeconds& aInterval );

    virtual void MapcPlayComplete( TInt aError );

private:

    /**
     * Constructor.
     */
    CFSMailSoundHandler( MFSNotificationHandlerMgr& aOwner );

    void ConstructL();

    /**
    * Fetches the sound and volume for received messagetype
    * @param    aProfile Current profile.
    * @param    aSound      The variable to store the soundfile information
    * @param    aVolume     The variable to store the volume
    * @return   KErrNone when no problems found
    */
    void GetSoundFileAndVolume(
        const MProfile& aProfile,
        TFileName& aSound,
        TInt& aVolume ) const;

// from base class CFSNotificationHandlerBase
    virtual void TurnNotificationOn();
    virtual void TurnNotificationOff();

    /**
     * Returns ETrue if beep-once is set in profile. If it is,
     * beep sequence should be played instead of message alert tone.
     * @param Active profile.
     * @return ETrue if beep-once is set in profile.
     */
    TBool IsBeepOnceSetL( const MProfile& aProfile ) const;

private: // data members

    // Currently active sound state
    CEmailSoundState* iState;

    // Audio player utility used by a sound state to play new email tone
    CMdaAudioPlayerUtility* iAudioPlayer;

    // Profile engine for accessing active profile
    MProfileEngine* iProfileEngine;

    // For receiving profile change events
    CProfileChangeNotifyHandler* iHandler;

    // PubSub subsriber for key press events when playing a tone
    CPSSubscriber* iMsgToneSubscriber;

    CDriveObserver* iDriveObserver;
#ifdef __HANDLER_TEST
    // for module testing
public:

    TRequestStatus* iTesterReqStatus;

    friend class CHsSoundHandlerTester;
#endif
};

#endif // __EMAILSOUNDHANDLER_H
