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
* Description:  Synchronisation field for viewers
*
*/

#include "cesmrviewersyncfield.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "mesmrlistobserver.h"
#include "cesmrglobalnote.h"

#include <calentry.h>
#include <StringLoader.h>
#include <AknLayout2ScalableDef.h>

#include <esmrgui.rsg>
// DEBUG
#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerSyncField* CESMRViewerSyncField::NewL()
    {
    FUNC_LOG;
    CESMRViewerSyncField* self = new (ELeave) CESMRViewerSyncField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::~CESMRViewerSyncField()
// ---------------------------------------------------------------------------
//
CESMRViewerSyncField::~CESMRViewerSyncField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLockIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::CESMRViewerSyncField()
// ---------------------------------------------------------------------------
//
CESMRViewerSyncField::CESMRViewerSyncField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldSync );
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL( this );
    CESMRField::ConstructL( iLabel ); // ownership transfered

    iIcon = CMRImage::NewL(
            NMRBitmapManager::EMRBitmapSynchronization,
            this );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry();
    CCalEntry::TReplicationStatus rStatus = entry.ReplicationStatusL();

    HBufC* statusHolder = NULL;
    switch( rStatus )
        {
        case CCalEntry::ERestricted:
            {
            statusHolder = StringLoader::LoadLC(
                    R_QTN_CALENDAR_TASK_SYNC_NONE,
                    iCoeEnv );
            break;
            }
        case CCalEntry::EPrivate:
            {
            statusHolder = StringLoader::LoadLC(
                    R_QTN_CALENDAR_TASK_SYNC_PRIVATE,
                    iCoeEnv );
            break;
            }
        case CCalEntry::EOpen:
            {
            statusHolder = StringLoader::LoadLC(
                    R_QTN_CALENDAR_TASK_SYNC_PUBLIC,
                    iCoeEnv );
            break;
            }
        default: // There is no replication status set - hide field- this should never happen
            {
            iObserver->HideControl( iFieldId );
            }
        }

    if ( statusHolder )
        {
        iLabel->SetTextL( *statusHolder );
        CleanupStack::PopAndDestroy( statusHolder );
        }

    iRecorded = EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::InitializeL()
    {
    FUNC_LOG;
    // Setting Font for the rich text viewer
    TAknLayoutText text = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );

    iLabel->SetFont( text.Font() );

    // This is called so theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iLabel->FocusChanged( EDrawNow );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::SizeChanged()
    {
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
// CESMRViewerSyncField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerSyncField::CountComponentControls() const
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
// CESMRViewerSyncField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerSyncField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerSyncField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::SetOutlineFocusL( TBool aFocus )
    {
    CESMRField::SetOutlineFocusL ( aFocus );

    iLabel->SetFocus( aFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::LockL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::LockL()
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
// CESMRViewerSyncField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerSyncField::ExecuteGenericCommandL( TInt aCommand )
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

