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
* Description:  MR multi calendar field implementation
 *
*/

#include "cmrmulticalenfield.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "cesmrtextitem.h"
#include "cesmrlistquery.h"
#include "nmrlayoutmanager.h"
#include "nmrcolormanager.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrglobalnote.h"
#include <calinstance.h>
#include <esmrgui.rsg>
#include <stringloader.h>
#include <barsread.h>
#include <avkon.hrh>
#include <aknutils.h>
#include <calentry.h>
#include <caleninterimutils2.h>
// DEBUG
#include "emailtrace.h"
#include "cesmrcaldbmgr.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRMultiCalenField::NewL
// ---------------------------------------------------------------------------
//
CMRMultiCalenField* CMRMultiCalenField::NewL( )
    {
    FUNC_LOG;
    CMRMultiCalenField* self = new (ELeave) CMRMultiCalenField;
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::~CMRMultiCalenField
// ---------------------------------------------------------------------------
//
CMRMultiCalenField::~CMRMultiCalenField( )
    {
    FUNC_LOG;
    iCalenNameList.Close();
    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::InitializeL()
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::InitializeL()
    {
    TAknLayoutText text = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );

    iCalenName->SetFont( text.Font() );
    // This is called so that theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iCalenName->FocusChanged( EDrawNow );
        }
    }
// ---------------------------------------------------------------------------
// CMRMultiCalenField::InternalizeL
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRCalDbMgr& dbMgr = aEntry.GetDBMgr();
    iCalenNameList.Reset();

    dbMgr.GetMultiCalendarNameListL( iCalenNameList );
    TPtrC calenName = dbMgr.GetCalendarNameByEntryL( aEntry );
    iIndex = dbMgr.GetCurCalendarIndex();
    iCalenName->SetTextL( calenName );

    iEntry = &aEntry;

    // Set's the calendar type change enabled or disabled.
    if( FieldMode() == EESMRFieldModeEdit && !TypeChangeEnabledL() )
        {
        iTypeChangeEnabled = EFalse;

        LockL();
        }
    else
        {
        iTypeChangeEnabled = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus gained
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL( 
                EESMRCmdOpenMultiCalenSelectQuery, R_QTN_MSK_OPEN );
        }
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CMRMultiCalenField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;

    TBool isUsed( EFalse );
    if( aCommand == EESMRCmdOpenMultiCalenSelectQuery ||
       aCommand == EAknCmdOpen )
        {
        if( IsLocked() )
        	{
			HandleTactileFeedbackL();
			
        	CESMRGlobalNote::ExecuteL(
        			CESMRGlobalNote::EESMRUnableToEdit );
        	isUsed = ETrue;
			}
        		    	
		else if( iTypeChangeEnabled )
            {
			HandleTactileFeedbackL();
		
		    ExecuteTypeQueryL();
            isUsed = ETrue;
            }
        }

    return isUsed;
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::Lock
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::LockL()
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
	iIcon->SetParent( this );		
	}

// ---------------------------------------------------------------------------
// CMRMultiCalenField::ExecuteTypeQueryL
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::ExecuteTypeQueryL()
    {
    FUNC_LOG;
    if ( FieldMode() == EESMRFieldModeView )
    	return;

    TInt ret =
        CESMRListQuery::ExecuteL(
                CESMRListQuery::EESMRMultiCalenQuery,
                iCalenNameList );

    if ( ret != KErrCancel )
        {
        iIndex = ret;
        iEntry->GetDBMgr().SetCurCalendarByIndex( iIndex );

        // Send command to dialog
        NotifyEventAsyncL( EESMRCmdCalendarChange );
        UpdateLabelL( ret );
        }
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::CMRMultiCalenField
// ---------------------------------------------------------------------------
//
CMRMultiCalenField::CMRMultiCalenField( ) :
    iIndex( 0 )
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldCalendarName );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::ConstructL
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::ConstructL( )
    {
    FUNC_LOG;

    iCalenName = CMRLabel::NewL();
    CESMRField::ConstructL( iCalenName );
    iCalenName->SetTextL( KNullDesC() );

    TGulAlignment align;
    align.SetHAlignment( EHLeft );
    align.SetVAlignment( EVCenter );
    iCalenName->SetAlignment( align );

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapCalendarSelection );
    
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::UpdateLabelL
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::UpdateLabelL( TInt aIndex )
    {
    FUNC_LOG;

    iCalenName->SetTextL( iCalenNameList[aIndex] );
    iCalenName->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRMultiCalenField::CountComponentControls() const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iIcon )
    	{
    	++count;
    	}

    if ( iCalenName )
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
// CMRMultiCalenField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRMultiCalenField::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
    CCoeControl* control = NULL;
    switch( aIndex )
        {
        case 0:
            {
            control = iCalenName;
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
// CMRMultiCalenField::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );
    TAknLayoutRect iconLayout =
        NMRLayoutManager::GetLayoutRect(
                rect, NMRLayoutManager::EMRLayoutTextEditorIcon );
    TRect iconRect( iconLayout.Rect() );
    iIcon->SetRect( iconRect );
    
    // Layouting lock icon
    TAknLayoutRect rowLayoutRect( 
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    TRect rowRect( rowLayoutRect.Rect() );
    if( iLockIcon )
        {
        TAknWindowComponentLayout iconLayout( 
                NMRLayoutManager::GetWindowComponentLayout( 
                    NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
        AknLayoutUtils::LayoutImage( iLockIcon, rowRect, iconLayout );
        }

    // Layouting label
    TAknLayoutText viewerLayoutText;
    if( iLockIcon )
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect, 
    			NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
    	}
    else
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect, 
    			NMRLayoutManager::EMRTextLayoutTextEditor );
    	}

    TRect viewerRect( viewerLayoutText.TextRect() );    
    iCalenName->SetRect( viewerRect );
    
    // Move focus rect so that it's relative to field's position.
    viewerRect.Move( -Position() );
    
    TAknLayoutRect bgLayoutRect = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    
    // Adjust background rect according to viewerRect 
    bgRect.SetWidth( viewerRect.Width() );
    
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
        
    SetFocusRect( bgRect );
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::SetContainerWindowL(
        const CCoeControl& aContainer )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );
    iCalenName->SetContainerWindowL( aContainer );
    iCalenName->SetParent( this );
    iIcon->SetContainerWindowL( aContainer );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::SetTextDimmed
// ---------------------------------------------------------------------------
//
void CMRMultiCalenField::SetTextDimmed()
    {
    FUNC_LOG;
    
    NMRColorManager::SetColor(
            *iCalenName, 
            NMRColorManager::EMRMainAreaTextColorDimmed );
    }

// ---------------------------------------------------------------------------
// CMRMultiCalenField::TypeChangeEnabledL
// ---------------------------------------------------------------------------
//
TBool CMRMultiCalenField::TypeChangeEnabledL()
    {  
    TBool ret( ETrue );
    
    /*
     * Case 1:
     * If entry is single occurancy of a series, calendar type change  
     * is disabled for the user.
     */
    if( iEntry->IsRecurrentEventL() && 
            iEntry->RecurrenceModRule() == MESMRCalEntry::EESMRThisOnly &&
                FieldMode() == EESMRFieldModeEdit )
        {
        ret = EFalse;
        }
    
    /*
     * Case 2:
     * If entry is meeting request, and opened from mailbox that does not
     * support multiple calendar, calendar type change is disabled 
     * for the user.
     */
    if( ret && CCalenInterimUtils2::IsMeetingRequestL( iEntry->Entry() ) &&
            FieldMode() == EESMRFieldModeEdit )
        {
        MESMRMeetingRequestEntry* entry = 
            static_cast< MESMRMeetingRequestEntry* >( iEntry );
        
        // If entry's current plugin is active sync, it means that 
        // Mail For Exchange is in use. This means, that multiple calendar 
        // functionality is not supported.
        if( entry->CurrentPluginL() == EESMRActiveSync )
            {
            ret = EFalse;
            }
        }
    
    /*
     * Case 3:
	 * When the organizer of the MR edits an already sent MR, calendar 
	 * cannot be changed --> calendar selection needs to be locked.
     */
    if( ret && CCalenInterimUtils2::IsMeetingRequestL( iEntry->Entry() ) &&
            FieldMode() == EESMRFieldModeEdit )
    	{
		MESMRMeetingRequestEntry* entry = 
				static_cast< MESMRMeetingRequestEntry* >( iEntry );
		if( entry->RoleL() == EESMRRoleOrganizer && entry->IsSentL() )
			{
			ret = EFalse;    
			}
    	}
    
    return ret;
    }

// EOF

