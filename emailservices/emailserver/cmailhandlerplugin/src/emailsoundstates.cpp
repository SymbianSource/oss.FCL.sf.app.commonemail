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
* Description:  Class to handle email sound playing.
*
*/


#include <AudioPreference.h>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileTones.h>
#include <MProfileExtraTones.h>
#include <TProfileToneSettings.h>
#include <CProfileChangeNotifyHandler.h>
#include <coreapplicationuisdomainpskeys.h>

#include "emailtrace.h"
#include "memailsoundstatecontext.h"
#include "emailsoundstates.h"
#include "fsnotificationhandlertimer.h"
#include "cmailhandlerpluginpanic.h"

// Local constants
static const TInt KToneMinVolume = 0;
static const TInt KToneMaxVolume = 10;
static const TInt KDelayTimerInterval = 60000000; // 60s

// ---------------------------------------------------------------------------
// Initializes the state machine by setting initial state and creating
// failure state that can be entered at any time in error cases.
// ---------------------------------------------------------------------------
//
void CEmailSoundState::InitializeL( MEmailSoundStateContext* aContext )
    {
    FUNC_LOG
    TAny* tls = Dll::Tls();
    if ( !tls )
        {
        // prepare failed state so its always available
        CEmailSoundState* failedState = new ( ELeave ) CEmailSoundFailed( *aContext );
        Dll::SetTls( failedState );
        // now enter to initial state, use failed state for obtaining context
        CEmailInitilisingTone::EnterL( failedState );
        }
    }

// ---------------------------------------------------------------------------
// Exits the state machine and frees memory allocated for states
// ---------------------------------------------------------------------------
//
void CEmailSoundState::Uninitialize( CEmailSoundState* aCurrentState )
    {
    FUNC_LOG

    if ( aCurrentState )
        {
        // Exit must be called before deleting failedState because it may
        // also be current state.
        aCurrentState->Exit();
        }

    TAny* tls = Dll::Tls();
    if ( tls )
        {
        CEmailSoundState* failedState =
            reinterpret_cast<CEmailSoundState*>( tls );
        delete failedState;
        Dll::SetTls( NULL );
        }
    }

// ---------------------------------------------------------------------------
// No-op in base class, also enough for some derived classes.
// ---------------------------------------------------------------------------
//
void CEmailSoundState::ProfileChanged()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Tone initialization failure always leads to failed state.
// ---------------------------------------------------------------------------
//
void CEmailSoundState::AudioInitFailed()
    {
    FUNC_LOG
    // enter to failed state.
    StateTransition( EFailed );
    }

// ---------------------------------------------------------------------------
// Derived classes implement
// ---------------------------------------------------------------------------
//
void CEmailSoundState::AudioInitCompleted()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Derived classes implement
// ---------------------------------------------------------------------------
//
void CEmailSoundState::AudioPlayCompleted()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Handled in CEmailReadyToPlay state, in most other states ignored.
// ---------------------------------------------------------------------------
//
void CEmailSoundState::PlayTone()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Applicable only in playing state
// ---------------------------------------------------------------------------
//
void CEmailSoundState::StopTone()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Tries to create audio player or activas failed state if error occurs
// ---------------------------------------------------------------------------
//
TBool CEmailSoundState::TryStartToneInitialization()
    {
    FUNC_LOG
    TRAPD( err, iContext.RecreateAudioPlayerL() );
    if ( err )
        {
        StateTransition( EFailed );
        }
    return ( err == KErrNone );
    }

// ---------------------------------------------------------------------------
// Stores as current state in owner of the state machine.
// ---------------------------------------------------------------------------
//
void CEmailSoundState::BaseEnter()
    {
    FUNC_LOG
    iContext.SetState( this );
    }

// ---------------------------------------------------------------------------
// State transition to requested state or failed state if attempt doesn't
// succeed.
// ---------------------------------------------------------------------------
//
void CEmailSoundState::StateTransition( const TSoundState aState )
    {
    FUNC_LOG
    TRAPD( err,
        {
        switch ( aState )
            {
            case EInit:
                CEmailInitilisingTone::EnterL( this );
                break;
            case EReady:
                CEmailReadyToPlay::EnterL( this );
                break;
            case EPlaying:
                CEmailPlayingTone::EnterL( this );
                break;
            case EPendingPlay:
                CEmailPendingPlay::EnterL( this );
                break;
            case EPendingInit:
                CEmailPendingInit::EnterL( this );
                break;
            case EDelay:
                CEmailTimerDelay::EnterL( this );
                break;
            case EFailed:
            default:
                // to failed state
                /* CodeScanner warning ignored because CS does not regognize
                 * TRAP being used */
                User::Leave( KErrArgument ); // codescanner::leave
                break;
            }
        } );
    if ( err != KErrNone )
        {
        CEmailSoundFailed::Enter( this );
        }
    // exit old state
    Exit();
    }

// ---------------------------------------------------------------------------
// For most states destructor call is fine.
// See also CEmailSoundFailed::Exit()
// ---------------------------------------------------------------------------
//
void CEmailSoundState::Exit()
    {
    FUNC_LOG
    delete this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
MEmailSoundStateContext& CEmailSoundState::Context() const
    {
    FUNC_LOG
    return iContext;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CEmailSoundState::~CEmailSoundState()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailSoundState::CEmailSoundState(
    MEmailSoundStateContext& aContext ) :
    iContext( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailInitilisingTone::CEmailInitilisingTone(
    MEmailSoundStateContext& aContext ) :
    CEmailSoundState( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Need to cancel and restart tone initialization
// ---------------------------------------------------------------------------
//
void CEmailInitilisingTone::ProfileChanged()
    {
    FUNC_LOG
    TryStartToneInitialization();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEmailInitilisingTone::AudioInitCompleted()
    {
    FUNC_LOG
    StateTransition( EReady );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEmailInitilisingTone::PlayTone()
    {
    FUNC_LOG
    // not ready yet, postpone playing
    StateTransition( EPendingPlay );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEmailInitilisingTone::DoEnterL()
    {
    FUNC_LOG
    iContext.RecreateAudioPlayerL();
    CEmailSoundState::BaseEnter();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEmailInitilisingTone::EnterL( CEmailSoundState* aPreviousState )
    {
    FUNC_LOG
    CEmailInitilisingTone* state = new ( ELeave )
        CEmailInitilisingTone( aPreviousState->Context() );
    CleanupStack::PushL( state );
    state->DoEnterL();
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailReadyToPlay::CEmailReadyToPlay( MEmailSoundStateContext& aContext )
 : CEmailSoundState( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// New profile may have different tone, initialize it.
// ---------------------------------------------------------------------------
//
void CEmailReadyToPlay::ProfileChanged()
    {
    FUNC_LOG
    StateTransition( EInit );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEmailReadyToPlay::PlayTone()
    {
    FUNC_LOG
    StateTransition( EPlaying );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CEmailReadyToPlay::DoEnterL()
    {
    FUNC_LOG
    CEmailSoundState::BaseEnter();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CEmailReadyToPlay::EnterL(
    CEmailSoundState* aPreviousState )
    {
    FUNC_LOG
    CEmailReadyToPlay* state = new ( ELeave ) CEmailReadyToPlay(
        aPreviousState->Context() );
    CleanupStack::PushL( state );
    state->DoEnterL();
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailSoundFailed::CEmailSoundFailed( MEmailSoundStateContext& aContext )
 : CEmailSoundState( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Something has gone wrong earlier, now try recovery (re-init).
// ---------------------------------------------------------------------------
//
void CEmailSoundFailed::ProfileChanged()
    {
    FUNC_LOG
    // try re-initialization
    StateTransition( EInit );
    }

// ---------------------------------------------------------------------------
// Something has gone wrong earlier, now try recovery (re-init + play ).
// ---------------------------------------------------------------------------
//
void CEmailSoundFailed::PlayTone()
    {
    FUNC_LOG
    // kick off tone re-creation again
    if ( TryStartToneInitialization() )
        {
        StateTransition( EPendingPlay );
        }
    }

// ---------------------------------------------------------------------------
// Set failed state as current state.
// ---------------------------------------------------------------------------
//
void CEmailSoundFailed::DoEnter()
    {
    FUNC_LOG
    CEmailSoundState::BaseEnter();
    }

// ---------------------------------------------------------------------------
// Failed state is preserved until plugin shutdown, therefore base class Exit()
// is overridden and explicit destruction for this class is done in
// CEmailSoundState::Uninitialize.
// ---------------------------------------------------------------------------
//
void CEmailSoundFailed::Exit()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Obtain failed state from TLS and set it as current state.
// ---------------------------------------------------------------------------
//
void CEmailSoundFailed::Enter( CEmailSoundState* /*aPreviousState*/ )
    {
    FUNC_LOG
    TAny* tls = Dll::Tls();
    __ASSERT_ALWAYS( tls, Panic( ECmailHandlerPluginPanicNoFailedState ) );
    CEmailSoundFailed* failedState = reinterpret_cast<CEmailSoundFailed*>( tls );
    failedState->DoEnter();
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailPlayingTone::CEmailPlayingTone( MEmailSoundStateContext& aContext )
 : CEmailSoundState( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// When playing completes, recreate tone because new profile may have
// different one.
// ---------------------------------------------------------------------------
//
void CEmailPlayingTone::ProfileChanged()
    {
    FUNC_LOG
    StateTransition( EPendingInit );
    }

// ---------------------------------------------------------------------------
// Proceed to delay state.
// Existing player instance owned by context is preserved.
// ---------------------------------------------------------------------------
//
void CEmailPlayingTone::AudioPlayCompleted()
    {
    FUNC_LOG

    // Tell sysap that tone playing has stopped
    RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, ECoreAppUIsStopTonePlaying );

    StateTransition( EDelay );
    }

// ---------------------------------------------------------------------------
// Stop tone and proceed to delay state.
// ---------------------------------------------------------------------------
//
void CEmailPlayingTone::StopTone()
    {
    FUNC_LOG
    CMdaAudioPlayerUtility* player = iContext.AudioPlayer();
    // should never be null in this state
    __ASSERT_ALWAYS( player, Panic( ECmailHandlerPluginPanicNullAudioPlayer ) );
    player->Stop();

    // Tell sysap that tone playing has stopped
    RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, ECoreAppUIsStopTonePlaying );

    StateTransition( EDelay );
    }

// ---------------------------------------------------------------------------
// Starts playing of already initialized tone.
// ---------------------------------------------------------------------------
//
void CEmailPlayingTone::EnterL( CEmailSoundState* aPreviousState )
    {
    FUNC_LOG
    CEmailPlayingTone* state = new ( ELeave ) CEmailPlayingTone(
        aPreviousState->Context() );
    CleanupStack::PushL( state );
    state->DoEnterL();
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// Playes 'new email' tone.
// ---------------------------------------------------------------------------
//
void CEmailPlayingTone::DoEnterL()
    {
    FUNC_LOG
    CMdaAudioPlayerUtility* player = iContext.AudioPlayer();
    // should never be null in this state
    __ASSERT_ALWAYS( player, Panic( ECmailHandlerPluginPanicNullAudioPlayer ) );

    MProfile* profile = iContext.ProfileEngine().ActiveProfileL(); // owned

    const TInt profileVolume =
        profile->ProfileTones().ToneSettings().iRingingVolume;
    TInt volume = Max( KToneMinVolume, Min( profileVolume, KToneMaxVolume ) );
    profile->Release();
    profile = NULL;

    // scale with player but only if volume is greater than zero
    if ( volume > 0 )
        {
        volume = volume * player->MaxVolume() / KToneMaxVolume ;
        }
    INFO_1( "email tone volume", volume )
    player->SetVolume( volume );

    // Set time
    TTimeIntervalMicroSeconds time = TTimeIntervalMicroSeconds( 0 );

    // Tell sysap that there's a tone playing at the moment
    RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, ECoreAppUIsTonePlaying );

    // Set repeats and start playing
    player->SetRepeats( 0, time );
    player->Play();
    CEmailSoundState::BaseEnter();
    };

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailPendingPlay::CEmailPendingPlay( MEmailSoundStateContext& aContext )
 : CEmailSoundState( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Currently initializing tone is outdated, need to re-init and play
// immediately.
// ---------------------------------------------------------------------------
//
void CEmailPendingPlay::ProfileChanged()
    {
    FUNC_LOG
    TryStartToneInitialization();
    }

// ---------------------------------------------------------------------------
// Play was requested while tone initialization was ongoing, no its ready
// and tone can be played.
// ---------------------------------------------------------------------------
//
void CEmailPendingPlay::AudioInitCompleted()
    {
    FUNC_LOG
    StateTransition( EPlaying );
    }

// ---------------------------------------------------------------------------
// Activates this state.
// ---------------------------------------------------------------------------
//
void CEmailPendingPlay::DoEnterL()
    {
    FUNC_LOG
    CEmailSoundState::BaseEnter();
    }

// ---------------------------------------------------------------------------
// Playing tone was requested during initialization.
// ---------------------------------------------------------------------------
//
void CEmailPendingPlay::EnterL( CEmailSoundState* aPreviousState )
    {
    FUNC_LOG
    CEmailPendingPlay* state = new ( ELeave ) CEmailPendingPlay(
        aPreviousState->Context() );
    CleanupStack::PushL( state );
    state->DoEnterL();
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailPendingInit::CEmailPendingInit( MEmailSoundStateContext& aContext )
 : CEmailSoundState( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// Playback finished and tone can be initialized.
// ---------------------------------------------------------------------------
//
void CEmailPendingInit::AudioPlayCompleted()
    {
    FUNC_LOG
    // now we can re-init new tone
    StateTransition( EInit );
    }

// ---------------------------------------------------------------------------
// Activates this state.
// ---------------------------------------------------------------------------
//
void CEmailPendingInit::DoEnterL()
    {
    FUNC_LOG
    CEmailSoundState::BaseEnter();
    }

// ---------------------------------------------------------------------------
// Tone initialization was requested during playback.
// ---------------------------------------------------------------------------
//
void CEmailPendingInit::EnterL( CEmailSoundState* aPreviousState )
    {
    FUNC_LOG
    CEmailPendingInit* state = new ( ELeave ) CEmailPendingInit(
        aPreviousState->Context() );
    CleanupStack::PushL( state );
    state->DoEnterL();
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// c++ constructor
// ---------------------------------------------------------------------------
//
CEmailTimerDelay::CEmailTimerDelay( MEmailSoundStateContext& aContext )
 : CEmailSoundState( aContext )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// New profile may have different tone, try initialize it.
// ---------------------------------------------------------------------------
//
void CEmailTimerDelay::ProfileChanged()
    {
    FUNC_LOG
    TryStartToneInitialization();
    }

// ---------------------------------------------------------------------------
// Timer expired. Ready to play again.
// ---------------------------------------------------------------------------
//
void CEmailTimerDelay::TimerCallBackL( TInt /*aError*/ )
    {
    FUNC_LOG
    StateTransition( EReady );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CEmailTimerDelay::~CEmailTimerDelay()
    {
    FUNC_LOG
    delete iTimer;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailTimerDelay::DoEnterL()
    {
    FUNC_LOG
    iTimer = CFSNotificationHandlerTimer::NewL( *this );
    iTimer->After( KDelayTimerInterval );
    CEmailSoundState::BaseEnter();
    }
// ---------------------------------------------------------------------------
// Starts delay timer
// ---------------------------------------------------------------------------
//
void CEmailTimerDelay::EnterL( CEmailSoundState* aPreviousState )
    {
    FUNC_LOG
    CEmailTimerDelay* state = new ( ELeave ) CEmailTimerDelay(
        aPreviousState->Context() );
    CleanupStack::PushL( state );
    state->DoEnterL();
    CleanupStack::Pop( state );
    }

// End of file
