/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMRAgnVersit2StringProvider.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmragnversit2stringprovider.h"
#include "esmrhelper.h"

//debug
//<cmail>
//</cmail>

// System includes.
#include <barsc.h>              // RResourceFile
#include <bautils.h>            // BaflUtils
#include <data_caging_path_literals.hrh>

// Resource include.
#include <esmragnversit2strings.rsg>

// Constants
namespace{//local namespace for constants

// CESMRAgnVersit2StringProvider resource name
_LIT( KESMRAgnVersit2ResourceFilename, "esmragnversit2strings.rsc");

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnVersit2StringProvider::NewL
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2StringProvider* CESMRAgnVersit2StringProvider::NewL()
    {
    FUNC_LOG;

    CESMRAgnVersit2StringProvider* self = new (ELeave) CESMRAgnVersit2StringProvider;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2StringProvider::~CESMRAgnVersit2StringProvider
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2StringProvider::~CESMRAgnVersit2StringProvider()
    {
    FUNC_LOG;

    iStringArray.ResetAndDestroy();
    iResourceFile.Close();
    iFs.Close();

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2StringProvider::StringL
// ---------------------------------------------------------------------------
//
const TDesC& CESMRAgnVersit2StringProvider::StringL(TStringId aStringId)
    {
    FUNC_LOG;

    ASSERT(iStringArray.Count() >= aStringId + 1);
    if ( !iStringArray[aStringId] )
        {
        ReadStringFromResourceL(aStringId);
        }

    return *iStringArray[aStringId];
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2StringProvider::CESMRAgnVersit2StringProvider
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2StringProvider::CESMRAgnVersit2StringProvider()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2StringProvider::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2StringProvider::ConstructL()
    {
    FUNC_LOG;

    //Fill iStringArray with NULL pointers...
    for (TInt x = 0; x < EICalNumberOfStrings; ++x)
        {
        User::LeaveIfError(iStringArray.Append(NULL));
        }
    User::LeaveIfError(iFs.Connect());

    TFileName resource;


    User::LeaveIfError(
            ESMRHelper::LocateResourceFile(
                    KESMRAgnVersit2ResourceFilename,
                    KDC_RESOURCE_FILES_DIR,
                    resource,
                    &iFs ) );

    // Find a localised resource file if available -- if not, it will be left as .rsc
    BaflUtils::NearestLanguageFile(iFs, resource);
    // Now we've got the full resource file name, so open it
    iResourceFile.OpenL(iFs, resource);
    iResourceFile.ConfirmSignatureL(0);

    // Read all strings immediately in debug mode.
    __DEBUG_ONLY(ReadAllStringsL());

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2StringProvider::ReadStringFromResourceL
// ---------------------------------------------------------------------------
//
/**
 * Reads a string from the resource file and adds it to the array of strings.
 * @param aStringId string to add
 * @internalTechnology
 * */
void CESMRAgnVersit2StringProvider::ReadStringFromResourceL(TStringId aStringId)
    {
    FUNC_LOG;

    TInt offset = 0;

    switch (aStringId)
        {
        case EICalProdIdValue:
            offset = R_ICAL_PRODID;
            break;
        case EICalTzidUtc:
            offset = R_TZID_UTC;
            break;
        case EICalTzidPlus:
            offset = R_TZID_PLUS;
            break;
        case EICalTzidMinus:
            offset = R_TZID_MINUS;
            break;
        case EICalTzidSlash:
            offset = R_TZID_SLASH;
            break;
        case EICalTzidStandard:
            offset = R_TZID_STANDARD;
            break;
        case EICalTzidDaylight:
            offset = R_TZID_DAYLIGHT;
            break;
        case EICalErrRruleNumRules:
            offset = R_ERRIMPORT_NUMRULES;
            break;
        case EICalErrRruleNoFreq:
            offset = R_ERRIMPORT_NOFREQ;
            break;
        case EICalErrRruleNotYearly:
            offset = R_ERRIMPORT_NOTYEARLY;
            break;
        case EICalErrValarmNotAllowed:
            offset = R_ERRIMPORT_NOTALLOWED;
            break;
        case EICalErrValarmNoTrigger:
            offset = R_ERRIMPORT_NOTRIGGER;
            break;
        case EICalErrValarmNoStart:
            offset = R_ERRIMPORT_NOSTART;
            break;
        case EICalErrAlarmAfterEvent:
            offset = R_ERRIMPORT_AFTEREVENT;
            break;
        case EICalXParamType:
            offset = R_XPARAM_TYPE;
            break;
        case EICalXParamAppointment:
            offset = R_XPARAM_APPT;
            break;
        case EICalXParamTodo:
            offset = R_XPARAM_TODO;
            break;
        case EICalXParamEvent:
            offset = R_XPARAM_EVENT;
            break;
        case EICalXParamReminder:
            offset = R_XPARAM_REMINDER;
            break;
        case EICalXParamAnniversary:
            offset = R_XPARAM_ANNIVERSARY;
            break;
        case EICalAppointment:
            offset = R_CAT_APPOINTMENT;
            break;
        case EICalBusiness:
            offset = R_CAT_BUSINESS;
            break;
        case EICalEducation:
            offset = R_CAT_EDUCATION;
            break;
        case EICalHoliday:
            offset = R_CAT_HOLIDAY;
            break;
        case EICalMeeting:
            offset = R_CAT_MEETING;
            break;
        case EICalMisc:
            offset = R_CAT_MISC;
            break;
        case EICalPersonal:
            offset = R_CAT_PERSONAL;
            break;
        case EICalPhoneCall:
            offset = R_CAT_PHONECALL;
            break;
        case EICalSick:
            offset = R_CAT_SICK;
            break;
        case EICalSpecial:
            offset = R_CAT_SPECIAL;
            break;
        case EICalTravel:
            offset = R_CAT_TRAVEL;
        break;
        case EICalVacation:
            offset = R_CAT_VACATION;
            break;
        default:
            User::Leave(KErrCorrupt);
            break;
        }
    HBufC8* resourceData = iResourceFile.AllocReadLC(offset);
    iResourceReader.SetBuffer(resourceData);
    iResourceBuffer = iResourceReader.ReadTPtrC().AllocL();
    
    iStringArray[aStringId] = iResourceBuffer;  // iStringArray takes ownership here.	
    
    iResourceBuffer = NULL;

    CleanupStack::PopAndDestroy(resourceData);

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2StringProvider::ReadAllStringsL
// ---------------------------------------------------------------------------
//
/**
 * Reads every string into the string array to prevent lazy initialisation from
 * confusing any OOM tests. If we don't do this, then the strings stored in the
 * array between one import and the next look like a memory leak.
 * @internalTechnology
 */
void CESMRAgnVersit2StringProvider::ReadAllStringsL()
    {
    FUNC_LOG;

    for (TInt id = 0; id < EICalNumberOfStrings; ++id)
        {
        ReadStringFromResourceL(static_cast<TStringId>(id));
        }

    }

// End of file.

