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
* Description:  ESMR UI Container class
*
*/

#include "cesmrvieweralarmfield.h"
#include "cesmreditor.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"
#include "mesmrlistobserver.h"
#include "cesmrglobalnote.h"

#include <caluser.h>
#include <calalarm.h>
#include <calentry.h>
#include <StringLoader.h>
#include <AknLayout2ScalableDef.h>
#include <esmrgui.rsg>

// DEBUG
#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace{ //codescanner::namespace

const TInt KHourInMinutes(60);
const TInt KDayInMinutes(1440);

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmField* CESMRViewerAlarmField::NewL()
    {
    FUNC_LOG;
    CESMRViewerAlarmField* self = new (ELeave) CESMRViewerAlarmField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::~CESMRViewerAlarmField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmField::~CESMRViewerAlarmField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::CESMRViewerAlarmField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmField::CESMRViewerAlarmField()
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldAlarm );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAlarm );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.IsAllDayEventL() )
        {
        iObserver->HideControl( FieldId() );
        }
    else
        {
        CCalAlarm* alarm = aEntry.Entry().AlarmL();
        TInt minutes(0);
        HBufC* stringholder;
        if( alarm )
            {
            minutes = alarm->TimeOffset().Int();
            delete alarm;
            alarm = NULL;
            }
        else
            {
            stringholder = StringLoader::LoadLC(
                    R_QTN_MEET_REQ_ALARM_OFF , iEikonEnv );
            iLabel->SetTextL( *stringholder );
            CleanupStack::PopAndDestroy( stringholder );
            return;
            }

        // time units can be minutes, hours or days
        TInt timeUnits(0);

        // Alarm is one hour before
        if( minutes == KHourInMinutes )
            {
            timeUnits = minutes / KHourInMinutes;
            stringholder = StringLoader::LoadLC(
                    R_QTN_MEET_REQ_ALARM_HOUR , timeUnits, iEikonEnv );
            }
        // Alarm time is more than one hour and less than one day before
        else if( minutes > KHourInMinutes && minutes < KDayInMinutes )
            {
            timeUnits = minutes / KHourInMinutes;
            stringholder = StringLoader::LoadLC(
                    R_QTN_MEET_REQ_ALARM_HOURS , timeUnits, iEikonEnv );
            }
        // Alarm is several days before //
        else if( minutes >= KDayInMinutes )
            {
            timeUnits = minutes / KDayInMinutes;
            stringholder = StringLoader::LoadLC(
                    R_QTN_MEET_REQ_ALARM_DAYS , timeUnits, iEikonEnv );
            }
        // Alarm is minutes before //
        else
            {
            stringholder = StringLoader::LoadLC(
                    R_QTN_MEET_REQ_ALARM_MINUTES , minutes, iEikonEnv );
            }

        iLabel->SetTextL( *stringholder );
        CleanupStack::PopAndDestroy( stringholder );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmField::InitializeL()
    {
    FUNC_LOG;
    TAknLayoutText text = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );

    iLabel->SetFont( text.Font() );
    // This is called so that theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iLabel->FocusChanged( EDrawNow );
        }
    // No implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmField::SizeChanged()
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
    iLabel->SetRect( viewerRect );

    // Move focus rect so that it's relative to field's position.
    viewerRect.Move( -Position() );
    SetFocusRect( viewerRect );

    // Setting font also for the label
    iLabel->SetFont( viewerLayoutText.Font() );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAlarmField::CountComponentControls() const
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
    
    if ( iLockIcon )
    	{
    	++count;
    	}
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAlarmField::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
    switch ( aIndex )
        {
        case 0:
            return iIcon;
        case 1:
            return iLabel;
        case 2:
        	return iLockIcon;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmField::LockL()
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
// CESMRViewerAlarmField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerAlarmField::ExecuteGenericCommandL( TInt aCommand )
	{
    FUNC_LOG;
    
    TBool retValue( EFalse );
    
    if( (aCommand == EAknCmdOpen) && IsLocked()  )
    	{
		HandleTactileFeedbackL();
		
    	CESMRGlobalNote::ExecuteL(
    			CESMRGlobalNote::EESMRUnableToEdit );
    	retValue = ETrue;
    	}
    
    return retValue;
	}

// EOF
