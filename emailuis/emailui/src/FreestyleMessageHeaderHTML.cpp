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
* Description:  Message header HTML
*
*/

#include <s32strm.h>
#include <f32file.h>
#include <s32file.h>
#include <e32cmn.h>
#include <utf.h>
#include <StringLoader.h>

#include <FreestyleEmailUi.rsg>

#include "FreestyleMessageHeaderHTML.h"
#include "FreestyleEmailUiUtilities.h"
#include "ncsconstants.h"



_LIT8( KShowDetailIconFileName, "plus.gif");
_LIT8( KAttachementIconGeneral, "attachment.gif");

_LIT8( KHeaderTableName, "header_table");
_LIT8( KToTableName, "to_table");
_LIT8( KCcTableName, "cc_table");
_LIT8( KBccTableName, "bcc_table");
_LIT8( KAttachmentTableName, "attachment_table");
_LIT8( KFromTableName,"from_table");

_LIT8( KFromFieldName, "from_field");
_LIT8( KToFieldName, "to_field");
_LIT8( KCcFieldName, "cc_field");
_LIT8( KBccFieldName, "bcc_field");
_LIT8( KSentFieldName, "sent_field");
_LIT8( KSubjectFieldName, "subject_field");

_LIT8( KToImageName, "to_img");
_LIT8( KCcImageName, "cc_img");
_LIT8( KBccImageName, "bcc_img");
_LIT8( KAttachmentImageName, "attachment_img");
_LIT8( KDetailImageName, "detail_img");

_LIT8( KAttachmentSizeUnit, "kb");
_LIT8( KSpace8, " ");

_LIT8( KMetaHeader, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n" );
const TInt KMaxEventLength( 256 );
const TInt KFreestyleMessageHeaderHTMLRightMarginInPx( 10 );
const TInt KFreestyleMessageHeaderHTMLMaxBufferSizeForWidth( 5 );

EXPORT_C CFreestyleMessageHeaderHTML* CFreestyleMessageHeaderHTML::NewL( CFSMailMessage& aMailMessage, TInt aVisibleWidth   )
    {
    CFreestyleMessageHeaderHTML* self = new (ELeave) CFreestyleMessageHeaderHTML( aMailMessage, aVisibleWidth  );
    self->ConstructL();
    return self;
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, RWriteStream& aWriteStream, TInt aVisibleWidth   )
    {
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, aVisibleWidth   );
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL( aWriteStream );
    CleanupStack::PopAndDestroy( headerHtml );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, RFile& aFile, TInt aVisibleWidth   )
    {
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, aVisibleWidth   );
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL( aFile );
    CleanupStack::PopAndDestroy( headerHtml );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, RFs& aFs, const TPath& aFilePath, TInt aVisibleWidth  )
    {
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, aVisibleWidth   );
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL( aFs, aFilePath );
    CleanupStack::PopAndDestroy( headerHtml );
    }

CFreestyleMessageHeaderHTML::~CFreestyleMessageHeaderHTML()
    {
    iAttachments.ResetAndDestroy();
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( RWriteStream& aWriteStream ) const
    {
    HTMLStartL( aWriteStream );
    ExportHTMLHeaderL( aWriteStream );
    ExportHTMLBodyL( aWriteStream );
    HTMLEndL( aWriteStream );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( RFile& aFile ) const
    {
    RFileWriteStream fwstream;
    fwstream.Attach( aFile, 0 );
    CleanupClosePushL( fwstream );
    ExportL( fwstream );
    CleanupStack::PopAndDestroy( &fwstream );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( RFs& aFs, const TPath& aFilePath) const
    {
    RFileWriteStream fwstream;
    User::LeaveIfError( fwstream.Replace( aFs, aFilePath, EFileStreamText | EFileWrite) );
    CleanupClosePushL( fwstream );
    ExportL( fwstream );
    CleanupStack::PopAndDestroy( &fwstream );
    }

CFreestyleMessageHeaderHTML::CFreestyleMessageHeaderHTML( CFSMailMessage& aMailMessage,  TInt aVisibleWidth  )
    : iMailMessage( aMailMessage )
    {
    iVisibleWidth = aVisibleWidth - KFreestyleMessageHeaderHTMLRightMarginInPx;
  	}
  	
void CFreestyleMessageHeaderHTML::ConstructL()
    {
    iMailMessage.AttachmentListL( iAttachments );
    }

void CFreestyleMessageHeaderHTML::HTMLStartL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL(_L8("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::HTMLEndL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL(_L8("</html>\n"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportHTMLHeaderL( RWriteStream& aWriteStream ) const
    {
    HTMLHeaderStartL( aWriteStream );
    HTMLMetaL( aWriteStream );
    aWriteStream.WriteL(_L8("<title>Email Header</title>\n"));
    AddJavascriptL( aWriteStream );
    AddStyleSheetL( aWriteStream );
    HTMLHeaderEndL( aWriteStream );
    }

void CFreestyleMessageHeaderHTML::HTMLHeaderStartL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL(_L8("<head>"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::HTMLMetaL( RWriteStream& aWriteStream ) const
    {
    // Html file representing email header fields, is always constructed locally
    // in the phone, and it is always of charset UTF-8 irrespective of what
    // the email html-format body is
    aWriteStream.WriteL( KMetaHeader );
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::HTMLHeaderEndL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL(_L8("</head>\n"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportHTMLBodyL( RWriteStream& aWriteStream ) const
    {
    HTMLBodyStartL( aWriteStream );
    ExportInitialTableL( aWriteStream );
    StartHeaderTableL( aWriteStream, KHeaderTableName );
    ExportFromL( aWriteStream );
    ExportToL( aWriteStream );
    ExportCcL( aWriteStream );
    ExportBccL( aWriteStream );
    ExportSentTimeL( aWriteStream );
    ExportSubjectL( aWriteStream );
    EndHeaderTableL( aWriteStream );
    ExportAttachmentsL( aWriteStream );
    HTMLBodyEndL( aWriteStream );
    }

void CFreestyleMessageHeaderHTML::HTMLBodyStartL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL(_L8("<body>\n"));
    aWriteStream.CommitL();
    }
void CFreestyleMessageHeaderHTML::ExportInitialTableL( RWriteStream& aWriteStream ) const
    {

    // set the width, using the visible screen width
    TBuf8<KFreestyleMessageHeaderHTMLMaxBufferSizeForWidth> tableWidth;
    tableWidth.AppendNum( iVisibleWidth );
    aWriteStream.WriteL(_L8("<table id=\"table_initial\" border=\"0\" width=\""));
    aWriteStream.WriteL( tableWidth );
    aWriteStream.WriteL( _L8("px\">\n"));
    

    // start first row: table with the sent info and the '+' icon
    aWriteStream.WriteL(_L8("<tr><td><table id=\"table_sent_and_plus\" border=\"0\" width=\""));
    aWriteStream.WriteL( tableWidth );
    aWriteStream.WriteL( _L8("px\">\n"));
    
    aWriteStream.WriteL(_L8("<tr>\n"));
    
    // add Sent time and date
    aWriteStream.WriteL(_L8("<td id=\"sent_initial\" align=\"left\" valign=\"bottom\">"));
    
    HBufC* dateText = TFsEmailUiUtility::DateTextFromMsgLC( &iMailMessage );
    HBufC* timeText = TFsEmailUiUtility::TimeTextFromMsgLC( &iMailMessage );

    TInt len = dateText->Length() + KSentLineDateAndTimeSeparatorText().Length() + timeText->Length();
    HBufC* sentTimeText = HBufC::NewLC( len );
    TPtr sentTimeTextPtr = sentTimeText->Des();
    sentTimeTextPtr.Append( *dateText );
    sentTimeTextPtr.Append( KSentLineDateAndTimeSeparatorText );
    sentTimeTextPtr.Append( *timeText );
    HBufC8* sentTimeText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( sentTimeTextPtr );
    CleanupStack::PushL( sentTimeText8 );
    aWriteStream.WriteL( *sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText );
    CleanupStack::PopAndDestroy( timeText );
    CleanupStack::PopAndDestroy( dateText );
    
    aWriteStream.WriteL(_L8("</td>\n"));
    
    // add "show details" image on the same line as Sent time and date
    aWriteStream.WriteL(_L8("<td width=\"1\" valign=\"top\" align=\"right\" style=\"padding: 0px 10px 0px 0px;\"><image id=\"detail_img\" border=\"0\" src=\"plus.gif\" onClick=\"expandHeader()\" ></td>\n"));
        
    // finish first row
    aWriteStream.WriteL(_L8("</tr>\n"));  
    aWriteStream.WriteL(_L8("</table></td></tr>\n"));

    //=============================
    // start second row which contains subject
    aWriteStream.WriteL(_L8("<tr>\n"));
    aWriteStream.WriteL(_L8("<td id=\"subject_initial\" align=\"left\"><b>"));

    HBufC8* subject8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( iMailMessage.GetSubject() );
    CleanupStack::PushL( subject8 );
    aWriteStream.WriteL( *subject8 );
    CleanupStack::PopAndDestroy( subject8 );

    aWriteStream.WriteL(_L8("</b></td>\n"));
    aWriteStream.WriteL(_L8("</tr>\n"));  // finish subject row
    
    // end table_initial
    aWriteStream.WriteL(_L8("</table>\n"));
        
    aWriteStream.CommitL();
    }
void CFreestyleMessageHeaderHTML::HTMLBodyEndL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL(_L8("</body>\n"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportSubjectL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL( _L8("<tr id=\"") );
    aWriteStream.WriteL( KSubjectFieldName );
    aWriteStream.WriteL( _L8("\">") );


    aWriteStream.WriteL( _L8("<td width=\"1\">") );
    aWriteStream.WriteL( _L8("<b>") );
    HBufC8* subjectHeadingText = HeadingTextLC( R_FREESTYLE_EMAIL_UI_VIEWER_SUBJECT );
    aWriteStream.WriteL( *subjectHeadingText );
    CleanupStack::PopAndDestroy( subjectHeadingText );
    aWriteStream.WriteL( _L8("</b>") );
    aWriteStream.WriteL( _L8("</td>") );
    aWriteStream.WriteL( _L8("</tr>\n") );
    
    // subject text
    aWriteStream.WriteL( _L8("<tr>") );
    aWriteStream.WriteL( _L8("<td>") );

    HBufC8* subject8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( iMailMessage.GetSubject() );
    CleanupStack::PushL( subject8 );
    aWriteStream.WriteL( *subject8 );
    CleanupStack::PopAndDestroy( subject8 );

    aWriteStream.WriteL( _L8("</td>") );
    aWriteStream.WriteL( _L8("</tr>\n") );

    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportFromL( RWriteStream& aWriteStream ) const
    {
    RPointerArray<CFSMailAddress> froms;
    CleanupClosePushL( froms );
    CFSMailAddress* from = iMailMessage.GetSender();  // ownership not transferred
    if ( from )
        {
        froms.AppendL( from );
        }
    ExportEmailAddressesL( aWriteStream, FreestyleMessageHeaderURLFactory::EEmailAddressTypeFrom, froms, 
                           KFromFieldName, KFromTableName, R_FREESTYLE_EMAIL_UI_VIEWER_FROM );
    CleanupStack::PopAndDestroy( &froms );
    }

void CFreestyleMessageHeaderHTML::ExportToL( RWriteStream& aWriteStream ) const
    {
    RPointerArray<CFSMailAddress>& recipients = iMailMessage.GetToRecipients();
    ExportEmailAddressesL( aWriteStream, FreestyleMessageHeaderURLFactory::EEmailAddressTypeTo, recipients,
                           KToFieldName, KToTableName, R_FREESTYLE_EMAIL_UI_VIEWER_TO );
    }

void CFreestyleMessageHeaderHTML::ExportCcL( RWriteStream& aWriteStream ) const
    {
    RPointerArray<CFSMailAddress>& recipients = iMailMessage.GetCCRecipients();
    ExportEmailAddressesL( aWriteStream, FreestyleMessageHeaderURLFactory::EEmailAddressTypeCc, recipients,
                           KCcFieldName, KCcTableName, R_FREESTYLE_EMAIL_UI_VIEWER_CC );
    }

void CFreestyleMessageHeaderHTML::ExportBccL( RWriteStream& aWriteStream ) const
    {
    RPointerArray<CFSMailAddress>& recipients = iMailMessage.GetBCCRecipients();
    ExportEmailAddressesL( aWriteStream, FreestyleMessageHeaderURLFactory::EEmailAddressTypeBcc, recipients,
                           KBccFieldName, KBccTableName, R_FREESTYLE_EMAIL_UI_VIEWER_BCC );
    }

void CFreestyleMessageHeaderHTML::ExportSentTimeL( RWriteStream& aWriteStream ) const
    {

    aWriteStream.WriteL( _L8("<tr id=\"") );
    aWriteStream.WriteL( KSentFieldName );
    aWriteStream.WriteL( _L8("\">") );


    aWriteStream.WriteL( _L8("<td width=\"1\">") );
    aWriteStream.WriteL( _L8("<b>") );
    HBufC8* sentHeadingText = HeadingTextLC( R_FREESTYLE_EMAIL_UI_VIEWER_SENT );
    aWriteStream.WriteL( *sentHeadingText );
    CleanupStack::PopAndDestroy( sentHeadingText );
    aWriteStream.WriteL( _L8("</b>") );
    aWriteStream.WriteL( _L8("</td>") );
    aWriteStream.WriteL( _L8("</tr>\n") );

    aWriteStream.WriteL( _L8("<tr>") );
    aWriteStream.WriteL( _L8("<td>") );

    HBufC* dateText = TFsEmailUiUtility::DateTextFromMsgLC( &iMailMessage );
    HBufC* timeText = TFsEmailUiUtility::TimeTextFromMsgLC( &iMailMessage );

    TInt len = dateText->Length() + KSentLineDateAndTimeSeparatorText().Length() + timeText->Length();
    HBufC* sentTimeText = HBufC::NewLC( len );
    TPtr sentTimeTextPtr = sentTimeText->Des();
    sentTimeTextPtr.Append( *dateText );
    sentTimeTextPtr.Append( KSentLineDateAndTimeSeparatorText );
    sentTimeTextPtr.Append( *timeText );
    HBufC8* sentTimeText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( sentTimeTextPtr );
    CleanupStack::PushL( sentTimeText8 );
    aWriteStream.WriteL( *sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText );
    CleanupStack::PopAndDestroy( timeText );
    CleanupStack::PopAndDestroy( dateText );

    aWriteStream.WriteL( _L8("</td>") );

    aWriteStream.WriteL( _L8("</tr>\n") );

    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportAttachmentsL( RWriteStream& aWriteStream ) const
    {

    TInt attachmentsCount( iAttachments.Count() );
    if ( attachmentsCount )
        {
        // The attachments table consists of one row that contains 2 cells
        // first cell contains the attachment icon
        // second cell contains a table which contains the attachments list

        // start attachments table
        aWriteStream.WriteL( _L8("<table id=\"") ); 
        aWriteStream.WriteL( KAttachmentTableName );
        aWriteStream.WriteL( _L8("\" border=\"0\" width=\"100%\">\n") );  // width is set at 100% intentionally
        
        // start row
        aWriteStream.WriteL( _L8("<tr>\n") );
          
        // add attachment icon
        aWriteStream.WriteL( _L8("<td width=\"1\" valign=\"top\" align=\"right\"><image src=\"") );
        aWriteStream.WriteL( KAttachementIconGeneral );
        aWriteStream.WriteL( _L8("\" ></td>\n") );
        
        // start table of attachments as a table within a cell
        aWriteStream.WriteL( _L8("<td>\n") );
        aWriteStream.WriteL(_L8("<table id=\"table_attachments_list\" border=\"0\" width=\"100%\">\n"));
        for (TInt i=0; i < attachmentsCount; i++)
            {
            AddAttachmentL( aWriteStream, *iAttachments[i] );
            }

        aWriteStream.WriteL(_L8("</table>\n")); // end table_attachments_list
        aWriteStream.WriteL( _L8("</td>\n") );
        
        aWriteStream.WriteL( _L8("</tr>\n") );
        aWriteStream.WriteL(_L8("</table>\n")); // end attachments table
        }
    }

void CFreestyleMessageHeaderHTML::ExportEmailAddressesL( RWriteStream& aWriteStream, 
                                                        FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType, 
                                                        const RPointerArray<CFSMailAddress>& aEmailAddresses,
                                                        const TDesC8& aRowId,
                                                        const TDesC8& /*aTableId*/,
                                                        TInt aHeaderTextResourceId ) const
    {
    if ( aEmailAddresses.Count() )
        {
        // begin table row
        aWriteStream.WriteL( _L8("<tr id=\"")); 
        aWriteStream.WriteL( aRowId );
        aWriteStream.WriteL( _L8("\">") );
        
        // heading text
        aWriteStream.WriteL( _L8("<td><b>"));
        HBufC8* headingText = HeadingTextLC( aHeaderTextResourceId );
        aWriteStream.WriteL( *headingText );
        CleanupStack::PopAndDestroy( headingText );
        aWriteStream.WriteL( _L8("</b></td>"));
        
        aWriteStream.WriteL( _L8("</tr>\n") );  // end table row      
        
        // add addresses, one address per row
        TInt count( aEmailAddresses.Count() );
        for (TInt i = 0; i < count; ++i )
            {
            aWriteStream.WriteL( _L8("<tr><td style=\"padding: 0px 0px 7px 0px;\">") );
            AddEmailAddressL (aWriteStream, aEmailAddressType, *aEmailAddresses[i] );
            aWriteStream.WriteL( _L8("</td></tr>\n") );
            }

        aWriteStream.CommitL();
        }
    }

void CFreestyleMessageHeaderHTML::AddEmailAddressL( RWriteStream& aWriteStream,
        FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType,
        const CFSMailAddress& aEmailAddress ) const
    {
    CFreestyleMessageHeaderURL* emailUrl = FreestyleMessageHeaderURLFactory::CreateEmailAddressUrlL( aEmailAddressType, aEmailAddress );
    CleanupStack::PushL( emailUrl );

    HBufC* url = emailUrl->ExternalizeL();
    CleanupStack::PushL( url );
    HBufC8* url8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *url );
    CleanupStack::PushL( url8 );
    StartHyperlinkL( aWriteStream, *url8 );
    CleanupStack::PopAndDestroy( url8 );
    CleanupStack::PopAndDestroy( url );

    HBufC8* displayName8 = NULL;
    if ( aEmailAddress.GetDisplayName().Length() > 0 )
        {
        displayName8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aEmailAddress.GetDisplayName() );
        }
    else
        {
        displayName8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aEmailAddress.GetEmailAddress() );
        }
    CleanupStack::PushL( displayName8 );
    aWriteStream.WriteL( *displayName8 );
    CleanupStack::PopAndDestroy( displayName8 );

    EndHyperlinkL( aWriteStream );

    CleanupStack::PopAndDestroy( emailUrl );
    }

void CFreestyleMessageHeaderHTML::AddAttachmentL( RWriteStream& aWriteStream, CFSMailMessagePart& aAttachment ) const
    {
    aWriteStream.WriteL( _L8("<tr><td style=\"padding: 0px 0px 7px 0px;\">") ); // pad bottom to allow some space between the lines

    TUint id = aAttachment.GetPartId().Id();
    TBuf<32> itemId;
    itemId.AppendNum( id );
    CFreestyleMessageHeaderURL* attnUrl = FreestyleMessageHeaderURLFactory::CreateAttachmentUrlL( itemId );
    CleanupStack::PushL( attnUrl );
    HBufC* attnUrlText = attnUrl->ExternalizeL();
    CleanupStack::PushL( attnUrlText );
    HBufC8* attnUrlText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *attnUrlText );
    CleanupStack::PushL( attnUrlText8 );
    StartHyperlinkL( aWriteStream, *attnUrlText8 );
    CleanupStack::PopAndDestroy( attnUrlText8 );
    CleanupStack::PopAndDestroy( attnUrlText );
    CleanupStack::PopAndDestroy( attnUrl );

    TDesC& attnName = aAttachment.AttachmentNameL();
    HBufC8* attnName8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( attnName );
    CleanupStack::PushL( attnName8 );
    aWriteStream.WriteL( *attnName8 );
    CleanupStack::PopAndDestroy( attnName8 );

    TUint size = aAttachment.ContentSize();
    TUint sizeInKB = size / 1024;
    
    if ( size % 1024 )
      {
      // round up
      ++sizeInKB;
      }
    
    TBuf8<32> sizeText;
    sizeText.Append( KSpace8 );
    sizeText.Append( _L8("(") );
    sizeText.AppendNum( sizeInKB );
    sizeText.Append( KSpace8 );
    sizeText.Append( KAttachmentSizeUnit );
    sizeText.Append( _L8(")") );
    aWriteStream.WriteL( sizeText );

    EndHyperlinkL( aWriteStream );

    aWriteStream.WriteL( _L8("</td></tr>\n") );
    aWriteStream.CommitL();
    }


void CFreestyleMessageHeaderHTML::StartHyperlinkL( RWriteStream& aWriteStream, const TDesC8& aUrl ) const
    {
    aWriteStream.WriteL( _L8("<a href=\"") );
    aWriteStream.WriteL( aUrl );
    aWriteStream.WriteL( _L8("\">"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::EndHyperlinkL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL( _L8("</a>") );
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddImageL( RWriteStream& aWriteStream, const TDesC8& aImageUrl ) const
    {
    aWriteStream.WriteL( _L8("<image border=\"0\" src=\"") );
    aWriteStream.WriteL( aImageUrl );
    aWriteStream.WriteL( _L8("\">"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddImageL( RWriteStream& aWriteStream,
        const TDesC8& aImageId,
        const TDesC8& aImageUrl,
        const TDesC8& aImageEvent ) const
    {
    aWriteStream.WriteL( _L8("<image id=\"") );
    aWriteStream.WriteL( aImageId );
    aWriteStream.WriteL( _L8("\" ") );
    aWriteStream.WriteL( _L8("border=\"0\" src=\"") );
    aWriteStream.WriteL( aImageUrl );
    aWriteStream.WriteL( _L8("\" "));
    aWriteStream.WriteL( aImageEvent );
    aWriteStream.WriteL( _L8(">"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddJavascriptL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL( _L8("<script language=\"javascript\" src=\"header.js\"></script>\n"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::StartHeaderTableL( RWriteStream& aWriteStream, const TDesC8& aTableId ) const
    {
    aWriteStream.WriteL( _L8("<table id=\"") );
    aWriteStream.WriteL( aTableId );
    
    // use style="display:none" so that full header table is hidden initially
    aWriteStream.WriteL( _L8("\" border=\"0\" width=\"100%\" style=\"display: none\">\n") );
    
    
    TBuf8<KFreestyleMessageHeaderHTMLMaxBufferSizeForWidth> tableWidth;
    tableWidth.AppendNum( iVisibleWidth );

    // Add "hide details" image as its own table with its own width
    aWriteStream.WriteL( _L8("<tr><td>\n"));
    aWriteStream.WriteL(_L8("<table id =\"table_minus_icon\" border=\"0\" width=\""));
    aWriteStream.WriteL( tableWidth );
    aWriteStream.WriteL( _L8("px\">\n"));
    aWriteStream.WriteL( _L8("<tr>\n"));
    aWriteStream.WriteL( _L8("<td valign=\"top\" align=\"right\" style=\"padding: 0px 10px 0px 0px;\"><image id=\"hideDetails_img\" border=\"0\" src=\"minus.gif\" onClick=\"collapseHeader()\"></td>\n"));
    aWriteStream.WriteL( _L8("</tr>\n"));
    aWriteStream.WriteL( _L8("</table></td></tr>\n"));
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::EndHeaderTableL( RWriteStream& aWriteStream ) const
    {
    EndTableL( aWriteStream );
    }

void CFreestyleMessageHeaderHTML::StartTableL( RWriteStream& aWriteStream, const TDesC8& aTableId ) const
    {
    aWriteStream.WriteL( _L8("<table id=\"") );
    aWriteStream.WriteL( aTableId );
    aWriteStream.WriteL( _L8("\" border=\"0\">\n") );
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::EndTableL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL( _L8("</table>\n") );
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddShowDetailL( RWriteStream& aWriteStream ) const
    {
    HBufC8* event = ClickImageEventL( KDetailImageName );
    CleanupStack::PushL( event );
    AddImageL( aWriteStream, KDetailImageName, KShowDetailIconFileName, *event );
    CleanupStack::PopAndDestroy( event );
    aWriteStream.CommitL();
    }

HBufC8* CFreestyleMessageHeaderHTML::ClickImageEventL( const TDesC8& aImageName ) const
    {
    TBuf8<KMaxEventLength> event;
    if ( aImageName.Compare( KToImageName ) == 0 )
        {
        event.Append( _L8("onClick=\"toggleField('") );
        event.Append( KToTableName );
        event.Append( _L8("', '") );
        event.Append( KToImageName );
        event.Append( _L8("')\"") );
        }
    else if ( aImageName.Compare( KCcImageName ) == 0 )
        {
        event.Append( _L8("onClick=\"toggleField('") );
        event.Append( KCcTableName );
        event.Append( _L8("', '") );
        event.Append( KCcImageName );
        event.Append( _L8("')\"") );
        }
    else if ( aImageName.Compare( KBccImageName ) == 0 )
        {
        event.Append( _L8("onClick=\"toggleField('") );
        event.Append( KBccTableName );
        event.Append( _L8("', '") );
        event.Append( KBccImageName );
        event.Append( _L8("')\"") );
        }
    else if ( aImageName.Compare( KDetailImageName ) == 0 )
        {
        event.Append( _L8("onClick=\"toggleHeader('") );
        event.Append( KHeaderTableName );
        event.Append( _L8("', '") );
        event.Append( KDetailImageName );
        event.Append( _L8("')\"") );
        }
    else if ( aImageName.Compare( KAttachmentImageName ) == 0 )
        {
        event.Append( _L8("onClick=\"toggleField('") );
        event.Append( KAttachmentTableName );
        event.Append( _L8("', '") );
        event.Append( KAttachmentImageName );
        event.Append( _L8("')\"") );
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    return event.AllocL();
    }

HBufC8* CFreestyleMessageHeaderHTML::HeadingTextLC( TInt aId ) const
    {
    HBufC* headingText = StringLoader::LoadLC( aId );
    HBufC8* headingText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *headingText );
    CleanupStack::PopAndDestroy( headingText );
    CleanupStack::PushL( headingText8 );
    return headingText8;
    }

HBufC8* CFreestyleMessageHeaderHTML::HeadingTextLC( TInt aId, TInt aSize ) const
    {
    HBufC* headingText = StringLoader::LoadLC( aId, aSize );
    HBufC8* headingText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *headingText );
    CleanupStack::PopAndDestroy( headingText );
    CleanupStack::PushL( headingText8 );
    return headingText8;
    }

void CFreestyleMessageHeaderHTML::AddStyleSheetL( RWriteStream& aWriteStream ) const
    {
    // Add an internal style sheet
    // If the style becomes numerous or complicated, consider using an external style sheet
    
    aWriteStream.WriteL( _L8("<style type=\"text/css\">\n") );
    
    // define a div class "header", specifying the background color
    // for the email header part
    
    // In future, query for which background color to use
    aWriteStream.WriteL( _L8("body { background-color: lightblue; }\n") );
    
    // set font size to 75% of the default size
    // because, at the default size, the header text is too big relative to the text in the email body
    // Note: since the text in the body is too small at "normal" level, 
    // we have the text size level in the browser set to "Larger" which is 20% larger than the specified size
    // the "larger" size affects all text which includes the header.
    aWriteStream.WriteL( _L8("td { font-family:arial,sans-serif ; font-size:75% }\n"));
    
    aWriteStream.WriteL( _L8("</style>\n") );
    aWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::StartDivL( RWriteStream& aWriteStream ) const
    {
    // Add div, using "header" class
    aWriteStream.WriteL( _L8("<div class=\"header\">\n") );
    aWriteStream.CommitL();
    }
    
void CFreestyleMessageHeaderHTML::EndDivL( RWriteStream& aWriteStream ) const
    {
    aWriteStream.WriteL( _L8("</div>\n") );
    aWriteStream.CommitL();
    }


                                
