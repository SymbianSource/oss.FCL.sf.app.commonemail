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

#include "emailtrace.h"
#include <ProfileEngineDomainCRKeys.h>
#include <AudioPreference.h>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileTones.h>
#include <MProfileExtraTones.h>
#include <TProfileToneSettings.h>
#include <CProfileChangeNotifyHandler.h>
#include <bautils.h>
#include <coreapplicationuisdomainpskeys.h>

#include "emailsoundhandler.h"
#include "emailsoundstates.h"
#include "cmailhandlerpluginpanic.h"

_LIT( KDefaultEmailTone, "z:\\data\\sounds\\digital\\Message 1.aac");
_LIT8( KEmailBeepSequence, "\x2\x4a\x3a\x51\x9\x95\x95\xc0\x4\x0\xb\x1c\x41\x8d\x51\xa8\x0\x0" );
_LIT( KProfileSilentTone, "Z:\\resource\\No_Sound.wav" );

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::DriveStateChangedL(TBool /*aState*/)
    {
    //causes a reload of soundpayer
    iState->ProfileChanged();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CFSMailSoundHandler* CFSMailSoundHandler::NewL(
    MFSNotificationHandlerMgr& aOwner )
    {
    FUNC_LOG;
    CFSMailSoundHandler* self =
        new( ELeave ) CFSMailSoundHandler( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::ConstructL()
    {
    FUNC_LOG;

    SetObserving(ETrue);

    iProfileEngine = CreateProfileEngineL();
    iHandler = CProfileChangeNotifyHandler::NewL( this );
    iMsgToneSubscriber = CPSSubscriber::NewL(
        *this, KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit );
    iDriveObserver = CDriveObserver::NewL( *this );

    // After sound state initialization iState is valid pointer until
    // CEmailSoundState::Uninitialize is called in the destructor.
    CEmailSoundState::InitializeL(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CFSMailSoundHandler::CFSMailSoundHandler( MFSNotificationHandlerMgr& aOwner )
: CFSNotificationHandlerBase(aOwner)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CFSMailSoundHandler::~CFSMailSoundHandler()
    {
    FUNC_LOG;
    if (iProfileEngine)
        {
        iProfileEngine->Release();
        iProfileEngine = NULL;
        }
    delete iHandler;
    ReleaseAudioPlayer();
    delete iDriveObserver;
    delete iMsgToneSubscriber;
    CEmailSoundState::Uninitialize(iState);
    }

// ---------------------------------------------------------------------------
// HandleEvent is implemented also here because we need to check the
// Home Screen status.
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::HandleEventL(TFSMailEvent aEvent,
        TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3)
    {
    FUNC_LOG;
    // assumption: base class handles event only if it is TFSEventNewMail
    CFSNotificationHandlerBase::HandleEventL( aEvent,
                                              aMailbox,
                                              aParam1,
                                              aParam2,
                                              aParam3 );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::SetState(CEmailSoundState* aNewState)
    {
    FUNC_LOG;
    INFO_1( "email sound state => %d", (TInt) aNewState )
    iState = aNewState;
#ifdef __HANDLER_TEST 
    // for module testing
    if ( iTesterReqStatus )
        {
        TRequestStatus*& status = iTesterReqStatus;
        User::RequestComplete( status, KErrNone );
        iTesterReqStatus = NULL;
        }
#endif    
    }

// ---------------------------------------------------------------------------
// Returns audio player utility
// ---------------------------------------------------------------------------
//
CMdaAudioPlayerUtility* CFSMailSoundHandler::AudioPlayer()
    {
    return iAudioPlayer;
    }

// ---------------------------------------------------------------------------
// CFSMailSoundHandler::HandleActiveProfileChangeL - Callback from end of
// play from Audio player.
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::HandleActiveProfileEventL(TProfileEvent /*aPE*/,
        TInt /*aId*/ )
    {
    FUNC_LOG;
    // iState should never be null
    __ASSERT_ALWAYS( iState, Panic ( ECmailHandlerPluginPanicNullState ) );
    iState->ProfileChanged();
    }

// ---------------------------------------------------------------------------
// CFSMailSoundHandler::MapcInitComplete - Callback from audio player
// initialization.
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::MapcInitComplete( 
    TInt aError,
    const TTimeIntervalMicroSeconds& /*aInterval*/)
    {
    FUNC_LOG;
    // iState should never be null
    __ASSERT_ALWAYS( iState, Panic ( ECmailHandlerPluginPanicNullState ) );
    if ( aError )
        {
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        iState->AudioInitFailed();
        }
    else
        {
        iState->AudioInitCompleted();
        }
    }

// ---------------------------------------------------------------------------
// CFSMailSoundHandler::MapcPlayComplete - Callback from end of play from
// Audio player.
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::MapcPlayComplete(TInt /*aError*/)
    {
    FUNC_LOG;
    // iState should never be null
    __ASSERT_ALWAYS( iState, Panic ( ECmailHandlerPluginPanicNullState ) );
    // error is ignored because there's no corrective action, next play
    // request triggers re-initialization of tone.
    iState->AudioPlayCompleted();
    iMsgToneSubscriber->Cancel();
    }

// ---------------------------------------------------------------------------
// CFSMailSoundHandler::RecreateAudioPlayerL
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::RecreateAudioPlayerL()
    {
    FUNC_LOG;
    delete iAudioPlayer;
    iAudioPlayer = NULL;

    MProfile* profile = iProfileEngine->ActiveProfileL();
    CleanupReleasePushL(*profile);

    TBool vibraEnabled = profile->ProfileTones().ToneSettings().iVibratingAlert;
    TBool mailVibraEnabled = vibraEnabled & profile->ProfileTones().ToneSettings().iEmailVibratingAlert;

    TInt preference = KAudioPrefNewSpecialMessage;
    if ( !mailVibraEnabled )
        {
        preference = EMdaPriorityPreferenceQuality;
        }

    if (IsBeepOnceSetL(*profile))
        {

        // create audio player based on hard coded sequence
        // (Platform does not offer any "play platform-wide beep" service)
        iAudioPlayer = CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(
            KEmailBeepSequence(),
            *this, 
            KAudioPriorityRecvMsg, 
            preference );
        }
    else
        {
        // Otherwise loading tone from file
        TFileName fileToPlay = profile->ProfileExtraTones().EmailAlertTone();
        
        if ( (fileToPlay.Compare(KProfileSilentTone) == 0) &&
                (!vibraEnabled) )
            {
            // Play the silent tone with KAudioPrefNewSpecialMessage
            // in order to avoid the distortion
			// KAudioPrefNewSpecialMessage does not play vibra if KProfileSilentTone is played
            preference = KAudioPrefNewSpecialMessage;
            }
        RFs fs;
        TInt err = fs.Connect();
		CleanupClosePushL( fs );
            
        if ( err == KErrNone )
            {
            TChar chr = fileToPlay[0];
            TInt drive;
            fs.CharToDrive( chr, drive );
            
            //we'll observe any drive where the tone is just because
            //the drive letter of the memory card can  
            //vary from product to product.
            iDriveObserver->SetDriveL( (TDriveNumber)drive );
            
            iDriveObserver->WaitForChange();
            
            //test does the file exist
            if ( !BaflUtils::FileExists( fs, fileToPlay ) )
                {
                //if the file set in profile does not exist, we use default
                fileToPlay.Zero();
                fileToPlay.Append( KDefaultEmailTone );
                }
            
		    CleanupStack::PopAndDestroy( &fs );
            }
        
        iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL( 
                fileToPlay,
                *this, 
                KAudioPriorityRecvMsg, 
                static_cast<TMdaPriorityPreference>( preference ) );
        }
    CleanupStack::PopAndDestroy( profile );
    }

// ---------------------------------------------------------------------------
// Deletes audio player utility
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::ReleaseAudioPlayer()
    {
    FUNC_LOG;
    delete iAudioPlayer;
    iAudioPlayer = NULL;
    }

// ---------------------------------------------------------------------------
// Returns profile engine
// ---------------------------------------------------------------------------
//
MProfileEngine& CFSMailSoundHandler::ProfileEngine() const
    {
    return *iProfileEngine;
    }

// ---------------------------------------------------------------------------
// Playes 'new email' tone
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::TurnNotificationOn()
    {
    FUNC_LOG;
    // iState should never be null
    __ASSERT_ALWAYS( iState, Panic ( ECmailHandlerPluginPanicNullState ) );
    iMsgToneSubscriber->Subscribe();
    iState->PlayTone();
    }

// ---------------------------------------------------------------------------
// Stops playback
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::TurnNotificationOff()
    {
    FUNC_LOG;
    // iState should never be null
    __ASSERT_ALWAYS( iState, Panic ( ECmailHandlerPluginPanicNullState ) );
    iState->StopTone();
    }

// ---------------------------------------------------------------------------
// IsBeepOnceSetL
// ---------------------------------------------------------------------------
//
TBool CFSMailSoundHandler::IsBeepOnceSetL(const MProfile& aProfile) const
    {
    FUNC_LOG;
    // default to false
    TBool ret = EFalse;

    // get tone settings    
    const TProfileToneSettings& toneSettings = aProfile.ProfileTones().ToneSettings();
    
    // if beep-once is set, set return value to ETrue
    if (toneSettings.iRingingType == EProfileRingingTypeBeepOnce)
        {
        ret = ETrue;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// HandlePropertyChangedL
// ---------------------------------------------------------------------------
//
void CFSMailSoundHandler::HandlePropertyChangedL( const TUid& aCategory, TInt aKey )
    {
    FUNC_LOG;
    TInt state(0);

    //
    // Handling the event of user pressing a key while "msg received" -tone is playing
    //
    if ( aCategory == KPSUidCoreApplicationUIs && aKey == KCoreAppUIsMessageToneQuit )
        {
        RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, state );
        INFO_1("KCoreAppUIsMessageToneQuit == %d" , state );
        if (state == ECoreAppUIsStopTonePlaying)
            {
            iState->StopTone();
            iMsgToneSubscriber->Cancel();
            }
        }

    //
    // Handling the event of <other PubSub event can be added here>
    //
    }
