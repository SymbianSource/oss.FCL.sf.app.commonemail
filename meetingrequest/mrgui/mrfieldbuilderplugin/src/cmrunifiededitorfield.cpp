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
* Description:  ESMR Unified editor field implementation
 *
*/

#include "cmrunifiededitorfield.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "cesmrtextitem.h"
#include "cesmrlistquery.h"
#include "nmrlayoutmanager.h"
#include "mesmrlistobserver.h"
#include "cesmrglobalnote.h"

#include <esmrgui.rsg>
#include <stringloader.h>
#include <barsread.h>
#include <avkon.hrh>
#include <aknutils.h>
#include <calentry.h>
#include <caleninterimutils2.h>
// DEBUG
#include "emailtrace.h"

namespace
    {
    /**
     * Maps listbox selection index to correct command id
     */
    TInt MapIndexToCommandId( TInt aIndex )
        {
        TInt command = KErrNotFound;

        switch ( aIndex )
            {
            case EMRTypeMeetingRequest:
                {
                command = EMRCommandSwitchToMR;
                break;
                }

            case EMRTypeMeeting:
                {
                command = EMRCommandSwitchToMeeting;
                break;
                }

            case EMRTypeMemo:
                {
                command = EMRCommandSwitchToMemo;
                break;
                }

            case EMRTypeAnniversary:
                {
                command = EMRCommandSwitchToAnniversary;
                break;
                }

            case EMRTypeToDo:
                {
                command = EMRCommandSwitchToTodo;
                break;
                }

            default:
                {
                ASSERT( EFalse );
                break;
                }
            }

        return command;
        }

    } // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::NewL
// ---------------------------------------------------------------------------
//
CMRUnifiedEditorField* CMRUnifiedEditorField::NewL( )
    {
    FUNC_LOG;
    CMRUnifiedEditorField* self = new (ELeave) CMRUnifiedEditorField;
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::~CMRUnifiedEditorField
// ---------------------------------------------------------------------------
//
CMRUnifiedEditorField::~CMRUnifiedEditorField( )
    {
    FUNC_LOG;
    iArray.ResetAndDestroy ( );
    iArray.Close ( );
    iFilter.Reset();

    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::InternalizeL
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    if ( aEntry.IsRecurrentEventL()
         && aEntry.RecurrenceModRule() == MESMRCalEntry::EESMRThisOnly )
        {
        // Hide Unified editor type field
        iObserver->HideControl( EESMRFieldUnifiedEditor );
        }
    else
        {
        // Make sure field is visible
        iObserver->ShowControl( EESMRFieldUnifiedEditor );

        TInt eventValue = aEntry.Type();
        iFilter.Reset();

        // Checking if mailbox has been defined and unified editor
        // field value can be changed to Meeting Request
        CCalenInterimUtils2* calUtils = CCalenInterimUtils2::NewL();
        CleanupStack::PushL( calUtils );
        TBool mrEnabled = calUtils->MRViewersEnabledL( ETrue );
        CleanupStack::PopAndDestroy();

        if ( !mrEnabled )
            {
            iFilter.AppendL( 0 );
            }

        // esmrgui.rss
        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC ( reader, R_ESMREDITOR_UNIFIED );

        // Read event type items to array
        iArray.ResetAndDestroy( );
        TInt count = reader.ReadInt16 ( );
        iArray.ReserveL( count );

        for (TInt i(0); i < count; i++ )
            {
            CESMRTextItem* event = new (ELeave) CESMRTextItem();
            CleanupStack::PushL( event );
            event->ConstructFromResourceL ( reader );
            iArray.AppendL ( event );
            CleanupStack::Pop( event );
            if ( eventValue == event->Id ( ) )
                {
                iIndex = i;
                iType->SetTextL ( event->TextL ( ) );

                // Adding current event type to filter, not to be shown in popup
                iFilter.AppendL( event->Id() );
                }
            }
        // resource reader
        CleanupStack::PopAndDestroy(); // codescanner::cleanup
        
        SetIconL( aEntry );
        }
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus gained
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdOpenUnifiedEditorQuery, R_QTN_MSK_OPEN);
        }
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CMRUnifiedEditorField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    if(aCommand == EESMRCmdOpenUnifiedEditorQuery ||
       aCommand == EAknCmdOpen )
        {
        if( IsLocked() )
        	{
			HandleTactileFeedbackL();
			
            CESMRGlobalNote::ExecuteL(
            		CESMRGlobalNote::EESMRUnableToEdit );
            isUsed = ETrue;            
        	}
        else
        	{
			HandleTactileFeedbackL();
			
        	ExecuteTypeQueryL();
        	isUsed = ETrue;
        	}        
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::LockL
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::LockL()
	{
	FUNC_LOG;
	if( IsLocked() )
		{
		return;
		}
	
	CESMRField::LockL();
	
	delete iLockIcon;
	iLockIcon = NULL;
	iLockIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapLockField, ETrue );
	iLockIcon->SetParent( this );
	}

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::ExecuteTypeQueryL
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::ExecuteTypeQueryL()
    {
    FUNC_LOG;

    TInt ret =
        CESMRListQuery::ExecuteL( CESMRListQuery::EESMRUnifiedEditorPopup,
                                                                  iFilter );

    if ( ret != KErrCancel )
        {
        // Send command to dialog
        NotifyEventAsyncL( MapIndexToCommandId( ret ) );
        }
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::CMRUnifiedEditorField
// ---------------------------------------------------------------------------
//
CMRUnifiedEditorField::CMRUnifiedEditorField( ) :
    iIndex(0)
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldUnifiedEditor );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::ConstructL
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::ConstructL( )
    {
    FUNC_LOG;
    iType = CMRLabel::NewL();
    CESMRField::ConstructL( iType );
    iType->SetTextL( KNullDesC() );

    TGulAlignment align;
    align.SetHAlignment( EHLeft );
    align.SetVAlignment( EVCenter );
    iType->SetAlignment( align );
    
    // An icon is required for layouting the field. Actual correct icon 
    // is set after InternalizeL.
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapEventTypeMeeting );
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::UpdateLabelL
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::UpdateLabelL( TInt aIndex )
    {
    FUNC_LOG;
    CESMRTextItem* event = iArray[ aIndex ];
    iType->SetTextL( event->TextL() );
    iType->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRUnifiedEditorField::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iIcon )
    	{
    	++count;
    	}

    if ( iType )
    	{
    	++count;
    	}
    
    if ( iLockIcon )
    	{
    	++count;
    	}
    return count;
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRUnifiedEditorField::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
    CCoeControl* control = NULL;
    switch( aIndex )
        {
        case 0:
            {
            control = iType;
            break;
            }
        case 1:
            {
            control = iIcon;
            break;
            }
        case 2:
        	{
        	control = iLockIcon;
        	break;
        	}
        default:
            ASSERT( EFalse );
        }

    return control;
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );
    TAknLayoutRect iconLayout( NMRLayoutManager::GetLayoutRect(
                rect, NMRLayoutManager::EMRLayoutTextEditorIcon ) );
    iIcon->SetRect( iconLayout.Rect() );

    // Layouting lock icon
    if( iLockIcon )
    	{
    	TAknWindowComponentLayout iconLayout( 
    			NMRLayoutManager::GetWindowComponentLayout( 
    					NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
    	AknLayoutUtils::LayoutImage( iLockIcon, rect, iconLayout );
    	}
        
    // Layouting label
    TAknLayoutText viewerLayoutText;
    if( iLockIcon )
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rect, 
    			NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
    	}
    else
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rect, 
    			NMRLayoutManager::EMRTextLayoutTextEditor );
    	}

    TRect viewerRect( viewerLayoutText.TextRect() );    
    iType->SetRect( viewerRect );

    // Move focus rect so that it's relative to field's position.
    viewerRect.Move( -Position() );
    SetFocusRect( viewerRect );
        
    TAknTextComponentLayout editorLayout(
    		NMRLayoutManager::GetTextComponentLayout(
    				NMRLayoutManager::EMRTextLayoutTextEditor ) );
    AknLayoutUtils::LayoutLabel( iType, rect, editorLayout );
    }

// ---------------------------------------------------------------------------
// CMRUnifiedEditorField::SetIconL
// ---------------------------------------------------------------------------
//
void CMRUnifiedEditorField::SetIconL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    NMRBitmapManager::TMRBitmapId bitmapId( 
            NMRBitmapManager::EMRBitmapNotSet );
    
    CCalEntry& entry = aEntry.Entry();
    
    switch ( entry.EntryTypeL() )
        {
        case CCalEntry::EAppt:
            {
            if( CCalenInterimUtils2::IsMeetingRequestL( entry ) )
                {
                bitmapId = NMRBitmapManager::EMRBitmapEventTypeMR;
                }
            else
                {
                bitmapId = NMRBitmapManager::EMRBitmapEventTypeMeeting;
                }
            break;
            }

        case CCalEntry::ETodo:
            {
            bitmapId = NMRBitmapManager::EMRBitmapEventTypeTodo;
            break;
            }

        case CCalEntry::EEvent: // Memo
            {
            bitmapId = NMRBitmapManager::EMRBitmapEventTypeMemo;
            break;
            }

        case CCalEntry::EAnniv:
            {
            bitmapId = NMRBitmapManager::EMRBitmapEventTypeAnniversary;
            break;
            }

        default:
            {
            ASSERT( EFalse );
            break;
            }
        }

    delete iIcon;
    iIcon = NULL;
    iIcon = CMRImage::NewL( bitmapId );
    }

// EOF

