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


#include "cesmrvieweralldayeventfield.h"
#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "cesmrglobalnote.h"

#include <caluser.h>
#include <calalarm.h>
#include <calentry.h>
#include <stringloader.h>
#include <aknlayout2scalabledef.h>
#include <esmrgui.rsg>

#include "emailtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerAllDayEventField* CESMRViewerAllDayEventField::NewL()
    {
    FUNC_LOG;
    CESMRViewerAllDayEventField* self = new (ELeave) CESMRViewerAllDayEventField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::~CESMRViewerAllDayEventField()
// ---------------------------------------------------------------------------
//
CESMRViewerAllDayEventField::~CESMRViewerAllDayEventField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::CESMRViewerAllDayEventField()
// ---------------------------------------------------------------------------
//
CESMRViewerAllDayEventField::CESMRViewerAllDayEventField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldAllDayEvent );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapCheckBoxOn );
    iIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.IsAllDayEventL() )
        {
        HBufC* allDayEventText =
            StringLoader::LoadLC( R_QTN_MEET_REQ_ALL_DAY_EVENT, iEikonEnv );
        iLabel->SetTextL( *allDayEventText );
        CleanupStack::PopAndDestroy( allDayEventText );
        }
    else
        {
        // set the field as hidden:
        MakeVisible( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::InitializeL()
    {
    // No implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::SizeChanged()
    {
    TRect rect = Rect();
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = rowLayoutRect.Rect();

    TAknWindowComponentLayout iconLayout =
        NMRLayoutManager::GetWindowComponentLayout( NMRLayoutManager::EMRLayoutTextEditorIcon );
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
    }

// ---------------------------------------------------------------------------
// CESMRViewerAllDayEventField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAllDayEventField::CountComponentControls() const
    {
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
// CESMRViewerAllDayEventField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAllDayEventField::ComponentControl( TInt aIndex ) const
    {
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
// CESMRViewerStartDateField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::SetOutlineFocusL( TBool aFocus )
    {
    CESMRField::SetOutlineFocusL ( aFocus );

    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAllDayEventField::LockL()
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
// CESMRViewerStartDateField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerAllDayEventField::ExecuteGenericCommandL( TInt aCommand )
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
