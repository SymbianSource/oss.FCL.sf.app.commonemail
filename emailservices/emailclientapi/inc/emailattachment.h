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
* Description: Definition of Email attachment.
*
*/


#ifndef EMAILATTACHMENT_H
#define EMAILATTACHMENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "cfsmailclient.h"
#include <memailcontent.h>
#include "emailapiutils.h"

using namespace EmailInterface;

class CEmailMessageContent;
// CLASS DECLARATION

/**
 *  CEmailAttachment
 * 
 */

NONSHARABLE_CLASS(CEmailAttachment) : public CBase, public MEmailAttachment
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CEmailAttachment();

    /**
     * Two-phased constructor.
     */
    static CEmailAttachment* NewL(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart *aAtt,
        const TDataOwner aOwner );    

    /**
     * Two-phased constructor.
     */
    static CEmailAttachment* NewLC(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,    
        CFSMailMessagePart *aAtt,
        const TDataOwner aOwner );    

public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();

public: // from MEmailAttachment
    /**
    * Returns file handle of this attachment. If the attachment is
    * not associated with a file, null file handle is returned.
    */
    virtual RFile FileL() const;

    /**
    * Sets file name field
    */
    virtual void SetFileNameL( const TDesC& aFileName );

    /**
    * Returns file name or null pointer descriptor if attachment
    * is not associated with any file
    */
    virtual TPtrC FileNameL() const;
    
    
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
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CEmailAttachment( const TDataOwner aOwner );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(
            CPluginData& aPluginData,
            const TMessageContentId& aMsgContentId );    

    void ConstructL(
            CPluginData& aPluginData,
            const TMessageContentId& aMsgContentId,
            CFSMailMessagePart *aPart);    
    
private:        
    CEmailMessageContent*   iEmailMsgContent;
    TDataOwner iOwner;
    };

#endif // EMAILATTACHMENT_H
