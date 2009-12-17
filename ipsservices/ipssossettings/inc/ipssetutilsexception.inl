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
* Description:  Definition for exception handling.
*
*/


// ----------------------------------------------------------------------------
// TIpsSetUtilsException::LeaveOrPanicL()
// ----------------------------------------------------------------------------
//
#ifdef _DEBUG
void TIpsSetUtilsException::LeaveOrPanicL( const TInt aReason, const TInt aId )
#else
void TIpsSetUtilsException::LeaveOrPanicL( const TInt aReason, const TInt /*aId*/ )
#endif
    {
    __ASSERT_DEBUG( ETrue, User::Panic( Id2Name( aId ), aReason ) );
    User::Leave( aReason );
    }

// ----------------------------------------------------------------------------
// TIpsSetUtilsException::Panic()
// ----------------------------------------------------------------------------
//
#ifdef _DEBUG
void TIpsSetUtilsException::Panic( const TInt aReason, const TUint aId )
#else
void TIpsSetUtilsException::Panic( const TInt /*aReason*/, const TUint /*aId*/ )
#endif
    {
    __ASSERT_DEBUG( ETrue, User::Panic( Id2Name( aId ), aReason ) );
    }

// End of File

