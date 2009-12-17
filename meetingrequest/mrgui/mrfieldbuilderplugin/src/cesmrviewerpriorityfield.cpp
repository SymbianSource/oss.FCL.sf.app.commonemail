/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Priority field for viewers
 *
*/

#include "emailtrace.h"
#include "cesmrviewerpriorityfield.h"

#include <eiklabel.h>
#include <calentry.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <AknsConstants.h>

#include <esmrgui.rsg>
#include "esmrdef.h"
#include "mesmrlistobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerPriorityField* CESMRViewerPriorityField::NewL()
    {
    FUNC_LOG;
    CESMRViewerPriorityField* self = new (ELeave) CESMRViewerPriorityField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::~CESMRViewerPriorityField()
// ---------------------------------------------------------------------------
//
CESMRViewerPriorityField::~CESMRViewerPriorityField()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::CESMRViewerPriorityField()
// ---------------------------------------------------------------------------
//
CESMRViewerPriorityField::CESMRViewerPriorityField()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerPriorityField::ConstructL()
    {
    FUNC_LOG;
    // As a default set icon and text as it should be in priority NORMAL
    SetFieldId( EESMRFieldPriority );
    CESMRViewerLabelField::ConstructL( KAknsIIDQgnFsIndiPriorityNormal);
    HBufC* priorityText = StringLoader::LoadLC(
                        R_QTN_CALENDAR_MEETING_OPT_PRIORITY_NORMAL,
                        iEikonEnv );
    iLabel->SetTextL( *priorityText );
    CleanupStack::PopAndDestroy( priorityText );
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerPriorityField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    // Get the priority
    TInt priority = aEntry.GetPriorityL();

    HBufC* priorityText;
    TInt resourceId(KErrNotFound);
    TAknsItemID priorityIconId;

    if ( aEntry.Type() == MESMRCalEntry::EESMRCalEntryTodo )
        {
        switch( priority )
            {
            case EFSCalenTodoPriorityLow:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_LOW;
                priorityIconId = KAknsIIDQgnFsIndiPriorityLow;
                break;
                }
            case EFSCalenTodoPriorityHigh:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_HIGH;
                priorityIconId = KAknsIIDQgnFsIndiPriorityHigh;
                break;
                }
            default:
                {
                // Do nothing, priority normal remains
                break;
                }
            }

        }
    else
        {
        switch( priority )
            {
            case EFSCalenMRPriorityLow:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_LOW;
                priorityIconId = KAknsIIDQgnFsIndiPriorityLow;
                break;
                }
            case EFSCalenMRPriorityHigh:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_HIGH;
                priorityIconId = KAknsIIDQgnFsIndiPriorityHigh;
                break;
                }
            default:
                {
                // Do nothing, priority normal remains
                break;
                }
            }
        }

    // if not changed, the default set in ConstructL will be used:
    if ( resourceId != KErrNotFound )
        {
        priorityText = StringLoader::LoadLC( resourceId,
                                             iEikonEnv );
        iLabel->SetTextL( *priorityText );
        CleanupStack::PopAndDestroy( priorityText );

        IconL( priorityIconId );

        // This needs to be called so icon will be redrawn
        SizeChanged();
        }
    }

// EOF

