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
* Description: This file defines class MComposerFetchLogicCallback.
*
*/


#ifndef __FSCOMPOSERFETCHLOGIC_H__
#define __FSCOMPOSERFETCHLOGIC_H__


#include<e32base.h>
//#include <aknwaitnotewrapper.h> //cmail

#include"FreestyleEmailDownloadInformationMediator.h"

class CFSMailClient;
class CFSMailBox;
class CFSMailFolder;
class CFSMailMessage;
class CFreestyleEmailUiAppUi;

enum TComposerFetchState
	{
	EStateNotStarted,	// not started
	EStateSmartReply,	// smart reply test
	EStateSmartForward, // smart forward test
	EStateStructure,	// fetch message structure
	EStateBody,			// fetch message body
	EStateAttachments   // fetch attachments
	};


/**
* MComposerFetchLogicCallback
*/
class MComposerFetchLogicCallback
	{
public:
    virtual void FetchLogicComplete( TComposerFetchState aState, TInt aError ) = 0;
	};
	
	
/**
* CFsComposerFetchLogic
*/
class CFsComposerFetchLogic: 
    public CBase, 
    //public MProgressDialogCallback, //<cmail>
	public MFSEmailDownloadInformationObserver, 
	public MFSMailRequestObserver
	{
public: // new public methods
	
    static CFsComposerFetchLogic* NewL( CFSMailClient& aClient, 
                                        TFSMailMsgId aMailBoxId, TFSMailMsgId aFolderId, TFSMailMsgId aMessageId, 
                                        MComposerFetchLogicCallback& aObserver,
                                        CFreestyleEmailUiAppUi& aAppUi );

    ~CFsComposerFetchLogic();
		
    void RunReplyLogicL();
		
    void RunForwardLogicL();

    void CancelFetchings(); //<cmail>

public: // from MFSMailRequestObserver
		
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
		
public: // from MFSEmailDownloadInformationObserver

    void RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart );

public: // from MProgressDialogCallback <cmail> //MAknBackgroundProcess
        
    /**
    * Completes one cycle of the process.
    */
    //virtual void StepL(); <cmail>
    
    /**
    * Return true when the process is done.
    */
    //virtual TBool IsProcessDone() const; <cmail>
    
    /**
    * Callback when the process is finished.
    */
    //virtual void ProcessFinished();<cmail>

    /**
    * Callback when the dialog is dismissed
    */
    //void DialogDismissedL( TInt aButtonId ); <cmail>

    /**
    * Handles error occurred in StepL
    * @param aError error code that occurred
    * @return translated error code. If this is != KErrNone process will be stopped.
    */
    //virtual TInt CycleError( TInt aError ); <cmail>
        
private:
	
    CFsComposerFetchLogic( CFSMailClient& aClient, 
                           MComposerFetchLogicCallback& aObserver,
                           CFreestyleEmailUiAppUi& aAppUi );
				
    void ConstructL( TFSMailMsgId aMailBoxId, TFSMailMsgId aFolderId, 
                     TFSMailMsgId aMessageId );
		
    void RunStateL();
		
    void ShowWaitNoteL();

        /**
        * AskIfUserWantsToFetchAttachmentsL
        * @return ETrue if user select yes
        */
//        TBool AskIfUserWantsToFetchAttachmentsL();
        
    //void CancelFetchings(); <cmail>
        
    void Complete();
        
private:
	
    enum TRunMode
        {
        EModeInvalid,
        EModeReply,
        EModeForward
        };
			
    TRunMode iRunMode;
	
    TComposerFetchState iState;
		
	CFreestyleEmailUiAppUi& iAppUi;	
		
    CFSMailClient& iClient;

    MComposerFetchLogicCallback& iObserver;
		
    //<cmail> using different wait note than freestyle
    //CAknWaitNoteWrapper* iAsyncWaitNote;
    CAknWaitDialog* iAsyncWaitNote;
    TBool iFetchingCancelGoingOn;
    //</cmail>
    	
    TBool iFetchComplete;
    	
    CFSMailBox* iMailBox;
    	
    CFSMailFolder* iFolder;
    	
    CFSMailMessage* iMessage;

    TInt iError;
    	
    TInt iStructureRequestId;
    TBool iFetchingStructure;
    
    TInt iBodyRequestId;
    TBool iFetchingBody;
    	
    TBool iRequestCompleted; //<cmail>
    	
    /**
    * part data for those parts that are being downloaded
    */
    RArray<TPartData> iParts;
        
	};


#endif
