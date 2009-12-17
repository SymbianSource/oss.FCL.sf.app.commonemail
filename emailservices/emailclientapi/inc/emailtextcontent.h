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
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();

public: // from MEmailTextContent
    virtual TTextType TextType() const;

    virtual void SetTextL(
            const TTextType aPlainOrHtml,
            const TDesC& aText );
    
public: // from MEmailMessageContent
    
    virtual TMessageContentId Id() const;

    virtual TPtrC ContentType() const;

    virtual void SetContentType( const TDesC& aContentType );

    virtual TPtrC ContentId() const;

    virtual void SetContentId( const TDesC& aContentId );

    virtual TPtrC ContentDescription() const;

    virtual void SetContentDescription( const TDesC& aContentDescription );

    virtual TPtrC ContentDisposition() const;

    virtual void SetContentDisposition( const TDesC& aContentDisposition );

    virtual TPtrC ContentClass() const;

    virtual void SetContentClass( const TDesC& aContentClass );

    virtual TInt AvailableSize() const;

    virtual TInt TotalSize() const;

    virtual TPtrC ContentL() const;

    virtual void SetContentL( const TDesC& aContent );

    virtual void FetchL( MEmailFetchObserver& aObserver );

    virtual void CancelFetch();

    virtual void SaveToFileL( const TDesC& aPath );

    virtual MEmailMultipart* AsMultipartOrNull() const;
    
    virtual MEmailTextContent* AsTextContentOrNull() const;

    virtual MEmailAttachment* AsAttachmentOrNull() const;
    
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

#endif /* EMAILTEXTCONTENT_H_ */
