/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef __CFREESTYLE_MESSAGE_HEADER_HTML_H__
#define __CFREESTYLE_MESSAGE_HEADER_HTML_H__

#include <e32base.h>
#include <biditext.h>  
#include <babitflags.h>

#include "cfsmailmessage.h"
#include "cfsmailaddress.h"
#include "FreestyleMessageHeaderURLFactory.h"

class RWriteStream;
class RFs;
class RFile;


class CFreestyleMessageHeaderHTML : public CBase
    {
public:
    
    /**
     * Flags to control exporting
     */
    enum TExportFlag
        {
        // Header is expanded
        EHeaderExpanded,
        // Attachments are expanded
        EAttachmentExpanded,
        // To field is expanded
        EToExpanded,
        // Cc field is expanded
        ECcExpanded,
        // Bcc field is expanded
        EBccExpanded,
        
        // ** For internal use only **         
        // Load images automatically
        EAutoLoadImages,
        // Mirrored layout in use
        EMirroredLayout        
        };
    
public:
    
    static void ExportL( CFSMailMessage& aMailMessage,
        RWriteStream& aWriteStream, TInt aVisibleWidth, TInt aScrollPosition,
        TBool aAutoLoadImages, const TBitFlags& aFlags );

    static void ExportL( CFSMailMessage& aMailMessage, RFile& aFile,
        TInt aVisibleWidth, TInt aScrollPosition, TBool aAutoLoadImages,
        const TBitFlags& aFlags );

    static void ExportL( CFSMailMessage& aMailMessage, RFs& aFs,
        const TPath& aFilePath, TInt aVisibleWidth, TInt aScrollPosition,
        TBool aAutoLoadImages, const TBitFlags& aFlags );

    static CFreestyleMessageHeaderHTML* NewL( CFSMailMessage& aMailMessage,
        RWriteStream& aWriteStream, TInt aVisibleWidth, TInt aScrollPosition,
        TBool aAutoLoadImages, const TBitFlags& aFlags );

    ~CFreestyleMessageHeaderHTML();

    void ExportL() const;

private:
    CFreestyleMessageHeaderHTML( CFSMailMessage& aMailMessage,
        RWriteStream& aWriteStream, TInt aVisibleWidth, TInt aScrollPosition,
        TBool aAutoLoadImages, const TBitFlags& aFlags );
    void ConstructL();

    void ExportHeaderTablesL() const;
    void ExportCollapsedHeaderTableL() const;
    void ExportExpandedHeaderTablesL() const;

    void ExportTableVisibilityParameterL( const TDesC& aTableName, const TBitFlags& aFlags ) const;
    void ExportTableVisibilityParameterL( const TDesC& aTableName, const TBool aVisible ) const;
    void ExportTableBeginL( const TDesC& aTableName, const TBitFlags& aFlags = TBitFlags() ) const;
    void ExportTableEndL() const;
    void ExportInnerTableBeginL( const TDesC& aTableName, const TInt aColSpan, const TBitFlags& aFlags = TBitFlags()  ) const;
    void ExportInnerTableBeginWithRowBeginL( const TDesC& aTableName, const TInt aColSpan, const TBool aVisible, const TBitFlags& aFlags = TBitFlags() ) const;
    void ExportInnerTableEndL() const;
    void ExportInnerTableEndWithRowEndL() const;
    
    void ExportAttachmentTablesL() const;
    void ExportCollapsedAttachmentTableL( const TBool aHide ) const;
    void ExportCollapsedAttachmentsTableRowL() const;
    void ExportExpandedAttachmentTableL( const TBool aHide ) const;
    void ExportExpandedAttachmentsTableRowsL() const;
    void ExportAttachmentsL() const;    
    void ExportAttachmentL( CFSMailMessagePart& aAttachment ) const;    
    void ExportAttachmentIconL() const;

    void ExportSenderTableRowL( const TBool aCollapsed ) const;
    void ExportSenderAddressTableRowL() const;
    void ExportDateTimeTableRowL( const TInt aColSpan = 1 ) const;
    void ExportSubjectTableRowL( const TBool aShowLabel = EFalse ) const;
    void ExportMessageIconsL() const;
    void ExportFromTableRowL() const;
    void ExportToTableL() const;
    void ExportCcTableL() const;
    void ExportBccTableL() const;
    void ExportRecipientsTableL( const TDesC& aType, const TInt aLabelResourceId,
            const RPointerArray<CFSMailAddress>& aRecipients, const TBool aExpanded ) const;
    void ExportRecipientsL( const TDesC& aType, const RPointerArray<CFSMailAddress>& aRecipients ) const;
    void ExportExpandRecipientsL( const TDesC& aType, const TInt aCount ) const;
    void ExportExpandAttachmentsL( const TDesC& aType, const TInt aCount ) const;
    void ExportLabelTableRowL( const TInt aResourceId, const TInt aColSpan = 1 ) const;
    void ExportIconL( const TDesC& aIconName ) const;
    HBufC* SubjectLC() const;
    TInt CalculateTotalSpaceRequired( const TDesC& aText, CFindItemEngine& aItemEngine, 
            TInt& aMaxLength ) const;
    HBufC* CreateLinksLC( const TDesC& aText, const TInt aSearchCases ) const;
    void ExportHtmlStartL() const;
    void ExportHeadL() const;
    void ExportBodyStartL() const;
    void ExportBodyEndL() const;
    void ExportHtmlEndL() const;
    void ExportDisplayImagesTableL() const;
    void ExportBodyStyleL() const;
    
private:

    // Table formatting flags
    enum TTableStyleFlags
        {
        // Table is hidden
        EHidden,
        // Table width is fixed
        EFixed
        };

    CFSMailMessage&             iMailMessage; 
    RWriteStream&               iWriteStream;
    TInt                        iVisibleWidth;
    TInt                        iScrollPosition;
    RPointerArray<CFSMailMessagePart> iAttachments;
    TBitFlags iExportFlags;
};

#endif //__CFREESTYLE_MESSAGE_HEADER_HTML_H__
