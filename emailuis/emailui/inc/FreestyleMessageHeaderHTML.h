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

#include "CFSMailMessage.h"
#include "CFSMailAddress.h"
#include "FreestyleMessageHeaderURLFactory.h"

class RWriteStream;
class RFs;
class RFile;


class CFreestyleMessageHeaderHTML : public CBase
    {
public:
    IMPORT_C static void ExportL( CFSMailMessage& aMailMessage, RWriteStream& aWriteStream );
    IMPORT_C static void ExportL( CFSMailMessage& aMailMessage, RFile& aFile );
    IMPORT_C static void ExportL( CFSMailMessage& aMailMessage, RFs& aFs, const TPath& aFilePath);
    
    IMPORT_C static CFreestyleMessageHeaderHTML* NewL( CFSMailMessage& aMailMessage );
    
    ~CFreestyleMessageHeaderHTML();
    
    IMPORT_C void ExportL( RWriteStream& aWriteStream ) const;
    IMPORT_C void ExportL( RFile& aFile ) const;
    IMPORT_C void ExportL( RFs& aFs, const TPath& aFilePath) const;
    
private:
    CFreestyleMessageHeaderHTML( CFSMailMessage& aMailMessage );
    void ConstructL();
    
    void HTMLStartL( RWriteStream& aWriteStream ) const;
    void HTMLEndL( RWriteStream& aWriteStream ) const;
    
    void ExportHTMLHeaderL( RWriteStream& aWriteStream ) const;
    void HTMLHeaderStartL( RWriteStream& aWriteStream ) const;
    void HTMLMetaL( RWriteStream& aWriteStream ) const;
    void HTMLHeaderEndL( RWriteStream& aWriteStream ) const;

    void ExportHTMLBodyL( RWriteStream& aWriteStream ) const;
    void HTMLBodyStartL( RWriteStream& aWriteStream ) const;
    void HTMLBodyEndL( RWriteStream& aWriteStream ) const;
    
    void ExportSubjectL( RWriteStream& aWriteStream ) const;
    void ExportFromL( RWriteStream& aWriteStream ) const;
    void ExportToL( RWriteStream& aWriteStream ) const;
    void ExportCcL( RWriteStream& aWriteStream ) const;
    void ExportBccL( RWriteStream& aWriteStream ) const;
    void ExportSentTimeL( RWriteStream& aWriteStream ) const;
    void ExportAttachmentsL( RWriteStream& aWriteStream ) const;
    
    void ExportEmailAddressesL( RWriteStream& aWriteStream, 
                                FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType, 
                                const RPointerArray<CFSMailAddress>& aEmailAddresses ) const;
    
    void AddEmailAddressL( RWriteStream& aWriteStream, 
                           FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType, 
                           const CFSMailAddress& aEmailAddress ) const;
    
    void AddAttachmentL( RWriteStream& aWriteStream, CFSMailMessagePart& aAttachment ) const;

    void StartHyperlinkL( RWriteStream& aWriteStream, const TDesC8& aUrl ) const;
    void EndHyperlinkL( RWriteStream& aWriteStream ) const;
    
    void AddImageL( RWriteStream& aWriteStream, const TDesC8& aImageUrl ) const;
    void AddImageL( RWriteStream& aWriteStream, const TDesC8& aImageId, const TDesC8& aImageUrl, const TDesC8& aImageEvent ) const;
    
    void StartHeaderTableL( RWriteStream& aWriteStream, const TDesC8& aTableId ) const;
    void EndHeaderTableL( RWriteStream& aWriteStream ) const;

    void StartTableL( RWriteStream& aWriteStream, const TDesC8& aTableId ) const;
    void EndTableL( RWriteStream& aWriteStream ) const;
    
    void AddShowDetailL( RWriteStream& aWriteStream ) const;
    
    HBufC8* ClickImageEventL( const TDesC8& aImageName ) const;

    void AddJavascriptL( RWriteStream& aWriteStream ) const;    
    
    HBufC8* HeadingTextLC( TInt aId ) const;
    HBufC8* HeadingTextLC( TInt aId, TInt aSize ) const;
    
    void ExportInitialTableL( RWriteStream& aWriteStream ) const;
    
    void AddStyleSheetL( RWriteStream& aWriteStream ) const;
    void StartDivL( RWriteStream& aWriteStream ) const;
    void EndDivL( RWriteStream& aWriteStream ) const;
    
private:
    CFSMailMessage& iMailMessage; 
    
    RPointerArray<CFSMailMessagePart> iAttachments;
};

#endif //__CFREESTYLE_MESSAGE_HEADER_HTML_H__
