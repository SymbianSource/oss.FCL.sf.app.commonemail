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
* Description:  Declearation for exception handling.
*
*/

#ifndef IPSSETUTILSEXCEPTION_H
#define IPSSETUTILSEXCEPTION_H

class TIpsSetUtilsException;

#ifdef __DEBUG

/**
 * Panic, when the test fails
 */
#define IPS_ASSERT_DEBUG( test, reason, id ) !test ? IPSPANIC(reason, id) : NULL
#else
#define IPS_ASSERT_DEBUG( test, reason, id )
#endif /* _DEBUG */

/**
 * Invokes panic
 *
 * @param reason Reason from the TIpsSetUtilsErrorCode enumeration
 * @param id ID from the TIpsSetUtilsExceptionIds enumeration
 */
#define IPS_PANIC( reason, id ) TIpsSetUtilsException::Panic( TIpsSetUtilsException::reason, TIpsSetUtilsException::id )

/**
 * Invokes panic or leave based on the
 *
 * @param reason Reason from the TIpsSetUtilsErrorCode enumeration
 * @param id ID from the TIpsSetUtilsExceptionIds enumeration
 */
#define IPSASSERT_LEAVE_OR_PANIC_L( reason, id ) TIpsSetUtilsException::LeaveOrPanicL( TIpsSetUtilsException::reason, TIpsSetUtilsException::id )


/**
 * Class for exception control
 *
 * @lib IpsSosSettings.lib
 * @since FS v1.0
 */
class TIpsSetUtilsException
    {
public:

    /** Id's for classes */
    enum TIpsSetUtilsExceptionIds
        {
        EItemText = 0,
        EItemValue,
        EItemLink,
        EItemLinkExt,
        EBaseArray,
        EBaseDialog,
        ESoftkeys,
        EPageResource,
        EDialogCtrl,
        EAccountCtrl,
        EParameter,
        EStorer
        };

    /** Enumeration of all panics for IPS SOS Settings */
    enum TIpsSetUtilsErrorCode
        {
        ENoError = 0,
        ENullPointer = -1100,
        ENullPointerInParam,
        EUnidentifiedItem,
        ESoftkeyLeft,
        ESoftkeyMiddle,
        ESoftkeyRight,
        EWrongType,
        EStackEmpty,
        EStackNull,
        EArrayNull,
        EUnknownType,
        EInvalidAccount,
        EInvalidMtm,
        EUnderflowMailboxId,
        EUnderflowAccountId,
        EOverflowAccountId,
        EIllegalMtm,
        ESettingsNotValid
        };

    /**
     * Invokes panic in debug mode, does nothing in release mode
     *
     * @param aReason Reason for panic
     * @param aId ID of the panicing component
     */
    inline static void Panic( const TInt aReason, const TUint aId );

    /**
     * Leaves or Panic based on rel/debug mode
     *
     * @param aReason Reason for panic
     * @param aId ID of the panicing component
     */
    inline static void LeaveOrPanicL( const TInt aReason, const TInt aId );

private:

    /**
     * Converts the id to string
     *
     * @param aId ID of the panicing component
     */
    static const TDesC& Id2Name( const TInt aId );
    };

#include "ipssetutilsexception.inl"

#endif // IPSSETUTILSEXCEPTION_H

// End of File
