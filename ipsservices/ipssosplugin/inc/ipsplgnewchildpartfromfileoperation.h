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
* Description:  New child part from file operation
*
*/

// <qmail>
#ifndef IPSPLGNEWCHILDPARTFROMFILEOPERATION_H
#define IPSPLGNEWCHILDPARTFROMFILEOPERATION_H

class MFSMailRequestObserver;

NONSHARABLE_CLASS ( CCIpsPlgNewChildPartFromFileOperation ): public CMsvOperation
{
public:
    /**
    * Constructor.
    */
    static CCIpsPlgNewChildPartFromFileOperation* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId,
        const TDesC& aContentType,
        const TDesC& aFilePath,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId,
        CIpsPlgMsgMapper *aMsgMapper);

    /**
    * Destructor.
    */
    virtual ~CCIpsPlgNewChildPartFromFileOperation();

public: // From CMsvOperation
    virtual const TDesC8& ProgressL();
    
protected:
    /**
    * Constructor.
    */
    CCIpsPlgNewChildPartFromFileOperation(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId);
    
    /**
    * Constructor.
    */
    void ConstructL(CIpsPlgMsgMapper *aMsgMapper,
        const TDesC& aContentType,
        const TDesC& aFilePath);

    /**
    * From CActive
    */
    void RunL();

    /**
    * From CActive
    */
    void DoCancel();

    /**
    * From CActive
    */
    TInt RunError(TInt aError);
    
    void SignalFSObserver(TInt aStatus, CFSMailMessagePart* aMessagePart );

private:
    void GetMessageEntryL( TMsvId aId,
                           CMsvEntry*& aMessageEntry,
                           CImEmailMessage*& aImEmailMessage );
    
    void CleanCachedMessageEntries();
    
    enum TOperationStep {EStep1, EStep2, EStep3, EStep4} ;
    
protected:
    CMsvOperation* iOperation;   // owned
    TBuf8<1> iBlank;
    TFSMailMsgId iMailBoxId;
    TFSMailMsgId iMessageId;
    MFSMailRequestObserver& iOperationObserver;  // not owned
    TInt iRequestId;
    HBufC* iContentType;
    HBufC* iFilePath;
    CMsvEntry* iCachedEntry;
    CImEmailMessage* iCachedEmailMessage;
    CImEmailMessage* iMessage;
    CMsvEntry* iEntry;
    CIpsPlgMsgMapper* iMsgMapper;
    TMsvId iNewAttachmentId;
    TOperationStep iStep;
};

#endif // IPSPLGNEWCHILDPARTFROMFILEOPERATION_H

// </qmail>
