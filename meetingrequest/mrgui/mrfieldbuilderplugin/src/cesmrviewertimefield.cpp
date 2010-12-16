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
* Description:  Time field for viewers
*
*/

#include "cesmrviewertimefield.h"
#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "nmrbitmapmanager.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "cesmrglobalnote.h"

#include <eiklabel.h>
#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <StringLoader.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <AknLayout2ScalableDef.h>
#include <esmrgui.rsg>
// DEBUG
#include "emailtrace.h"

//unnamed namespace for local constants functions
namespace{ // codescanner::namespace

const TInt KTimeBufferSize( 50 );

const TInt KTempBufferSize( 20 );

// Literal for start and end time separator
_LIT(KTimeSeparator," - ");

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::CESMRViewerTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerTimeField::CESMRViewerTimeField()
    {
    FUNC_LOG;

    SetFieldId( EESMRFieldMeetingTime );
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::~CESMRViewerTimeField()
// ---------------------------------------------------------------------------
//
CESMRViewerTimeField::~CESMRViewerTimeField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLockIcon;
    }


// ---------------------------------------------------------------------------
// CESMRViewerTimeField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerTimeField* CESMRViewerTimeField::NewL()
    {
    FUNC_LOG;
    CESMRViewerTimeField* self = new (ELeave) CESMRViewerTimeField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::ConstructL()
    {
    FUNC_LOG;

    iLabel = CMRLabel::NewL( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapClock, this );

    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    CCalEntry& entry = aEntry.Entry();

    iStartTime = entry.StartTimeL().TimeLocalL();
    iEndTime   = entry.EndTimeL().TimeLocalL();

    if ( aEntry.IsAllDayEventL() )
        {
        // set the field as hidden:
        iObserver->HideControl( FieldId() );
        }
    else
        {
        FormatTimeFieldStringL();
        }

    iRecorded = EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::InitializeL()
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
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::SizeChanged()
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
// CESMRViewerTimeField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerTimeField::CountComponentControls() const
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
// CESMRViewerTimeField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerTimeField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerTimeField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;

    CESMRField::SetOutlineFocusL ( aFocus );

    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerTimeField::ExecuteGenericCommandL( TInt aCommand )
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
        FormatTimeFieldStringL();
        retValue = ETrue;
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerTimeField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::LockL()
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
// CESMRViewerTimeField::FormatTimeFieldStringL()
// ---------------------------------------------------------------------------
//
void CESMRViewerTimeField::FormatTimeFieldStringL()
    {
    HBufC* timeFormatString =
        iEikonEnv->AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );

    HBufC* finalBuf = HBufC::NewLC( KTimeBufferSize );
    HBufC* startBuf = HBufC::NewLC( KTempBufferSize );
    HBufC* endBuf = HBufC::NewLC( KTempBufferSize );
    const TInt KNumBuffers( 4 );

    TPtr startPtr( startBuf->Des() );
    iStartTime.FormatL( startPtr, *timeFormatString );
    TPtr endPtr( endBuf->Des() );
    iEndTime.FormatL( endPtr, *timeFormatString );
    TPtr finalPtr( finalBuf->Des() );
    finalPtr.Append( startPtr );
    finalPtr.Append( KTimeSeparator );
    finalPtr.Append( endPtr );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( finalPtr );
    iLabel->SetTextL( finalPtr );
    CleanupStack::PopAndDestroy( KNumBuffers, timeFormatString );

    }

// EOF
