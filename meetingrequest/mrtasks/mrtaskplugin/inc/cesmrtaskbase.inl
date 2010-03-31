/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline implementation for esmr base tasks
*
*/


// ---------------------------------------------------------------------------
// CESMRTaskBase::CalDbMgr
// ---------------------------------------------------------------------------
//
inline MESMRCalDbMgr& CESMRTaskBase::CalDbMgr()
    {
    return iCalDbMgr;
    }

// ---------------------------------------------------------------------------
// CESMRTaskBase::ESMREntry
// ---------------------------------------------------------------------------
//
inline MESMRMeetingRequestEntry& CESMRTaskBase::ESMREntry()
    {
    return iEntry;
    }

// ---------------------------------------------------------------------------
// CESMRTaskBase::MailboxUtils
// ---------------------------------------------------------------------------
//
inline CMRMailboxUtils& CESMRTaskBase::MailboxUtils()
    {
    return iMRMailboxUtils;
    }
