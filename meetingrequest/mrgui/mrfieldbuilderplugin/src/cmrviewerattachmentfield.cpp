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
 * Description:  MR viewer attahcment field implementation
 *
 */
#include "cmrviewerattachmentfield.h"

#include "mesmrlistobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrimage.h"
#include "cesmrrichtextviewer.h"
#include "cesmreditor.h"
#include "cesmrrichtextlink.h"
#include "cmrattachmentcommandhandler.h"
#include "ccalremoteattachmentapi.h"
#include "ccalremoteattachmentinfo.h"
#include "mcalremoteattachmentoperation.h"
#include "cesmrglobalnote.h"

#include <AknsBasicBackgroundControlContext.h>
#include <calentry.h>
#include <calattachment.h>
#include <StringLoader.h>
#include <esmrgui.rsg>
#include <gulicon.h>
#include <ct/rcpointerarray.h>
#include <AknBidiTextUtils.h>
#include <badesca.h>

// DEBUG
#include "emailtrace.h"


// unnamed namespace for local definitions
namespace { // codescanner::namespace
//CONSTANTS

_LIT( KNewLine, "\f" );

const TInt KOneKiloByte( 1024 );
const TInt KOneMegaByte( 1048576 ); // (1024^2 or 2^20):
const TInt KMargin = 5;

/**
 * Checks that command is valid attachment viewer command.
 * @param aCommmand Command ID
 */
TBool IsValidAttachmentViewerCommand( TInt aCommand )
    {
    TBool ret( EFalse );

    switch( aCommand )
        {
        case EESMRViewerOpenAttachment:
        case EESMRViewerSaveAttachment:
        case EESMRViewerSaveAllAttachments:
        case EESMRViewerCancelAttachmentDownload:
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
// CMRViewerAttachmentsField::CMRViewerAttachmentsField
// ---------------------------------------------------------------------------
//
CMRViewerAttachmentsField::CMRViewerAttachmentsField()
    {
    FUNC_LOG;
    SetFocusType( EESMRHighlightFocus );
    SetFieldId( EESMRFieldViewerAttachments );
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::~CMRViewerAttachmentsField
// ---------------------------------------------------------------------------
//
CMRViewerAttachmentsField::~CMRViewerAttachmentsField( )
    {
    FUNC_LOG;
    delete iBgCtrlContext;
    delete iFieldIcon;
    delete iLockIcon;
    delete iAttachmentCommandHandler;
    delete iRemoteAttUris;
    delete iAttachmentInfo;
    delete iRemoteAttApi;
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::NewL
// ---------------------------------------------------------------------------
//
CMRViewerAttachmentsField* CMRViewerAttachmentsField::NewL()
    {
    FUNC_LOG;
    CMRViewerAttachmentsField* self = new( ELeave )CMRViewerAttachmentsField();
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::ConstructL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::ConstructL( )
    {
    FUNC_LOG;
    SetComponentsToInheritVisibility( ETrue );

    iFieldIcon = CMRImage::NewL(
            NMRBitmapManager::EMRBitmapAttachment,
            this );

    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    CESMRField::ConstructL( iRichTextViewer ); // ownership transferred
    iRichTextViewer->SetEdwinSizeObserver ( this );
    iRichTextViewer->SetLinkObserver (this );
    iRichTextViewer->SetParent( this );

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
    iSelectedLink = NULL;
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CMRViewerAttachmentsField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );

    TRect fieldRect =
       NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();

    TRect viewerRect( NMRLayoutManager::GetLayoutText(
            fieldRect,
            NMRLayoutManager::EMRTextLayoutTextEditor ).TextRect() );

    // Adjust field size so that there's room for expandable editor.
    fieldRect.Resize( 0, iRichTextSize.iHeight - viewerRect.Height() );

    return fieldRect.Size();
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::InitializeL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::InitializeL()
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
// CMRViewerAttachmentsField::InternalizeL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    delete iAttachmentCommandHandler;
    iAttachmentCommandHandler = NULL;

    iEntry = &aEntry;

    CreateRemoteAttApiL();
    UpdateAttachmentsListL();

    if ( iAttachmentCount )
        {
        //wake up current contact menu selection by calling this
        iRichTextViewer->FocusChanged( ENoDrawNow );
        iDisableRedraw = ETrue;
        iAttachmentCommandHandler =
                 CMRAttachmentCommandHandler::NewL( iEntry->Entry(), *iEventQueue );

        if ( iRemoteAttApi )
            {
            iAttachmentCommandHandler->SetRemoteAttachmentInformationL(
                    *iRemoteAttApi,
                    *iAttachmentInfo );
            }
        }
    else
        {
        iObserver->HideControl( EESMRFieldViewerAttachments );
        }
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::ExternalizeL( MESMRCalEntry& /*aEntry*/ )
    {
    FUNC_LOG;
    // No implementation
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CMRViewerAttachmentsField::CountComponentControls( ) const
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

    if ( iLockIcon )
    	{
    	++count;
    	}

    return count;

    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CMRViewerAttachmentsField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            return iFieldIcon;
        case 1:
            return iRichTextViewer;
        case 2:
        	return iLockIcon;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::SizeChanged( )
    {
    FUNC_LOG;
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

    // Layouting lock icon
    if( iLockIcon )
        {
        TAknWindowComponentLayout iconLayout(
                NMRLayoutManager::GetWindowComponentLayout(
                    NMRLayoutManager::EMRLayoutSingleRowDColumnGraphic ) );
        AknLayoutUtils::LayoutImage( iLockIcon, rowRect, iconLayout );
        }

	// Layouting viewer rect
    TAknLayoutText viewerLayoutText;
    if( iLockIcon )
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect,
    			NMRLayoutManager::EMRTextLayoutSingleRowEditorText );
    	}
    else
    	{
    	viewerLayoutText = NMRLayoutManager::GetLayoutText( rowRect,
    			NMRLayoutManager::EMRTextLayoutTextEditor );
    	}

    TRect viewerRect( viewerLayoutText.TextRect() );

    // Resize height according to actual height required by edwin.
    viewerRect.Resize( 0, iSize.iHeight - viewerRect.Height() );
    iRichTextViewer->SetRect( viewerRect );

    // Layouting focus
    TRect bgRect( 0, 0, 0, 0 );
    if( iAttachmentCount > 1 )
        {
        bgRect.SetRect(
                TPoint( viewerRect.iTl.iX,
                        viewerRect.iTl.iY - FocusMargin() ),
                TPoint( viewerRect.iBr.iX,
                        viewerRect.iBr.iY + FocusMargin() ) );
        }
    else
        {
        TAknLayoutRect bgLayoutRect =
            NMRLayoutManager::GetLayoutRect(
                    rowRect, NMRLayoutManager::EMRLayoutTextEditorBg );
        bgRect = bgLayoutRect.Rect();
        }

    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );

    // Failures are ignored.
    TRAP_IGNORE(
				// Set font
				iRichTextViewer->SetFontL( viewerLayoutText.Font() );
				if( iAttachmentCount > 1 )
					{
					// Set line spacing
					iRichTextViewer->SetLineSpacingL( LineSpacing() );
					}

				// Apply changes.
				iRichTextViewer->ApplyLayoutChangesL();
				)
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRViewerAttachmentsField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );

    if( iAttachmentCount > 0 )
        {
        response = iRichTextViewer->OfferKeyEventL( aEvent, aType );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::HandleEdwinSizeEventL
// ---------------------------------------------------------------------------
//
TBool CMRViewerAttachmentsField::HandleEdwinSizeEventL( CEikEdwin* aEdwin,
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
           
           if ( !iOutlineFocus )
               {
               RecordField();
               }
           }
        }

    return reDraw;
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::ListObserverSet()
    {
    FUNC_LOG;
    iRichTextViewer->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CMRViewerAttachmentsField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;

    TBool handled( EFalse );

    //handle locked field first
    if( IsLocked() )
    	{
    	if(  aCommand == EESMRViewerOpenAttachment ||
        		aCommand == EAknCmdOpen )
    		{
			HandleTactileFeedbackL();

    		CESMRGlobalNote::ExecuteL(
    				CESMRGlobalNote::EESMRUnableToEdit );
    		return ETrue;
    		}
    	}

    if ( IsValidAttachmentViewerCommand(aCommand) && iAttachmentCommandHandler )
        {
        const CESMRRichTextLink* currentLink = iRichTextViewer->GetSelectedLink();
        // remember the state if Saving all is not in progress
        if( currentLink && !iAttachmentCommandHandler->IsSaveAllAttachmentsInProgress() ) 
        	{
            iSelectedLink = currentLink;
        	}      
        else
            {
            currentLink = iSelectedLink;// restore link when selection is lost
        	}

        ASSERT( currentLink );

        TBool remoteCommand(
                iAttachmentCommandHandler->IsRemoteAttachmentL( *currentLink) );

        if ( !remoteCommand )
            {
            iAttachmentCommandHandler->HandleAttachmentCommandL(
                    aCommand,
                    *currentLink );

    		HandleTactileFeedbackL();
            }
        else
            {
            iAttachmentCommandHandler->HandleRemoteAttachmentCommandL(
                    aCommand,
                    *currentLink );

    		HandleTactileFeedbackL();
            }

        handled = ETrue;
        }
    return handled;
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );

    if ( aFocus && iAttachmentCount)
        {
        ChangeMiddleSoftKeyL(
                EAknSoftkeyContextOptions,
                R_QTN_MSK_OPEN );
        }
    }

// -----------------------------------------------------------------------------
// CMRViewerAttachmentsField::HandleRichTextLinkSelection
// -----------------------------------------------------------------------------
//
TBool CMRViewerAttachmentsField::HandleRichTextLinkSelection(
        const CESMRRichTextLink* aLink )
    {
    FUNC_LOG;
    TBool ret( EFalse );

    // Open context menu in editor and viewer modes with link selection
    if ( aLink->Type() == CESMRRichTextLink::ETypeAttachment )
        {
        TRAP_IGNORE( NotifyEventL( EMRLaunchAttachmentContextMenu ));
        ret = ETrue;

        TRAP_IGNORE( HandleTactileFeedbackL() );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMRViewerAttachmentsField::UpdateAttachmentsListL
// -----------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::UpdateAttachmentsListL()
    {
    FUNC_LOG;


    if ( iRemoteAttApi )
        {
        // Let's fetch remote attachment information
        iRemoteAttApi->AttachmentInfo(
                *iAttachmentInfo,
                *iRemoteAttUris );
        }

    RBuf buffer; // codescanner::resourcenotoncleanupstack
    buffer.CleanupClosePushL();

    RCPointerArray<CESMRRichTextLink> attachmentLinks;
    CleanupClosePushL( attachmentLinks );

    TAknLayoutText layoutText = NMRLayoutManager::GetLayoutText(
            Rect(),
            NMRLayoutManager::EMRTextLayoutTextEditor );

    iAttachmentCount = iEntry->Entry().AttachmentCountL();
    attachmentLinks.ReserveL( iAttachmentCount );
    for ( TInt i = 0; i < iAttachmentCount; ++i )
        {
		CCalAttachment* attachment = iEntry->Entry().AttachmentL( i ); // Ownership not gained

		const CFont* font = layoutText.Font();
		TInt maxFileNameLengthInPixels( MaxTextLengthInPixelsL() );

		// Size information needs to be reduced from max filename length
		HBufC* attachmentSizeOnly(
				AttachmentNameAndSizeL( KNullDesC(), *attachment ) );
		TInt sizeInformationLengthInPixels(
				font->TextWidthInPixels( *attachmentSizeOnly ) );
		maxFileNameLengthInPixels -= sizeInformationLengthInPixels;

		// Trimming file name to fit to one line
		TPtrC text = attachment->Label();
		HBufC* clippedTextHBufC = ClipTextLC(
				text, *font, maxFileNameLengthInPixels );
		TPtr clippedText = clippedTextHBufC->Des();
		clippedText.Trim();

        if ( clippedText.Length() > 0 )
            {
			HBufC* buf = AttachmentNameAndSizeL( clippedText, *attachment );
			CleanupStack::PushL( buf );

			// Creating rich text link
			CESMRRichTextLink* link = CESMRRichTextLink::NewL(
					buffer.Length(),
					buf->Length(),
					text,
					CESMRRichTextLink::ETypeAttachment,
					CESMRRichTextLink::ETriggerKeyRight );
			CleanupStack::PushL( link );
			attachmentLinks.AppendL( link );
			CleanupStack::Pop( link );

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

        while ( attachmentLinks.Count() > 0 )
            {
            CESMRRichTextLink* link = attachmentLinks[0];
            CleanupStack::PushL( link );
            attachmentLinks.Remove( 0 );
            iRichTextViewer->AddLinkL( link );
            CleanupStack::Pop( link );
            }

        iRichTextViewer->ApplyLayoutChangesL();

        iRichTextViewer->SetMargins( KMargin ); // What's this?
        iRichTextViewer->HandleTextChangedL();
        }

    CleanupStack::PopAndDestroy( &attachmentLinks );
    CleanupStack::PopAndDestroy( &buffer );

    if ( iAttachmentCount )
        {
        ChangeMiddleSoftKeyL(
                EAknSoftkeyContextOptions,
                R_QTN_MSK_OPEN );
        }
    }

// -----------------------------------------------------------------------------
// CMRViewerAttachmentsField::ClipTextLC
// -----------------------------------------------------------------------------
//
HBufC* CMRViewerAttachmentsField::ClipTextLC(
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
// CMRViewerAttachmentsField::AttachmentNameAndSizeL
// ---------------------------------------------------------------------------
//
HBufC* CMRViewerAttachmentsField::AttachmentNameAndSizeL(
        const TDesC& aDes,
        const CCalAttachment& aAttachment )
    {
    HBufC* stringholder = NULL;

    CDesCArrayFlat* attachmentStrings =
        new(ELeave)CDesCArrayFlat( 1 );
    CleanupStack::PushL( attachmentStrings );

    CArrayFixFlat<TInt>* integers =
        new (ELeave) CArrayFixFlat<TInt>(1);
    CleanupStack::PushL( integers );

    CCalAttachment::TType attType( aAttachment.Type() );

    TInt attachmentSize(0);
    if ( CCalAttachment::EFile == attType )
        {
        attachmentSize = aAttachment.FileAttachment()->Size();
        }
    else if ( iAttachmentInfo && iAttachmentInfo->AttachmentCount() )
        {
        TInt remoteAttCount( iAttachmentInfo->AttachmentCount() );
        for ( TInt i(0); i < remoteAttCount; ++i )
            {
            TPtrC label( aAttachment.Label() );
            TPtrC remoteLabel(
                    iAttachmentInfo->AttachmentL(i).AttachmentLabel() );
            if ( !label.Compare( remoteLabel ) )
                {
                attachmentSize = iAttachmentInfo->AttachmentL(i).AttachmentSizeInBytes();
                }
            }
        }

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
                R_MEET_REQ_ATTACHMENT_FILE_LESS_THAN_KILOBYTE ,
                *attachmentStrings );
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
// CMRViewerAttachmentsField::HandleLongtapEventL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::HandleLongtapEventL( const TPoint& aPosition )
    {
    FUNC_LOG;

    // prevent link selection if SaveAllAttachments is in progress
    TBool saveAllInProgress ( EFalse );
    if( iAttachmentCommandHandler ) 
    	{
        saveAllInProgress = iAttachmentCommandHandler->IsSaveAllAttachmentsInProgress( );
    	}
    
    if ( !saveAllInProgress && iRichTextViewer->Rect().Contains( aPosition ) )
        {
        iRichTextViewer->LinkSelectedL();
        }
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::HandleRawPointerEventL
// ---------------------------------------------------------------------------
//
TBool CMRViewerAttachmentsField::HandleRawPointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;
    // Handles short on attachment fields actual attachments
    TBool ret( EFalse );

    if( iAttachmentCount > 0 &&
        aPointerEvent.iType == TPointerEvent::EButton1Up )
        {

        // prevent handling events if SaveAllAttachments is in progress
        TBool saveAllInProgress ( EFalse );
        if( iAttachmentCommandHandler ) 
    	    {
            saveAllInProgress = iAttachmentCommandHandler->IsSaveAllAttachmentsInProgress( );
    	    }
           
        if(  !saveAllInProgress && iRichTextViewer->Rect().Contains( aPointerEvent.iPosition ) )
            {
            iRichTextViewer->HandlePointerEventL( aPointerEvent );
            ret = ETrue;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::DynInitMenuPaneL(
             TInt aResourceId,
             CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    if ( R_MR_VIEWER_ATTACHMENT_MENU == aResourceId )
        {
        DynInitMenuPaneForCurrentAttachmentL( aMenuPane );
        }
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::LockL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::LockL()
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
// CMRViewerAttachmentsField::LineSpacing
// ---------------------------------------------------------------------------
//
TInt CMRViewerAttachmentsField::LineSpacing()
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
// CMRViewerAttachmentsField::FocusMargin
// ---------------------------------------------------------------------------
//
TInt CMRViewerAttachmentsField::FocusMargin()
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

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::CreateRemoteAttApiL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::CreateRemoteAttApiL()
    {
    FUNC_LOG;

    delete iAttachmentCommandHandler;
    iAttachmentCommandHandler = NULL;

    delete iRemoteAttUris;
    iRemoteAttUris = NULL;

    delete iAttachmentInfo;
    iAttachmentInfo = NULL;

    TInt attachmentCount( iEntry->Entry().AttachmentCountL() );
    if ( attachmentCount )
        {
        iRemoteAttUris = new (ELeave) CDesCArrayFlat( 4 );
        iAttachmentInfo = CCalRemoteAttachmentInfo::NewL();
        }

    for (TInt i(0); i < attachmentCount; ++i )
        {
        // Ownership not gained
        CCalAttachment* att = iEntry->Entry().AttachmentL( i );
        CCalAttachment::TType attachhmentType( att->Type() );

        if ( CCalAttachment::EUri == attachhmentType )
            {
            if ( !iRemoteAttApi )
                {
                TPtrC8 uri( att->Value() );
                iRemoteAttApi = CCalRemoteAttachmentApi::NewL( uri );
                }

            HBufC* uri16 = HBufC16::NewLC(att->Value().Length() );
            uri16->Des().Copy( att->Value() );
            iRemoteAttUris->AppendL( *uri16 );
            CleanupStack::PopAndDestroy( uri16 );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::DynInitMenuPaneForCurrentAttachmentL
// ---------------------------------------------------------------------------
//
void CMRViewerAttachmentsField::DynInitMenuPaneForCurrentAttachmentL(
             CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    TInt commandInProgress(
           iAttachmentCommandHandler->CurrentCommandInProgress() );

    // Get selected attachment
    CCalAttachment* selectedAttachment = NULL;

    const CESMRRichTextLink* currentLink = iRichTextViewer->GetSelectedLink();
    ASSERT( currentLink );

    TPtrC currentAttachmentName( currentLink->Value() );
    for ( TInt i(0); i < iAttachmentCount && !selectedAttachment; ++i )
        {
        CCalAttachment* attachment = iEntry->Entry().AttachmentL( i );
        TPtrC attachmentLabel ( attachment->Label() );

        if ( !attachmentLabel.Compare( currentAttachmentName ) )
            {
            selectedAttachment = attachment;
            }
        }

    ASSERT( selectedAttachment );


    /*
     * Case 1: Download in progress
     */
    if( commandInProgress )
    	{
		// A) If selected attachment is remote attachment and download is in progress
		// or attachment is queued for downloading
		if( selectedAttachment->Type() == CCalAttachment::EUri &&
				EESMRViewerSaveAllAttachments == commandInProgress )
			{
			aMenuPane->SetItemDimmed(
					EESMRViewerOpenAttachment,
					ETrue );

			aMenuPane->SetItemDimmed(
				EESMRViewerSaveAttachment,
				ETrue );

			aMenuPane->SetItemDimmed(
					EESMRViewerSaveAllAttachments,
					ETrue );
			}

		// B) If selected attachment is local attachment
		else if( selectedAttachment->Type() == CCalAttachment::EFile )
			{
			// Always hide cancel attachment download item
			aMenuPane->SetItemDimmed(
					EESMRViewerCancelAttachmentDownload,
					ETrue );

			// If attachment count is one, hide 'Save All' also
			if ( iAttachmentCount == 1 )
				{
				aMenuPane->SetItemDimmed(
						EESMRViewerSaveAllAttachments,
						ETrue );
				}
			}

		// C) If selected attachment is Uri, and some other command is in progress
		// than save all
		else if( selectedAttachment->Type() == CCalAttachment::EUri &&
				commandInProgress != EESMRViewerSaveAllAttachments )
			{
			// If this attachment is the one being downloaded
			if( IsAttachmentDownloadInProgress( *selectedAttachment ) )
				{
				aMenuPane->SetItemDimmed(
					EESMRViewerOpenAttachment,
					ETrue );

				aMenuPane->SetItemDimmed(
					EESMRViewerSaveAttachment,
					ETrue );

				aMenuPane->SetItemDimmed(
						EESMRViewerSaveAllAttachments,
						ETrue );
				}


			// if this attacment is not the one being downloaded
			else
				{
				// Always hide cancel attachment download item
				aMenuPane->SetItemDimmed(
						EESMRViewerCancelAttachmentDownload,
						ETrue );

				// If attachment count is one, hide 'Save All' also
				if ( iAttachmentCount == 1 )
					{
					aMenuPane->SetItemDimmed(
							EESMRViewerSaveAllAttachments,
							ETrue );
					}
				}
			}
    	}

    /*
     * Case 2: No download in progress
     */
    else
    	{
		// Always hide cancel attachment download item
		aMenuPane->SetItemDimmed(
				EESMRViewerCancelAttachmentDownload,
				ETrue );

		// If attachment count is one, hide 'Save All' also
		if ( iAttachmentCount == 1 )
			{
			aMenuPane->SetItemDimmed(
					EESMRViewerSaveAllAttachments,
					ETrue );
			}
    	}
    }

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::IsAttachmentDownloadInProgress
// ---------------------------------------------------------------------------
//
TBool CMRViewerAttachmentsField::IsAttachmentDownloadInProgress(
		const CCalAttachment& aAttachment )
	{
    TBool ret( EFalse );

    RPointerArray<MCalRemoteAttachmentOperation>& remoteOperations(
                    iAttachmentCommandHandler->RemoteOperations() );

    TPtrC currentAttachmentName( aAttachment.Label() );

	TInt downloadOperationCount( remoteOperations.Count() );
	if ( downloadOperationCount )
		{
		for ( TInt i(0); i < downloadOperationCount; ++i )
			{
			// There are download operations in progress
			const MCalRemoteAttachment& attachInfo =
			remoteOperations[i]->AttachmentInformation();

			TPtrC label( attachInfo.AttachmentLabel() );

			if ( label.Compare( currentAttachmentName ) == 0 )
				{
				ret = ETrue;
				}
			}
		}

    return ret;
	}

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::SupportsLongTapFunctionalityL
// ---------------------------------------------------------------------------
//
TBool CMRViewerAttachmentsField::SupportsLongTapFunctionalityL(
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

// ---------------------------------------------------------------------------
// CMRViewerAttachmentsField::MaxTextLengthInPixelsL
// ---------------------------------------------------------------------------
//
TInt CMRViewerAttachmentsField::MaxTextLengthInPixelsL()
    {
    FUNC_LOG;
    /*
     * RichTextViewer length cannot be used here directly, since
     * this value is not always available. Value needs to be calculated.
     */
    TRect tempFieldRect( TPoint( 0, 0 ), MinimumSize() );

    // Rich Text viewer width is the same as other common rich text components
    // width
    TAknLayoutRect rowLayoutRect(
            NMRLayoutManager::GetFieldRowLayoutRect( tempFieldRect, 1 ) );
    TRect rowRect( rowLayoutRect.Rect() );

    TAknLayoutText layoutText( NMRLayoutManager::GetLayoutText(
    		rowRect,
            NMRLayoutManager::EMRTextLayoutTextEditor ) );

    TRect tempRichTextViewerRect( layoutText.TextRect() );

	return tempRichTextViewerRect.Width();
    }

//EOF
