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
* Description: mail content implementation definition
*
*/

#ifndef EMAILCONTENT_H
#define EMAILCONTENT_H

#include <memailcontent.h>

#include "CFSMailClient.h"
#include "emailapiutils.h"

using namespace EmailInterface;

class CFSMailPlugin;
class CFSMailRequestObserver;

NONSHARABLE_CLASS( CEmailMessageContent ) : public CBase, 
                                            public MEmailMessageContent
{
public:
    static CEmailMessageContent* NewL( 
            CPluginData& aPluginData,
            const TMessageContentId& aMsgContentId,
            CFSMailMessagePart* aPart);
        
    ~CEmailMessageContent();
    
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
    
public:     
    CFSMailMessagePart& Part();
    void SetPart( CFSMailMessagePart *aPart );
    
    CPluginData& PluginData();
        
protected:
    CEmailMessageContent( CPluginData& aPluginData, const TMessageContentId& aMsgContentId, CFSMailMessagePart* aPart );
    
private:
    class CContentRequestObserver: public CBase, 
                                   public MFSMailRequestObserver
        {
    public:
        CContentRequestObserver( CEmailMessageContent &aParent);
        
        virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
        void SetObserverL( MEmailFetchObserver* aObserver );
    private:
        MEmailFetchObserver* iObserver;
        CEmailMessageContent& iParent;
        };

private:
    void ConstructL( );
private:
    CPluginData&            iPluginData;
    CFSMailPlugin*          iPlugin;
    CFSMailMessagePart*     iPart;
    TMessageContentId       iMsgContentId;
    mutable HBufC*          iBuf;
    mutable TUint           iUsed;
    CContentRequestObserver *iFetchObserver;
    TInt                    iRequestId;
};

#endif // EMAILCONTENT_H

// End of file
