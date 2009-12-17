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
* Description:  ESMR priority field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrpriorityfield.h"

#include "cesmrlistquery.h"
#include "mesmrtitlepaneobserver.h"

//<cmail>
#include "mesmrcalentry.h"
//</cmail>
#include <calentry.h>
#include <esmrgui.rsg>
#include <StringLoader.h>
#include <barsread.h>
#include <eiklabel.h>
#include <avkon.hrh>
#include <AknsConstants.h>
#include <AknUtils.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRPriorityField::NewL
// ---------------------------------------------------------------------------
//
CESMRPriorityField* CESMRPriorityField::NewL( )
    {
    FUNC_LOG;
    CESMRPriorityField* self = new (ELeave) CESMRPriorityField;
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::~CESMRPriorityField
// ---------------------------------------------------------------------------
//
CESMRPriorityField::~CESMRPriorityField( )
    {
    FUNC_LOG;
    iArray.ResetAndDestroy ( );
    iArray.Close ( );
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::InitializeL()
    {
    FUNC_LOG;
    iPriority->SetFont( iLayout->Font( iCoeEnv, iFieldId ) );
    iPriority->SetLabelAlignment( CESMRLayoutManager::IsMirrored()
                                  ? ELayoutAlignRight : ELayoutAlignLeft );

    AknLayoutUtils::OverrideControlColorL(*iPriority,
                                          EColorLabelText,
                                          iLayout->GeneralListAreaTextColor() );
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TInt prior = aEntry.GetPriorityL ( );

    iEntryType = aEntry.Type();

    // esmrgui.rss
    TResourceReader reader;
    TInt resourceId(KErrNotFound);
    if ( aEntry.Type() == MESMRCalEntry::EESMRCalEntryTodo )
        {
        resourceId = R_ESMREDITOR_TODO_PRIORITY;
        }
    else
        {
        resourceId = R_ESMREDITOR_PRIORITY;
        }

    iCoeEnv->CreateResourceReaderLC ( reader, resourceId );

    iArray.Reset ( );
    // Read priority items to array
    TInt count = reader.ReadInt16 ( );
    for (TInt i(0); i < count; i++ )
        {
        CESMRPriority* priority = new (ELeave) CESMRPriority;
        CleanupStack::PushL ( priority );
        priority->ConstructFromResourceL ( reader );
        iArray.AppendL ( priority );
        CleanupStack::Pop ( priority );
        if ( priority->Id ( )== prior )
            {
            iIndex = i;
            UpdateTextL ( iIndex );
            }
        }
    // reader
    CleanupStack::PopAndDestroy(); // codescanner::cleanup 
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( iIndex < iArray.Count() )
        {
        CCalEntry& calEntry = aEntry.Entry ( );
        calEntry.SetPriorityL ( iArray[iIndex]->Id ( ) );
        }
    else
        {
        User::Leave( KErrOverflow );
        }
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus gained
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdOpenPriorityQuery, R_QTN_MSK_OPEN);
        }
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if( aCommand == EESMRCmdOpenPriorityQuery ||
        aCommand == EAknCmdOpen )
        {
        ExecutePriorityQueryL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ExecutePriorityQueryL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::ExecutePriorityQueryL()
    {
    FUNC_LOG;

    TInt ret(KErrNone);
    if ( iEntryType == MESMRCalEntry::EESMRCalEntryTodo)
        {
        ret = CESMRListQuery::ExecuteL (CESMRListQuery::EESMRTodoPriorityPopup );
        }
    else
        {
        ret = CESMRListQuery::ExecuteL (CESMRListQuery::EESMRPriorityPopup );
        }

    if ( ret != KErrCancel )
        {
        // ret is priority value, not index
        for ( TInt i=0; i<iArray.Count(); ++i )
            {
            if ( iArray[i]->Id() == ret )
                {
                iIndex = i;
                UpdateTextL( iIndex );
                break;
                }
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRPriorityField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( aType == EEventKey )
        {
        switch ( aEvent.iScanCode )
            {
            case EStdKeyLeftArrow:
                {
                if ( iIndex > 0 )
                    {
                    UpdateTextL ( --iIndex );
                    response = EKeyWasConsumed;
                    }
                }
                break;
            case EStdKeyRightArrow:
                {
                if ( iIndex < iArray.Count ( )- 1 )
                    {
                    UpdateTextL ( ++iIndex );
                    response = EKeyWasConsumed;
                    }
                }
                break;
            case EStdKeyDevice3:
                {
                TInt ret(KErrNone);
                if ( iEntryType == MESMRCalEntry::EESMRCalEntryTodo)
                    {
                    ret = CESMRListQuery::ExecuteL (CESMRListQuery::EESMRTodoPriorityPopup );
                    }
                else
                    {
                    ret = CESMRListQuery::ExecuteL (CESMRListQuery::EESMRPriorityPopup );
                    }
                if ( ret != KErrCancel )
                    {
                    // ret is priority value, not index
                    for ( TInt i=0; i<iArray.Count(); ++i )
                        {
                        if ( iArray[i]->Id() == ret )
                            {
                            iIndex = i;
                            UpdateTextL( iIndex );
                            break;
                            }
                        }
                    }
                response = EKeyWasConsumed;
                }
                break;
            default:
                break;
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::CESMRPriorityField
// ---------------------------------------------------------------------------
//
CESMRPriorityField::CESMRPriorityField( )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::ConstructL( )
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldPriority );
    iPriority = new( ELeave ) CEikLabel();
    _LIT(KEmptyText, "");
    iPriority->SetTextL( KEmptyText );

    // ownership transferred
    CESMRIconField::ConstructL (KAknsIIDQgnFsIndiPriorityNormal, iPriority );
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::UpdateTextL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::UpdateTextL( TInt aIndex )
    {
    FUNC_LOG;
    CESMRPriority* priority = iArray[ aIndex ];
    iPriority->SetTextL ( priority->Text ( ) );

    TAknsItemID iconID = { 0 , 0 };

    switch ( priority->Id() )
        {
        case EFSCalenMRPriorityLow:
        case EFSCalenTodoPriorityLow:
            {
            iconID = KAknsIIDQgnFsIndiPriorityLow;
            break;
            }
        case EFSCalenMRPriorityUnknown:
        case EFSCalenMRPriorityNormal:
        case EFSCalenTodoPriorityNormal:
            {
            iconID = KAknsIIDQgnFsIndiPriorityNormal;
            break;
            }
        case EFSCalenMRPriorityHigh: // same value as EFSCalenTodoPriorityHigh
            {
            iconID = KAknsIIDQgnFsIndiPriorityHigh;
            break;
            }
        default:
        	ASSERT(EFalse);
        	break;
        }
    IconL( iconID );
    DrawDeferred ( );

    // update titlepane priority icon
    if ( iObserver )
        {
        iObserver->UpdateTitlePanePriorityIconL( priority->Id() );
        }

    // this is needed to call in order to get new
    // icon drawn
    CESMRIconField::SizeChanged();
    }


// ---------------------------------------------------------------------------
// CESMRPriorityField::UpdateTextL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::SetTitlePaneObserver( MESMRTitlePaneObserver* aObserver )
    {
    iObserver = aObserver;
    }

// EOF

