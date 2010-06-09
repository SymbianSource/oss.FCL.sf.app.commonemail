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

#include "cesmrvieweralarmdatefield.h"

#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrcalentry.h"
#include "esmrfieldbuilderdef.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "cesmrglobalnote.h"

#include <caluser.h>
#include <eikenv.h>
#include <avkon.rsg>
#include <calalarm.h>
#include <calentry.h>

#include "emailtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::CESMRViewerAlarmDateField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmDateField::CESMRViewerAlarmDateField()
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldAlarmDate );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::~CESMRViewerAlarmDateField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmDateField::~CESMRViewerAlarmDateField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmDateField* CESMRViewerAlarmDateField::NewL()
    {
    CESMRViewerAlarmDateField* self = new (ELeave) CESMRViewerAlarmDateField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapDateStart );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::InternalizeL( MESMRCalEntry& aEntry )
    {
    MESMRCalEntry::TESMRAlarmType alarmType;
    aEntry.GetAlarmL( alarmType, iAlarmTime );

    if( alarmType == MESMRCalEntry::EESMRAlarmAbsolute )
        {
        FormatAlarmTimeL();
        }
    else // Remove the alarm fields
        {
        CCoeControl::MakeVisible(EFalse);
        iObserver->HideControl( EESMRFieldAlarmDate );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::SizeChanged()
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
    TAknTextComponentLayout viewerLayoutText;

    if( iLockIcon )
    	{
    	viewerLayoutText = NMRLayoutManager::GetTextComponentLayout(
    	        NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
    	}
    else
    	{
    	viewerLayoutText = NMRLayoutManager::GetTextComponentLayout(
    	        NMRLayoutManager::EMRTextLayoutTextEditor );
    	}

    AknLayoutUtils::LayoutLabel( iLabel, rect, viewerLayoutText );
    TRect viewerRect( iLabel->Rect() );

    // Move focus rect so that it's relative to field's position.
    viewerRect.Move( -Position() );
    SetFocusRect( viewerRect );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAlarmDateField::CountComponentControls() const
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
// CESMRViewerAlarmDateField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAlarmDateField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerAlarmDateField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerAlarmDateField::ExecuteGenericCommandL( TInt aCommand )
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

    if ( EMRCmdDoEnvironmentChange == aCommand )
        {
        FormatAlarmTimeL();
        retValue = ETrue;
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmDateField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::LockL()
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
// CESMRViewerAlarmDateField::FormatAlarmTimeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmDateField::FormatAlarmTimeL()
    {
    FUNC_LOG;

    // Read format string from AVKON resource
    HBufC* dateFormatString = iEikonEnv->AllocReadResourceLC(
            R_QTN_DATE_USUAL_WITH_ZERO );
    HBufC* buf = HBufC::NewLC( KBufferLength );
    TPtr ptr( buf->Des() );

    iAlarmTime.FormatL( ptr, *dateFormatString );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    iLabel->SetTextL( *buf );

    CleanupStack::PopAndDestroy( 2, dateFormatString );
    }


// EOF
