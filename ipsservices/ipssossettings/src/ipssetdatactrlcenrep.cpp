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
* Description: This file implements class CIpsSetDataCtrlCenrep.
*
*/


#include "emailtrace.h"
#include <centralrepository.h>          // CRepository

#include "ipssetdatactrlcenrep.h"
#include "ipssetdatactrlaccount.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::CIpsSetDataCtrlCenrep()
// ----------------------------------------------------------------------------
//
CIpsSetDataCtrlCenrep::CIpsSetDataCtrlCenrep()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::~CIpsSetDataCtrlCenrep()
// ----------------------------------------------------------------------------
//
CIpsSetDataCtrlCenrep::~CIpsSetDataCtrlCenrep()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::ConstructL()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetDataCtrlCenrep* CIpsSetDataCtrlCenrep::NewL()
    {
    FUNC_LOG;
    CIpsSetDataCtrlCenrep* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetDataCtrlCenrep* CIpsSetDataCtrlCenrep::NewLC()
    {
    FUNC_LOG;
    CIpsSetDataCtrlCenrep* self = new ( ELeave ) CIpsSetDataCtrlCenrep();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

/******************************************************************************

    Storer Tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::SetActiveCentralRepository()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::SetActiveCentralRepository(
    CRepository& aRepository )
    {
    FUNC_LOG;
    iCenRep = &aRepository;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::SetActiveBaseKey()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::SetActiveBaseKey(
    const TUint32 aAccountId,
    const TUid& aProtocol )
    {
    FUNC_LOG;
    iBaseKey = IpsSetDataCtrlAccount::CreateBaseKeyFromAccountId(
        aAccountId, aProtocol );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::SetActiveBaseKey()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::SetActiveBaseKey(
    const TUint32 aExtendedAccountId )
    {
    FUNC_LOG;
    iBaseKey = IpsSetDataCtrlAccount::CreateBaseKeyFromExtendedAccountId(
        aExtendedAccountId );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::CreateToAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::CreateToAccountL(
    const TUint32 aSetting,
    const TInt aValue )
    {
    FUNC_LOG;
    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;

    TInt err = iCenRep->Create( settingId, aValue );

    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::CreateToAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::CreateToAccountL(
    const TUint32 aSetting,
    const TDesC& aString )
    {
    FUNC_LOG;
    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;

    TInt err = iCenRep->Create( settingId, aString );

    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::CreateToAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::CreateToAccountL(
    const TInt aSettingHigh,
    const TInt aSettingLow,
    const TInt64 aValue )
    {
    FUNC_LOG;
    // The high and low part has to be stored separately
    CreateToAccountL( aSettingHigh, I64HIGH( aValue ) );
    CreateToAccountL( aSettingLow, I64LOW( aValue ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::DeleteFromAccountL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataCtrlCenrep::DeleteFromAccount(
    const TInt aSetting ) const
    {
    FUNC_LOG;
    // Load the values
    TUint32 settingId = iBaseKey + aSetting;
    TInt error = iCenRep->Delete( settingId );
    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::DeleteFromAccountL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataCtrlCenrep::DeleteFromAccount(
    const TInt aSettingHigh,
    const TInt aSettingLow ) const
    {
    FUNC_LOG;
    // Load the values
    TUint32 settingId = iBaseKey + aSettingHigh;
    TInt error = iCenRep->Delete( settingId );

    settingId = iBaseKey + aSettingLow;
    TInt error2 = iCenRep->Delete( settingId );
    
    if ( error != KErrNone )
        {
        return error;
        }
    else 
        {
        return error2;
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::SetToAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::SetToAccountL(
    const TUint32 aSetting,
    const TInt aValue )
    {
    FUNC_LOG;
    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;
    User::LeaveIfError( iCenRep->Set( settingId, aValue ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::SetToAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::SetToAccountL(
    const TUint32 aSetting,
    const TDesC& aString )
    {
    FUNC_LOG;
    // Determine the setting id and create the new key
    TUint32 settingId = iBaseKey + aSetting;
    User::LeaveIfError( iCenRep->Set( settingId, aString ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::SetToAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::SetToAccountL(
    const TInt aSettingHigh,
    const TInt aSettingLow,
    const TInt64 aSetting )
    {
    FUNC_LOG;
    // The high and low part has to be stored separately
    SetToAccountL( aSettingHigh, I64HIGH( aSetting ) );
    SetToAccountL( aSettingLow, I64LOW( aSetting ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::GetFromAccount()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataCtrlCenrep::GetFromAccount(
    const TInt aSettingHigh,
    const TInt aSettingLow,
    TInt64& aValue ) const
    {
    FUNC_LOG;
    TInt error = KErrNone;
    // Prepare the number
    TInt high = 0;
    TInt low = 0;

    // Load the values
    TUint32 settingId = iBaseKey + aSettingHigh;
    error = iCenRep->Get( settingId, high );
    if ( error == KErrNone )
        {
    settingId = iBaseKey + aSettingLow;
        error = iCenRep->Get( settingId, low );
        }

    if ( error == KErrNone )
        {
        aValue = MAKE_TINT64( high, low );
        }
    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::GetFromAccount()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataCtrlCenrep::GetFromAccount(
    const TInt aSetting,
    TInt& aValue ) const
    {
    FUNC_LOG;
    TInt error = KErrNone;
    TUint32 settingId = iBaseKey + aSetting;
    error = iCenRep->Get( settingId, aValue );
    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::GetTextFromAccount()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataCtrlCenrep::GetTextFromAccount(
    const TInt aSetting,
    TDes& aText ) const
    {
    FUNC_LOG;
    TInt error = KErrNone;
    TUint32 settingId = iBaseKey + aSetting;
    error = iCenRep->Get( settingId, aText );
    return error;
    }
// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::GetFromAccountAndCreateIfNotFound()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataCtrlCenrep::GetFromAccountAndCreateIfNotFound(
    const TInt aSettingHigh,
    const TInt aSettingLow,
    const TInt64 aDefaultValue,
    TInt64& aValue )
    {
    FUNC_LOG;
    TInt error = GetFromAccount( aSettingHigh, aSettingLow, aValue );
    if ( error == KErrNotFound )
        {
        TRAPD( trapErr, CreateToAccountL( 
                aSettingHigh, aSettingLow, aDefaultValue ) );
        if ( trapErr != KErrNone )
            {
            error = trapErr;
            }
        aValue = aDefaultValue;
        }
    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::GetFromAccountAndCreateIfNotFound()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataCtrlCenrep::GetFromAccountAndCreateIfNotFound(
    const TInt aSetting,
    const TInt aDefaultValue,
    TInt& aValue )
    {
    FUNC_LOG;
    TInt error = GetFromAccount( aSetting, aValue );
    if ( error == KErrNotFound )
        {
        TRAPD( trapErr, CreateToAccountL( 
                aSetting, aDefaultValue ) );
        if ( trapErr != KErrNone )
            {
            error = trapErr;
            }
        aValue = aDefaultValue;
        }
    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataCtrlCenrep::SetActiveDefaultDataKey()
// ----------------------------------------------------------------------------
//
void CIpsSetDataCtrlCenrep::SetActiveDefaultDataKey()
    {
    FUNC_LOG;
    iBaseKey = 0;
    }

// End of File

