/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Definition of Email text content.
*
*/


#ifndef EMAILTEXTCONTENT_H_
#define EMAILTEXTCONTENT_H_

// INCLUDES
#include "CFSMailClient.h"
#include <memailcontent.h>
#include "emailapiutils.h"

using namespace EmailInterface;

class CEmailMessageContent;
// CLASS DECLARATION

/**
 *  CEmailTextContent
 * 
 */
NONSHARABLE_CLASS( CEmailTextContent ) : public CBase,
                                         public MEmailTextContent
{   
public:
    static CEmailTextContent* NewL( CPluginData& aPluginData, 
            const TMessageContentId& aMsgContentId, 
            CFSMailMessagePart* aPart,
            const TDataOwner aOwner );
    
    ~CEmailTextContent();
    
public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();

public: // from MEmailTextContent
    TTextType TextType() const;

    void SetTextL(
            const TTextType aPlainOrHtml,
            const TDesC& aText );
    
public: // from MEmailMessageContent
    
    TMessageContentId Id() const;

    TPtrC ContentType() const;

    void SetContentType( const TDesC& aContentType );

    TPtrC ContentId() const;

    void SetContentId( const TDesC& aContentId );

    TPtrC ContentDescription() const;

    void SetContentDescription( const TDesC& aContentDescription );

    TPtrC ContentDisposition() const;

    void SetContentDisposition( const TDesC& aContentDisposition );

    TPtrC ContentClass() const;

    void SetContentClass( const TDesC& aContentClass );

    TInt AvailableSize() const;

    TInt TotalSize() const;

    TPtrC ContentL() const;

    void SetContentL( const TDesC& aContent );

    void FetchL( MEmailFetchObserver& aObserver );

    void CancelFetch();

    void SaveToFileL( const TDesC& aPath );

    MEmailMultipart* AsMultipartOrNull() const;
    
    MEmailTextContent* AsTextContentOrNull() const;

    MEmailAttachment* AsAttachmentOrNull() const;
    
public: // for internal usage
    void SetOwner( const TDataOwner aOwner );

    
private:
    CEmailTextContent( const TDataOwner aOwner );
    
    void ConstructL( CPluginData& aPluginData, const TMessageContentId& aMsgContentId, CFSMailMessagePart* aPart);
    
private:        
    TTextType               iTextType;
    CEmailMessageContent*   iEmailMsgContent;
    TDataOwner              iOwner;
};

#endif // EMAILTEXTCONTENT_H_

// End of file

