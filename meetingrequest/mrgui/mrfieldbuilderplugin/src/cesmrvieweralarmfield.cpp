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
* Description:  ESMR UI Container class
*
*/

#include "cesmrvieweralarmfield.h"
#include "cesmreditor.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"

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
    delete iLabel;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmField::CESMRViewerAlarmField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmField::CESMRViewerAlarmField()
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldAlarm );
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
    iIcon = CMRImage::NewL( KAknsIIDQgnMeetReqIndiAlarm );
    iIcon->SetParent( this );
    
    SetFocusType( EESMRHighlightFocus );
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
        iObserver->RemoveControl( FieldId() );
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

    AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                             KRgbBlack );
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

    if ( !aFocus )
        {
        AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                                 KRgbBlack );
        }
    }

// EOF
