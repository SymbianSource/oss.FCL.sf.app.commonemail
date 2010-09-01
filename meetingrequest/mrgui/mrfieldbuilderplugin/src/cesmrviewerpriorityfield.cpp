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
* Description:  Priority field for viewers
 *
*/

#include "cesmrviewerpriorityfield.h"

#include "esmrdef.h"
#include "mesmrlistobserver.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cesmrglobalnote.h"

#include <calentry.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <esmrgui.rsg>

// DEBUG
#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerPriorityField* CESMRViewerPriorityField::NewL()
    {
    FUNC_LOG;
    CESMRViewerPriorityField* self = new (ELeave) CESMRViewerPriorityField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::~CESMRViewerPriorityField()
// ---------------------------------------------------------------------------
//
CESMRViewerPriorityField::~CESMRViewerPriorityField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::CESMRViewerPriorityField()
// ---------------------------------------------------------------------------
//
CESMRViewerPriorityField::CESMRViewerPriorityField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldPriority );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerPriorityField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapPriorityNormal, this );
    
    HBufC* priorityText = StringLoader::LoadLC(
                        R_QTN_CALENDAR_MEETING_OPT_PRIORITY_NORMAL,
                        iEikonEnv );
    iLabel->SetTextL( *priorityText );
    CleanupStack::PopAndDestroy( priorityText );
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerPriorityField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    // Get the priority
    TInt priority = aEntry.GetPriorityL();

    HBufC* priorityText;
    TInt resourceId(KErrNotFound);
    NMRBitmapManager::TMRBitmapId bitmapId;

    if ( aEntry.Type() == MESMRCalEntry::EESMRCalEntryTodo )
        {
        switch( priority )
            {
            case EFSCalenTodoPriorityLow:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_LOW;
                bitmapId = NMRBitmapManager::EMRBitmapPriorityLow;
                break;
                }
            case EFSCalenTodoPriorityHigh:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_HIGH;
                bitmapId = NMRBitmapManager::EMRBitmapPriorityHigh;
                break;
                }
            default:
                {
                // Do nothing, priority normal remains
                break;
                }
            }
        }
    else
        {
        switch( priority )
            {
            case EFSCalenMRPriorityLow:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_LOW;
                bitmapId = NMRBitmapManager::EMRBitmapPriorityLow;
                break;
                }
            case EFSCalenMRPriorityHigh:
                {
                resourceId = R_QTN_CALENDAR_MEETING_OPT_PRIORITY_HIGH;
                bitmapId = NMRBitmapManager::EMRBitmapPriorityHigh;
                break;
                }
            default:
                {
                // Do nothing, priority normal remains
                break;
                }
            }
        }

    // if not changed, the default set in ConstructL will be used:
    if ( resourceId != KErrNotFound )
        {
        priorityText = StringLoader::LoadLC( resourceId,
                                             iEikonEnv );
        iLabel->SetTextL( *priorityText );
        CleanupStack::PopAndDestroy( priorityText );

        delete iIcon;
        iIcon = NULL;
        iIcon = CMRImage::NewL( bitmapId, this );
        
        // This needs to be called so icon will be redrawn
        SizeChanged();
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerPriorityField::SizeChanged()
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
// CESMRViewerPriorityField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerPriorityField::CountComponentControls() const
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
// CESMRViewerPriorityField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerPriorityField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerPriorityField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerPriorityField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerPriorityField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerPriorityField::LockL()
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
// CESMRViewerPriorityField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerPriorityField::ExecuteGenericCommandL( TInt aCommand )
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

