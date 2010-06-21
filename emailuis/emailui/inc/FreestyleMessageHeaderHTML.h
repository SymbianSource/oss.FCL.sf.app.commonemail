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

#include "cfsmailmessage.h"
#include "cfsmailaddress.h"
#include "FreestyleMessageHeaderURLFactory.h"

class RWriteStream;
class RFs;
class RFile;


class CFreestyleMessageHeaderHTML : public CBase
    {
public:
    IMPORT_C static void ExportL( CFSMailMessage& aMailMessage, 
                                  RWriteStream& aWriteStream, 
                                  TInt aVisibleWidth, 
                                  TInt aScrollPosition,
                                  const TBool aAutoLoadImages,                                  
                                  const TBool aExpanded );
    IMPORT_C static void ExportL( CFSMailMessage& aMailMessage, 
                                  RFile& aFile, 
                                  TInt aVisibleWidth, 
                                  TInt aScrollPosition,
                                  const TBool aAutoLoadImages,
                                  const TBool aExpanded );
    IMPORT_C static void ExportL( CFSMailMessage& aMailMessage, 
                                  RFs& aFs, 
                                  const TPath& aFilePath, 
                                  TInt aVisibleWidth, 
                                  TInt aScrollPosition,
                                  const TBool aAutoLoadImages,
                                  const TBool aExpanded );
    
    IMPORT_C static CFreestyleMessageHeaderHTML* NewL( CFSMailMessage& aMailMessage,
                                                       RWriteStream& aWriteStream,
                                                       TInt aVisibleWidth,
                                                       TInt aScrollPosition,
                                                       const TBool aAutoLoadImages,
                                                       const TBool aExpanded );
    
    ~CFreestyleMessageHeaderHTML();
    
    IMPORT_C void ExportL() const;
    
private:
    CFreestyleMessageHeaderHTML( CFSMailMessage& aMailMessage,
                                 RWriteStream& aWriteStream,
                                 TInt aVisibleWidth,
                                 TInt aScrollPosition,
                                 const TBool aAutoLoadImages,
                                 const TBool aExpanded );
    void ConstructL();
    
    void HTMLStartL() const;
    void HTMLEndL() const;
    
    void ExportHTMLHeaderL() const;
    void HTMLHeaderStartL() const;
    void HTMLMetaL() const;
    void HTMLHeaderEndL() const;

    void ExportDisplayImagesTableL() const;

    void ExportHTMLBodyL() const;
    void HTMLBodyStartL() const;
    void HTMLBodyEndL() const;
    
    /*
     * Writes the subject to iWriteStream and also
     * takes care of the urls and marks them as hotspots
     */
    void WriteSubjectL(TDesC& aText ) const; 

    void ExportCollapseButtonL() const;
    void ExportTimeAndExpandButtonL() const;
    void ExportSubjectL() const;
    void ExportSubjectCollapsedL() const;
    void ExportFromL() const;
    void ExportToL() const;
    void ExportCcL() const;
    void ExportBccL() const;
    void ExportSentTimeL() const;
    void ExportAttachmentsL() const;
    
    void ExportEmailAddressesL( FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType, 
                                const RPointerArray<CFSMailAddress>& aEmailAddresses,
                                const TDesC8& aRowId,
                                const TDesC8& aTableId,
                                TInt aHeaderTextResourceId ) const;
    
    void AddEmailAddressL( FreestyleMessageHeaderURLFactory::TEmailAddressType aEmailAddressType, 
                           const CFSMailAddress& aEmailAddress ) const;
    
    void AddAttachmentL( CFSMailMessagePart& aAttachment ) const;

    void StartHyperlinkL( const TDesC8& aUrl ) const;
    void EndHyperlinkL() const;
    
    void AddImageL( const TDesC8& aImageUrl ) const;
    void AddImageL( const TDesC8& aImageId, const TDesC8& aImageUrl, const TDesC8& aImageEvent ) const;
    
    void StartHeaderTableL( const TDesC8& aTableId, TBool aVisible ) const;
    void EndHeaderTableL() const;

    void StartTableL( const TDesC8& aTableId ) const;
    void EndTableL() const;

    HBufC8* ClickImageEventL( const TDesC8& aImageName ) const;

    void AddJavascriptL() const;    
    
    HBufC8* HeadingTextLC( TInt aId ) const;
    HBufC8* HeadingTextLC( TInt aId, TInt aSize ) const;

    void ExportCollapsedHeaderTableL() const;
    void ExportExpandedHeaderTableL() const;

    /**
     * Function for generating follow up icon's HTML code.
     * Returns NULL if no follow up flags are set.
     *
     * @param aShowText Whether to show icon's text after the icon or not.
     */
    HBufC8* HTMLHeaderFollowUpIconLC( TBool aShowText ) const;
    
    /**
     * Function for generating priority icon's HTML code.
     * Returns NULL if mail message's priority is normal.
     *
     * @param aShowText Whether to show icon's text after the icon or not.
     */
    HBufC8* HTMLHeaderPriorityIconLC( TBool aShowText ) const;
    void AddStyleSheetL() const;
    void StartDivL() const;
    void EndDivL() const;
    
private:
    CFSMailMessage&             iMailMessage; 
    RWriteStream&               iWriteStream;
    TInt                        iVisibleWidth;
    TInt                        iScrollPosition;
    TBool                       iAutoLoadImages;
    TBool                       iMirrorLayout;
    TBool                       iExpanded;
    RPointerArray<CFSMailMessagePart> iAttachments;
};

#endif //__CFREESTYLE_MESSAGE_HEADER_HTML_H__