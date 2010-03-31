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
* Description:  Implementation for location history item factory
*
*/


#include "emailtrace.h"
#include "cesmrlocationhistoryitemfactory.h"
#include "mesmrlocationhistoryitem.h"
#include "cesmrlocationhistoryitem.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRLocationHistoryItemFactory::NewL
// ---------------------------------------------------------------------------
//
CESMRLocationHistoryItemFactory* CESMRLocationHistoryItemFactory::NewL()
    {
    FUNC_LOG;
    CESMRLocationHistoryItemFactory* self =
        new (ELeave) CESMRLocationHistoryItemFactory();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRLocationHistoryItemFactory::~CESMRLocationHistoryItemFactory
// ---------------------------------------------------------------------------
//
CESMRLocationHistoryItemFactory::~CESMRLocationHistoryItemFactory()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRLocationHistoryItemFactory::CreateLocationHistoryItemL
// ---------------------------------------------------------------------------
//
MESMRLocationHistoryItem* CESMRLocationHistoryItemFactory::CreateLocationHistoryItemL(
        const TDesC& aAddress, 
        const TDesC& aUrl )
    {
    FUNC_LOG;
    MESMRLocationHistoryItem* item = CESMRLocationHistoryItem::NewL( iIndex, aAddress, aUrl );
    iIndex++;
    return item;
    }

// ---------------------------------------------------------------------------
// CESMRLocationHistoryItemFactory::CESMRLocationHistoryItemFactory
// ---------------------------------------------------------------------------
//
CESMRLocationHistoryItemFactory::CESMRLocationHistoryItemFactory() :
    iIndex( 0 )
    {
    FUNC_LOG;
    }
    
// EOF

