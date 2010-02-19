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
#include <finditemengine.h>
#include "FreestyleMessageHeaderHTML.h"
#include "FreestyleEmailUiUtilities.h"
#include "ncsconstants.h"



_LIT8( KShowDetailIconFileName, "plus.gif");
_LIT8( KAttachementIconGeneral, "attachment.gif");
_LIT8( KFollowUpIconFileName, "follow_up.png");
_LIT8( KFollowUpCompleteIconFileName, "follow_up_complete.png");
_LIT8( KPriorityHighIconFileName, "todo_high_add.png");
_LIT8( KPriorityLowIconFileName, "todo_low_add.png");

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
_LIT8( KFollowUpImageName, "follow_up_img");
_LIT8( KFollowUpCompleteImageName, "follow_up_complete_img");
_LIT8( KPriorityHighImageName, "todo_high_add_img");
_LIT8( KPriorityLowImageName, "todo_low_add_img");

_LIT8( KAttachmentSizeUnit, "kb");
_LIT8( KSpace8, " ");

_LIT8( KHTMLImgTagId, "<image id=\"" );
_LIT8( KHTMLImgTagSrcBefore, "\" border=\"0\" src=\"" );
_LIT8( KHTMLImgTagSrcAfter, "\">" );

_LIT8( KMetaHeader, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n" );
const TInt KMaxEventLength( 256 );
const TInt KFreestyleMessageHeaderHTMLRightMarginInPx( 10 );
const TInt KFreestyleMessageHeaderHTMLMaxBufferSizeForWidth( 5 );

EXPORT_C CFreestyleMessageHeaderHTML* CFreestyleMessageHeaderHTML::NewL( CFSMailMessage& aMailMessage, 
                                                                         RWriteStream& aWriteStream,
                                                                         TInt aVisibleWidth,
                                                                         TInt aScrollPosition,
                                                                         TBidiText::TDirectionality aDirectionality
                                                                         )
    {
    CFreestyleMessageHeaderHTML* self = new (ELeave) CFreestyleMessageHeaderHTML( aMailMessage, aWriteStream, aVisibleWidth, aScrollPosition, aDirectionality);
    self->ConstructL();
    return self;
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, 
                                                    RWriteStream& aWriteStream, 
                                                    TInt aVisibleWidth,
                                                    TInt aScrollPosition,
                                                    TBidiText::TDirectionality aDirectionality)
    {
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, aWriteStream, aVisibleWidth, aScrollPosition, aDirectionality);
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL();
    CleanupStack::PopAndDestroy( headerHtml );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, 
                                                    RFile& aFile, 
                                                    TInt aVisibleWidth,
                                                    TInt aScrollPosition,
                                                    TBidiText::TDirectionality aDirectionality)
    {
    RFileWriteStream fwstream;
    fwstream.Attach( aFile, 0 );
    CleanupClosePushL( fwstream );
    
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, fwstream, aVisibleWidth, aScrollPosition, aDirectionality );
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL();
    CleanupStack::PopAndDestroy( headerHtml );
    
    CleanupStack::PopAndDestroy( &fwstream );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, 
                                                    RFs& aFs, 
                                                    const TPath& aFilePath, 
                                                    TInt aVisibleWidth,
                                                    TInt aScrollPosition,
                                                    TBidiText::TDirectionality aDirectionality)
    {
    RFileWriteStream fwstream;
    User::LeaveIfError( fwstream.Replace( aFs, aFilePath, EFileStreamText | EFileWrite) );
    CleanupClosePushL( fwstream );
    
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, fwstream, aVisibleWidth, aScrollPosition, aDirectionality);
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL();
    CleanupStack::PopAndDestroy( headerHtml );

    CleanupStack::PopAndDestroy( &fwstream );    
    }

CFreestyleMessageHeaderHTML::~CFreestyleMessageHeaderHTML()
    {
    iAttachments.ResetAndDestroy();
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL() const
    {
    HTMLStartL();
    ExportHTMLHeaderL();
    ExportHTMLBodyL();
    HTMLEndL();
    }

CFreestyleMessageHeaderHTML::CFreestyleMessageHeaderHTML( CFSMailMessage& aMailMessage,  
                                                          RWriteStream& aWriteStream,
                                                          TInt aVisibleWidth,
                                                          TInt aScrollPosition,
                                                          TBidiText::TDirectionality aDirectionality )
    : iMailMessage( aMailMessage ),
    iWriteStream( aWriteStream ),
    iVisibleWidth( aVisibleWidth - KFreestyleMessageHeaderHTMLRightMarginInPx ),
    iScrollPosition(aScrollPosition),
    iDirectionality( aDirectionality ),
    iMirrorLayout( EFalse )
    {
    }

void CFreestyleMessageHeaderHTML::ConstructL()
    {
    iMailMessage.AttachmentListL( iAttachments );
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iMirrorLayout = ETrue;
        }
    }

void CFreestyleMessageHeaderHTML::HTMLStartL() const
    {
    iWriteStream.WriteL(_L8("<html"));
    if ( iMirrorLayout )
        {
        iWriteStream.WriteL(_L8(" dir=\"rtl\""));
        }
    else
        {
        iWriteStream.WriteL(_L8(" dir=\"ltr\""));
        }
    iWriteStream.WriteL(_L8(" xmlns=\"http://www.w3.org/1999/xhtml\">\n"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::HTMLEndL() const
    {
    iWriteStream.WriteL(_L8("</html>\n"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportHTMLHeaderL() const
    {
    HTMLHeaderStartL();
    HTMLMetaL();
    iWriteStream.WriteL( _L8("<title>Email Header</title>\n") );
    AddJavascriptL();
    AddStyleSheetL();
    HTMLHeaderEndL();
    }

void CFreestyleMessageHeaderHTML::HTMLHeaderStartL() const
    {
    iWriteStream.WriteL(_L8("<head>"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::HTMLMetaL() const
    {
    // Html file representing email header fields, is always constructed locally
    // in the phone, and it is always of charset UTF-8 irrespective of what
    // the email html-format body is
    iWriteStream.WriteL( KMetaHeader );
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::HTMLHeaderEndL() const
    {
    iWriteStream.WriteL(_L8("</head>\n"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportHTMLBodyL() const
    {
    HTMLBodyStartL();
    ExportInitialTableL();
    StartHeaderTableL( KHeaderTableName );
    ExportFromL();
    ExportToL();
    ExportCcL();
    ExportBccL();
    ExportSentTimeL();
    ExportSubjectL();
    EndHeaderTableL();
    ExportAttachmentsL();
    HTMLBodyEndL();
    }

void CFreestyleMessageHeaderHTML::HTMLBodyStartL() const
    {
    TBuf8<KFreestyleMessageHeaderHTMLRightMarginInPx> scrollPos;
    scrollPos.AppendNum(iScrollPosition);
    iWriteStream.WriteL(_L8("<body onLoad = init("));
    iWriteStream.WriteL(scrollPos);
    iWriteStream.WriteL(_L8(")>\n"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportInitialTableL() const
    {
    // set the width, using the visible screen width
    TBuf8<KFreestyleMessageHeaderHTMLMaxBufferSizeForWidth> tableWidth;
    tableWidth.AppendNum( iVisibleWidth );
    iWriteStream.WriteL(_L8("<table id=\"table_initial\" border=\"0\" width=\""));
    iWriteStream.WriteL( tableWidth );
    iWriteStream.WriteL( _L8("px\">\n"));
    

    // start first row: table with the sent info and the '+' icon
    iWriteStream.WriteL(_L8("<tr><td><table id=\"table_sent_and_plus\" border=\"0\" width=\""));
    iWriteStream.WriteL( tableWidth );
    iWriteStream.WriteL( _L8("px\">\n"));
    
    iWriteStream.WriteL(_L8("<tr>\n"));
    
    // add Sent time and date
    iWriteStream.WriteL(_L8("<td id=\"sent_initial\""));

    if ( !iMirrorLayout )
        {
        iWriteStream.WriteL(_L8(" align=\"left\""));
        }
    else
        {
        iWriteStream.WriteL(_L8(" align=\"right\""));
        }
    iWriteStream.WriteL(_L8(" valign=\"bottom\">"));
    
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
    iWriteStream.WriteL( *sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText );
    CleanupStack::PopAndDestroy( timeText );
    CleanupStack::PopAndDestroy( dateText );
    
    iWriteStream.WriteL(_L8("</td>\n"));
    
    // add "show details" image on the same line as Sent time and date
    iWriteStream.WriteL(_L8("<td width=\"1\" valign=\"top\""));
    if ( !iMirrorLayout )
        {
        iWriteStream.WriteL(_L8(" align=\"right\""));
        }
    else
        {
        iWriteStream.WriteL(_L8(" align=\"left\""));
        }
    iWriteStream.WriteL(_L8(" style=\"padding: 0px 10px 0px 0px;\"><image id=\"detail_img\" border=\"0\" src=\"plus.gif\" onClick=\"expandHeader()\" ></td>\n"));

    
    // finish first row
    iWriteStream.WriteL(_L8("</tr>\n"));  
    iWriteStream.WriteL(_L8("</table></td></tr>\n"));

    //=============================
    // start second row which contains subject
    iWriteStream.WriteL(_L8("<tr>\n"));
    iWriteStream.WriteL(_L8("<td id=\"subject_initial\""));
    if ( !iMirrorLayout )
        {
        iWriteStream.WriteL(_L8(" align=\"left\""));
        }
    else
        {
        iWriteStream.WriteL(_L8(" align=\"right\""));
        }
    iWriteStream.WriteL(_L8("><b>"));

	HBufC* subject = iMailMessage.GetSubject().Alloc();
	/*
     * Writes the subject to iWriteStream and also
     * takes care of the urls and marks them as hotspots
     */
    WriteSubjectL(*subject);

    iWriteStream.WriteL(_L8("</b>"));
        
    // Write icons (if necessary).
    HBufC8* followUp = HTMLHeaderFollowUpIconLC( EFalse );
    HBufC8* priority = HTMLHeaderPriorityIconLC( EFalse );
        
    if ( priority )
        {
        iWriteStream.WriteL( *priority );
        CleanupStack::PopAndDestroy( priority);
        }
    
    if ( followUp )
        {
        iWriteStream.WriteL( *followUp );
        CleanupStack::PopAndDestroy( followUp );
        }
    
    iWriteStream.WriteL(_L8("</td></tr>\n"));  // finish subject row
    
    // end table_initial
    iWriteStream.WriteL(_L8("</table>\n"));
        
    iWriteStream.CommitL();
    }
// -----------------------------------------------------------------------------
// CFreestyleMessageHeaderHTML::WriteSubjectL
// Writes the subject to iWriteStream and also
// takes care of the urls and marks them as hotspots
// -----------------------------------------------------------------------------
//
void CFreestyleMessageHeaderHTML::WriteSubjectL(TDesC& aText ) const
    {
    /* 
    * Add these to searchcases to make it also search 
    * for emailaddress and phonenumber :
    * CFindItemEngine::EFindItemSearchMailAddressBin |
    * CFindItemEngine::EFindItemSearchPhoneNumberBin |
    */
    // Search for urls    
    TInt searchCases = CFindItemEngine::EFindItemSearchURLBin;
                       
    CFindItemEngine* itemEngine =
              CFindItemEngine::NewL ( aText,
                       ( CFindItemEngine::TFindItemSearchCase ) searchCases );
    CleanupStack::PushL (itemEngine );

    const CArrayFixFlat<CFindItemEngine::SFoundItem>
            * foundItems = itemEngine->ItemArray ( );
    
    TInt index = 0;
    // For each found item
    for (TInt i=0; i<foundItems->Count ( ); ++i )
        {
        // iItemType, iStartPos, iLength
        const CFindItemEngine::SFoundItem& item = foundItems->At (i );
        HBufC* valueBuf = aText.Mid (item.iStartPos, item.iLength ).AllocL ( );
        CleanupStack::PushL (valueBuf );
        // We write the normal text to iWriteStream before and between the links in the header subject field
        if(item.iStartPos > 0)
            {
            TInt itemstart = item.iStartPos;
            HBufC* normalText = aText.Mid(index, itemstart-index).Alloc();
            CleanupStack::PushL( normalText );
            HBufC8* normalText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *normalText );
            CleanupStack::PushL( normalText8 );
            iWriteStream.WriteL( *normalText8 );
            CleanupStack::PopAndDestroy( 2 ); //normalText8m, normalText
            }        
        switch (item.iItemType )
            {
            /* To make header subjectfield to recognise also phonenumber and emailadress just 
             * add CFindItemEngine::EFindItemSearchMailAddressBin & 
             * CFindItemEngine::EFindItemSearchPhoneNumberBin cases here.
             */
            case CFindItemEngine::EFindItemSearchURLBin:
                {
                HBufC8* url8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *valueBuf );
                FreestyleMessageHeaderURLFactory::CreateEmailSubjectUrlL(*valueBuf);
                CleanupStack::PushL( url8 );
                StartHyperlinkL(*url8);
                iWriteStream.WriteL(*url8);
                EndHyperlinkL();
                CleanupStack::PopAndDestroy( url8 );

                break;
                }
            default:
                {
                break;
                }
            }        
        index = item.iStartPos+item.iLength;
        CleanupStack::PopAndDestroy (valueBuf );
        }
        //Write the rest of the subject to subject field if we are not 
        //at the end of the subject already, or if there wasn't any
        //url items write the whole subject field here
        if(index < aText.Length() )
            {
            HBufC* normalText = aText.Mid(index, aText.Length()-index).Alloc();
            CleanupStack::PushL( normalText );
            HBufC8* normalText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *normalText );
            CleanupStack::PushL( normalText8 );
            iWriteStream.WriteL( *normalText8 );
            CleanupStack::PopAndDestroy( 2 ); //normalText8m, normalText
            }
    CleanupStack::PopAndDestroy (itemEngine );
    }

HBufC8* CFreestyleMessageHeaderHTML::HTMLHeaderFollowUpIconLC( TBool aShowText ) const
    {
    HBufC8* followUpText8( NULL );
    HBufC8* followUpCompletedText8( NULL );
        
    // Reserve space with worst case scenario in mind. 
    TInt textLength( 0 );
    if ( aShowText )
        {
        // Follow up completed.
        HBufC* followUpCompletedText = StringLoader::LoadLC( 
                R_FREESTYLE_EMAIL_UI_VIEWER_COMPLETED );
        followUpCompletedText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( 
                *followUpCompletedText );
        CleanupStack::PopAndDestroy( followUpCompletedText );
        CleanupStack::PushL( followUpCompletedText8 );
        
        // Follow up.
        HBufC* followUpText = StringLoader::LoadLC( 
                R_FREESTYLE_EMAIL_UI_VIEWER_FOLLOWUP );
        followUpText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( 
                *followUpText );
        CleanupStack::PopAndDestroy( followUpText );
        CleanupStack::PushL( followUpText8 );
    
        textLength += KHTMLImgTagId().Length() +
                      KFollowUpCompleteImageName().Length() +
                      KHTMLImgTagSrcBefore().Length() +
                      KFollowUpCompleteIconFileName().Length() +
                      KHTMLImgTagSrcAfter().Length() +
                      ( followUpText8->Length() >= followUpCompletedText8->Length() ?
                      followUpText8->Length() : followUpCompletedText8->Length() ); 
        }
    else
        {
        // Plain icon and no text.
        textLength += KHTMLImgTagId().Length() + 
                      KFollowUpCompleteImageName().Length() +
                      KHTMLImgTagSrcBefore().Length() +
                      KFollowUpCompleteIconFileName().Length() +
                      KHTMLImgTagSrcAfter().Length();
        }

    // Allocate space.
    HBufC8* iconText8 = HBufC8::NewLC( textLength );    
        
    // Generate HTML code
    TPtr8 iconPtr( iconText8->Des() );
        
    if ( iMailMessage.IsFlagSet( EFSMsgFlag_FollowUp ) )
        {
        iconPtr.Append( KHTMLImgTagId );
        iconPtr.Append( KFollowUpImageName );
        iconPtr.Append( KHTMLImgTagSrcBefore );
        iconPtr.Append( KFollowUpIconFileName );
        iconPtr.Append( KHTMLImgTagSrcAfter );
        if ( aShowText && followUpText8 )
            {
            iconPtr.Append( followUpText8->Des() );
            }
        }
    else if ( iMailMessage.IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
        {
        iconPtr.Append( KHTMLImgTagId );
        iconPtr.Append( KFollowUpCompleteImageName );
        iconPtr.Append( KHTMLImgTagSrcBefore );
        iconPtr.Append( KFollowUpCompleteIconFileName );
        iconPtr.Append( KHTMLImgTagSrcAfter );
        if ( aShowText && followUpCompletedText8 )
            {
            iconPtr.Append( followUpCompletedText8->Des() );
            }
        }
    else
        {
        // No follow up flag set.
        CleanupStack::PopAndDestroy( iconText8 );
        iconText8 = NULL;
        }

    if ( aShowText )
        {
        if ( iconText8 )
            {
            CleanupStack::Pop( iconText8 );
            }
        CleanupStack::PopAndDestroy( followUpText8 );
        CleanupStack::PopAndDestroy( followUpCompletedText8 );
        if ( iconText8 )
            {
            CleanupStack::PushL( iconText8 );
            }
        }
    
    return iconText8;
    }

HBufC8* CFreestyleMessageHeaderHTML::HTMLHeaderPriorityIconLC( TBool aShowText ) const
    {
    HBufC8* highPrioText8( NULL );
    HBufC8* lowPrioText8( NULL );
        
    // Reserve space with worst case scenario in mind. 
    TInt textLength( 0 );
    if ( aShowText )
        {
        // High priority.
        HBufC* highPrioText = StringLoader::LoadLC( 
                R_FREESTYLE_EMAIL_UI_VIEWER_HIGH_PRIO );
        highPrioText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( 
                *highPrioText );
        CleanupStack::PopAndDestroy( highPrioText );
        CleanupStack::PushL( highPrioText8 );
    
        // Low priority.
        HBufC* lowPrioText = StringLoader::LoadLC( 
                R_FREESTYLE_EMAIL_UI_VIEWER_LOW_PRIO );
        lowPrioText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( 
                *lowPrioText );
        CleanupStack::PopAndDestroy( lowPrioText );
        CleanupStack::PushL( lowPrioText8 );
            
        textLength += KHTMLImgTagId().Length() +
                      KPriorityHighImageName().Length() +
                      KHTMLImgTagSrcBefore().Length() +
                      KPriorityHighIconFileName().Length() +
                      KHTMLImgTagSrcAfter().Length() +
                      ( lowPrioText8->Length() >= highPrioText8->Length() ?
                      lowPrioText8->Length() : highPrioText8->Length() ); 
        }
    else
        {
        // Plain icon and no text.
        textLength += KHTMLImgTagId().Length() + 
                      KPriorityHighImageName().Length() +
                      KHTMLImgTagSrcBefore().Length() +
                      KPriorityHighIconFileName().Length() +
                      KHTMLImgTagSrcAfter().Length();
        }

    // Allocate space.
    HBufC8* iconText8 = HBufC8::NewLC( textLength );    
        
    // Generate HTML code
    TPtr8 iconPtr( iconText8->Des() );
        
    if ( iMailMessage.IsFlagSet( EFSMsgFlag_Low ) )
        {
        iconPtr.Append( KHTMLImgTagId );
        iconPtr.Append( KPriorityLowImageName );
        iconPtr.Append( KHTMLImgTagSrcBefore );
        iconPtr.Append( KPriorityLowIconFileName );
        iconPtr.Append( KHTMLImgTagSrcAfter );
        if ( aShowText && lowPrioText8 )
            {
            iconPtr.Append( lowPrioText8->Des() );
            }
        }
    else if ( iMailMessage.IsFlagSet( EFSMsgFlag_Important ) )
        {
        iconPtr.Append( KHTMLImgTagId );
        iconPtr.Append( KPriorityHighImageName );
        iconPtr.Append( KHTMLImgTagSrcBefore );
        iconPtr.Append( KPriorityHighIconFileName );
        iconPtr.Append( KHTMLImgTagSrcAfter );
        if ( aShowText && highPrioText8 )
            {
            iconPtr.Append( highPrioText8->Des() );
            }
        }
    else
        {
        // No priority flag set.
        CleanupStack::PopAndDestroy( iconText8 );
        iconText8 = NULL;
        }

    if ( aShowText )
        {
        if ( iconText8 )
            {
            CleanupStack::Pop( iconText8 );
            }
        CleanupStack::PopAndDestroy( lowPrioText8 );
        CleanupStack::PopAndDestroy( highPrioText8 );
        if ( iconText8 )
            {
            CleanupStack::PushL( iconText8 );
            }
        }
    
    return iconText8;
    }

void CFreestyleMessageHeaderHTML::HTMLBodyEndL() const
    {
    iWriteStream.WriteL(_L8("</body>\n"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportSubjectL() const
    {
    iWriteStream.WriteL( _L8("<tr id=\"") );
    iWriteStream.WriteL( KSubjectFieldName );
    iWriteStream.WriteL( _L8("\">") );


    iWriteStream.WriteL( _L8("<td width=\"1\">") );
    iWriteStream.WriteL( _L8("<b>") );
    HBufC8* subjectHeadingText = HeadingTextLC( R_FREESTYLE_EMAIL_UI_VIEWER_SUBJECT );
    iWriteStream.WriteL( *subjectHeadingText );
    CleanupStack::PopAndDestroy( subjectHeadingText );
    iWriteStream.WriteL( _L8("</b>") );
    iWriteStream.WriteL( _L8("</td>") );
    iWriteStream.WriteL( _L8("</tr>\n") );
    
    // subject text
    iWriteStream.WriteL( _L8("<tr>") );
    iWriteStream.WriteL( _L8("<td>") );
	HBufC* subject = iMailMessage.GetSubject().Alloc();
    /*
     * Writes the subject to iWriteStream and also
     * takes care of the urls and marks them as hotspots
     */
    WriteSubjectL(*subject);

    iWriteStream.WriteL( _L8("</td>") );
    iWriteStream.WriteL( _L8("</tr>\n") );
    // Write icons (if necessary).
    HBufC8* followUp = HTMLHeaderFollowUpIconLC( ETrue );
    HBufC8* priority = HTMLHeaderPriorityIconLC( ETrue );
   
    if ( priority )
       {
       iWriteStream.WriteL(_L8("<tr><td>"));
       iWriteStream.WriteL( *priority );
       iWriteStream.WriteL(_L8("</td></tr>"));
       CleanupStack::PopAndDestroy( priority);
       }
   
    if ( followUp )
       {
       iWriteStream.WriteL(_L8("<tr><td>"));
       iWriteStream.WriteL( *followUp );
       iWriteStream.WriteL(_L8("</td></tr>"));
       CleanupStack::PopAndDestroy( followUp );
       }

    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportFromL() const
    {
    RPointerArray<CFSMailAddress> froms;
    CleanupClosePushL( froms );
    CFSMailAddress* from = iMailMessage.GetSender();  // ownership not transferred
    if ( from )
        {
        froms.AppendL( from );
        }
    ExportEmailAddressesL( FreestyleMessageHeaderURLFactory::EEmailAddressTypeFrom, froms, 
                           KFromFieldName, KFromTableName, R_FREESTYLE_EMAIL_UI_VIEWER_FROM );
    CleanupStack::PopAndDestroy( &froms );
    }

void CFreestyleMessageHeaderHTML::ExportToL() const
    {
    RPointerArray<CFSMailAddress>& recipients = iMailMessage.GetToRecipients();
    ExportEmailAddressesL( FreestyleMessageHeaderURLFactory::EEmailAddressTypeTo, recipients,
                           KToFieldName, KToTableName, R_FREESTYLE_EMAIL_UI_VIEWER_TO );
    }

void CFreestyleMessageHeaderHTML::ExportCcL() const
    {
    RPointerArray<CFSMailAddress>& recipients = iMailMessage.GetCCRecipients();
    ExportEmailAddressesL( FreestyleMessageHeaderURLFactory::EEmailAddressTypeCc, recipients,
                           KCcFieldName, KCcTableName, R_FREESTYLE_EMAIL_UI_VIEWER_CC );
    }

void CFreestyleMessageHeaderHTML::ExportBccL() const
    {
    RPointerArray<CFSMailAddress>& recipients = iMailMessage.GetBCCRecipients();
    ExportEmailAddressesL( FreestyleMessageHeaderURLFactory::EEmailAddressTypeBcc, recipients,
                           KBccFieldName, KBccTableName, R_FREESTYLE_EMAIL_UI_VIEWER_BCC );
    }

void CFreestyleMessageHeaderHTML::ExportSentTimeL() const
    {

    iWriteStream.WriteL( _L8("<tr id=\"") );
    iWriteStream.WriteL( KSentFieldName );
    iWriteStream.WriteL( _L8("\">") );


    iWriteStream.WriteL( _L8("<td width=\"1\">") );
    iWriteStream.WriteL( _L8("<b>") );
    HBufC8* sentHeadingText = HeadingTextLC( R_FREESTYLE_EMAIL_UI_VIEWER_SENT );
    iWriteStream.WriteL( *sentHeadingText );
    CleanupStack::PopAndDestroy( sentHeadingText );
    iWriteStream.WriteL( _L8("</b>") );
    iWriteStream.WriteL( _L8("</td>") );
    iWriteStream.WriteL( _L8("</tr>\n") );

    iWriteStream.WriteL( _L8("<tr>") );
    iWriteStream.WriteL( _L8("<td>") );

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
    iWriteStream.WriteL( *sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText8 );
    CleanupStack::PopAndDestroy( sentTimeText );
    CleanupStack::PopAndDestroy( timeText );
    CleanupStack::PopAndDestroy( dateText );

    iWriteStream.WriteL( _L8("</td>") );

    iWriteStream.WriteL( _L8("</tr>\n") );

    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportAttachmentsL() const
    {
    TInt attachmentsCount (iAttachments.Count());
    if ( attachmentsCount)
        {
        // The attachments table consists of one row that contains 2 cells
        // first cell contains the attachment icon
        // second cell contains a table which contains the attachments list

        // start attachments table
        iWriteStream.WriteL( _L8("<table id=\"") ); 
        iWriteStream.WriteL( KAttachmentTableName );
        iWriteStream.WriteL( _L8("\" border=\"0\" width=\"100%\">\n") );  // width is set at 100% intentionally
        
        // start row
        iWriteStream.WriteL( _L8("<tr>\n") );
          
        // add attachment icon
        iWriteStream.WriteL( _L8("<td width=\"1\" valign=\"top\"") );
        if ( !iMirrorLayout )
            {
            iWriteStream.WriteL(_L8(" align=\"left\""));
            }
        else
            {
            iWriteStream.WriteL(_L8(" align=\"right\""));
            }
        iWriteStream.WriteL( _L8("><image src=\"") );
        
        iWriteStream.WriteL( KAttachementIconGeneral );
        iWriteStream.WriteL( _L8("\" ></td>\n") );
        
        // start table of attachments as a table within a cell
        iWriteStream.WriteL( _L8("<td>\n") );
        iWriteStream.WriteL(_L8("<table id=\"table_attachments_list\" border=\"0\" width=\"100%\">\n"));
        for (TInt i=0; i < attachmentsCount; i++)
            {
            AddAttachmentL( *iAttachments[i] );
            }

        iWriteStream.WriteL(_L8("</table>\n")); // end table_attachments_list
        iWriteStream.WriteL( _L8("</td>\n") );
        
        iWriteStream.WriteL( _L8("</tr>\n") );
        iWriteStream.WriteL(_L8("</table>\n")); // end attachments table
        }
    }

void CFreestyleMessageHeaderHTML::ExportEmailAddressesL(FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType, 
                                                        const RPointerArray<CFSMailAddress>& aEmailAddresses,
                                                        const TDesC8& aRowId,
                                                        const TDesC8& /*aTableId*/,
                                                        TInt aHeaderTextResourceId ) const
    {
    if ( aEmailAddresses.Count() )
        {
        // begin table row
        iWriteStream.WriteL( _L8("<tr id=\"")); 
        iWriteStream.WriteL( aRowId );
        iWriteStream.WriteL( _L8("\">") );
        
        // heading text
        iWriteStream.WriteL( _L8("<td><b>"));
        HBufC8* headingText = HeadingTextLC( aHeaderTextResourceId );
        iWriteStream.WriteL( *headingText );
        CleanupStack::PopAndDestroy( headingText );
        iWriteStream.WriteL( _L8("</b></td>"));
        
        iWriteStream.WriteL( _L8("</tr>\n") );  // end table row      
        
        // add addresses, one address per row
        TInt count( aEmailAddresses.Count() );
        for (TInt i = 0; i < count; ++i )
            {
            iWriteStream.WriteL( _L8("<tr><td style=\"padding: 0px 0px 7px 0px;\">") );
            AddEmailAddressL (aEmailAddressType, *aEmailAddresses[i] );
            iWriteStream.WriteL( _L8("</td></tr>\n") );
            }

        iWriteStream.CommitL();
        }
    }

void CFreestyleMessageHeaderHTML::AddEmailAddressL( FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType,
                                                    const CFSMailAddress& aEmailAddress ) const
    {
    CFreestyleMessageHeaderURL* emailUrl = FreestyleMessageHeaderURLFactory::CreateEmailAddressUrlL( aEmailAddressType, aEmailAddress );
    CleanupStack::PushL( emailUrl );

    HBufC* url = emailUrl->ExternalizeL();
    CleanupStack::PushL( url );
    HBufC8* url8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *url );
    CleanupStack::PushL( url8 );
    StartHyperlinkL( *url8 );
    CleanupStack::PopAndDestroy( url8 );
    CleanupStack::PopAndDestroy( url );

    HBufC8* displayName8 = NULL;
    //  ENLN-7ZVBES
    //  Display name not shown in From:, instead, email address is shown
    if ( (aEmailAddress.GetDisplayName().Length() > 0) && aEmailAddressType != FreestyleMessageHeaderURLFactory::EEmailAddressTypeFrom )
        {
        displayName8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aEmailAddress.GetDisplayName() );
        }
    else
        {
        displayName8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aEmailAddress.GetEmailAddress() );
        }
    CleanupStack::PushL( displayName8 );
    iWriteStream.WriteL( *displayName8 );
    CleanupStack::PopAndDestroy( displayName8 );

    EndHyperlinkL();

    CleanupStack::PopAndDestroy( emailUrl );
    }

void CFreestyleMessageHeaderHTML::AddAttachmentL( CFSMailMessagePart& aAttachment ) const
    {
    iWriteStream.WriteL( _L8("<tr><td style=\"padding: 0px 0px 7px 0px;\">") ); // pad bottom to allow some space between the lines

    TUint id = aAttachment.GetPartId().Id();
    TBuf<32> itemId;
    itemId.AppendNum( id );
    CFreestyleMessageHeaderURL* attnUrl = FreestyleMessageHeaderURLFactory::CreateAttachmentUrlL( itemId );
    CleanupStack::PushL( attnUrl );
    HBufC* attnUrlText = attnUrl->ExternalizeL();
    CleanupStack::PushL( attnUrlText );
    HBufC8* attnUrlText8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *attnUrlText );
    CleanupStack::PushL( attnUrlText8 );
    StartHyperlinkL( *attnUrlText8 );
    CleanupStack::PopAndDestroy( attnUrlText8 );
    CleanupStack::PopAndDestroy( attnUrlText );
    CleanupStack::PopAndDestroy( attnUrl );

    TDesC& attnName = aAttachment.AttachmentNameL();
    HBufC8* attnName8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( attnName );
    CleanupStack::PushL( attnName8 );
    iWriteStream.WriteL( *attnName8 );
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
    iWriteStream.WriteL( sizeText );

    EndHyperlinkL();

    iWriteStream.WriteL( _L8("</td></tr>\n") );
    iWriteStream.CommitL();
    }


void CFreestyleMessageHeaderHTML::StartHyperlinkL( const TDesC8& aUrl ) const
    {
    iWriteStream.WriteL( _L8("<a href=\"") );
    iWriteStream.WriteL( aUrl );
    iWriteStream.WriteL( _L8("\">"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::EndHyperlinkL() const
    {
    iWriteStream.WriteL( _L8("</a>") );
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddImageL( const TDesC8& aImageUrl ) const
    {
    iWriteStream.WriteL( _L8("<image border=\"0\" src=\"") );
    iWriteStream.WriteL( aImageUrl );
    iWriteStream.WriteL( _L8("\">"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddImageL( const TDesC8& aImageId,
                                             const TDesC8& aImageUrl,
                                             const TDesC8& aImageEvent ) const
    {
    iWriteStream.WriteL( _L8("<image id=\"") );
    iWriteStream.WriteL( aImageId );
    iWriteStream.WriteL( _L8("\" ") );
    iWriteStream.WriteL( _L8("border=\"0\" src=\"") );
    iWriteStream.WriteL( aImageUrl );
    iWriteStream.WriteL( _L8("\" "));
    iWriteStream.WriteL( aImageEvent );
    iWriteStream.WriteL( _L8(">"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddJavascriptL() const
    {
    iWriteStream.WriteL( _L8("<script language=\"javascript\" src=\"header.js\"></script>\n"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::StartHeaderTableL( const TDesC8& aTableId ) const
    {
    iWriteStream.WriteL( _L8("<table id=\"") );
    iWriteStream.WriteL( aTableId );
    
    // use style="display:none" so that full header table is hidden initially
    iWriteStream.WriteL( _L8("\" border=\"0\" width=\"100%\" style=\"display: none\">\n") );
    
    
    TBuf8<KFreestyleMessageHeaderHTMLMaxBufferSizeForWidth> tableWidth;
    tableWidth.AppendNum( iVisibleWidth );

    // Add "hide details" image as its own table with its own width
    iWriteStream.WriteL( _L8("<tr><td>\n"));
    iWriteStream.WriteL(_L8("<table id =\"table_minus_icon\" border=\"0\" width=\""));
    iWriteStream.WriteL( tableWidth );
    iWriteStream.WriteL( _L8("px\">\n"));
    iWriteStream.WriteL( _L8("<tr>\n"));
    
    iWriteStream.WriteL( _L8("<td valign=\"top\""));
    if ( !iMirrorLayout )
        {
        iWriteStream.WriteL(_L8(" align=\"right\""));
        }
    else
        {
        iWriteStream.WriteL(_L8(" align=\"left\""));
        }
    iWriteStream.WriteL( _L8(" style=\"padding: 0px 10px 0px 0px;\"><image id=\"hideDetails_img\" border=\"0\" src=\"minus.gif\" onClick=\"collapseHeader()\"></td>\n"));

    iWriteStream.WriteL( _L8("</tr>\n"));
    iWriteStream.WriteL( _L8("</table></td></tr>\n"));
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::EndHeaderTableL() const
    {
    EndTableL();
    }

void CFreestyleMessageHeaderHTML::StartTableL( const TDesC8& aTableId ) const
    {
    iWriteStream.WriteL( _L8("<table id=\"") );
    iWriteStream.WriteL( aTableId );
    iWriteStream.WriteL( _L8("\" border=\"1\">\n") );
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::EndTableL() const
    {
    iWriteStream.WriteL( _L8("</table>\n") );
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::AddShowDetailL() const
    {
    HBufC8* event = ClickImageEventL( KDetailImageName );
    CleanupStack::PushL( event );
    AddImageL( KDetailImageName, KShowDetailIconFileName, *event );
    CleanupStack::PopAndDestroy( event );
    iWriteStream.CommitL();
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

void CFreestyleMessageHeaderHTML::AddStyleSheetL() const
    {
    // Add an internal style sheet
    // If the style becomes numerous or complicated, consider using an external style sheet
    
    iWriteStream.WriteL( _L8("<style type=\"text/css\">\n") );
    
    // define a div class "header", specifying the background color
    // for the email header part
    
    // In future, query for which background color to use
    iWriteStream.WriteL( _L8("body { background-color: lightblue; }\n") );
    
    // set font size to 75% of the default size
    // because, at the default size, the header text is too big relative to the text in the email body
    // Note: since the text in the body is too small at "normal" level, 
    // we have the text size level in the browser set to "Larger" which is 20% larger than the specified size
    // the "larger" size affects all text which includes the header.
    iWriteStream.WriteL( _L8("td { font-family:arial,sans-serif ; font-size:75% }\n"));
    
    iWriteStream.WriteL( _L8("</style>\n") );
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::StartDivL() const
    {
    // Add div, using "header" class
    iWriteStream.WriteL( _L8("<div class=\"header\">\n") );
    iWriteStream.CommitL();
    }
    
void CFreestyleMessageHeaderHTML::EndDivL() const
    {
    iWriteStream.WriteL( _L8("</div>\n") );
    iWriteStream.CommitL();
    }


                                
