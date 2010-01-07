/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Meeting request from (organizer) field
 *
*/

#include "cesmrviewerfromfield.h"
#include "cesmrrichtextviewer.h"
#include "cesmrrichtextlink.h"
#include "mesmrlistobserver.h"
#include "esmrfieldbuilderdef.h"
#include "cesmrlayoutmgr.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"

#include <calentry.h>
#include <caluser.h>
#include <StringLoader.h>
#include <eikenv.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <esmrgui.rsg>

// DEBUG
#include "emailtrace.h"

/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

const TInt KMargin = 5;

} // namespace

using namespace ESMRLayout;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerFromField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerFromField* CESMRViewerFromField::NewL( )
    {
    FUNC_LOG;
    CESMRViewerFromField* self = new (ELeave) CESMRViewerFromField;
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop (self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::~CESMRViewerFromField()
// ---------------------------------------------------------------------------
//
CESMRViewerFromField::~CESMRViewerFromField( )
    {
    FUNC_LOG;
    delete iFieldTopic;
    delete iRichTextViewer;
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerFromField::InitializeL()
    {
    FUNC_LOG;
    TAknLayoutText editorRect = 
    NMRLayoutManager::GetLayoutText( 
            Rect(), NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    iRichTextViewer->SetFontL( editorRect.Font(), iLayout );
    const CFont* topicFont = 
        AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont, NULL );
    iFieldTopic->SetFont( topicFont );
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerFromField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalUser* organizer = aEntry.Entry().OrganizerL();
    if (organizer )
        {
        TPtrC text = organizer->CommonName();
        if (text.Length() == 0 )
            {
            text.Set (organizer->Address() );
            }
        iRichTextViewer->SetTextL( &text );
        iRichTextViewer->SetMargins( KMargin );

        TPtrC address = organizer->Address();
        if ( address.Length() > 0 )
            {
            CESMRRichTextLink* link = CESMRRichTextLink::NewL( 0,
                    text.Length(), address, CESMRRichTextLink::ETypeEmail,
                    CESMRRichTextLink::ETriggerKeyRight );
            CleanupStack::PushL( link );
            iRichTextViewer->AddLinkL( link );
            CleanupStack::Pop( link );
            }

        // Set the topic text
        HBufC* stringholder = 
            StringLoader::LoadLC( R_QTN_MEET_REQ_LABEL_FROM, iEikonEnv );
        iFieldTopic->SetTextL( *stringholder );
        CleanupStack::PopAndDestroy( stringholder );

        //wake up current contact menu selection by calling this
        iRichTextViewer->FocusChanged( ENoDrawNow );
        SizeChanged( );
        }
    iDisableRedraw = ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerFromField::SizeChanged( )
    {
    FUNC_LOG;
    TRect rect( Rect() );
    
    TAknLayoutRect row1LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = row1LayoutRect.Rect();
    
    TAknTextComponentLayout labelLayout =
        NMRLayoutManager::GetTextComponentLayout( 
                NMRLayoutManager::EMRTextLayoutText );
    AknLayoutUtils::LayoutLabel( iFieldTopic, rect, labelLayout );
    // Color should be overrided after layouting
    // If this function leaves we'll have to use default color
    TRAP_IGNORE( AknLayoutUtils::OverrideControlColorL( 
                                *iFieldTopic, 
                                EColorLabelText,
                                KRgbBlack ));    

    rect = Rect();
    TAknLayoutRect row2LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 2 );
    rect = row2LayoutRect.Rect();

    // Move the iY down the height of the title field
    TInt moveHeight = row1LayoutRect.Rect().Height();
    rect.Move( 0, moveHeight );

    TAknLayoutText editorRect = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    // Count the rect for the richtextviewer. It will be placed into the 
    // top left corner of the editorRect and size is calculated from 
    // viewer's real size iExpandedSize and editorRect's width.
    TPoint tlPoint( 
            editorRect.TextRect().iTl.iX, editorRect.TextRect().iTl.iY );
    TSize prefSize( editorRect.TextRect().Width(), iExpandedSize.iHeight );
    TRect viewerRect( tlPoint, prefSize );
    iRichTextViewer->SetRect( viewerRect );
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::MinimumSize()
// ---------------------------------------------------------------------------
//
TSize CESMRViewerFromField::MinimumSize()
    {    
    // Minimum size ->  Height: TitleRow + Editor size + Margin
    //                  Width: Parent's Width 
    //                   (so the content pane that holds all the fields)     
    TRect rect = Rect();
    TAknLayoutRect row1LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TInt titleHeight = row1LayoutRect.Rect().Height();
    // Add title field height
    TInt totalHeight = titleHeight;
    
    TAknLayoutRect row2LayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 2 );
    TInt editorRowHeight = row2LayoutRect.Rect().Height();
    
    TAknTextComponentLayout editorLayout =
        NMRLayoutManager::GetTextComponentLayout( 
                NMRLayoutManager::EMRTextLayoutMultiRowTextEditor );
    // Editor height from Layout data
    TInt editorHeight = editorLayout.H();
    // Margin is the outer row's height - editor's layout height
    TInt margin = editorRowHeight - editorHeight;
    // Parent has the width
    TInt width( Parent()->Size().iWidth );
    // Count the total height of the From -field.
    // iExpandedSize is used because the Editor size might be something else
    // than what is stated in Layout data.
    totalHeight += iExpandedSize.iHeight + margin;
    return TSize( width, totalHeight );
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerFromField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count = 0;
    if ( iFieldTopic )
        {
        count++;
        }
    if ( iRichTextViewer )
        {
        count++;
        }
    
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerFromField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    if ( aInd == 0 )
        {
        return iFieldTopic;
        }
    else if ( aInd == 1 )
        {
        return iRichTextViewer;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRViewerFromField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    return iRichTextViewer->OfferKeyEventL ( aEvent, aType );
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::HandleEdwinSizeEventL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerFromField::HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    iExpandedSize = aSize;
    if ( iObserver )
        {
        iObserver->ControlSizeChanged ( this );
        }
    return iDisableRedraw;
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::CESMRViewerFromField()
// ---------------------------------------------------------------------------
//
CESMRViewerFromField::CESMRViewerFromField( )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerFromField::ConstructL( )
    {
    FUNC_LOG;
    SetFieldId ( EESMRFieldOrganizer );

    iFieldTopic = CMRLabel::NewL();
    iFieldTopic->SetTextL( KNullDesC() );

    iRichTextViewer = CESMRRichTextViewer::NewL ( this );
    iRichTextViewer->SetEdwinSizeObserver ( this );
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
void CESMRViewerFromField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if (aCommand == EESMRCmdClipboardCopy)
        {
        iRichTextViewer->CopyCurrentLinkValueToClipBoardL();
        }
    if ( aCommand == EAknSoftkeySelect )
        {
        iRichTextViewer->LinkSelectedL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
void CESMRViewerFromField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( iRichTextViewer )
        {
        iRichTextViewer->SetFocus( aFocus );
    
		if ( aFocus )
			{
			SetMiddleSoftKeyVisible( ETrue );
			}
		else
			{
			//need to tell action menu that focus has changed
			iRichTextViewer->ResetActionMenuL();
			}
        }    
    }

// ---------------------------------------------------------------------------
// CESMRViewerFromField::SetContainerWindowL()
// ---------------------------------------------------------------------------
//
void CESMRViewerFromField::SetContainerWindowL(const CCoeControl& aContainer)
    {
    FUNC_LOG;
    CESMRField::SetContainerWindowL( aContainer );
    if ( iRichTextViewer )
        {
        iRichTextViewer->SetContainerWindowL( aContainer );
        }
    }
// -----------------------------------------------------------------------------
// CESMRViewerFromField::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CESMRViewerFromField::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    CCoeControl::HandlePointerEventL(aPointerEvent);

    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iRichTextViewer->SetFocus( ETrue );
        }
    }
//EOF

