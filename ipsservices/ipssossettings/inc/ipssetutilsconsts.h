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
* Description:  Header for common constant declearations.
*
*/


#ifndef IPSSETUTILSCONSTS_H
#define IPSSETUTILSCONSTS_H

#include <e32std.h>

#include "ipssetutilsconsts.hrh"
#include "ipssetdata.h"

/** Plain text buffer. */
typedef TBuf<KIpsSetUiMaxSettingsLongTextLength> TIpsSetUtilsTextPlain;
/** Password text buffer. */
typedef TBuf<KIpsSetUiMaxPasswordLength> TIpsSetUiPasswordText;
/** Space string. */
_LIT( KIpsSetUiStrSpace, " " );

_LIT( KIpsSetGenPanicLit, "IpsSet");


/**
 * Supported always online states.
 */
enum TIpsSetDataAoStates
    {
    EMailAoAlways = 0,
    EMailAoHomeOnly,
    EMailAoOff
    };

/**
 * Supported OMA EMN states
 */
enum TIpsSetDataEmnStates
    {
    EMailEmnAutomatic = 0,
    EMailEmnHomeOnly,
    EMailEmnOff
    };

/**
 * Update modes.
 */
// not used in fs email
enum TIpsSetDataAoUpdateModes
    {
    EMailAoHeadersOnly = 0,
    EMailAoBody,
    EMailAoBodyAndAttas
    };


/**
 * Flags to handle status of settings
 */
enum TIpsSetDataMailSettingsStatusFlags
    {
    EIpsSetDataStatusTempRcvPassword = 0x1,
    EIpsSetDataStatusTempSndPassword = 0x2
    };

/**
 * Utility structure TAOInfo.
 * Contains info was last update successful ( TBool )
 * and the date and time of last successful update ( TTime )
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
struct TAOInfo
    {
    TBool iLastUpdateFailed; 
    TTime iLastSuccessfulUpdate;
    TBool iUpdateSuccessfulWithCurSettings;
    };

// default settings values

const TInt KIpsSetFetchHeadersDefaultLimit  = 30;
const TInt KIpsSetMaxFetchHeadersDefaultLimit  = 2000;
const TInt KIpsSetDataFiveMinutes           = 5;
const TInt KIpsSetDataTenMinutes            = 10;
const TInt KIpsSetDataFifteenMinutes        = 15;
const TInt KIpsSetDataThirtyMinutes         = 30;
const TInt KIpsSetDataMinutesInHour         = 60;
const TInt KIpsSetDataMinutesInFourHours    = 4 * KIpsSetDataMinutesInHour;
const TInt KIpsSetDataMinutesInTwelveHours  = 12 * KIpsSetDataMinutesInHour;
const TInt KIpsSetDataMinutesInOneDay       = 24 * KIpsSetDataMinutesInHour;
const TInt KIpsSetDataHeadersOnly           = -2;
const TInt KIpsSetDataFullBodyAndAttas      = -1;
const TInt KIpsSetDataFullBodyOnly          = -3;
const TInt KIpsSetDataDefaultDownloadSizeKb = 2; 
const TUint KIpsSetDataEveryDayMask = 0x7f;

// default values for ipssetdataextension
//<cmail> cmail has different default value
//The default value for sync interval is 1 hour, so ao state should be always on.
const TInt KIpsSetExtDefaultAlwaysOnlineState = EMailAoAlways;
//</cmail>
const TInt KIpsSetExtDefaultEmailNotificationState = EMailEmnOff;
const TBool KIpsSetExtDefaultHideMsgs = EFalse;
const TInt KIpsSetExtDefaultInboxRefreshTime = KIpsSetDataMinutesInHour;
const TInt64 KIpsSetExtDefaultLastModified = 0;
const TBool KIpsSetExtDefaultTAOInfoLastUpdateFailed = EFalse; 
const TInt64 KIpsSetExtDefaultTAOInfoLastSuccessfulUpdate = 0;
const TBool KIpsSetExtDefaultTAOInfoUpdateSuccessfulWithCurSettings = EFalse;
const TInt KIpsSetExtDefaultNewMailIndicators = ETrue;
const TInt KIpsSetExtDefaultOpenHtmlMail = EFalse;
const TInt KIpsSetExtDefaultOutgoingLogin = CIpsSetData::EOwnOutgoing;
const TInt64 KIpsSetExtDefaultSelectedTimeStart = 0;
const TInt64 KIpsSetExtDefaultSelectedTimeStop = 0;
const TUint KIpsSetExtDefaultSelectedWeekDays = KIpsSetDataEveryDayMask;
const TUint64 KIpsSetExtDefaultStatusFlags = 0;
const TBool KIpsSetExtDefaultEmnReceivedButNotSyncedFlag = EFalse;
const TBool KIpsSetExtDefaultFirstEMNReceived = EFalse;
// this is from TSSMailSyncState defined in CFSMailCommon.h
// 0 is Idle value
const TInt KIpsSetExtDefaultSyncStatus = 0;
const TInt KIpsSetExtDefaultUpdateMode = EMailAoBody;
// not used in freestyle
const TInt KIpsSetExtDefaultAoVersion = 0;
//<cmail>
const TBool KIpsSetExtDefaultHiddenData = EFalse;
//</cmail>
/**
 * Common UI on/off switch.
 */
enum TIpsSetUiSwitch
    {
    EIpsSetUiOn = 0,
    EIpsSetUiOff
    };

/**
 * Common option / user defined UI editor layout.
 */
enum TIpsSetUiChoice
    {
    EIpsSetUiDefault = 0,
    EIpsSetUiCustom
    };

/**
 * States used in UI editor launching and handling.
 */
enum TIpsSetUiPageResult
    {
    EIpsSetUiPageResultOk = 0,
    EIpsSetUiPageResultSubMenuOpen,
    EIpsSetUiPageResultInvalidResult,
    EIpsSetUiPageResultReadOnly,
    EIpsSetUiPageResultCancelled,
    EIpsSetUiPageResultEditing,
    EIpsSetUiPageResultEventCancel
    };

/**
 * User input evaluation results.
 */
enum TIpsSetUiEventResult
    {
    // Given value is approved
    EIpsSetUiPageEventResultApproved = 0,
    // User has entered an invalid value, editor page should be reopened
    EIpsSetUiPageEventResultDisapproved,
    // The editor has been cancelled and should be closed
    EIpsSetUiPageEventResultCancel
    };

/**
 * Note errors for notes.
 */
enum TIpsSetUiNoteErrors
    {
    /** No errors occurred. */
    EIpsSetUiNoError = 0,
    /** Item contains invalid data. */
    EIpsSetUiItemInvalid,
    /** Item is empty and empty values are not allowed. */
    EIpsSetUiItemEmpty
    };

const TInt KIpsSetDataDefaultPortImap4          = 143;
const TInt KIpsSetDataDefaultSecurityPortImap4  = 993;
const TInt KIpsSetDataDefaultPortPop3           = 110;
const TInt KIpsSetDataDefaultSecurityPortPop3   = 995;
const TInt KIpsSetDataDefaultPortSmtp           = 25;
const TInt KIpsSetDataDefaultSecurityPortSmtp   = 465;
const TInt KIpsSetUtilsMaxMailboxCount          = 9;

#endif /* IPSSETUTILSCONSTS_H */