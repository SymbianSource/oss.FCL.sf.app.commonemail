/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CFsMtmClient.
*
*/


#ifndef CFSCLIENTMTM_H
#define CFSCLIENTMTM_H

#include <e32base.h>
#include <mtclbase.h>
#include "CFSMailCommon.h"

#include <msvstd.hrh>
enum TEasEmailMTMCmds
	{
	KEasEmailMTMReadEntry = KMtmFirstFreeMtmFunctionId, //from <msvstd.hrh>
	KEasEmailMTMAddEmailAttachment,
	KEasEmailMTMDeleteEntry,
	// KEEP THIS ALWAYS LAST!
	KEasEmailMTMLastCommand
	};

class CFsSendAsHelper;

/**
 * Implements MFsEmailBaseMtm
 *
 * @since S60 v5.2
 * @lib fsclientmtm.lib
 */
class MFsEmailBaseMtm
    {
    /**
     * Creates a new MTM account. The account also acts as an Inbox folder
     *
     * @param aAccountName Freestyle Account name.
     * @param aOwnAddress Freestyle Account own mail address.
     * @param aMsgId Freestyle Account details.
     * @return KErrNone if no problem in creating account.
    */
    virtual TInt CreateAccountL( const TDesC& aAccountName, 
    	const TDesC& aOwnAddress, TFSMailMsgId aMsgId) = 0;

    /**
     * Deletes created MTM account.
     *
     * @param aAccountName Freestyle Account name.
     * @param aMsgId Freestyle Account details.
     * @return KErrNone if no problem in deleting account.
    */
    virtual TInt DeleteAccountL( const TDesC& aAccountName, 
        TFSMailMsgId aMsgId) = 0;

    /**
     * Renames existing MTM account.
     *
     * @param aAccountName Freestyle Account name.
     * @param aOwnAddress Freestyle Account own mail address.
     * @param aMsgId Freestyle Account details.
     * @return KErrNone if no problem in deleting account.
     */
    virtual TInt RenameAccountL( const TDesC& aAccountNewName, 
    	const TDesC& aOwnAddress, TFSMailMsgId aMsgId) = 0;
    };
    
/**
 * Implements CBaseMtm to provide Client-side MTM functionality
 *
 * @since S60 3.1
 * @lib fsclientmtm.lib
 */
class CFsMtmClient : public CBaseMtm, public MFsEmailBaseMtm
    {
public: // Construction and destruction 
    /**
     * Constructs new object.
     * @return created object.
    */
    IMPORT_C static CFsMtmClient* NewL(
        CRegisteredMtmDll& aRegisteredMtmDll, CMsvSession& aMsvSession);

    /**
     * Destructor.
    */
    ~CFsMtmClient();
    
public: // From MFsEmailBaseMtm
    virtual TInt CreateAccountL(const TDesC& aAccountName, 
        const TDesC& aOwnAddress, TFSMailMsgId aMsgId);
    
    virtual TInt DeleteAccountL(const TDesC& aAccountName, 
        TFSMailMsgId aMsgId);
    
    virtual TInt RenameAccountL( const TDesC& aAccountNewName, 
        const TDesC& aOwnAddress, TFSMailMsgId aMsgId);

public: // From CBaseMtm 

    virtual void SetSubjectL(const TDesC& aSubject);    
    
    // Attachment functions to support the SendAs API
    virtual void AddAttachmentL(const TDesC& aFilePath, 
        const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);

    virtual void AddAttachmentL(RFile& aFile, const TDesC8& aMimeType, 
        TUint aCharset, TRequestStatus& aStatus);

    virtual void AddLinkedAttachmentL(const TDesC& aFilePath, 
        const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus);

    virtual void AddEntryAsAttachmentL(TMsvId aAttachmentId, 
        TRequestStatus& aStatus);

    virtual void CreateAttachmentL(const TDesC& aFileName, 
       RFile& aAttachmentFile, const TDesC8& aMimeType, TUint aCharset, 
       TRequestStatus& aStatus);
    
    // Addressee list (used by objects with no MTM knowledge)
    void AddAddresseeL(const TDesC& aRealAddress);
    
    void AddAddresseeL(const TDesC& aRealAddress, const TDesC& aAlias);
    
    void AddAddresseeL(TMsvRecipientType aType, const TDesC& aRealAddress);
    
    void AddAddresseeL(TMsvRecipientType aType, const TDesC& aRealAddress, 
            const TDesC& aAlias);
    
    void RemoveAddressee(TInt aIndex);

    // Store and restore entry data 
    void SaveMessageL(); 
    
    void LoadMessageL();

    // Response message creation 
    CMsvOperation* ReplyL  (TMsvId aReplyEntryId, TMsvPartList aPartlist, 
        TRequestStatus& aCompletionStatus);
    
    CMsvOperation* ForwardL(TMsvId aForwardEntryId, TMsvPartList aPartList, 
        TRequestStatus& aCompletionStatus);

    // Validation and search 
    TMsvPartList ValidateMessage(TUint aPartList);
    
    TMsvPartList Find(const TDesC& aTextToFind, TMsvPartList aPartList);

    // MTM-specific functionality
    TInt QueryCapability(TUid aCapability, TInt& aResponse);
    
    void InvokeSyncFunctionL(TInt aFunctionId, 
            const CMsvEntrySelection& aSelection, TDes8& aParameter);
    
    CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId, 
        const CMsvEntrySelection& aSelection, TDes8& aParameter, 
        TRequestStatus& aCompletionStatus);
    
    IMPORT_C TInt RemoveRedundantAccountsL(RArray<TFSMailMsgId>& aMailBoxIds);
	
	IMPORT_C TInt AddEmailAsAttachmentL( const TUint& aPluginId, const TDesC& aAttachmentPath, TMsvId& aNewAttachEntryId );
	IMPORT_C TInt DeleteEmailAsAttachmentL( const TMsvId aNewAttachEntryId );

protected: // From CBaseMtm 
    void ContextEntrySwitched(); 

private: // methods used internally
     
    /**
     * Finds the accounts entry based on account name.
     *
     * @param aAccountName Freestyle Account name.
     * @param aMsgId Freestyle Account details.
    */
    void SetTMsvIdsL( const TDesC& aAccountName, TFSMailMsgId aMsgId);
    
private: // constructors

    /**
     * Constructor.
     *
     * @param aRegisteredMtmDll  To access self registry information.
     * @param aMsvSession To communicate from Client to Server
     */
    CFsMtmClient(CRegisteredMtmDll& aRegisteredMtmDll, 
        CMsvSession& aMsvSession);
    /**
     * Second-phase constructor.
     */
    void ConstructL();
        
private: //data
    // Current account details
    TMsvId iAccountId;

    // Current account details
    CMsvEntry* iAccountEntry;

    // To call sendasinfo functions.    
    CFsSendAsHelper* iSendAsHelper; 
    
    /**
     * Tells if feature Email Attached In Email is ON or OFF.
     * State is looked from cenrep.
     */
	TBool 				iEmailAttachedInEmail;
	
    CMsvOperationActiveSchedulerWait* iActiveSchedulerWait;
    TMsvId iMTMAttachmentId;
    };

#endif  // CFSCLIENTMTM_H
