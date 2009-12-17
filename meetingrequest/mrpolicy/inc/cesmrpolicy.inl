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
* Description:  ESMR policy inline methods 
*
*/


// -----------------------------------------------------------------------------
// CESMRPolicy::PolicyId
// -----------------------------------------------------------------------------
//
inline TESMRPolicyID CESMRPolicy::PolicyId() const
    {
    return iPolicyId;
    }

// -----------------------------------------------------------------------------
// CESMRPolicy::ViewMode
// -----------------------------------------------------------------------------
//
inline TESMRViewMode CESMRPolicy::ViewMode() const
    {
    return iViewMode;
    }

// -----------------------------------------------------------------------------
// CESMRPolicy::Fields
// -----------------------------------------------------------------------------
//
inline const RArray<TESMREntryField>& CESMRPolicy::Fields() const
    {
    return iMRFields;
    }

// -----------------------------------------------------------------------------
// CESMRPolicy::AllowedApp
// -----------------------------------------------------------------------------
//
inline TESMRUsingApp CESMRPolicy::AllowedApp() const
    {
    return iAllowedApp;
    }
    
// EOF
