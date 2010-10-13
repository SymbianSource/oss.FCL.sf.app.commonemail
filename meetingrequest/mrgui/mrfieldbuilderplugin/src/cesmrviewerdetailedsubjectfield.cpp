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
* Description:  Detailed subject field
 *
*/

#include "cesmrviewerdetailedsubjectfield.h"
#include "cesmrrichtextviewer.h"
#include "cesmrrichtextlink.h"
#include "mesmrlistobserver.h"
#include "mesmrtitlepaneobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cesmrglobalnote.h"

#include <esmrgui.rsg>
#include <StringLoader.h>
#include <calentry.h>
#include <AknLayout2ScalableDef.h>

// DEBUG
#include "emailtrace.h"

// Unnamed namespace for local definitions and functions
namespace { // codescanner::namespace
/**
 * Adds forward prefix to calendar (meeting request) entry.
 * @param aEntry Reference to calendar entry.
 */
void AddForwardPrefixToEntryL(
        CCalEntry& aEntry )
    {
    TPtrC summary( aEntry.SummaryL() );

    // loading 'FW:' prefix from resource
    HBufC* fwPrefix =
            StringLoader::LoadLC(
                    R_QTN_MEET_REQ_PLAIN_TEXT_FORWARDED );

    HBufC* subjectBuffer = HBufC::NewLC(
                    fwPrefix->Length() + summary.Length() );
    TPtr subject( subjectBuffer->Des() );

    // Construc 'Fw: <summary>'
    subject.Copy( *fwPrefix );
    subject.Append( summary );

    aEntry.SetSummaryL( subject );

    CleanupStack::PopAndDestroy( subjectBuffer );
    subjectBuffer = NULL;

    CleanupStack::PopAndDestroy( fwPrefix );
    fwPrefix = NULL;
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::CESMRViewerDetailedSubjectField()
// ---------------------------------------------------------------------------
//
CESMRViewerDetailedSubjectField::CESMRViewerDetailedSubjectField()
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldDetailedSubject );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::~CESMRViewerDetailedSubjectField()
// ---------------------------------------------------------------------------
//
CESMRViewerDetailedSubjectField::~CESMRViewerDetailedSubjectField()
    {
    FUNC_LOG;
    iObserver = NULL;
    delete iFieldIcon;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerDetailedSubjectField* CESMRViewerDetailedSubjectField::NewL()
    {
    FUNC_LOG;
    CESMRViewerDetailedSubjectField* self =
            new( ELeave )CESMRViewerDetailedSubjectField;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::ConstructL()
    {
    FUNC_LOG;
    iFieldIcon = CMRImage::NewL(
            NMRBitmapManager::EMRBitmapOccasion,
            this );
	iPriorityIcon = CMRImage::NewL(
			NMRBitmapManager::EMRBitmapLockField,
			this );
    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    CESMRField::ConstructL( iRichTextViewer ); // ownership transferred
    iRichTextViewer->SetEdwinSizeObserver( this );
    iRichTextViewer->SetParent( this );
    iRichTextViewer->SetLinkObserver( this );
    iCurrentPriority = EFSCalenMRPriorityNormal;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::SetTitlePaneObserver
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::SetTitlePaneObserver(
        MESMRTitlePaneObserver* aObserver )
    {
    FUNC_LOG;
    iTitlePaneObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRViewerDetailedSubjectField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );

    TRect fieldRect =
       NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();

    TRect viewerRect( NMRLayoutManager::GetLayoutText(
       fieldRect,
       NMRLayoutManager::EMRTextLayoutTextEditor ).TextRect() );

    // Adjust field size so that there's room for expandable editor.
    fieldRect.Resize( 0, iSize.iHeight - viewerRect.Height() );

    return fieldRect.Size();
    }


// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::InitializeL()
    {
    FUNC_LOG;
    // Setting Font for the rich text viewer
    TAknLayoutText text = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );

    iRichTextViewer->SetFontL( text.Font() );
    iRichTextViewer->ApplyLayoutChangesL();

    // This is called so theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iRichTextViewer->FocusChanged( EDrawNow );
        }
    iRichTextViewer->SetEventQueue( iEventQueue );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::InternalizeL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRCalEntry::TESMRCalEntryType entryType(
            aEntry.Type() );

    CCalEntry& entry = aEntry.Entry();

    if( !IsLocked() )
    	{
    	if( entry.PriorityL() == EFSCalenMRPriorityHigh )
    		{
			if( iPriorityIcon )
				{
				delete iPriorityIcon;
				iPriorityIcon = NULL;			
				}
    		iPriorityIcon = CMRImage::NewL(
    				NMRBitmapManager::EMRBitmapPriorityHigh,
    				this,
    				ETrue );
    		iCurrentPriority = EFSCalenMRPriorityHigh;
    		}
    	if( entry.PriorityL() == EFSCalenMRPriorityLow )
    		{
			if( iPriorityIcon )
				{
				delete iPriorityIcon;
				iPriorityIcon = NULL;			
				}
    		iPriorityIcon = CMRImage::NewL(
    				NMRBitmapManager::EMRBitmapPriorityLow,
    				this,
    				ETrue );
    		iCurrentPriority = EFSCalenMRPriorityLow;
    		}
    	}


    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == entryType )
        {
        MESMRMeetingRequestEntry* mrEntry =
                static_cast<MESMRMeetingRequestEntry*>(&aEntry);

        if ( mrEntry->IsForwardedL() )
            {
            AddForwardPrefixToEntryL( entry );

            if ( iTitlePaneObserver )
                {
                iTitlePaneObserver->UpdateTitlePaneTextL(
                        entry.SummaryL() );
                }
            }
        }

    HBufC* unnamed = StringLoader::LoadLC( R_QTN_MEET_REQ_CONFLICT_UNNAMED );

    TPtrC summary( entry.SummaryL() );
    HBufC* subject = summary.AllocLC();

    if ( summary.Length() )
        {
        iRichTextViewer->SetTextL( subject, ETrue );
        }
    else
        {
        iRichTextViewer->SetTextL( unnamed, ETrue );
        }
    CleanupStack::PopAndDestroy( subject );
    subject = NULL;

    if ( iTitlePaneObserver && aEntry.IsStoredL() )
        {
        if ( summary.Length() )
            {
            iTitlePaneObserver->UpdateTitlePaneTextL( summary );
            }
        else
            {
            iTitlePaneObserver->UpdateTitlePaneTextL( *unnamed );
            }
        }

    CleanupStack::PopAndDestroy( unnamed );
    unnamed = NULL;

    // if the entry isn't anniversary let's change the icon.
    if ( CCalEntry::EAnniv  != aEntry.Entry().EntryTypeL() )
        {
        delete iFieldIcon;
        iFieldIcon = NULL;

        iFieldIcon = CMRImage::NewL(
                NMRBitmapManager::EMRBitmapSubject,
                this );
        }
    
    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRViewerDetailedSubjectField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    return iRichTextViewer->OfferKeyEventL ( aEvent, aType );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::HandleEdwinSizeEventL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDetailedSubjectField::HandleEdwinSizeEventL(
        CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aType*/,
        TSize aSize )
    {
    FUNC_LOG;
    TBool reDraw( EFalse );

    // Let's save the required size for the iRichTextViewer
    iSize = aSize;

    if ( iObserver && aEdwin == iRichTextViewer )
       {
       iObserver->ControlSizeChanged( this );
       
       if ( !iOutlineFocus )
           {
           RecordField();
           }
       
       reDraw = ETrue;
       }

    return reDraw;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::ListObserverSet()
    {
    FUNC_LOG;
    iRichTextViewer->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDetailedSubjectField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    if (aCommand == EESMRCmdClipboardCopy)
        {
        iRichTextViewer->CopyCurrentLinkToClipBoardL();
        isUsed = ETrue;
        }
    else if ( aCommand == EAknSoftkeySelect )
        {
        iRichTextViewer->LinkSelectedL();
        isUsed = ETrue;
        }
    else if ( aCommand == EAknCmdOpen )
    	{
    	if ( IsLocked() )
    		{
			HandleTactileFeedbackL();

    		CESMRGlobalNote::ExecuteL(
    				CESMRGlobalNote::EESMRUnableToEdit );
    		isUsed = ETrue;
    		}
    	}
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    iRichTextViewer->SetFocus( aFocus );

    if( aFocus )
        {
        SetMiddleSoftkeyL();
        }

    else
        {
        //need to tell action menu that focus has changed
        iRichTextViewer->ResetActionMenuL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::GetCursorLineVerticalPos
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::GetCursorLineVerticalPos(
        TInt& aUpper,
        TInt& aLower )
    {
    FUNC_LOG;

    aLower = iRichTextViewer->CurrentLineNumber() * iRichTextViewer->RowHeight();
    aUpper = aLower - iRichTextViewer->RowHeight();
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::HandleLongtapEventL(
        const TPoint& aPosition )
    {
    FUNC_LOG;

    if ( iRichTextViewer->Rect().Contains( aPosition ) )
        {
        iRichTextViewer->LinkSelectedL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::LockL
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::LockL()
	{
	FUNC_LOG;
	if( IsLocked() )
		{
		return;
		}

	CESMRField::LockL();

	if( !iPriorityIcon )
		{
		delete iPriorityIcon;
		iPriorityIcon = NULL;
		iPriorityIcon = CMRImage::NewL(
				NMRBitmapManager::EMRBitmapLockField,
				this,
				ETrue );
		}
	}

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::SetMiddleSoftkeyL
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::SetMiddleSoftkeyL()
    {
    FUNC_LOG;
    const CESMRRichTextLink* link = iRichTextViewer->GetSelectedLink();
    if( link )
        {
        if ( ( link->Type() == CESMRRichTextLink::ETypeURL ) ||
                ( link->Type() == CESMRRichTextLink::ETypeEmail ) ||
                ( link->Type() == CESMRRichTextLink::ETypePhoneNumber ) )
            {
            SetMiddleSoftKeyVisible( ETrue );
            }
        else
            {
            RestoreMiddleSoftKeyL();
            }
        }
    }


// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::HandleRichTextLinkSelection
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDetailedSubjectField::HandleRichTextLinkSelection(
        const CESMRRichTextLink* /*aLink*/ )
    {
    TBool result = EFalse;

    // No implementation yet: waiting for UI spec.

    return result;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRViewerDetailedSubjectField::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iFieldIcon )
        {
        ++count;
        }

    if ( iPriorityIcon )
        {
        ++count;
        }

    if ( iRichTextViewer )
        {
        ++count;
        }

    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerDetailedSubjectField::ComponentControl(
        TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iFieldIcon;
        case 1:
            return iPriorityIcon;
        case 2:
            return iRichTextViewer;

        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::SizeChanged( )
    {
    // Store iRichTextViewer original width.
    TInt richTextViewerWidth = iRichTextViewer->Size().iWidth;
    TRect rect( Rect() );

    TAknLayoutRect rowLayoutRect(
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    TRect rowRect( rowLayoutRect.Rect() );

    // Layouting field icon
    if( iFieldIcon )
        {
        TAknWindowComponentLayout iconLayout(
            NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutSingleRowAColumnGraphic ) );
        AknLayoutUtils::LayoutControl( iFieldIcon, rowRect, iconLayout );
        }

    // Layouting priority icon
    if( iPriorityIcon && (  IsLocked() || iCurrentPriority != EFSCalenMRPriorityNormal ) )
        {
        TAknWindowComponentLayout iconLayout(
                NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
        AknLayoutUtils::LayoutImage( iPriorityIcon, rowRect, iconLayout );
        }

    TAknLayoutText viewerLayoutText;

    if( iPriorityIcon && (  IsLocked() || iCurrentPriority != EFSCalenMRPriorityNormal ) )
        {
        viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect,
                    NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
        }
    else
        {
        viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect,
                    NMRLayoutManager::EMRTextLayoutTextEditor );
        }

    // Layouting viewer field
    TRect viewerRect( viewerLayoutText.TextRect() );

    // Resize height according to actual height required by edwin.
    viewerRect.Resize( 0, iSize.iHeight - viewerRect.Height() );
    iRichTextViewer->SetRect( viewerRect );

    // Layouting focus
    TRect bgRect( viewerRect );

    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );

    // Failures are ignored.
    TRAP_IGNORE(
            // Try setting font
            iRichTextViewer->SetFontL( viewerLayoutText.Font() );
            // Try applying changes
            iRichTextViewer->ApplyLayoutChangesL();
            );

    if ( iRichTextViewer->Size().iWidth != richTextViewerWidth )
        {
        // Most of this case is screen orientation, in this case we need to
        // Record the index of focusing link, after updating link array, then
        // reset the focusing to original one.
        TInt focusingIndex = iRichTextViewer->GetFocusLink();
        if ( KErrNotFound != focusingIndex )
            {
            iRichTextViewer->SetFocusLink( focusingIndex );
            //wake up current contact menu selection by calling this
            iRichTextViewer->FocusChanged(ENoDrawNow);
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::SupportsLongTapFunctionalityL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDetailedSubjectField::SupportsLongTapFunctionalityL(
		const TPointerEvent &aPointerEvent )
	{
    FUNC_LOG;
    TBool ret( EFalse );

    if ( iRichTextViewer->Rect().Contains( aPointerEvent.iPosition ) )
        {
        if( iRichTextViewer->PointerEventOccuresOnALinkL( aPointerEvent ) )
        	{
			ret = ETrue;
        	}
        }

    return ret;
	}

//EOF
