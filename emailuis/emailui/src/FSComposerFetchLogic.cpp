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
* Description: This file implements class CFsComposerFetchLogic.
*
*/


//<cmail>
#include "emailtrace.h"
#include "CFSMailClient.h"
#include "CFSMailCommon.h"
//</cmail>
#include <FreestyleEmailUi.rsg>
//#include <aknquerydialog.h> //<cmail>
#include <StringLoader.h>

#include "FreestyleEmailUiAppui.h"
#include "FSComposerFetchLogic.h"
#include "FreestyleEmailUiUtilities.h"
#include "FSEmail.pan"

// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::NewL
// -----------------------------------------------------------------------------
//
CFsComposerFetchLogic* CFsComposerFetchLogic::NewL( CFSMailClient& aClient,
	TFSMailMsgId aMailBoxId, TFSMailMsgId aFolderId, TFSMailMsgId aMessageId,
	MComposerFetchLogicCallback& aObserver, CFreestyleEmailUiAppUi& aAppUi )
	{
    FUNC_LOG;

	CFsComposerFetchLogic* self = new (ELeave) CFsComposerFetchLogic( aClient, aObserver, aAppUi );
	CleanupStack::PushL( self );
	self->ConstructL( aMailBoxId, aFolderId, aMessageId );
	CleanupStack::Pop( self );

    return self;
	}


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::CFsComposerFetchLogic
// -----------------------------------------------------------------------------
//
CFsComposerFetchLogic::CFsComposerFetchLogic( CFSMailClient& aClient,
	MComposerFetchLogicCallback& aObserver, CFreestyleEmailUiAppUi& aAppUi ):
	iRunMode( EModeInvalid ), iState( EStateNotStarted ), iAppUi( aAppUi ),
	iClient( aClient ), iObserver( aObserver ),iFetchingCancelGoingOn(EFalse),//<cmail>
        iFetchingStructure(EFalse), iFetchingBody(EFalse),iRequestCompleted(EFalse)//<cmail>
	{
    FUNC_LOG;
	}


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::~CFsComposerFetchLogic
// -----------------------------------------------------------------------------
//
CFsComposerFetchLogic::~CFsComposerFetchLogic()
	{
    FUNC_LOG;

    //CancelFetchings(); <cmail>
    if ( iAppUi.DownloadInfoMediator() )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this );
        }

	iParts.Close();

    //delete iAsyncWaitNote; //<cmail>
	delete iMessage;
	delete iFolder;
	delete iMailBox;

	}


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::ConstructL
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::ConstructL( TFSMailMsgId aMailBoxId,
	TFSMailMsgId aFolderId, TFSMailMsgId aMessageId )
	{
    FUNC_LOG;

	iMailBox = iClient.GetMailBoxByUidL( aMailBoxId );
	iFolder = iClient.GetFolderByUidL( aMailBoxId, aFolderId );
	iMessage = iClient.GetMessageByUidL( aMailBoxId, aFolderId, aMessageId, EFSMsgDataStructure );

	}


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::RunReplyLogicL
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::RunReplyLogicL()
	{
    FUNC_LOG;

	iRunMode = EModeReply;
	iState = EStateSmartReply;
	iError = KErrNone;
	RunStateL();
	}


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::RunForwardLogicL
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::RunForwardLogicL()
	{
    FUNC_LOG;

	iRunMode = EModeForward;
	iState = EStateSmartForward;
	iError = KErrNone;
	if ( iAppUi.DownloadInfoMediator() )
		{
		iAppUi.DownloadInfoMediator()->AddObserver( this, iMessage->GetMessageId() );
		}
	RunStateL();

	}


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::RequestResponseL
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::RequestResponseL( TFSProgress aEvent, TInt aRequestId )
    {
    FUNC_LOG;

    iError = aEvent.iError;

    if ( !iError )
        {
        if ( TFsEmailUiUtility::IsCompleteOrCancelEvent( aEvent ) )
            {
            if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled )
                {
                iError = KErrCancel;
                }
            else
                {
                if ( iStructureRequestId == aRequestId )
                    {
                    iState = EStateBody;
                    }
                else if ( iBodyRequestId == aRequestId )
                    {
                    iState = EStateAttachments;
                    }
                else
                    {
                    __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
                    iError = KErrUnknown;
                    }

                if ( !iError )
                    {
                    TRAP( iError, RunStateL() );
                    }
                }
            }
        }
    
    if ( iError )
        {
        // Show error note for connection errors. Notes for other errors
        // are shown in CNcsComposeView::DoeActivateL()
        if ( iError == KErrCouldNotConnect ||
             iError == KErrConnectionTerminated )
            {
            TFsEmailUiUtility::ShowErrorNoteL(
                R_FREESTYLE_EMAIL_ERROR_GENERAL_CONNECTION_ERROR,
                ETrue );
            }
        Complete();
        }

    }
	

// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::RequestResponseL
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart )
	{
    FUNC_LOG;

    // Ignore all responses when cancelling is in progress.
    if ( !iFetchingCancelGoingOn )
        {
        if ( !iError && aEvent.iError )
            {
            // Cancel all fetching when first error occurs
            iError = aEvent.iError;
            CancelFetchings();
            }
        else if ( TFsEmailUiUtility::IsCompleteOrCancelEvent( aEvent ) )
            {
            // Remove the completed/failed download item
            for ( TInt i=0; i<iParts.Count(); i++ )
                {
                if ( iParts[i] == aPart )
                    {
                    iParts.Remove( i );
                    break;
                    }
                }

            // If last part just got removed, then move on
            if ( !iParts.Count() )
                {
                Complete();
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::StepL
// -----------------------------------------------------------------------------
//
//<cmail>
/*
void CFsComposerFetchLogic::StepL()
	{
    FUNC_LOG;
	}*/
//<cmail>


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::IsProcessDone
// -----------------------------------------------------------------------------
//
//<cmail>
/*
TBool CFsComposerFetchLogic::IsProcessDone() const
	{
    FUNC_LOG;
	return iFetchComplete;
	}*/
//<cmail>


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::ProcessFinished
// -----------------------------------------------------------------------------
//
//<cmail>
/*
void CFsComposerFetchLogic::ProcessFinished()
	{
    FUNC_LOG;
	}*/
//<cmail>


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::DialogDismissedL
// -----------------------------------------------------------------------------
//
/*void CFsComposerFetchLogic::DialogDismissedL( TInt aButtonId )//<cmail>
	{
	if(aButtonId == EAknSoftkeyCancel)
	    {
	    CancelFetchings();
	    iError = KErrCancel;
	    }
    FUNC_LOG;
	}*///<cmail>


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::CycleError
// -----------------------------------------------------------------------------
//
//<cmail>
/*
TInt CFsComposerFetchLogic::CycleError( TInt aError )
	{
    FUNC_LOG;
	return aError;
	}*/
//</cmail>


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::RunState
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::RunStateL()
	{
    FUNC_LOG;

	TBool doNextState = EFalse;

	do
		{
		doNextState = EFalse;

		switch ( iState )
			{
			case EStateSmartReply:
				{
				TBool supported = iMailBox->HasCapability( EFSMBoxCapaSmartReply );
				if ( supported )
					{
					Complete();
					}
				else
					{
					iState = EStateStructure;
					doNextState = ETrue;
					}
				}
			break;

			case EStateSmartForward:
				{
				TBool supported = iMailBox->HasCapability( EFSMBoxCapaSmartForward );
				if ( supported )
					{
					Complete();
					}
				else
					{
					iState = EStateStructure;
					doNextState = ETrue;
					}
				}
			break;

			case EStateStructure:
				{
				TBool hasStructure = TFsEmailUiUtility::IsMessageStructureKnown( *iMessage );
				if ( hasStructure )
					{
					iState = EStateBody;
					doNextState = ETrue;
					}
				else
					{
					RArray<TFSMailMsgId> ids;
					CleanupClosePushL( ids );
					ids.AppendL( iMessage->GetMessageId() );
					TRAP( iError, iStructureRequestId = iFolder->FetchMessagesL( ids, EFSMsgDataStructure, *this ) );
					CleanupStack::PopAndDestroy( &ids );
					if ( KErrNone == iError )
						{
						// fetching started successfully
						iFetchingStructure = ETrue;
						// show wait note
						//ShowWaitNoteL(); //<cmail>
						}
					else
						{
						// error occurred
					    Complete();
						}
					}
				}
			break;

			case EStateBody:
				{
				CFSMailMessagePart* body = iMessage->PlainTextBodyPartL();
                TBool fetched = EFalse;
                // Do we have plain text body part
                if ( !body )
                    {
                    // No plain text body part so try to get HTML body part
                    body = iMessage->HtmlBodyPartL();
                    }

                // Do we have any body part
                if ( body )
                    {
                    fetched = TFsEmailUiUtility::IsMessagePartFullyFetched( *body );
                    }
                else
                    {
                    // If there is no body part, then it doens't need to be fetched
                    // and we may move on.
                    fetched = ETrue;
                    }
                
                CleanupStack::PushL( body );
                if ( fetched )
                    {
                    if ( EModeReply == iRunMode )
                        {
                        Complete();
                        }
                    else if ( EModeForward == iRunMode )
                        {
                        iState = EStateAttachments;
                        doNextState = ETrue;
                        }
                    else
                        {
                        // should not come here
                        __ASSERT_DEBUG( EFalse, Panic( EFSEmailUiUnexpectedValue ) );
                        }
                    }
                else
                    {
                    TFSMailMsgId textPartId = body->GetPartId();

                    TRAP( iError, iBodyRequestId = body->FetchMessagePartL( textPartId, *this, 0 ) );
                    if ( KErrNone == iError )
                        {
                        // fetching started successfully
                        iFetchingBody = ETrue;
                        // show wait note if not already visible
                        //ShowWaitNoteL(); //<cmail>
                        }
                    else
                        {
                        // error occurred
                        Complete();
                        }
                    }
                CleanupStack::PopAndDestroy( body );
				}
			break;
			
			case EStateAttachments:
				{
				if ( TFsEmailUiUtility::HasUnfetchedAttachmentsL( *iMessage ) )
					{
					RPointerArray<CFSMailMessagePart> attachments;
					CleanupResetAndDestroyClosePushL( attachments );
                    iMessage->AttachmentListL( attachments );
                    for ( TInt i=0; i<attachments.Count(); i++ )
						{
						if ( !TFsEmailUiUtility::IsMessagePartFullyFetched( *attachments[i] ) )
							{
							if ( iAppUi.DownloadInfoMediator() ) 
								{
	                            TPartData data;
	                            data.iMailBoxId = iMailBox->GetId();
	                            data.iFolderId = iMessage->GetFolderId();
	                            data.iMessageId = iMessage->GetMessageId();
	                            data.iMessagePartId = attachments[i]->GetPartId();
	                            iParts.AppendL( data );
								iAppUi.DownloadInfoMediator()->DownloadL( data, EFalse );				
								}
							else
							    {
							    iError = KErrGeneral;
							    Complete();
							    }
							}
						}
					CleanupStack::PopAndDestroy( &attachments );
					// show wait note if not already visible
					//ShowWaitNoteL(); //<cmail>
					}
				else
					{
					Complete();
					}
				}
			break;
			}
		}
	while ( doNextState );

	}


// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::ShowWaitNoteL
// -----------------------------------------------------------------------------
//
/*void CFsComposerFetchLogic::ShowWaitNoteL() //<cmail>
	{
    FUNC_LOG;

	if ( !iAsyncWaitNote )
		{
        iFetchComplete = EFalse;

        //<cmail>
        iAsyncWaitNote = new(ELeave)CAknWaitDialog(
                       (REINTERPRET_CAST(CEikDialog**,&iAsyncWaitNote)), ETrue);
        iAsyncWaitNote->SetCallback(this);
        //iError = KErrNone;
        iAsyncWaitNote->ExecuteLD(R_FSE_FETCHING_WAIT_DIALOG);

        iAsyncWaitNote = CAknWaitNoteWrapper::NewL(); //<cmail>
        TBool result = iAsyncWaitNote->ExecuteL( R_FSE_FETCHING_WAIT_DIALOG, *this, ETrue );
        if ( !result )
        	{
        	// dialog was canceled
        	iError = KErrCancel;
        	// stop downloads
        	CancelFetchings();
        	}//<cmail>
        //<cmail>

        //iObserver.FetchLogicComplete( iState, iError ); //cmail
		}

	}*/ //<cmail>

// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::AskIfUserWantsToFetchAttachmentsL
// -----------------------------------------------------------------------------
//
/*
TBool CFsComposerFetchLogic::AskIfUserWantsToFetchAttachmentsL()
	{

	CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
	dlg->PrepareLC( R_FSE_EDITOR_CONFIRM_ATTACHMENT_DOWNLOAD_DIALOG );
	CAknQueryControl* control = STATIC_CAST( CAknQueryControl*, dlg->ControlOrNull( EGeneralQuery ) );
	HBufC* prompt = StringLoader::LoadLC( R_FSE_EDITOR_CONFIRM_ATTACHMENT_DOWNLOAD );
	control->SetPromptL( *prompt );
	CleanupStack::PopAndDestroy( prompt );
	return TBool( dlg->RunLD() );

	}
*/

// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::CancelFetchings
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::CancelFetchings()
    {
    FUNC_LOG;
    iFetchingCancelGoingOn = ETrue; //<cmail> during cancel other event may come

    // message structure fetching
    if ( iFetchingStructure )
        {
        TRAPD( error, iClient.CancelL( iStructureRequestId ) );
        if ( error )
            {
            }
        else
            {
            iFetchingStructure = EFalse;
            }
        }

    // message body fetching
    if ( iFetchingBody )
        {
        TRAPD( error, iClient.CancelL( iBodyRequestId ) );
        if ( error )
            {
            }
        else
            {
            iFetchingBody = EFalse;
            }
        }

    // message part downloads
    if ( iAppUi.DownloadInfoMediator() )
        {
        // Note that we don't bother removing anything from iParts now; they
        // will be cleaned up later.
        for ( TInt i = iParts.Count() - 1; i >= 0; i-- )
            {
            TRAP_IGNORE( iAppUi.DownloadInfoMediator()->CancelDownloadL( iParts[i].iMessagePartId ) );
            }
        }

    //<cmail>
    if( !iRequestCompleted )
        {
        if ( iObserver.FetchLogicComplete( iState, KErrCancel ))
            {
            // If the observer deleted this object, don't try to do anything
            // else.
            return;
            }
        iRequestCompleted = ETrue; //so that no stray events from plugins can cause panic
        }

    iFetchingCancelGoingOn = EFalse;
    //</cmail>
    }

// -----------------------------------------------------------------------------
// CFsComposerFetchLogic::Complete
// -----------------------------------------------------------------------------
//
void CFsComposerFetchLogic::Complete()
	{
    FUNC_LOG;

    //<cmail>
    if(!iRequestCompleted)
        {
    //</cmail>
	if ( iAppUi.DownloadInfoMediator() )
		{
		iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
		}

        /*if ( iAsyncWaitNote ) //<cmail>
		{
		iFetchComplete = ETrue;
		//<cmail> new note stops like this and not with IsProcessFinished method
		iAsyncWaitNote->ProcessFinishedL();
		//</cmail>
		// observer is called in ShowWaitNoteL where showing
		// wait note returns
		}
	else
            {*/ //<cmail>
	    // wait note not active, call observer
        //<cmail>
        iFetchingStructure = EFalse;
        iFetchingBody = EFalse;
        iRequestCompleted = ETrue;
        //</cmail>
		iObserver.FetchLogicComplete( iState, iError );
            //} //<cmail>
	    }

	}

