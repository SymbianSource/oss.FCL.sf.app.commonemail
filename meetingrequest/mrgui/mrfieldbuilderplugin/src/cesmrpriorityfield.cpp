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
* Description:  ESMR priority field implementation
 *
*/

#include "cesmrpriorityfield.h"

#include "cesmrlistquery.h"
#include "nmrcolormanager.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "mesmrtitlepaneobserver.h"

#include <calentry.h>
#include <esmrgui.rsg>
#include <stringloader.h>
#include <barsread.h>
#include <eiklabel.h>
#include <avkon.hrh>
#include <aknsconstants.h>
#include <aknutils.h>

#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRPriorityField::CESMRPriorityField
// ---------------------------------------------------------------------------
//
CESMRPriorityField::CESMRPriorityField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldPriority );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::~CESMRPriorityField
// ---------------------------------------------------------------------------
//
CESMRPriorityField::~CESMRPriorityField()
    {
    FUNC_LOG;
    iArray.ResetAndDestroy();
    iArray.Close();
    
    delete iIcon;    
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::NewL
// ---------------------------------------------------------------------------
//
CESMRPriorityField* CESMRPriorityField::NewL()
    {
    FUNC_LOG;
    CESMRPriorityField* self = new (ELeave) CESMRPriorityField;
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::ConstructL()
    {
    FUNC_LOG;    
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapPriorityNormal );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::InitializeL()
    {
    FUNC_LOG;
    NMRColorManager::SetColor( *iLabel, 
                               NMRColorManager::EMRMainAreaTextColor );
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
    iLabel->SetFocus( aFocus );

    //Focus gained
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdOpenPriorityQuery, R_QTN_MSK_OPEN);
        }
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect = Rect();
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = rowLayoutRect.Rect();
    
    TAknWindowComponentLayout iconLayout =
        NMRLayoutManager::GetWindowComponentLayout( 
                NMRLayoutManager::EMRLayoutTextEditorIcon );
    AknLayoutUtils::LayoutImage( iIcon, rect, iconLayout );
    
    TAknLayoutRect bgLayoutRect =
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    TAknLayoutText labelLayout = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutTextEditor );
    iLabel->SetRect( labelLayout.TextRect() );
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRPriorityField::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iIcon )
        {
        ++count;
        }

    if ( iLabel )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRPriorityField::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
    switch ( aIndex )
        {
        case 0:
            return iIcon;
        case 1:
            return iLabel;
        default:
            return NULL;
        }
    }



// ---------------------------------------------------------------------------
// CESMRPriorityField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRPriorityField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    if( aCommand == EESMRCmdOpenPriorityQuery ||
        aCommand == EAknCmdOpen )
        {
    	HandleTactileFeedbackL();
    	
        ExecutePriorityQueryL();
        isUsed = ETrue;
        }
    return isUsed;
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
// CESMRPriorityField::UpdateTextL
// ---------------------------------------------------------------------------
//
void CESMRPriorityField::UpdateTextL( TInt aIndex )
    {
    FUNC_LOG;
    CESMRPriority* priority = iArray[ aIndex ];
    iLabel->SetTextL ( priority->Text ( ) );

    NMRBitmapManager::TMRBitmapId bitmapId( 
            NMRBitmapManager::EMRBitmapPriorityNormal );

    switch ( priority->Id() )
        {
        case EFSCalenMRPriorityLow:
        case EFSCalenTodoPriorityLow:
            {
            bitmapId = NMRBitmapManager::EMRBitmapPriorityLow;
            break;
            }
        case EFSCalenMRPriorityUnknown:
        case EFSCalenMRPriorityNormal:
        case EFSCalenTodoPriorityNormal:
            {
            bitmapId = NMRBitmapManager::EMRBitmapPriorityNormal;
            break;
            }
        case EFSCalenMRPriorityHigh: // same value as EFSCalenTodoPriorityHigh
            {
            bitmapId = NMRBitmapManager::EMRBitmapPriorityHigh;
            break;
            }
        default:
        	ASSERT(EFalse);
        	break;
        }
    
    delete iIcon;
    iIcon = NULL;
    iIcon = CMRImage::NewL( bitmapId );
    iIcon->SetParent( this );

    // This needs to be called so icon will be redrawn
    SizeChanged();
    DrawDeferred();
    }

// EOF

