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
 * Description:  MR attahcment field implementation
 *
 */
#include "cmrattachmentsfield.h"

#include "mesmrlistobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrimage.h"
#include "cesmrrichtextviewer.h"
#include "cesmreditor.h"
#include "cesmrrichtextlink.h"
#include "cmrattachmentcommandhandler.h"

#include <AknsBasicBackgroundControlContext.h>
#include <calentry.h>
#include <calattachment.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <gulicon.h>
#include <ct/rcpointerarray.h>
#include <AknBidiTextUtils.h>

// DEBUG
#include "emailtrace.h"


// unnamed namespace for local definitions
namespace { // codescanner::namespace
//CONSTANTS

_LIT( KAddAttachment, "Add attachment" );
_LIT( KNewLine, "\f" );

const TInt KOneKiloByte( 1024 );
const TInt KOneMegaByte( 1048576 ); // (1024^2 or 2^20):
const TInt KMargin = 5;

/**
 * Checks that command is valid attachment viewer command.
 * @param aCommmand Command ID
 */
TBool IsValidAttachmentEditorCommand( TInt aCommand )
    {
    TBool ret( EFalse );

    switch( aCommand )
        {
        case EESMREditorOpenAttachment:
        case EESMREditorRemoveAttachment:
        case EESMREditorRemoveAllAttachments:
        case EESMREditorAddAttachment:
            ret = ETrue;
            break;

        default:
            ret = EFalse;
            break;
        }

    return ret;
    }

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRAttachmentsField::CMRAttachmentsField
// ---------------------------------------------------------------------------
//
CMRAttachmentsField::CMRAttachmentsField()
    {
    FUNC_LOG;
    SetFocusType( EESMRHighlightFocus );
    SetFieldId( EESMRFieldAttachments );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::~CMRAttachmentsField
// ---------------------------------------------------------------------------
//
CMRAttachmentsField::~CMRAttachmentsField( )
    {
    FUNC_LOG;
    delete iBgCtrlContext;
    delete iFieldIcon;
    delete iTitle;
    delete iAttCommandHandler;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::NewL
// ---------------------------------------------------------------------------
//
CMRAttachmentsField* CMRAttachmentsField::NewL()
    {
    FUNC_LOG;
    CMRAttachmentsField* self = new( ELeave )CMRAttachmentsField();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::ConstructL
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::ConstructL()
    {
    FUNC_LOG;

    SetComponentsToInheritVisibility( ETrue );

    iFieldIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAttachment );
    iFieldIcon->SetParent( this );

    iTitle = CMRLabel::NewL();
    iTitle->SetParent( this );

    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    CESMRField::ConstructL( iRichTextViewer ); // ownership transferred
    iRichTextViewer->SetEdwinSizeObserver ( this );
    iRichTextViewer->SetLinkObserver (this );
    iRichTextViewer->SetParent( this );

    iTitle->SetTextL( KAddAttachment );

    // Draw bg instead of using skin bg
    TRect tempRect( 0, 0, 0, 0 );
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct(
            NMRBitmapManager::EMRBitmapInputCenter );

    iBgCtrlContext = CAknsBasicBackgroundControlContext::NewL(
                bitmapStruct.iItemId,
                tempRect,
                EFalse );

    iRichTextViewer->SetSkinBackgroundControlContextL( iBgCtrlContext );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CMRAttachmentsField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );

    TRect richTextRect =
       NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();

    TRect textRect( NMRLayoutManager::GetLayoutText(
            richTextRect,
       NMRLayoutManager::EMRTextLayoutTextEditor ).TextRect() );

    // Adjust field size so that there's room for expandable editor.
    richTextRect.Resize( 0, iRichTextSize.iHeight - textRect.Height() );

    // Add title area to the required size
    TSize titleSize( CESMRField::MinimumSize() );

    TSize completeFieldSize( titleSize );
    if( iAttachmentCount )
        {
        completeFieldSize.iHeight += richTextRect.Height();
        }

    return completeFieldSize;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::InitializeL
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::InitializeL()
    {
    FUNC_LOG;
    TAknLayoutText layoutText =
        NMRLayoutManager::GetLayoutText( Rect(),
                NMRLayoutManager::EMRTextLayoutTextEditor );
    iRichTextViewer->SetFontL( layoutText.Font() );
    if( iAttachmentCount > 1 )
        {
        iRichTextViewer->SetLineSpacingL( LineSpacing() );
        }
    iRichTextViewer->ApplyLayoutChangesL();

    //wake up current contact menu selection by calling this
    iRichTextViewer->FocusChanged( ENoDrawNow );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::InternalizeL
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    iEntry = &aEntry;

    if ( aEntry.SupportsCapabilityL( MESMRCalEntry::EMRCapabilityAttachments ) )
        {
        UpdateAttachmentsListL();

        //wake up current contact menu selection by calling this
        iRichTextViewer->FocusChanged( ENoDrawNow );

        iDisableRedraw = ETrue;
        }
    else
        {
        // Field does not support attachments --> Remove field from UI.
        iObserver->HideControl( EESMRFieldAttachments );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::ExternalizeL( MESMRCalEntry& /*aEntry*/ )
    {
    FUNC_LOG;
    // No implementation
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentsField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iFieldIcon )
        {
        ++count;
        }

    if ( iRichTextViewer )
        {
        ++count;
        }
    if ( iTitle )
        {
        ++count;
        }

    return count;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRAttachmentsField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            return iRichTextViewer;
        case 1:
            return iFieldIcon;
        case 2:
            return iTitle;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::SizeChanged( )
    {
    FUNC_LOG;
    TRect rect( Rect() );

    TAknLayoutRect firstRowLayoutRect(
            NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 ) );
    TRect firstRowRect( firstRowLayoutRect.Rect() );

    TRect secondRowRect( firstRowRect );
    secondRowRect.Move( 0, firstRowRect.Height() );

    // Layout field button
    if( iFieldIcon )
        {
        TAknWindowComponentLayout iconLayout(
                NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutTextEditorIcon ) );
        AknLayoutUtils::LayoutControl(
                iFieldIcon, firstRowRect, iconLayout );
        }

    // Layout field title
    if( iTitle )
        {
        TAknLayoutText labelLayout(
                NMRLayoutManager::GetLayoutText(
                        firstRowRect,
                            NMRLayoutManager::EMRTextLayoutTextEditor ) );

        iTitle->SetRect( labelLayout.TextRect() );

        // Setting font also for the label. Failures are ignored.
        TRAP_IGNORE( iTitle->SetFont( labelLayout.Font() ) );
        }

    TRect edwinRect( 0, 0, 0, 0 );

    // Layout field editor
    if( iRichTextViewer )
        {
        TAknLayoutText layoutText = NMRLayoutManager::GetLayoutText(
                secondRowRect,
                NMRLayoutManager::EMRTextLayoutTextEditor );

        edwinRect = layoutText.TextRect();

        // Resize height according to actual height required by edwin.
        edwinRect.Resize( 0, iRichTextSize.iHeight - edwinRect.Height() );

        iRichTextViewer->SetRect( edwinRect );

        // Try setting font. Failures are ignored.
        TRAP_IGNORE(
                iRichTextViewer->SetFontL( layoutText.Font() );
                if( iAttachmentCount > 1 )
                    {
                    iRichTextViewer->SetLineSpacingL( LineSpacing() );
                    }
                iRichTextViewer->ApplyLayoutChangesL();
                );
        }

    // Layouting focus
    TRect bgRect( 0, 0, 0, 0 );
    // No attachments -> Focus to the title
    if( iAttachmentCount == 0 )
        {
        TAknLayoutRect bgLayoutRect =
            NMRLayoutManager::GetLayoutRect(
                    firstRowRect, NMRLayoutManager::EMRLayoutTextEditorBg );
        bgRect = bgLayoutRect.Rect();
        }
    // One attachment -> Focus to the attachment according to layout data
    if( iAttachmentCount == 1 )
        {
        TAknLayoutRect bgLayoutRect =
            NMRLayoutManager::GetLayoutRect(
                    secondRowRect, NMRLayoutManager::EMRLayoutTextEditorBg );
        bgRect = bgLayoutRect.Rect();
        }
    // More than one attachements -> Focus calculated manually
    if( iAttachmentCount > 1 )
        {
        bgRect.SetRect(
                TPoint( edwinRect.iTl.iX,
                        edwinRect.iTl.iY - FocusMargin() ),
                TPoint( edwinRect.iBr.iX,
                        edwinRect.iBr.iY + FocusMargin() ) );
        }

    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRAttachmentsField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );

    if( iAttachmentCount == 0 )
        {
        response = iTitle->OfferKeyEventL( aEvent, aType );
        }

    if( iAttachmentCount > 0 )
        {
        response = iRichTextViewer->OfferKeyEventL( aEvent, aType );
        }

    if ( aType == EEventKey &&
            ( aEvent.iScanCode != EStdKeyUpArrow &&
              aEvent.iScanCode != EStdKeyDownArrow ))
        {
        iTitle->DrawDeferred(); // ??
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::SetContainerWindowL()
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::SetContainerWindowL( const CCoeControl& aControl )
    {
    FUNC_LOG;
    CESMRField::SetContainerWindowL( aControl );
    iRichTextViewer->SetContainerWindowL( aControl );
    iRichTextViewer->SetParent( this );
    iTitle->SetContainerWindowL( aControl );
    iTitle->SetParent( this );
    iFieldIcon->SetContainerWindowL( aControl );
    iFieldIcon->SetParent( this );

    iButtonGroupContainer = CEikButtonGroupContainer::Current();
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentsField::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
        TEdwinSizeEvent /*aType*/, TSize aSize )
    {
    FUNC_LOG;
    TBool reDraw( EFalse );

    if( iRichTextSize.iHeight != aSize.iHeight && iAttachmentCount > 0 )
        {
        // Let's save the required size of the rich text viewer
        iRichTextSize = aSize;

        if ( iObserver && aEdwin == iRichTextViewer )
           {
           iObserver->ControlSizeChanged( this );
           reDraw = ETrue;
           }
        }

    return reDraw;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::ListObserverSet()
    {
    FUNC_LOG;
    iRichTextViewer->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentsField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;

    TBool handled( EFalse );

    // From cesmrattachmentfield
    if( aCommand == EAknCmdOpen )
        {
        handled = ETrue;
		HandleTactileFeedbackL();
        }
    else if ( aCommand == EESMREditorAddAttachment )
        {
        handled = AddAttachmentL();
        }

    else if ( IsValidAttachmentEditorCommand(aCommand) &&
            iAttCommandHandler )
        {
        const CESMRRichTextLink* currentLink = iRichTextViewer->GetSelectedLink();

        ASSERT( currentLink );

        iAttCommandHandler->HandleAttachmentCommandL(
                aCommand,
                *currentLink );
        
		HandleTactileFeedbackL();

        handled = ETrue;

        if( aCommand == EESMREditorRemoveAttachment ||
                aCommand == EESMREditorRemoveAllAttachments )
            {
            // Attachment list needs to be updated after removing
            // attachment(s)
            UpdateAttachmentsListL();

            //wake up current contact menu selection by calling this
            iRichTextViewer->FocusChanged( ENoDrawNow );

            if( iAttachmentCount == 0 )
                {
                // If no attachments are present anymore, let's hide the
                // whole rich text viewer.
                iRichTextViewer->MakeVisible( EFalse );

                // Because rich text part is removed from the UI,
                // we need to inform the parent, to re-size the
                // field container and re-initialize the physics
                // world size
                iObserver->ControlSizeChanged( this );
                }
            }
        }

    return handled;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );

    if ( aFocus )
        {
        if ( iAttachmentCount )
            {
            ChangeMiddleSoftKeyL(
                    EAknSoftkeyContextOptions,
                    R_QTN_MSK_OPEN );
            }
        else
            {
            ChangeMiddleSoftKeyL(
                    EESMREditorAddAttachment,
                    R_QTN_MSK_ADD );

            }
        }
    }

// -----------------------------------------------------------------------------
// CMRAttachmentsField::HandleRichTextLinkSelection
// -----------------------------------------------------------------------------
//
TBool CMRAttachmentsField::HandleRichTextLinkSelection(
        const CESMRRichTextLink* aLink )
    {
    FUNC_LOG;
    TBool ret( EFalse );

    // Open context menu in editor and viewer modes with link selection
    if ( aLink->Type() == CESMRRichTextLink::ETypeAttachment )
        {
        TRAP_IGNORE( NotifyEventL( EMRLaunchAttachmentContextMenu ));
        ret = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMRAttachmentsField::UpdateAttachmentsListL
// -----------------------------------------------------------------------------
//
void CMRAttachmentsField::UpdateAttachmentsListL()
    {
    FUNC_LOG;

    RBuf buffer; // codescanner::resourcenotoncleanupstack
    buffer.CleanupClosePushL();

    RCPointerArray<CESMRRichTextLink> attachmentLinks;
    CleanupClosePushL( attachmentLinks );

    iAttachmentCount = iEntry->Entry().AttachmentCountL();

    TAknLayoutText layoutText = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );

    attachmentLinks.ReserveL( iAttachmentCount );
    for ( TInt i = 0; i < iAttachmentCount; ++i )
        {
        CCalAttachment* att = iEntry->Entry().AttachmentL( i ); // Ownership not gained

        // Reducing space required by file size information from
        // max line width, so that file name can be trimmed to correct
        // length
        TInt maxLineWidth = MinimumSize().iWidth;
        const CFont* font = layoutText.Font();

        HBufC* attachmentSize = AttachmentNameAndSizeL( KNullDesC(), *att );
        TInt attachmentSizeLength = attachmentSize->Length();
        maxLineWidth -= font->TextWidthInPixels( *attachmentSize );
        delete attachmentSize;
        attachmentSize = NULL;

        TSize iconSize( 20, 20);
        maxLineWidth -= iconSize.iWidth;

        // Trimming file name to fit to one line
        TPtrC text = att->Label();
        HBufC* clippedTextHBufC = ClipTextLC( text, *font, maxLineWidth );
        TPtr clippedText = clippedTextHBufC->Des();
        clippedText.Trim();

        if ( clippedText.Length() > 0 )
            {
            // Creating rich text link
            CESMRRichTextLink* link = CESMRRichTextLink::NewL(
                    buffer.Length(),
                    clippedText.Length() + attachmentSizeLength,
                    text,
                    CESMRRichTextLink::ETypeAttachment,
                    CESMRRichTextLink::ETriggerKeyRight );
            CleanupStack::PushL( link );
            attachmentLinks.AppendL( link );
            CleanupStack::Pop( link );

            HBufC* buf = AttachmentNameAndSizeL( clippedText, *att );
            CleanupStack::PushL( buf );

            // Append attachment name and size with line feed to buffer
            buffer.ReAllocL( buffer.Length() +
                             clippedText.Length() +
                             buf->Length() +
                             KNewLine().Length() );

            buffer.Append( buf->Des() );
            buffer.Append( KNewLine );

            CleanupStack::PopAndDestroy( buf );
            }

        CleanupStack::PopAndDestroy( clippedTextHBufC );
        }

    // Remove unnecessary new line from the end of buffer.
    if ( buffer.Length() >= KNewLine().Length() )
        {
        buffer.SetLength( buffer.Length() - KNewLine().Length() );
        }

    // Adding all rich text links to the rich text viewer
    if ( buffer.Length() > 0 )
        {
        iRichTextViewer->SetTextL( &buffer );
        iRichTextViewer->SetFontL( layoutText.Font() );
        if( iAttachmentCount > 1 )
            {
            iRichTextViewer->SetLineSpacingL( LineSpacing() );
            }
        iRichTextViewer->ApplyLayoutChangesL();
        iRichTextViewer->SetMargins( KMargin ); // What's this?
        iRichTextViewer->HandleTextChangedL();

        while ( attachmentLinks.Count() > 0 )
            {
            CESMRRichTextLink* link = attachmentLinks[0];
            CleanupStack::PushL( link );
            attachmentLinks.Remove( 0 );
            iRichTextViewer->AddLinkL( link );
            CleanupStack::Pop( link );
            }
        }

    CleanupStack::PopAndDestroy( &attachmentLinks );
    CleanupStack::PopAndDestroy( &buffer );

    if ( iAttachmentCount )
        {
        iRichTextViewer->MakeVisible( ETrue );

        iObserver->ControlSizeChanged( this );

        ChangeMiddleSoftKeyL(
                EAknSoftkeyContextOptions,
                R_QTN_MSK_OPEN );

        delete iAttCommandHandler;
        iAttCommandHandler = NULL;

        iAttCommandHandler =
                CMRAttachmentCommandHandler::NewL(
                        iEntry->Entry(),
                        *iEventQueue );
        }
    }

// -----------------------------------------------------------------------------
// CMRAttachmentsField::ClipTextLC
// -----------------------------------------------------------------------------
//
HBufC* CMRAttachmentsField::ClipTextLC(
        const TDesC& aText, const CFont& aFont, TInt aWidth )
    {
    FUNC_LOG;
    HBufC* text = HBufC::NewLC( aText.Length() + KAknBidiExtraSpacePerLine );
    TPtr textPtr = text->Des();
    AknBidiTextUtils::ConvertToVisualAndClip(
            aText, textPtr, aFont, aWidth, aWidth );
    return text;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::AttachmentOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::AttachmentOperationCompleted(
        MDesC16Array& /*aArray*/ )
    {
    TRAPD( error, UpdateAttachmentsListL() );
    if( error != KErrNone )
        {
        // Error occured, could not update the field
        iCoeEnv->HandleError( error );
        }

    delete iAttachmentUi;
    iAttachmentUi = NULL;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::AttachmentNameAndSizeL
// ---------------------------------------------------------------------------
//
HBufC* CMRAttachmentsField::AttachmentNameAndSizeL(
        const TDesC& aDes,
        const CCalAttachment& aAttachment )
    {
    HBufC* stringholder;

    CDesCArrayFlat* attachmentStrings =
        new(ELeave)CDesCArrayFlat( 1 );
    CleanupStack::PushL( attachmentStrings );

    CArrayFixFlat<TInt>* integers =
        new (ELeave) CArrayFixFlat<TInt>(1);
    CleanupStack::PushL( integers );

    TInt attachmentSize = aAttachment.FileAttachment()->Size();

    if ( attachmentSize >= KOneMegaByte )
        {
        attachmentStrings->AppendL( aDes );

        integers->AppendL( attachmentSize / KOneMegaByte );

        stringholder = StringLoader::LoadLC(
                R_MEET_REQ_ATTACHMENT_FILE_MEGABYTE,
                *attachmentStrings,
                *integers );
        }
    else if (( attachmentSize < KOneKiloByte ) && (attachmentSize != 0))
        {
        attachmentStrings->AppendL( aDes );

        stringholder = StringLoader::LoadLC(
                R_MEET_REQ_ATTACHMENT_FILE_LESS_THAN_KILOBYTE,
                *attachmentStrings);
        }
    else
        {
        attachmentStrings->AppendL( aDes );

        integers->AppendL( attachmentSize / KOneKiloByte );

        stringholder = StringLoader::LoadLC(
                R_MEET_REQ_ATTACHMENT_FILE_KILOBYTE,
                *attachmentStrings,
                *integers );
        }

    CleanupStack::Pop( stringholder );
    CleanupStack::PopAndDestroy( integers );
    CleanupStack::PopAndDestroy( attachmentStrings );

    return stringholder;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::AddAttachmentL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentsField::AddAttachmentL()
    {
    TBool ret( EFalse );

    if( iEntry )
        {
        if ( !iAttachmentUi )
            {
            iAttachmentUi = CMRAttachmentUi::NewL();
            }

        iAttachmentUi->SetObserver( *this );
        TInt error( iAttachmentUi->LaunchFetchUi( *iEntry ) );
        ret = ETrue;
        if( error != KErrNone )
            {
            iCoeEnv->HandleError( error );
            ret = EFalse;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::HandleSingletapEventL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentsField::HandleSingletapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;
    TBool ret( EFalse );

    // When there are no attachments, the whole field area
    // will execute single tap the action
    if( iAttachmentCount == 0 )
        {
        if( Rect().Contains( aPosition ) )
            {
			HandleTactileFeedbackL();
			
            ret = AddAttachmentL();
            }
        }
    // Otherwise only the title and the fieldIcon will
    // execute the single tap action
    else
        {
        if( iTitle->Rect().Contains( aPosition ) ||
                iFieldIcon->Rect().Contains( aPosition ) )
            {
			HandleTactileFeedbackL();
			
			ret = AddAttachmentL();
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::HandleLongtapEventL( const TPoint& aPosition )
    {
    // When there are no attachments, the whole field area
    // will execute long tap the action
    if( iAttachmentCount == 0 )
        {
        if( Rect().Contains( aPosition ) )
            {
			AddAttachmentL();
            }
        }
    else
        {
        if ( iRichTextViewer->Rect().Contains( aPosition ) )
            {
			iRichTextViewer->LinkSelectedL();
            }

        if( iTitle->Rect().Contains( aPosition ) ||
                iFieldIcon->Rect().Contains( aPosition ) )
            {
			AddAttachmentL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::HandleRawPointerEventL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentsField::HandleRawPointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;
    TBool ret( EFalse );

    if( iAttachmentCount > 0 &&
        aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        if( iRichTextViewer->Rect().Contains( aPointerEvent.iPosition ) )
            {
            iRichTextViewer->HandlePointerEventL( aPointerEvent );
            ret = ETrue;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CMRAttachmentsField::DynInitMenuPaneL(
             TInt aResourceId,
             CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    if ( R_MR_EDITOR_ATTACHMENT_MENU == aResourceId )
        {
        if ( !iAttachmentCount )
            {
            aMenuPane->SetItemDimmed(
                    EESMREditorOpenAttachment,
                    ETrue );

            aMenuPane->SetItemDimmed(
                    EESMREditorRemoveAttachment,
                    ETrue );

            aMenuPane->SetItemDimmed(
                    EESMREditorRemoveAllAttachments,
                    ETrue );
            }
        else if ( iAttachmentCount == 1 )
            {
            aMenuPane->SetItemDimmed(
                    EESMREditorRemoveAllAttachments,
                    ETrue );

            }
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::LineSpacing
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentsField::LineSpacing()
    {
    FUNC_LOG;
    // Calculates the line spacing based on default one line layout data
    TInt lineSpacing( 0 );

    TRect rect( Rect() );

    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TRect rowRect = rowLayoutRect.Rect();

    TAknLayoutText labelLayout =
     NMRLayoutManager::GetLayoutText(
             rect, NMRLayoutManager::EMRTextLayoutTextEditor );

    TRect defaultTextRect = labelLayout.TextRect();

    TInt difference = rowRect.Height() - defaultTextRect.Height();

    lineSpacing = difference * 2;

    return lineSpacing;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentsField::FocusMargin
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentsField::FocusMargin()
    {
    FUNC_LOG;
    // Calculates focus margin based on default one line layout data
    TInt focusMagin( 0 );

    TRect rect( Rect() );

    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TRect rowRect = rowLayoutRect.Rect();

    TAknLayoutText labelLayout =
     NMRLayoutManager::GetLayoutText(
             rect, NMRLayoutManager::EMRTextLayoutTextEditor );

    TRect defaultTextRect = labelLayout.TextRect();

    TAknLayoutRect bgLayoutRect =
        NMRLayoutManager::GetLayoutRect(
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect defaultBgRect( bgLayoutRect.Rect() );

    TInt difference = defaultBgRect.Height() - defaultTextRect.Height();

    focusMagin = TReal( difference / 2 );

    return focusMagin;
    }

//EOF
