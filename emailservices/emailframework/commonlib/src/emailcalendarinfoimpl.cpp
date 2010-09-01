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
* Description:  implementation of calendar info extension for mailbox
*
*/

#include "emailtrace.h"
#include "cmrcalendarinfoimpl.h"          

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMRCalendarInfo::CMRCalendarInfo( const TUid& aUid ) : CEmailExtension( aUid )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMRCalendarInfoImpl::CMRCalendarInfoImpl() : 
    CMRCalendarInfo( KMailboxExtMrCalInfo )
    {
    iDatabaseId = MAKE_TINT64(0,0);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMRCalendarInfoImpl::~CMRCalendarInfoImpl()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMRCalendarInfoImpl::GetCalendarDatabaseIdL( TCalFileId& aId ) const
    {
    FUNC_LOG
    aId = iDatabaseId;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMRCalendarInfoImpl::SetCalendarDatabaseIdL( const TCalFileId& aId )
    {
    FUNC_LOG
    iDatabaseId = aId;
    }

