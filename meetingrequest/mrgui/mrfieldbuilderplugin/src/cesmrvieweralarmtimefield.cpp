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

#include "cesmrvieweralarmtimefield.h"

#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "esmrfieldbuilderdef.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "cesmrglobalnote.h"

#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>

#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::CESMRViewerAlarmTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmTimeField::CESMRViewerAlarmTimeField()
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldAlarmTime );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::~CESMRViewerAlarmTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmTimeField::~CESMRViewerAlarmTimeField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::NewL
// ---------------------------------------------------------------------------
//
CESMRViewerAlarmTimeField* CESMRViewerAlarmTimeField::NewL()
    {
    FUNC_LOG;
    CESMRViewerAlarmTimeField* self = new (ELeave) CESMRViewerAlarmTimeField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAlarmClock, this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRCalEntry::TESMRAlarmType alarmType;
    aEntry.GetAlarmL( alarmType, iAlarmTime );

    if( alarmType == MESMRCalEntry::EESMRAlarmAbsolute )
        {
        FormatAlarmTimeL();
        }
    else // Remove the alarm fields
        {
        CCoeControl::MakeVisible( EFalse );
        iObserver->HideControl( EESMRFieldAlarmTime );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::SizeChanged()
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
// CESMRViewerAlarmTimeField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAlarmTimeField::CountComponentControls() const
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
// CESMRViewerAlarmTimeField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAlarmTimeField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerAlarmTimeField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerAlarmTimeField::ExecuteGenericCommandL( TInt aCommand )
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
// CESMRViewerAlarmTimeField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::LockL()
	{
	FUNC_LOG;
	if( IsLocked() )
		{
		return;
		}

	CESMRField::LockL();

	delete iLockIcon;
	iLockIcon = NULL;
	iLockIcon = CMRImage::NewL(
	        NMRBitmapManager::EMRBitmapLockField,
	        this,
	        ETrue );
	}

// ---------------------------------------------------------------------------
// CESMRViewerAlarmTimeField::FormatAlarmTimeL
// ---------------------------------------------------------------------------
//
void CESMRViewerAlarmTimeField::FormatAlarmTimeL()
    {
    FUNC_LOG;

    // Read format string from AVKON resource
    HBufC* dateFormatString =
        iEikonEnv->AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );
    HBufC* buf = HBufC::NewLC( KBufferLength );
    TPtr ptr( buf->Des() );

    iAlarmTime.FormatL( ptr, *dateFormatString );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    iLabel->SetTextL( ptr );
    CleanupStack::PopAndDestroy( 2, dateFormatString );
    }
// EOF

