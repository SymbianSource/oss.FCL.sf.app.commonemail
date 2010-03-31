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
* Description:  Start date field for viewers
 *
*/

#include "cesmrviewerstartdatefield.h"

#include "cesmreditor.h"
#include "mesmrlistobserver.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "emailtrace.h"
#include "cesmrglobalnote.h"
#include <esmrgui.rsg>

#include <eiklabel.h>
#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <stringloader.h>
#include <aknsconstants.h>
#include <aknutils.h>
#include <aknlayout2scalabledef.h>

// ======== LOCAL FUNCTIONS ========
namespace // codescanner::namespace
    {
    const TInt KComponentCount( 2 ); // icon and label
    const TInt KMaxTimeBuffer( 32 ); // buffer for date formatting
    } // unnamed namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::CESMRViewerStartDateField()
// -----------------------------------------------------------------------------
//
CESMRViewerStartDateField::CESMRViewerStartDateField()
    {
    FUNC_LOG;
    
    SetFieldId ( EESMRFieldStartDate );
    SetFocusType( EESMRHighlightFocus );
    }

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::~CESMRViewerStartDateField()
// -----------------------------------------------------------------------------
//
CESMRViewerStartDateField::~CESMRViewerStartDateField()
    {
    FUNC_LOG;
    
    delete iIcon;
    delete iLockIcon;
    }

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::NewL()
// -----------------------------------------------------------------------------
//
CESMRViewerStartDateField* CESMRViewerStartDateField::NewL()
    {
    FUNC_LOG;
    
    CESMRViewerStartDateField* self = 
            new (ELeave) CESMRViewerStartDateField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::ConstructL()
// -----------------------------------------------------------------------------
//
void CESMRViewerStartDateField::ConstructL()
    {
    FUNC_LOG;
    
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapDateStart );
    iIcon->SetParent( this );
    }

// -----------------------------------------------------------------------------
// CESMRViewerStartDateField::InternalizeL()
// -----------------------------------------------------------------------------
//
void CESMRViewerStartDateField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    CCalEntry& entry = aEntry.Entry();    
    iStartTime = entry.StartTimeL().TimeLocalL();

    FormatDateStringL();
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerStartDateField::SizeChanged()
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

    // Layouting viewer field
    TRect viewerRect( viewerLayoutText.TextRect() );    
    iLabel->SetRect( viewerRect );

    // Move focus rect so that it's relative to field's position.
    viewerRect.Move( -Position() );
    SetFocusRect( viewerRect );

    // Setting font also for the label
    iLabel->SetFont( viewerLayoutText.Font() );
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerStartDateField::CountComponentControls() const
    {
    FUNC_LOG;
    
    TInt count( KComponentCount );
    if( iLockIcon )
    	{
    	++count;
    	}
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerStartDateField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerStartDateField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStartDateField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    
    CESMRField::SetOutlineFocusL ( aFocus );
    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerStartDateField::ExecuteGenericCommandL( TInt aCommand )
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
        FormatDateStringL();
        retValue = ETrue;
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerStartDateField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStartDateField::LockL()
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
// CESMRViewerStartDateField::FormatDateStringL()
// ---------------------------------------------------------------------------
//
void CESMRViewerStartDateField::FormatDateStringL()
    {
    FUNC_LOG;
    
    // Read format string from AVKON resource
    HBufC* dateFormatString = 
            iEikonEnv->AllocReadResourceLC(
                    R_QTN_DATE_USUAL_WITH_ZERO);
    
    TBuf<KMaxTimeBuffer> buf;
    iStartTime.FormatL(buf, *dateFormatString);

    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( buf );
    iLabel->SetTextL( buf );
    CleanupStack::PopAndDestroy( dateFormatString );    
    }

// EOF
