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


#ifndef EMAILMULTIPART_H_
#define EMAILMULTIPART_H_
// INCLUDES
#include "CFSMailClient.h"
#include <memailcontent.h>
#include "emailapiutils.h"

using namespace EmailInterface;

class CEmailMessageContent;


// CLASS DECLARATION

NONSHARABLE_CLASS( CEmailMultipart ) : public CBase,
                                       public MEmailMultipart
{
public:
    static CEmailMultipart* NewL( CPluginData& aPluginData, 
            const TMessageContentId& aMsgContentId, 
            CFSMailMessagePart* aPart,
            const TDataOwner aOwner );
    
    ~CEmailMultipart();
    
public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();

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
    
public: // from MEmailMultipart
    virtual TInt PartCountL();

    virtual MEmailMessageContent* PartByIndexL( const TUint aIndex ) const;

    virtual void DeletePartL( const TUint aIndex );

    virtual void AddPartL(
        const MEmailMessageContent& aPart,
        const TUint aPos );
    
public: // for internal usage
    void SetOwner( const TDataOwner aOwner );
    
    
private:
    CEmailMultipart( const TDataOwner aOwner );
    
    void ConstructL( CPluginData& aPluginData, const TMessageContentId& aMsgContentId, CFSMailMessagePart* aPart);
    
private: // Private data        
    CEmailMessageContent* iEmailMsgContent;
    RArray<TFSMailMsgId> iChildParts;
    TUint iChildPartCount;
    TDataOwner iOwner;
};

#endif /* EMAILMULTIPART_H_ */
