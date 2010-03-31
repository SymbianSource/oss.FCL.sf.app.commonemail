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
* Description:  Implementation of inline methods
*
*/

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::EmailAddress
// ---------------------------------------------------------------------------
//
inline const TDesC& CESMRNcsEmailAddressObject::EmailAddress() const
    {
	return *iEmailAddress;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::DisplayName
// ---------------------------------------------------------------------------
//
inline const TDesC& CESMRNcsEmailAddressObject::DisplayName() const
    {
	return *iDisplayName;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::IsUserAdded
// ---------------------------------------------------------------------------
//
inline TBool CESMRNcsEmailAddressObject::IsUserAdded()
    {
	return iUserAdded;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::SetDisplayFull
// ---------------------------------------------------------------------------
//
inline void CESMRNcsEmailAddressObject::SetDisplayFull( TBool aDisplayFull )
    {
    iDisplayFull = aDisplayFull;
    }

// ---------------------------------------------------------------------------
// CESMRNcsEmailAddressObject::DisplayFull
// ---------------------------------------------------------------------------
//
inline TBool CESMRNcsEmailAddressObject::DisplayFull() const
    {
    return iDisplayFull;
    }
