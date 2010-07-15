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
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <AknsConstants.h>

#include <FreestyleEmailUi.rsg>
#include <finditemengine.h>
#include "FreestyleMessageHeaderHTML.h"
#include "FreestyleEmailUiUtilities.h"
#include "ncsconstants.h"

_LIT( KAlignRight, "right" );
_LIT( KAlignLeft, "left" );
_LIT( KRtl, "rtl" );
_LIT( KLtr, "ltr" );

const TInt KMaxRecipientsShown( 10 );
const TInt KFreestyleMessageHeaderHTMLRightMarginInPx( 10 );

_LIT8( KTableRowBegin, "<tr>" );
_LIT8( KTableRowEnd, "</tr>\n" );
_LIT8( KTableCellEnd, "</td>" );

_LIT( KSchemeSeparator, "://" );
_LIT( KUrlFormat, "<a href=\"%S\">%S</a>" );
_LIT( KUrlFormatWithHttp, "<a href=\"http://%S\">%S</a>" );

// Define this to allow theme colorin for the header
#define __USE_THEME_COLOR_FOR_HEADER    

EXPORT_C CFreestyleMessageHeaderHTML* CFreestyleMessageHeaderHTML::NewL( CFSMailMessage& aMailMessage, 
                                                                         RWriteStream& aWriteStream,
                                                                         TInt aVisibleWidth,
                                                                         TInt aScrollPosition,
                                                                         const TBool aAutoLoadImages,
                                                                         const TBitFlags& aFlags 
                                                                         )
    {
    CFreestyleMessageHeaderHTML* self = new (ELeave) CFreestyleMessageHeaderHTML( aMailMessage, aWriteStream, aVisibleWidth, aScrollPosition, aAutoLoadImages, aFlags);
    self->ConstructL();
    return self;
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, 
                                                    RWriteStream& aWriteStream, 
                                                    TInt aVisibleWidth,
                                                    TInt aScrollPosition,
                                                    const TBool aAutoLoadImages,
                                                    const TBitFlags& aFlags )
    {
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, aWriteStream, aVisibleWidth, aScrollPosition, aAutoLoadImages, aFlags);
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL();
    CleanupStack::PopAndDestroy( headerHtml );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL( CFSMailMessage& aMailMessage, 
                                                    RFile& aFile, 
                                                    TInt aVisibleWidth,
                                                    TInt aScrollPosition,
                                                    const TBool aAutoLoadImages,
                                                    const TBitFlags& aFlags )
    {
    RFileWriteStream fwstream;
    fwstream.Attach( aFile, 0 );
    CleanupClosePushL( fwstream );
    
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, fwstream, aVisibleWidth, aScrollPosition, aAutoLoadImages, aFlags );
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
                                                    const TBool aAutoLoadImages,
                                                    const TBitFlags& aFlags )
    {
    RFileWriteStream fwstream;
    User::LeaveIfError( fwstream.Replace( aFs, aFilePath, EFileStreamText | EFileWrite) );
    CleanupClosePushL( fwstream );
    
    CFreestyleMessageHeaderHTML* headerHtml = CFreestyleMessageHeaderHTML::NewL( aMailMessage, fwstream, aVisibleWidth, aScrollPosition, aAutoLoadImages, aFlags);
    CleanupStack::PushL( headerHtml );
    headerHtml->ExportL();
    CleanupStack::PopAndDestroy( headerHtml );

    CleanupStack::PopAndDestroy( &fwstream );    
    }

CFreestyleMessageHeaderHTML::~CFreestyleMessageHeaderHTML()
    {
    iAttachments.ResetAndDestroy();
    }

CFreestyleMessageHeaderHTML::CFreestyleMessageHeaderHTML( CFSMailMessage& aMailMessage,  
                                                          RWriteStream& aWriteStream,
                                                          TInt aVisibleWidth,
                                                          TInt aScrollPosition,
                                                          const TBool aAutoLoadImages,
                                                          const TBitFlags& aFlags )
    : iMailMessage( aMailMessage ),
    iWriteStream( aWriteStream ),
    iVisibleWidth( aVisibleWidth - KFreestyleMessageHeaderHTMLRightMarginInPx ),
    iScrollPosition( aScrollPosition ),
    iExportFlags( aFlags )
    {
    iExportFlags.Assign( EAutoLoadImages, aAutoLoadImages );
    iExportFlags.Assign( EMirroredLayout, AknLayoutUtils::LayoutMirrored() );
    }

void CFreestyleMessageHeaderHTML::ConstructL()
    {
    iMailMessage.AttachmentListL( iAttachments );
    }

EXPORT_C void CFreestyleMessageHeaderHTML::ExportL() const
    {
    ExportBodyStyleL();
    ExportHTMLBodyStartL();
    ExportHeaderTablesL();
    ExportHTMLBodyEndL();    
    }

void CFreestyleMessageHeaderHTML::ExportHTMLBodyStartL() const
    {
    _LIT( KHtmlBodyStart, "<html dir=\"%S\" xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n<title>Email Header</title>\n<script language=\"javascript\" src=\"header.js\"></script>\n<link rel=\"stylesheet\" type=\"text/css\" href=\"header.css\"/>\n</head>\n<body onLoad = \"init(%d)\">\n" );
    //_LIT( KHtmlBodyStart, "<html dir=\"%S\" xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n<title>Email Header</title>\n<script language=\"javascript\" src=\"header.js\"></script>\n<link rel=\"stylesheet\" type=\"text/css\" href=\"header%d.css\"/>\n</head>\n<body onLoad = \"init(%d)\">\n" );
    const TPtrC direction(  iExportFlags.IsSet( EMirroredLayout ) ? KRtl() : KLtr() );
    HBufC* formatBuffer = HBufC::NewLC( KHtmlBodyStart().Length() + direction.Length() + 16 );
    formatBuffer->Des().Format( KHtmlBodyStart(), &direction, iScrollPosition );
    //formatBuffer->Des().Format( KHtmlBodyStart(), &direction, Math::Random() % 2, iScrollPosition );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf    
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportInnerTableBeginWithRowBeginL( const TDesC& aTableName, const TInt aColSpan, 
        const TBool aVisible, const TBitFlags& aFlags ) const
    {
    iWriteStream.WriteL( KTableRowBegin() );
    ExportTableVisibilityParameterL( aTableName, aVisible );
    ExportInnerTableBeginL( aTableName, aColSpan, aFlags );
    }


void CFreestyleMessageHeaderHTML::ExportInnerTableBeginL( const TDesC& aTableName, const TInt aColSpan, 
        const TBitFlags& aFlags ) const
    {
    _LIT( KHidden, " style=\"display: none;\"");
    _LIT( KFixed, " class=\"fixed\"");   
    _LIT( KTableHeader, "<td colspan=\"%d\"><table id=\"%S\" width=\"100%%\"%S%S>\n" );
    const TPtrC style( aFlags.IsClear( EHidden ) ? KNullDesC() : KHidden() );
    const TPtrC fixed( aFlags.IsClear( EFixed ) ? KNullDesC() : KFixed() );
    HBufC* formatBuffer = HBufC::NewLC( KTableHeader().Length() + aTableName.Length() + 16 + style.Length() + fixed.Length() );
    formatBuffer->Des().Format( KTableHeader(), aColSpan, &aTableName, &style, &fixed );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf    
    }

void CFreestyleMessageHeaderHTML::ExportTableVisibilityParameterL( const TDesC& aTableName, const TBitFlags& aFlags ) const
    {   
    ExportTableVisibilityParameterL( aTableName, aFlags.IsClear( EHidden ) );
    }

void CFreestyleMessageHeaderHTML::ExportTableVisibilityParameterL( const TDesC& aTableName, const TBool aVisible ) const
    {   
    _LIT( KTrue, "true" );
    _LIT( KFalse, "false" );
    _LIT( KVisibilityParameter, "<script type=\"text/javascript\">var is_%S_visible=%S;</script>" );
    const TPtrC visible( aVisible ? KTrue() : KFalse() );
    HBufC* formatBuffer = HBufC::NewLC( KVisibilityParameter().Length() + aTableName.Length() + visible.Length() );
    formatBuffer->Des().Format( KVisibilityParameter(), &aTableName, &visible );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );        
    CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf
    }

void CFreestyleMessageHeaderHTML::ExportTableBeginL( const TDesC& aTableName, const TBitFlags& aFlags ) const
    {   
    _LIT( KFixed, " class=\"fixed\"");   
    _LIT( KTableHeader, "<table id=\"%S\" width=\"%dpx\"%S%S>\n" );
    const TPtrC style( aFlags.IsClear( EHidden ) ? KNullDesC() : KNullDesC() );
    const TPtrC fixed( aFlags.IsClear( EFixed ) ? KNullDesC() : KFixed() );
    ExportTableVisibilityParameterL( aTableName, aFlags );   
    HBufC* formatBuffer = HBufC::NewLC( KTableHeader().Length() + aTableName.Length() + 16 + style.Length() + fixed.Length() );
    formatBuffer->Des().Format( KTableHeader(), &aTableName, iVisibleWidth, &style, &fixed );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf    
    }

void CFreestyleMessageHeaderHTML::ExportInnerTableEndL() const
    {
    ExportTableEndL();
    iWriteStream.WriteL( KTableCellEnd() );
    }

void CFreestyleMessageHeaderHTML::ExportInnerTableEndWithRowEndL() const
    {
    ExportInnerTableEndL();
    iWriteStream.WriteL( KTableRowEnd() );
    }

void CFreestyleMessageHeaderHTML::ExportTableEndL() const
    {
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( _L("</table>\n") );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy(); // utf    
    }

void CFreestyleMessageHeaderHTML::ExportSenderTableRowL( const TBool aCollapsed ) const
    {
    _LIT( KStyleExpand, "expand" );
    _LIT( KStyleCollapse, "collapse" );
    _LIT( KExpandFunction, "expandHeader(true)" );
    _LIT( KCollapseFunction, "collapseHeader(true)" );
    _LIT( KSenderFormat, "<tr><td align=\"%S\" class=\"sender_name\"><div class=\"truncate\">%S</div></td><td valign=\"top\" rowSpan=\"2\" class=\"button_cell\"><button value=\"submit\" onClick=\"%S\" class=\"%S\"></button></td></tr>\n" );
    const CFSMailAddress* sender( iMailMessage.GetSender() );
    TPtrC displayName( sender->GetDisplayName() );
    if ( !displayName.Length() )
        {
        displayName.Set( sender->GetEmailAddress() );
        }
    const TPtrC function( aCollapsed ? KExpandFunction() : KCollapseFunction() );
    const TPtrC style( aCollapsed ? KStyleExpand() : KStyleCollapse() );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* formatBuffer = HBufC::NewLC( KSenderFormat().Length() + displayName.Length() + align.Length() + function.Length() + style.Length() );
    formatBuffer->Des().Format( KSenderFormat(), &align, &displayName, &function, &style );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf    
    }

void CFreestyleMessageHeaderHTML::ExportSenderAddressTableRowL() const
    {
    _LIT( KSenderAddressFormat, "<tr><td colspan=\"2\" align=\"%S\"><div class=\"truncate\"><a class=\"sender_address\" href=\"cmail://from/%S\">%S</a></div></td></tr>\n" );
    const CFSMailAddress* sender( iMailMessage.GetSender() );
    const TPtrC emailAddress( sender->GetEmailAddress() );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* formatBuffer = HBufC::NewLC( KSenderAddressFormat().Length() + emailAddress.Length() * 2 + align.Length() );
    formatBuffer->Des().Format( KSenderAddressFormat(), &align, &emailAddress, &emailAddress );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf    
    }

void CFreestyleMessageHeaderHTML::ExportLabelTableRowL( const TInt aResourceId, const TInt aColSpan ) const
    {
    _LIT( KLabelFormat, "<tr><td align=\"%S\" colspan=\"%d\" class=\"label\">%S</td></tr>\n" );
    HBufC* labelText = StringLoader::LoadLC( aResourceId );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* formatBuffer = HBufC::NewLC( KLabelFormat().Length() + labelText->Length() + align.Length() + 8 );
    formatBuffer->Des().Format( KLabelFormat(), &align, aColSpan, labelText );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 3 ); // labelText, formatBuffer, utf    
    }

void  CFreestyleMessageHeaderHTML::ExportFromTableRowL() const
    {
    ExportLabelTableRowL( R_FREESTYLE_EMAIL_UI_VIEWER_FROM );
    ExportSenderAddressTableRowL();
    }

void  CFreestyleMessageHeaderHTML::ExportExpandRecipientsL( const TDesC& aType, const TInt aCount ) const
    {
    _LIT( KExpandRecipientsFormat, "<tr><td align=\"%S\"><a class=\"recipient\" href=\"cmail://expand_%S/\" onclick=\"handleHeaderDisplay('%S_collapsed','%S_expanded')\">%S</a></td></tr>\n" );
    HBufC* text =  StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_N_MORE_RECIPIENTS, aCount );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* formatBuffer = HBufC::NewLC( KExpandRecipientsFormat().Length() + text->Length() + align.Length() + aType.Length() * 3 );
    formatBuffer->Des().Format( KExpandRecipientsFormat(), &align, &aType, &aType, &aType, text );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 3 ); // text, formatBuffer, utf    
    }

void  CFreestyleMessageHeaderHTML::ExportExpandAttachmentsL( const TDesC& aType, const TInt aCount ) const
    {
    _LIT( KExpandRecipientsFormat, "<td align=\"%S\" class=\"attachment\"><a href=\"cmail://expand_%S/\" onclick=\"handleHeaderDisplay('%S_collapsed','%S_expanded')\">%S</a></td>" );
    HBufC* text =  StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_TEXT, aCount );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* formatBuffer = HBufC::NewLC( KExpandRecipientsFormat().Length() + text->Length() + align.Length() + aType.Length() * 3 );
    formatBuffer->Des().Format( KExpandRecipientsFormat(), &align, &aType, &aType, &aType, text );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 3 ); // text, formatBuffer, utf    
    }

void  CFreestyleMessageHeaderHTML::ExportRecipientsTableL( const TDesC& aType, const TInt aLabelResourceId, 
        const RPointerArray<CFSMailAddress>& aRecipients, const TBool aExpanded ) const
    {  
    if ( aRecipients.Count() > 0 )
        { 
        TBool innerTableExpanded( aExpanded );
        TBitFlags flags;
        flags.Assign( EHidden, iExportFlags.IsClear( EHeaderExpanded ) );
        _LIT( KTableNameFormat, "%S_table" );
        HBufC* outerTableName = HBufC::NewLC( KTableNameFormat().Length() + aType.Length() );
        outerTableName->Des().Format( KTableNameFormat(), &aType );
        ExportTableBeginL( *outerTableName, flags );
        CleanupStack::PopAndDestroy(); // outerTableName
        
        ExportLabelTableRowL( aLabelResourceId, 2 );
        const TBool showCollapsed( aRecipients.Count() > KMaxRecipientsShown );
        TBitFlags tableFlags;
        tableFlags.Set( EFixed );        
        if ( showCollapsed )
            {
            _LIT( KFormatCollapsed, "%S_collapsed" );
            HBufC* tableName = HBufC::NewLC( KFormatCollapsed().Length() + 8 );
            tableName->Des().Format( KFormatCollapsed(), &aType );
            ExportInnerTableBeginWithRowBeginL( *tableName, 2, !innerTableExpanded, tableFlags );
            CleanupStack::PopAndDestroy(); // tableName
            ExportExpandRecipientsL( aType, aRecipients.Count() );        
            ExportInnerTableEndWithRowEndL();
            tableFlags.Set( EHidden );
            }
        else
            {
            innerTableExpanded = ETrue;
            }
        _LIT( KFormatExpanded, "%S_expanded" );
        HBufC* tableName = HBufC::NewLC( KFormatExpanded().Length() + 8 );
        tableName->Des().Format( KFormatExpanded(), &aType );
        ExportInnerTableBeginWithRowBeginL( *tableName, 2, innerTableExpanded, tableFlags );
        CleanupStack::PopAndDestroy(); // tableName
        ExportRecipientsL( aType, aRecipients );
        ExportInnerTableEndWithRowEndL();        
        ExportTableEndL();
        }
    }

void  CFreestyleMessageHeaderHTML::ExportRecipientsL( const TDesC& aType, 
        const RPointerArray<CFSMailAddress>& aRecipients) const
    {
    _LIT( KRecipientFormat, "<tr><td align=\"%S\"><div class=\"truncate\"><a class=\"recipient\" href=\"cmail://%S/%S\">%S</a></div></td></tr>" );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    for ( TInt i = 0; i < aRecipients.Count(); i++ )
        {
        const CFSMailAddress* sender( aRecipients[ i ] );
        TPtrC displayName( sender->GetDisplayName() );
        const TPtrC emailAddress( sender->GetEmailAddress() );
        if ( !displayName.Length() )
            {
            displayName.Set( emailAddress );
            }   
        HBufC* formatBuffer = HBufC::NewLC( KRecipientFormat().Length() + align.Length() + aType.Length() + emailAddress.Length() + displayName.Length() );
        formatBuffer->Des().Format( KRecipientFormat(), &align, &aType, &emailAddress, &displayName );
        HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
        CleanupStack::PushL( utf );
        iWriteStream.WriteL( *utf );
        CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf    
        }
    }

void  CFreestyleMessageHeaderHTML::ExportToTableL() const
    {
    ExportRecipientsTableL( _L("to"), R_FREESTYLE_EMAIL_UI_VIEWER_TO, iMailMessage.GetToRecipients(), iExportFlags.IsSet( EToExpanded ) );
    }

void  CFreestyleMessageHeaderHTML::ExportCcTableL() const
    {
    ExportRecipientsTableL( _L("cc"), R_FREESTYLE_EMAIL_UI_VIEWER_CC, iMailMessage.GetCCRecipients(), iExportFlags.IsSet( ECcExpanded ) );
    }

void  CFreestyleMessageHeaderHTML::ExportBccTableL() const
    {
    ExportRecipientsTableL( _L("bcc"), R_FREESTYLE_EMAIL_UI_VIEWER_BCC, iMailMessage.GetBCCRecipients(), iExportFlags.IsSet( EBccExpanded ) );
    }

void CFreestyleMessageHeaderHTML::ExportDateTimeTableRowL( const TInt aColSpan ) const
    {
    _LIT( KDateTimeFormat, "<tr><td colspan=\"%d\" align=\"%S\" class=\"datetime\">%S%S%S</td></tr>\n" );
    HBufC* dateText = TFsEmailUiUtility::DateTextFromMsgLC( &iMailMessage );
    HBufC* timeText = TFsEmailUiUtility::TimeTextFromMsgLC( &iMailMessage );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* formatBuffer = HBufC::NewLC( KDateTimeFormat().Length() + dateText->Length() + timeText->Length() +  KSentLineDateAndTimeSeparatorText().Length() + align.Length() + 3 );
    formatBuffer->Des().Format( KDateTimeFormat(), aColSpan, &align, dateText, &KSentLineDateAndTimeSeparatorText(), timeText );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 4 ); // dateText, timeText, formatBuffer, utf   
    }

void CFreestyleMessageHeaderHTML::ExportSubjectTableRowL( const TBool aShowLabel ) const
    {
    _LIT( KClassSubject, "subject" );
    TPtrC subjectClass( KClassSubject() );
    if ( aShowLabel )
        {
        _LIT( KClassSubjectIntended, "subject_intended");
        ExportLabelTableRowL( R_FREESTYLE_EMAIL_UI_VIEWER_SUBJECT, 2 );
        subjectClass.Set( KClassSubjectIntended() );
        }
    _LIT( KSubjectFormat, "<tr><td align=\"%S\" class=\"%S\">%S</td><td align=\"center\" valign=\"bottom\" width=\"1\">");
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* subject = SubjectLC();
    HBufC* formatBuffer = HBufC::NewLC( KSubjectFormat().Length() + align.Length() + subject->Length() + subjectClass.Length() );
    formatBuffer->Des().Format( KSubjectFormat(), &align, &subjectClass, subject );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 3 ); // subject, formatBuffer, utf
    ExportMessageIconsL();
    iWriteStream.WriteL( KTableCellEnd() );
    iWriteStream.WriteL( KTableRowEnd() );
    }

void CFreestyleMessageHeaderHTML::ExportMessageIconsL() const
    {
    _LIT( KPriorityLow, "priority_low" );
    _LIT( KPriorityHigh, "priority_high" );
    _LIT( KFollowUp, "follow_up" );
    _LIT( KFollowUpComplete, "follow_up_complete" );
    
    TBool iconShown( EFalse );
    
    if ( iMailMessage.IsFlagSet( EFSMsgFlag_Low ) )
        {
        ExportIconL( KPriorityLow() );
        iconShown = ETrue;
        }
    else if ( iMailMessage.IsFlagSet( EFSMsgFlag_Important ) )
        {
        ExportIconL( KPriorityHigh() );    
        iconShown = ETrue;
        }
    
    if ( iMailMessage.IsFlagSet( EFSMsgFlag_FollowUp ) )
        {
        ExportIconL( KFollowUp() );
        iconShown = ETrue;
        }
    else if ( iMailMessage.IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
        {
        ExportIconL( KFollowUpComplete() );    
        iconShown = ETrue;
        }
    if ( iconShown )
        {
        iWriteStream.WriteL( _L8("<div class=\"icon_cell\"></div>") );
        }
    }

void CFreestyleMessageHeaderHTML::ExportHeaderTablesL() const        
    {
    ExportCollapsedHeaderTableL();
    ExportExpandedHeaderTablesL();
    ExportAttachmentTablesL();
    ExportDisplayImagesTableL();
    }

void CFreestyleMessageHeaderHTML::ExportCollapsedHeaderTableL() const
    {
    TBitFlags flags;
    flags.Set( EFixed );
    flags.Assign( EHidden, iExportFlags.IsSet( EHeaderExpanded ) );
    ExportTableBeginL( _L("header_collapsed"), flags );
    ExportSenderTableRowL( ETrue );
    ExportDateTimeTableRowL();
    ExportTableEndL();
    flags.Clear( EFixed );
    ExportTableBeginL( _L("header_collapsed_2"), flags );
    ExportSubjectTableRowL();
    ExportTableEndL();
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportExpandedHeaderTablesL() const
    {
    TBitFlags flags;
    flags.Set( EFixed );
    flags.Assign( EHidden, iExportFlags.IsClear( EHeaderExpanded ) );
    ExportTableBeginL( _L("header_expanded"), flags );
    ExportSenderTableRowL( EFalse );
    ExportFromTableRowL();
    ExportTableEndL();
    ExportToTableL();
    ExportCcTableL();
    ExportBccTableL();
    flags.Clear( EFixed );
    ExportTableBeginL( _L("header_expanded_2"), flags );
    ExportDateTimeTableRowL( 2 );
    ExportSubjectTableRowL();
    ExportTableEndL();
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportAttachmentTablesL() const
    {
    const TInt attachmentCount( iAttachments.Count() );
    if ( attachmentCount )
        {
        if ( attachmentCount > 1 )
            {
            ExportCollapsedAttachmentTableL( iExportFlags.IsSet( EAttachmentExpanded ) );
            ExportExpandedAttachmentTableL( iExportFlags.IsClear( EAttachmentExpanded ) );
            }
        else
            {
            ExportExpandedAttachmentTableL( EFalse );        
            }
        }
    }

void CFreestyleMessageHeaderHTML::ExportAttachmentIconL() const
    {    
    _LIT8( KCellBegin, "<td width=\"1\" valign=\"top\" class=\"attachment\">" );
    iWriteStream.WriteL( KCellBegin() );
    _LIT( KAttachment, "attachment" );
    ExportIconL( KAttachment() );
    iWriteStream.WriteL( KTableCellEnd() );
    }

void CFreestyleMessageHeaderHTML::ExportCollapsedAttachmentTableL( const TBool aHide ) const
    {
    TBitFlags flags;
    flags.Assign( EHidden, aHide );
    ExportTableBeginL( _L("attachments_collapsed"), flags );
    ExportCollapsedAttachmentsTableRowL();
    ExportTableEndL();
    }

void CFreestyleMessageHeaderHTML::ExportCollapsedAttachmentsTableRowL() const
    {
    iWriteStream.WriteL( KTableRowBegin() );
    ExportAttachmentIconL();
    ExportExpandAttachmentsL( _L("attachments"), iAttachments.Count() );
    iWriteStream.WriteL( KTableRowEnd() );
    }

void CFreestyleMessageHeaderHTML::ExportExpandedAttachmentTableL( const TBool aHide ) const
    {    
    TBitFlags flags;
    flags.Assign( EHidden, aHide );
    ExportTableBeginL( _L("attachments_expanded"), flags );
    ExportExpandedAttachmentsTableRowsL();
    ExportTableEndL();
    }

void CFreestyleMessageHeaderHTML::ExportExpandedAttachmentsTableRowsL() const
    {
    iWriteStream.WriteL( KTableRowBegin() );
    ExportAttachmentIconL();
    ExportAttachmentsL();
    iWriteStream.WriteL( KTableRowEnd() );
    }

void CFreestyleMessageHeaderHTML::ExportAttachmentsL() const
    {
    ExportInnerTableBeginL( _L("_attachments_expanded"), 1 );
    for ( TInt i = 0; i < iAttachments.Count(); i++ )
        {
        ExportAttachmentL( *iAttachments[ i ] );
        }
    ExportInnerTableEndL();
    }

void CFreestyleMessageHeaderHTML::ExportAttachmentL( CFSMailMessagePart& aAttachment ) const
    {
    _LIT( KAttachmentFormat, "<tr><td align=\"%S\" class=\"attachment\"><a href=\"cmail://attachment/%d\">%S (%S)</a></td></tr>\n" );
    const TPtrC align(  iExportFlags.IsSet( EMirroredLayout ) ? KAlignRight() : KAlignLeft() );
    HBufC* size = TFsEmailUiUtility::CreateSizeDescLC( aAttachment.ContentSize(), EFalse );
    const TPtrC attachmentName( aAttachment.AttachmentNameL() );
    HBufC* formatBuffer = HBufC::NewLC( KAttachmentFormat().Length() + align.Length() + size->Length() + attachmentName.Length() + 16 );
    formatBuffer->Des().Format( KAttachmentFormat(), &align, aAttachment.GetPartId().Id(), &attachmentName, size );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 3 ); // size, formatBuffer, utf
    }

void CFreestyleMessageHeaderHTML::ExportIconL( const TDesC& aIconName ) const
    {
    _LIT( KMessageIconFormat, "<img src=\"%S.png\" class=\"icon\"/>" );
    HBufC* formatBuffer = HBufC::NewLC( KMessageIconFormat().Length() + aIconName.Length() );
    formatBuffer->Des().Format( KMessageIconFormat(), &aIconName );
    HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
    CleanupStack::PushL( utf );
    iWriteStream.WriteL( *utf );
    CleanupStack::PopAndDestroy( 2 ); // formatBuffer, utf
    }

HBufC* CFreestyleMessageHeaderHTML::SubjectLC() const
    {
    return CreateLinksLC( iMailMessage.GetSubject(), CFindItemEngine::EFindItemSearchURLBin );
    }

TInt CFreestyleMessageHeaderHTML::CalculateTotalSpaceRequired( const TDesC& aText, 
        CFindItemEngine& aItemEngine, TInt& aMaxLength ) const
    {
    TInt totalLength( 0 );
    aMaxLength = KMinTInt;
    CFindItemEngine::SFoundItem item;
    aItemEngine.ResetPosition();
    for ( TBool available( aItemEngine.Item( item ) ); available; available = aItemEngine.NextItem( item ) )
        {
        totalLength += item.iLength;
        if ( item.iItemType == CFindItemEngine::EFindItemSearchURLBin )
            {
            const TPtrC url( aText.Mid( item.iStartPos, item.iLength ) );
            if (  url.FindF( KSchemeSeparator() ) == KErrNotFound )
                {
                totalLength += KUrlFormatWithHttp().Length();
                }
            else
                {
                totalLength += KUrlFormat().Length();        
                }
            aMaxLength = ( aMaxLength < item.iLength ) ? item.iLength : aMaxLength;
            }
        }
    aItemEngine.ResetPosition();
    return totalLength;
    }

HBufC* CFreestyleMessageHeaderHTML::CreateLinksLC( const TDesC& aText, const TInt aSearchCases ) const
    {
    HBufC* result = NULL;
    CFindItemEngine* itemEngine = CFindItemEngine::NewL( aText, 
            CFindItemEngine::TFindItemSearchCase( aSearchCases ) );
    CleanupStack::PushL ( itemEngine );
    if ( itemEngine->ItemCount() > 0 )
        {
        RBuf buf;
        TInt maxLength;
        buf.CreateL( CalculateTotalSpaceRequired( aText, *itemEngine, maxLength ) + aText.Length() );
        buf.CleanupClosePushL();
        TInt currentReadPosition( 0 );
        CFindItemEngine::SFoundItem item;
        HBufC* urlBuffer = HBufC::NewLC( KUrlFormatWithHttp().Length() + maxLength * 2 );
        for ( TBool available( itemEngine->Item( item ) ); available; available = itemEngine->NextItem( item ) )
            {
            // Append characters from currentReadPosition to iStartPos
            buf.Append( aText.Mid( currentReadPosition, item.iStartPos - currentReadPosition ) );
            const TPtrC url( aText.Mid( item.iStartPos, item.iLength ) );
            TPtrC format( KUrlFormat() );
            if ( url.FindF( KSchemeSeparator() ) == KErrNotFound )
                {
                format.Set( KUrlFormatWithHttp() );
                }
            urlBuffer->Des().Format( format, &url, &url );
            buf.Append( *urlBuffer );
            currentReadPosition = item.iStartPos + item.iLength;
            }
        CleanupStack::PopAndDestroy(); // urlBuffer
        // Append characters that are left in buffer
        buf.Append( aText.Mid( currentReadPosition ) );
        result = buf.AllocL();
        CleanupStack::PopAndDestroy(); // buf.Close()
        }
    else
        {
        result = aText.AllocL();
        }
    CleanupStack::PopAndDestroy( itemEngine );
    CleanupStack::PushL( result );
    return result;
    }

void CFreestyleMessageHeaderHTML::ExportHTMLBodyEndL() const
    {
    iWriteStream.WriteL( _L8("</body>\n</html>\n") );
    iWriteStream.CommitL();
    }

void CFreestyleMessageHeaderHTML::ExportDisplayImagesTableL() const
    {
    _LIT( KDisplayImagesLeftToRight,
            "<script language=\"javascript\">var g_autoLoadImages = %d;</script><table style=\"display: none\" id=\"displayImagesTable\" width=\"%dpx\"><tr><td align=\"left\">%S</td><td align=\"right\"><button value=\"submit\" class=\"submitBtn\" onClick=\"displayImagesButtonPressed()\"><span><span class=\"buttonText\">%S</span></span></button></td></tr></table>" );
    
    _LIT( KDisplayImagesRightToLeft,
            "<script language=\"javascript\">var g_autoLoadImages = %d;</script><table style=\"display: none\" id=\"displayImagesTable\" width=\"%dpx\"><tr><td align=\"left\"><button value=\"submit\" class=\"submitBtn\" onClick=\"displayImagesButtonPressed()\"><span><span class=\"buttonText\">%S</span></span></button></td><td align=\"right\">%S</td></tr></table>" );

    if ( iExportFlags.IsClear( EAutoLoadImages ) )
        {
        _LIT(KDescription, "");
        HBufC* description = KDescription().AllocLC(); //StringLoader::LoadLC(R_FREESTYLE_EMAIL_UI_IMAGES_ARE_NOT_DISPLAYED);
        HBufC* button = StringLoader::LoadLC(R_FREESTYLE_EMAIL_UI_DISPLAY_IMAGES);
        HBufC* formatBuffer = NULL;
        if ( iExportFlags.IsSet( EMirroredLayout ) )
            {
            formatBuffer = HBufC::NewLC(KDisplayImagesRightToLeft().Length() + description->Length() + button->Length() + 8);
            formatBuffer->Des().Format(
                    KDisplayImagesRightToLeft(),
                    EFalse,
                    iVisibleWidth,
                    button,
                    description);
            }
        else
            {
            formatBuffer = HBufC::NewLC(KDisplayImagesLeftToRight().Length() + description->Length() + button->Length() + 8);
            formatBuffer->Des().Format(
                    KDisplayImagesLeftToRight(),
                    EFalse,
                    iVisibleWidth,
                    description,
                    button);
            }
        HBufC8* utf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( *formatBuffer );
        CleanupStack::PushL( utf );
        iWriteStream.WriteL( *utf );
        CleanupStack::PopAndDestroy( 4 ); // description, button, formatBuffer, utf
        iWriteStream.CommitL();
        }
    }

void CFreestyleMessageHeaderHTML::ExportBodyStyleL() const
    {
    iWriteStream.WriteL( _L8("<style type=\"text/css\">\n") );
#ifdef __USE_THEME_COLOR_FOR_HEADER    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor;
    TRgb bgColor;
    // Should use EAknsCIFsTextColorsCG3 if background is white
     if ( AknsUtils::GetCachedColor( skin, textColor,
                 KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG81 ) != KErrNone ||
          AknsUtils::GetCachedColor( skin, bgColor,
                 KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG22 ) != KErrNone )
         {
         iWriteStream.WriteL( _L8("body { color: black; background-color: lightblue; }\n") );
         }
     else 
         {
         // In future, query for which background color to use
         _LIT8( KBodyWithColor, "body { color: #%06x; background-color: #%06x; }\n" );
         HBufC8* formatBuffer = HBufC8::NewLC(KBodyWithColor().Length() + 16);
         formatBuffer->Des().Format(KBodyWithColor(), textColor.Internal() & 0xFFFFFF, bgColor.Internal() & 0xFFFFFF);
         iWriteStream.WriteL( *formatBuffer );
         CleanupStack::PopAndDestroy(); // formatBuffer
         }
#else
     //iWriteStream.WriteL( _L8("body { color: black; background-color: lightblue; }\n") );
     iWriteStream.WriteL( _L8("body { color: black; background-color: #c5c5c5; }\n") );
#endif // __USE_THEME_COLOR_FOR_HEADER    
    iWriteStream.WriteL( _L8("</style>\n") );
    iWriteStream.CommitL();
    }

                                
