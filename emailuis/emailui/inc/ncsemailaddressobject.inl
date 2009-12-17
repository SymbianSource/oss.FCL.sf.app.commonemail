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
* Description:  Implementation of inline methods
*
*/

// ---------------------------------------------------------------------------
// CNcsEmailAddressObject::EmailAddress
// ---------------------------------------------------------------------------
//
inline const TDesC& CNcsEmailAddressObject::EmailAddress() const
    {
	return *iEmailAddress;
    }

// ---------------------------------------------------------------------------
// CNcsEmailAddressObject::DisplayName
// ---------------------------------------------------------------------------
//
inline const TDesC& CNcsEmailAddressObject::DisplayName() const
    {
	return *iDisplayName;
    }

// ---------------------------------------------------------------------------
// CNcsEmailAddressObject::IsUserAdded
// ---------------------------------------------------------------------------
//
inline TBool CNcsEmailAddressObject::IsUserAdded() const
    {
	return iUserAdded;
    }

// ---------------------------------------------------------------------------
// CNcsEmailAddressObject::SetDisplayFull
// ---------------------------------------------------------------------------
//
inline void CNcsEmailAddressObject::SetDisplayFull( TBool aDisplayFull )
    {
    iDisplayFull = aDisplayFull;
    }

// ---------------------------------------------------------------------------
// CNcsEmailAddressObject::DisplayFull
// ---------------------------------------------------------------------------
//
inline TBool CNcsEmailAddressObject::DisplayFull() const
    {
    return iDisplayFull;
    }
