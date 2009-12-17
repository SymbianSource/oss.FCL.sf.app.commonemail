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
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::~CESMRViewerDetailedSubjectField()
// ---------------------------------------------------------------------------
//
CESMRViewerDetailedSubjectField::~CESMRViewerDetailedSubjectField()
    {
    FUNC_LOG;
    iObserver = NULL;
    delete iRichTextViewer;
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
    SetFocusType( EESMRHighlightFocus );
    
    iFieldIcon = CMRImage::NewL( KAknsIIDQgnFscalIndiOccasion );
    iFieldIcon->SetParent( this );
    
    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    iRichTextViewer->SetEdwinSizeObserver( this );
    iRichTextViewer->SetParent( this );
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
    // Let's calculate the required rect of the iRichTextViewer.
    // We will not use directly the iRichTextViewer height, because it might
    // not exist, or the height of the viewer might still be incorrect
    TRect richTextViewerRect = RichTextViewerRect();

    // We will use as minimum size the parents width 
    // but the calculated iRichTextViewers height 
    return TSize( Parent()->Size().iWidth, richTextViewerRect.Height() );
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
    
    iRichTextViewer->SetFontL( text.Font(), iLayout );
        
    // This is called so theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iRichTextViewer->FocusChanged( EDrawNow );
        }
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
        // After setting the text, the viewer line count is known
        iLineCount = iRichTextViewer->LineCount();
        }
    else
        {
        iRichTextViewer->SetTextL( unnamed, ETrue );
        // After setting the text, the viewer line count is known
        iLineCount = iRichTextViewer->LineCount();
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

    // if the entry is anniversary let's change the icon.
    if ( CCalEntry::EAnniv  != aEntry.Entry().EntryTypeL() )
        {
        delete iFieldIcon;
        iFieldIcon = NULL;
        
        iFieldIcon = CMRImage::NewL( KAknsIIDQgnFscalIndiSubject );
        iFieldIcon->SetParent( this );
        
        DrawDeferred();
        }
    iDisableRedraw = ETrue;
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
void CESMRViewerDetailedSubjectField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if (aCommand == EESMRCmdClipboardCopy)
        {
        iRichTextViewer->CopyCurrentLinkToClipBoardL();
        }
    else if ( aCommand == EAknSoftkeySelect )
        {
        iRichTextViewer->LinkSelectedL();
        }        
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
    if ( !aFocus )
        {
        //need to tell action menu that focus has changed
        iRichTextViewer->ResetActionMenuL();
        }
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
    TRect rect = Rect();

    // LAYOUTING FIELD ICON
    if( iFieldIcon )
        {
        TAknWindowComponentLayout iconLayout = 
            NMRLayoutManager::GetWindowComponentLayout( 
                    NMRLayoutManager::EMRLayoutTextEditorIcon );
        AknLayoutUtils::LayoutImage( iFieldIcon, rect, iconLayout );
        }

    // LAYOUTING FIELD BACKGROUND
    TAknLayoutRect bgLayoutRect =
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    
    // LAYOUTING FIELD TEXT VIEWER
    if( iRichTextViewer )
        {
        iRichTextViewer->SetRect( RichTextViewerRect() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRViewerDetailedSubjectField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iRichTextViewer->SetContainerWindowL( aContainer );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDetailedSubjectField::RichTextViewerRect
// ---------------------------------------------------------------------------
//
TRect CESMRViewerDetailedSubjectField::RichTextViewerRect()
    {
    TRect rect = Rect();
    
    TAknTextComponentLayout edwinLayout = NMRLayoutManager::GetTextComponentLayout( 
            NMRLayoutManager::EMRTextLayoutTextEditor );

    // Text layout rect for one line viewer
    TAknLayoutText textLayout;
    textLayout.LayoutText( rect, edwinLayout );
    TRect textLayoutRect = textLayout.TextRect();

    TRect viewerRect = textLayoutRect;
    
    // If iRichTextViewer has lines and iSize has been set, 
    // we will use iSize.iHeight as the viewers height
    if( iLineCount > 0 && iSize.iHeight > 0 )
        {
        viewerRect.SetHeight( iSize.iHeight );
        }
    // Otherwise we will use one row height as the height of the 
    // iRichTextViewer
    else
        {
        TAknLayoutRect rowLayoutRect = 
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
        viewerRect.SetHeight( rowLayoutRect.Rect().Height() );
        }
   
    return viewerRect;
    }

//EOF
