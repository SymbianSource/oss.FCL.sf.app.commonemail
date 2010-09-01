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
* Description: This file defines email sound state classes.
*
*/



#ifndef FSMAILSOUNDSTATES_H
#define FSMAILSOUNDSTATES_H

#include <e32std.h>
#include <mdaaudiosampleplayer.h>
#include <MProfileEngine.h>
#include <MProfileChangeObserver.h>
#include "fsnotificationhandlertimerobserver.h"

class MEmailSoundStateContext;
class CMdaAudioPlayerUtility;
class MEmailSoundStateContext;
class CFSNotificationHandlerTimer; 

/**
 *  Base class to handle email tone playing logic.
 *
 * Generic behavior of state machine:
 *
 * -- event --> handle event --> state transition -- NO--> do nothing
 *                                  needed?
 *                                     |
 *                                     | YES
 *  -----------------------------------
 *  |
 *  --> new state entry action -> store state to context -> delete old
 *                                                           state(*
 *
 *   (* 'failed state' is never deleted to guarantee entry to it. If
 *   any entry action or some other crusial actioin fails, FSM goes to
 *   'failed state' and any ubsequent event activates 'init' state.
 *
 *  @since S60 v5.2
 */
NONSHARABLE_CLASS( CEmailSoundState ) : public CBase
{
public:
    /* Event handlers called by context (owner of state machine).
     * Not all event handler methodss are applicable in each concrete
     * state class, e.g. AudioInitFailed() is reasonable only for EInit state.
     */

    /**
    * Profile changed event.
    */
    virtual void ProfileChanged();

    /**
    * Initializing tone failed. This always causes transition to EFailed state.
    */
    virtual void AudioInitFailed();

    /**
    * Audio tone initialization completed succesfully.
    */
    virtual void AudioInitCompleted();

    /**
    * Audio tone play completed (success or failure, doesn't matter)
    */
    virtual void AudioPlayCompleted();

    /**
    * Request to play tone that matches with currently active profile.
    */
    virtual void PlayTone();

    /**
    * Request to stop playback. Default implementation is empty
    */
    virtual void StopTone();

public: // state machine startup and stopping.

    /** Initializes state machine. Initial state is EInit
     *  (class CEmailInitilisingTone).
     */
    static void InitializeL( MEmailSoundStateContext* aContext );

    /** uninitializes state machine also deleting current state */
    static void Uninitialize( CEmailSoundState* aState );

    /** Returns state machine context providing services to states
     * @return context interface
     */
    MEmailSoundStateContext& Context() const;

protected:

    /** sound states */
    enum TSoundState {
        /** Tone is being initialized */
        EInit,

        /** Ready to play tone, tone initialization is complete
         */
        EReady,

        /** Playing tone
         */
        EPlaying,

        /** Waiting for initialization to complete after which tone
         * is immediately played. Note: this state can be entered also
         * from EFailed state (not just EInit state) meaning that tone
         *  playback starts after initialization.
         */
        EPendingPlay,

        /** Waiting for playback to complete in order to recreate audio
         * tone (e.g. after active profile changed).
         */
        EPendingInit,
        
        /** Waiting for delay timer expiration   
         */
        EDelay,

        /** Initialization has been failed, tone cannot be played.
         * In this state ProfileChanged causes EInit and PlayTone
         * EPendingPlay to become active state.
         */
        EFailed
        };

    /** Destructor */
    ~CEmailSoundState();

    /** Sets current state to context (owner of state machine) */
    void BaseEnter();

    /* Deletes current state. Derived class may implement additional
     * behavior and call base class Exit() as __last__ statement.
     */
    virtual void Exit();

    /**
     * Tries to initialize audio tone (async operation).
     * @return ETrue if initialization starts. When it finished,
     *         AudioInitCompleted() or AudioInitFailed() is called.
     *
     *         or EFalse if creation fails. In this case EFailed state
     *         is entered.
     */
    TBool TryStartToneInitialization();

    /**
    * Changes to new state and performs exit action for old state including
    * destruction. If any failure in state transition happens, EFailed
    * state becomes current state - it cannot fail.
    * @param aState requested new state
    */
    void StateTransition( const TSoundState aState );

    /** c++ constructor that must be called by derived class */
    CEmailSoundState( MEmailSoundStateContext& aContext );

protected: // data
    /** state machine owner */
    MEmailSoundStateContext& iContext;
};

/**
* In this state tone initialization is ongoing. Next state will be
* EReady (AudioInitCompleted called),
* EPendingPlay (PlayTone called), or
* EFailed (base class AudioInitFailed called)
*/
NONSHARABLE_CLASS( CEmailInitilisingTone ) : public CEmailSoundState
{
public:    // event handlers from CEmailSoundState
    virtual void ProfileChanged();
    virtual void AudioInitCompleted();
    virtual void PlayTone();

private:
    /** c++ constructor */
    CEmailInitilisingTone( MEmailSoundStateContext& aContext );

    /** state entry point */
    static void EnterL( CEmailSoundState* aPreviousState );

    /** state entry actions */
    void DoEnterL();

    // allows base class to do state transition
    friend class CEmailSoundState;
};

/**
* In this state tone initialization has been completed and tone
* can be played. Next state will be
* EInit (ProfileCanged called), or
* EPlaying (PlayTone called)
*/
NONSHARABLE_CLASS( CEmailReadyToPlay ) : public CEmailSoundState
{
public:    // event handlers from CEmailSoundState
    virtual void ProfileChanged();
    virtual void PlayTone();

private:

    /** c++ constructor */
    CEmailReadyToPlay( MEmailSoundStateContext& aContext );

    /** state entry point */
    static void EnterL( CEmailSoundState* aPreviousState );

    /** state entry actions */
    void DoEnterL();

    // allows base class to do state transition
    friend class CEmailSoundState;
};

/**
* In this state tone initialization has been failed.  Next state will be
* EInit (ProfileChanged called), or
* EPendingPlay (PlayTone called)
*/
NONSHARABLE_CLASS( CEmailSoundFailed ) : public CEmailSoundState
{
public:    // event handlers from CEmailSoundState
    virtual void ProfileChanged();
    virtual void PlayTone();

private:      // state entry and exit functions

    /** overrides base class */
    virtual void Exit();

    /** c++ constructor */
    CEmailSoundFailed( MEmailSoundStateContext& aContext );

    /** Enter cannot fail because object is pre-allocated */
    static void Enter( CEmailSoundState* aPreviousState );

    /** state entry actions */
    void DoEnter();

    // allows base class to do state transition
    friend class CEmailSoundState;
};

/**
* In this state tone is being played. Next state will be:
* EReady (StopTone or AudioPlayCompleted called), or
* EPendingInit (ProfileChanged called)
*/
NONSHARABLE_CLASS( CEmailPlayingTone ) : public CEmailSoundState
{
public:    // event handlers from CEmailSoundState
    virtual void ProfileChanged();
    virtual void AudioPlayCompleted();
    virtual void StopTone();

private:      // state entry and exit functions

    /** c++ constructor */
    CEmailPlayingTone( MEmailSoundStateContext& aContext );

    /** state entry point */
    static void EnterL( CEmailSoundState* aPreviousState );

    /** state entry actions */
    void DoEnterL();

    // allows base class to do state transition
    friend class CEmailSoundState;
};

/**
* In this tone initialization is ongoing. Next state will be:
* EPlaying (AudioInitCompleted called),
* EFailed (base class AudioInitFailed called)
*/
NONSHARABLE_CLASS( CEmailPendingPlay ) : public CEmailSoundState
{
public:    // event handlers from CEmailSoundState
    virtual void ProfileChanged();
    virtual void AudioInitCompleted();

private:

    /** state entry actions */
    void DoEnterL();

    /** c++ constructor */
    CEmailPendingPlay( MEmailSoundStateContext& aContext );

    /** state entry point */
    static void EnterL( CEmailSoundState* aPreviousState );

    // allows base class to do state transition
    friend class CEmailSoundState;
};


/**
* In this state profile has changed while tone is being played. Next state
* will be:
* EInit (AudioPlayCompleted called)
*/
NONSHARABLE_CLASS( CEmailPendingInit ) : public CEmailSoundState
{
public:    // event handlers from CEmailSoundState
    virtual void AudioPlayCompleted();

private:
    /** state entry actions */
   void DoEnterL();

    /** c++ constructor */
    CEmailPendingInit( MEmailSoundStateContext& aContext );

    /** state entry point */
    static void EnterL( CEmailSoundState* aPreviousState );

    // allows base class to do state transition
    friend class CEmailSoundState;
};

/**
 * In this state delay timer is started. Next state will be:
 * EReady (when inner timer expires),
 * EFailed (base class AudioInitFailed called)
 */   
NONSHARABLE_CLASS( CEmailTimerDelay ) : public CEmailSoundState,
                                        public MFSNotificationHandlerTimerObserver 
{
public: // event handlers from CEmailSoundState
    virtual void ProfileChanged();

public: // timer callback from base class MFSNotificationHandlerTimerObserver
    virtual void TimerCallBackL( TInt aError );

protected:
    /** Destructor */
    ~CEmailTimerDelay();

private:
    /** state entry actions */
    void DoEnterL();
    
    /** c++ constructor */
    CEmailTimerDelay( MEmailSoundStateContext& aContext );
    
    /** state entry point */
    static void EnterL( CEmailSoundState* aPreviousState );

    // allows base class to do state transition
    friend class CEmailSoundState;

private:
    /** timer service, owned */    
    CFSNotificationHandlerTimer* iTimer;
};

#endif // FSMAILSOUNDSTATES_H
