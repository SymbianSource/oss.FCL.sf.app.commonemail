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
* Description:  Meeting request attachments field
 *
*/
#include "emailtrace.h"
#include "cesmrviewerattachmentsfield.h"

//<cmail>
#include "mesmrcalentry.h"
#include "cesmrcontactmenuattachmenthandler.h"
//</cmail>
#include <esmrgui.rsg>
#include <StringLoader.h>
//<cmail>
#include "esmricalvieweropcodes.hrh"
#include "CFSMailMessage.h"
//</cmail>

#include "cesmrrichtextviewer.h"
#include "cesmrrichtextlink.h"
#include "mesmrlistobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "tesmrinputparams.h"
#include "cesmrattachment.h"
#include "cesmrattachmentinfo.h"
#include "cesmrcontactmenuhandler.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"


// Unnamed namespace for local definitions
namespace { // codescanner::namespace

//<cmail>
// MR does not contain attachments
const TInt KNoAttachments( 0 );
// MR contains one attachment
const TInt KOneAttachment( 1 );
//</cmail>

const TInt KOneKiloByte( 1024 );

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::CESMRViewerAttachmentsField
// ---------------------------------------------------------------------------
//
CESMRViewerAttachmentsField::CESMRViewerAttachmentsField()
: iFocused(EFalse)
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldAttachments );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::~CESMRViewerAttachmentsField
// ---------------------------------------------------------------------------
//
CESMRViewerAttachmentsField::~CESMRViewerAttachmentsField()
    {
    FUNC_LOG;
    delete iFieldIcon;
    delete iRichTextViewer;
    delete iAttachmentText;
    
    iESMRStatic.Close();
    
    delete iAttachmentInfo;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::NewL
// ---------------------------------------------------------------------------
//
CESMRViewerAttachmentsField* CESMRViewerAttachmentsField::NewL()
    {
    FUNC_LOG;
    CESMRViewerAttachmentsField* self = new( ELeave )CESMRViewerAttachmentsField;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::ConstructL()
    {
    FUNC_LOG;
    SetFocusType( EESMRNoFocus );
    
    iFieldIcon = CMRImage::NewL(  NMRBitmapManager::EMRBitmapMailAttachment );
    iFieldIcon->SetParent( this );
    
    iRichTextViewer = CESMRRichTextViewer::NewL( this );
    iRichTextViewer->SetEdwinSizeObserver( this );
    iRichTextViewer->SetLinkObserver( this );
    iRichTextViewer->SetParent( this );
    
    iRichTextViewer->SetActionMenuStatus( ETrue );
    iESMRStatic.ConnectL();   
    iCntMenuHdlr = &iESMRStatic.ContactMenuHandlerL();
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRViewerAttachmentsField::MinimumSize()
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
// CESMRViewerAttachmentsField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::InitializeL()
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
// CESMRViewerAttachmentsField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRMeetingRequestEntry* mrEntry = NULL;
    if ( aEntry.Type() != MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        // This is not meeting request entry so attachment field should be removed from ui
        iObserver->RemoveControl( FieldId() );
        return;
        }
    else
        {
        mrEntry = static_cast<MESMRMeetingRequestEntry*>( &aEntry );

        TESMRInputParams startupParams;
        if ( mrEntry && mrEntry->StartupParameters(startupParams) )
            {
            // Startup parameters exist --> Let's check if there are attachments

            if ( startupParams.iAttachmentInfo )
                {
                iMailMessage = startupParams.iMailMessage;
                
                UpdateAttachmentInfoL();
                
                iCntMenuHdlr->SetCommandObserver( this );
                
                iAttachmentCount = 0;
                
                if ( iAttachmentInfo )
                    {
                    iAttachmentCount = iAttachmentInfo->AttachmentCount();
                    }

                if ( iAttachmentCount > KOneAttachment )
                    {
                    // There are more than one attachments
                    TInt attachmentsSizeSum(0);
                    for (TInt i(0); i < iAttachmentCount; ++i )
                        {
                        const CESMRAttachment& attachment(
                                iAttachmentInfo->AttachmentL( i ) );

                        TInt attachmentSize( attachment.AttachmentSizeInBytes() );
                        attachmentsSizeSum += attachmentSize;
                        }
                    if ( attachmentsSizeSum >= KOneKiloByte )
                        {
                        CArrayFixFlat<TInt>* ints =
                                new ( ELeave ) CArrayFixFlat<TInt>( 1 );

                        CleanupStack::PushL(ints);
                        ints->AppendL( iAttachmentCount );
                        ints->AppendL( attachmentsSizeSum / KOneKiloByte );

                        iAttachmentText = StringLoader::LoadL(
                                R_QTN_FSE_VIEWER_HEADER_ATTACHMENTS_TOTAL,
                                *ints );

                        CleanupStack::PopAndDestroy(ints);
                        }
                    else
                        {
                        iAttachmentText = StringLoader::LoadL(
                                R_QTN_FSE_VIEWER_HEADER_ATTACHMENTS_LESS_THAN_KB,
                                iAttachmentCount);
                        }
                    }
                else if ( iAttachmentCount > 0 )
                    {
                    // There is one attachment
                    const CESMRAttachment& attachment(
                            iAttachmentInfo->AttachmentL( 0 ) );
                    TPtrC attachmentName( attachment.AttachmentName() );

                    TInt attachmentSize( attachment.AttachmentSizeInBytes() );

                    if ( attachmentSize >= KOneKiloByte )
                        {
                        // "%0U %1N kB"

                        CDesCArrayFlat* attachmentStrings = 
							new(ELeave)CDesCArrayFlat( 1 );
                        CleanupStack::PushL( attachmentStrings );
                        //First string
                        attachmentStrings->AppendL( attachmentName ); 

                        CArrayFixFlat<TInt>* integers =
                                new (ELeave) CArrayFixFlat<TInt>(1);
                        CleanupStack::PushL( integers );
                        integers->AppendL( attachmentSize / KOneKiloByte );

                        iAttachmentText = StringLoader::LoadL(
                                R_QTN_FSE_VIEWER_HEADER_ATTACHMENT,
                                *attachmentStrings,
                                *integers );

                        // Pop and delete strings array
                        CleanupStack::PopAndDestroy( integers );
                        CleanupStack::PopAndDestroy( attachmentStrings );
                        }
                    else
                        {
                        iAttachmentText = StringLoader::LoadL(
                                R_QTN_FSE_VIEWER_HEADER_ATTACHMENT_LESS_THAN_KB,
                                attachmentName);
                        }
                    }

                if ( iAttachmentText )
                    {
                    iRichTextViewer->SetTextL( iAttachmentText, EFalse );
                    // After setting the text, the viewer line count is known
                    iLineCount = iRichTextViewer->LineCount();
                    
                    CESMRRichTextLink* link = CESMRRichTextLink::NewL (
                            0, iAttachmentText->Length ( ),
                            iAttachmentText->Des(),
                            CESMRRichTextLink::ETypeAttachment,
                            CESMRRichTextLink::ETriggerKeyRight );
                    
                    CleanupStack::PushL( link );
                    iRichTextViewer->AddLinkL( link );
                    CleanupStack::Pop( link );
                    }
                }
            }
        }

    if ( KNoAttachments == iAttachmentCount )
        {
        iObserver->RemoveControl( FieldId() );
        }
    
    iDisableRedraw = ETrue;
    }

// -----------------------------------------------------------------------------
// CESMRViewerAttachmentsField::HandleRichTextLinkSelection
// -----------------------------------------------------------------------------
//
TBool CESMRViewerAttachmentsField::HandleRichTextLinkSelection(
        const CESMRRichTextLink* /*aLink*/ )
    {
    FUNC_LOG;
    
    // <cmail>
    TRAP_IGNORE( iCntMenuHdlr->ShowActionMenuL() );
    // </cmail>
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRViewerAttachmentsField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );

    if ( ( aType == EEventKey || aType == EEventKeyUp ) &&
            aEvent.iScanCode == EStdKeyDevice3 )
        {
        // Selection key was pressed --> Trigger open attachment view command
        NotifyEventL( EESMRCmdMskOpenEmailAttachment );
                
        response = EKeyWasConsumed;
        }
    else
        {
        response = iRichTextViewer->OfferKeyEventL ( aEvent, aType );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::HandleEdwinSizeEventL()
// ---------------------------------------------------------------------------
//
TBool CESMRViewerAttachmentsField::HandleEdwinSizeEventL( 
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
// CESMRViewerAttachmentsField::ListObserverSet
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::ListObserverSet()
    {
    FUNC_LOG;
    iRichTextViewer->SetListObserver( iObserver );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );
    
    if ( iRichTextViewer )
        {
        iRichTextViewer->SetFocus( aFocus );
        }

    if ( aFocus && aFocus != iFocused )
        {
        // <cmail>
        UpdateAttachmentInfoL();
    	
        if( iAttachmentInfo->AttachmentCount() > KOneAttachment )
        	{
	        ChangeMiddleSoftKeyL( EESMRCmdOpenAttachmentView,
	                              R_QTN_MSK_VIEWLIST );
        	}
       	else
       		{
            // There is one attachment
            const CESMRAttachment& attachment(
                    iAttachmentInfo->AttachmentL( 0 ) );
       		
       		if( attachment.AttachmenState() == CESMRAttachment::EAttachmentStateDownloaded )
       			{
       			ChangeMiddleSoftKeyL( EESMRCmdOpenAttachment,
	                              R_QTN_MSK_OPEN );
       			}
       		else
       			{
       			ChangeMiddleSoftKeyL( EESMRCmdDownloadAttachment,
	                              R_QTN_MSK_DOWNLOAD );
       			}
       		}       		
        // </cmail>
        }
    iFocused = aFocus;    
    }

// <cmail>    
// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::MessagePartPointerArrayCleanup
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::MessagePartPointerArrayCleanup( TAny* aArray )
    {
    FUNC_LOG;
    RPointerArray<CFSMailMessagePart>* messagePartArray =
        static_cast<RPointerArray<CFSMailMessagePart>*>( aArray );

    messagePartArray->ResetAndDestroy();
    messagePartArray->Close();
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::UpdateAttachmentInfoL
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::UpdateAttachmentInfoL()
    {
    if ( iMailMessage->IsFlagSet( EFSMsgFlag_Attachments ) )
        {
        RPointerArray<CFSMailMessagePart> attachmentParts;
        CleanupStack::PushL(
                TCleanupItem(
                    MessagePartPointerArrayCleanup,
                    &attachmentParts) );

        iMailMessage->AttachmentListL( attachmentParts );

        // find  calendar part from the list
        CFSMailMessagePart* calendarPart = 
            iMailMessage->FindBodyPartL( KFSMailContentTypeTextCalendar );
        CleanupStack::PushL( calendarPart );
        
        if ( calendarPart )
            {
            // remove calendar body part from attachment list
            for ( TInt ii = attachmentParts.Count() - 1; ii >= 0; --ii )
                {
                if ( attachmentParts[ii]->GetPartId() == calendarPart->GetPartId() )
                    {
                    delete attachmentParts[ii];
                    attachmentParts.Remove(ii);
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( calendarPart );

        iAttachmentCount = attachmentParts.Count();
        if ( iAttachmentCount > 0 )
            {
            delete iAttachmentInfo;
            iAttachmentInfo = NULL;

            CESMRAttachmentInfo* attachmentInfo = CESMRAttachmentInfo::NewL();
            CleanupStack::PushL( attachmentInfo );

            for( TInt i(0); i < iAttachmentCount; ++i )
                {
                CESMRAttachment::TESMRAttachmentState state(
                        CESMRAttachment::EAttachmentStateDownloaded );

                if ( EFSFull != attachmentParts[i]->FetchLoadState() )
                    {
                    state = CESMRAttachment::EAttachmentStateNotDownloaded;
                    }

                TInt contentSize( attachmentParts[i]->ContentSize() );
                TPtrC attachmentName( attachmentParts[i]->AttachmentNameL() );
                if ( contentSize > 0 && attachmentName.Length() )
                    {
                    attachmentInfo->AddAttachmentInfoL(
                            attachmentName,
                            contentSize,
                            state );
                    }
                }

            if ( attachmentInfo->AttachmentCount() )
                {
                iAttachmentInfo = attachmentInfo;
                CleanupStack::Pop( attachmentInfo );
                }
            else
                {
                CleanupStack::PopAndDestroy( attachmentInfo );
                }

            attachmentInfo = NULL;
            }
        CleanupStack::PopAndDestroy(); // attachmentparts

        // relay attachment info to contact menu so we can display
        // options based on attachment state
        if ( iAttachmentInfo )
            {
            iCntMenuHdlr->SetAttachmentInfoL( iAttachmentInfo );
            }
        }    
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;

    switch ( aCommand )
        {
       	case EESMRCmdOpenAttachment:// Fall through
        case EESMRCmdOpenAttachmentView:// Fall through
        case EESMRCmdDownloadAttachment: // Fall through
            {
            NotifyEventL( aCommand );
            
            break;
            }
        case EESMRCmdClipboardCopy:
            {
            iRichTextViewer->CopyCurrentLinkValueToClipBoardL();
            break;
            }
         default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRRichTextViewer::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::ProcessCommandL( TInt aCommandId )
    {
    FUNC_LOG;

    NotifyEventL( aCommandId );
    }


// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRViewerAttachmentsField::CountComponentControls() const
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
// CESMRViewerAttachmentsField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerAttachmentsField::ComponentControl( 
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
// CESMRViewerAttachmentsField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::SizeChanged( )
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
// CESMRViewerAttachmentsField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRViewerAttachmentsField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iRichTextViewer->SetContainerWindowL( aContainer );
    }

// ---------------------------------------------------------------------------
// CESMRViewerAttachmentsField::RichTextViewerRect
// ---------------------------------------------------------------------------
//
TRect CESMRViewerAttachmentsField::RichTextViewerRect()
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
