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
* Description:  MRUI alarm info handler object implementation
*
*/


//<cmail>
#include "emailtrace.h"
#include "cesmralarminfohandler.h"
#include "esmrdef.h"
//</cmail>

#include "mesmrmeetingrequestentry.h"

#include <coemain.h>
#include <barsread.h>
#include <barsc.h>

#include <calentry.h>
#include <calalarm.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// ESMRAlarmInfo module literal definition
_LIT( KTESMRAlarmInfoPanicTxt, "ESMRAlarmInfo" );

/** ESMRAlarmInfo panic code definitions */
enum TESMRAlarmInfoPanic
    {
    // Coe environment does not exist
    EESMRAlarmInfoCoeNotExist
    };

void Panic( TESMRAlarmInfoPanic aPanic )
    {

    User::Panic( KTESMRAlarmInfoPanicTxt, aPanic );
    }

#endif

// Definition for no alarm
const TInt KNoAlarm( -1 );

/**
 * Compares two alarm info objects.
 *
 * @param aLhs Left hand side alarm info object.
 * @param aRhs Right hand side alarm info object.
 * @return ETrue if objects matches by iRelativeAlarmInSeconds member.
 */
TBool CompareByAlarmTime(
        const TESMRAlarmInfo& aLhs,
        const TESMRAlarmInfo& aRhs )
    {
    if ( aLhs.iRelativeAlarmInSeconds == aRhs.iRelativeAlarmInSeconds )
        {
        return ETrue;
        }
    return EFalse;
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::CESMRAlarmInfoHandler
// ---------------------------------------------------------------------------
//
inline CESMRAlarmInfoHandler::CESMRAlarmInfoHandler()
    {
    FUNC_LOG;
    // No implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRAlarmInfoHandler::~CESMRAlarmInfoHandler
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRAlarmInfoHandler::~CESMRAlarmInfoHandler()
    {
    FUNC_LOG;
    iAlarmInfos.Reset();
    iAlarmInfos.Close();
    }

// ---------------------------------------------------------------------------
// CESMRAlarmInfoHandler::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRAlarmInfoHandler* CESMRAlarmInfoHandler::NewL()
    {
    FUNC_LOG;

    CESMRAlarmInfoHandler* self = NewLC();
    CleanupStack::Pop( self );


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAlarmInfoHandler::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRAlarmInfoHandler* CESMRAlarmInfoHandler::NewLC()
    {
    FUNC_LOG;

    CESMRAlarmInfoHandler* self =
            new (ELeave) CESMRAlarmInfoHandler();
    CleanupStack::PushL( self );
    self->ConstructL();


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAlarmInfoHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAlarmInfoHandler::ConstructL()
    {
    FUNC_LOG;
    // No implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRAlarmInfoHandler::ReadFromResourceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAlarmInfoHandler::ReadFromResourceL(
        const TDesC& aResourceFile,
        TInt aResouceId )
    {
    FUNC_LOG;

    iAlarmInfos.Reset();

    TFileName resourceFileName(aResourceFile);

    CCoeEnv* coeEnv = CCoeEnv::Static();
    __ASSERT_DEBUG( coeEnv, Panic(EESMRAlarmInfoCoeNotExist) );


    RResourceFile alarmInfoResourceFile;
    alarmInfoResourceFile.OpenL(
            coeEnv->FsSession(),
            resourceFileName);
    CleanupClosePushL( alarmInfoResourceFile );
    alarmInfoResourceFile.ConfirmSignatureL();

    HBufC8* resourceIdBuffer =
            alarmInfoResourceFile.AllocReadLC( aResouceId );

    // Construct resource reader
    TResourceReader reader;
    reader.SetBuffer(resourceIdBuffer);

    TInt alarmInfoCount( reader.ReadInt16() );
    for (TInt i(0); i < alarmInfoCount; ++i )
        {
        TESMRAlarmInfo alarmInfo;
        alarmInfo.iAlarmIdValue =
                static_cast<TESMRAlarmValue>( reader.ReadInt16() );
        alarmInfo.iRelativeAlarmInSeconds = reader.ReadInt32();
        User::LeaveIfError( iAlarmInfos.Append( alarmInfo) );
        }

    CleanupStack::PopAndDestroy( resourceIdBuffer );
    CleanupStack::PopAndDestroy( &alarmInfoResourceFile );
    coeEnv = NULL;

    }

// ---------------------------------------------------------------------------
// CESMRAlarmInfoHandler::GetAlarmInfoObjectL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAlarmInfoHandler::GetAlarmInfoObjectL(
        const CCalEntry& aEntry,
        TESMRAlarmInfo& aAlarmInfo )
    {
    FUNC_LOG;

    TESMRAlarmInfo alarmInfo;
    alarmInfo.iRelativeAlarmInSeconds = KNoAlarm;

    // Ownership transferred to us
    CCalAlarm* alarm = aEntry.AlarmL();
    CleanupStack::PushL( alarm );
    if ( alarm )
        {
        alarmInfo.iRelativeAlarmInSeconds =
            alarm->TimeOffset().Int();
        }
    CleanupStack::PopAndDestroy( alarm );

    TInt pos( iAlarmInfos.Find(alarmInfo, CompareByAlarmTime) );

    if ( KErrNotFound != pos )
        {
        aAlarmInfo = iAlarmInfos[pos];
        }
    else
        {

        User::Leave( KErrNotFound );
        }

    }

// ---------------------------------------------------------------------------
// CESMRAlarmInfoHandler::GetAbsoluteAlarmTimeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAlarmInfoHandler::GetAbsoluteAlarmTimeL(
        CCalEntry& aEntry,
        TTime& aAbsoluteAlarmTime )
    {
    FUNC_LOG;

    // Ownership transferred to us
    CCalAlarm* alarm = aEntry.AlarmL();

    if ( !alarm )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PushL( alarm );

    aAbsoluteAlarmTime =
            aEntry.StartTimeL().TimeLocalL() - alarm->TimeOffset();

    CleanupStack::PopAndDestroy( alarm );

    }

// EOF

