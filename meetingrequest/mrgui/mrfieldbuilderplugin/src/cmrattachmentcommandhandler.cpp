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
 * Description:  MR attahcment field implementation
 *
 */

#include "cmrattachmentcommandhandler.h"
#include "cmropenattachmentcommand.h"
#include "cmrremoveattachmentcommand.h"
#include "cmrsaveattachmentcommand.h"
#include "cmrsaveandopenattachmentcommand.h"
#include "cesmrrichtextlink.h"
#include "mcalremoteattachmentinfo.h"
#include "mcalremoteattachment.h"
#include "mcalremoteattachmentoperation.h"
#include "ccalremoteattachmentinfo.h"
#include "ccalremoteattachmentapi.h"
#include "cesmrgenericfieldevent.h"
#include "cesmrfieldcommandevent.h"
#include "mesmrfieldeventqueue.h"
#include "esmrdef.h"

#include <esmrgui.rsg>
#include <calentry.h>
#include <calattachment.h>
#include <f32file.h>
#include <apgcli.h>
#include <DocumentHandler.h>
#include <StringLoader.h>
#include <AknGlobalNote.h>

// DEBUG
#include "emailtrace.h"


// unnamed namespace for local definitions
namespace { // codescanner::namespace

_LIT8( KUnknownDatatype, "unknown");

#if defined(_DEBUG)

// Panic literal
_LIT( KMRAttachmentCommandHandler, "MRAttachmentCommandHandler" );

/**
 * Panic codes
 */
enum TMRAttachmentCommandHandlerPanic
    {
    // Invalid command
    EMRAttachmentCommandHandlerInvalidCommand,
    // Attachment is not found
    EMRAttachmentCommandHandlerAttachmentNotFound,
    // Remote attachment information not set
    EMRAttachmentCommandHandlerRemoteInfoNotSet,
    // Remote command cannot be found
    EMRAttachmentCommandHandlerRemoteCommandNotFound,
    // Remote attachment is not found
    EMRAttachmentCommandHandlerRemoteAttachmentNotFound
    };

void Panic( TMRAttachmentCommandHandlerPanic aPanic )
    {
    User::Panic( KMRAttachmentCommandHandler, aPanic );
    }

#endif // _DEBUG

/**
 * Shows information note when one attachment is saved.
 */
void ShowInfoNoteL()
    {
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    HBufC* text =
            StringLoader::LoadLC( R_MEET_REQ_INFO_NOTE_ONE_ATTACHMENT_SAVED );
    note->ShowNoteL(
            EAknGlobalInformationNote,
            *text );
    CleanupStack::PopAndDestroy( 2, note );
    }

/**
 * Shows information note when multiple attachments have been saved.
 */
void ShowInfoNoteL( TInt aAttachmentCount )
    {
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    HBufC* text = StringLoader::LoadLC(
            R_MEET_REQ_INFO_NOTE_MULTIPLE_ATTACHMENTS_SAVED,
            aAttachmentCount );
    note->ShowNoteL(
                EAknGlobalInformationNote,
                *text );
    CleanupStack::PopAndDestroy( 2, note );
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::CMRAttachmentCommandHandler
// ---------------------------------------------------------------------------
//
CMRAttachmentCommandHandler::CMRAttachmentCommandHandler(
        CCalEntry& aEntry,
        MESMRFieldEventQueue& aEventQueue) :
    iEntry( aEntry ),
    iEventQueue( aEventQueue )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::~CMRAttachmentCommandHandler
// ---------------------------------------------------------------------------
//
CMRAttachmentCommandHandler::~CMRAttachmentCommandHandler()
    {
    FUNC_LOG;

    iDownloadOperations.ResetAndDestroy();
    delete iDocHandler;
    }


// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::NewL
// ---------------------------------------------------------------------------
//
CMRAttachmentCommandHandler* CMRAttachmentCommandHandler::NewL(
            CCalEntry& aEntry,
            MESMRFieldEventQueue& aEventQueue )
    {
    FUNC_LOG;

    CMRAttachmentCommandHandler* self =
            new (ELeave) CMRAttachmentCommandHandler( aEntry, aEventQueue );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::ConstructL()
    {
    FUNC_LOG;

    iDocHandler = CDocumentHandler::NewL();
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::HandleAttachmentCommandL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::HandleAttachmentCommandL(
        TInt aCommandId,
        const CESMRRichTextLink& aSelectedLink )
    {
    FUNC_LOG;

    TInt attachmentIndex( ResolveAttachmentIndexL( aSelectedLink.Value() ) );
    HandleAttachmentCommandInternalL( aCommandId, attachmentIndex );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::IsRemoteAttachmentL
// ---------------------------------------------------------------------------
//
TBool CMRAttachmentCommandHandler::IsRemoteAttachmentL(
        const CESMRRichTextLink& aSelectedLink )
    {
    FUNC_LOG;

    TInt index = ResolveAttachmentIndexL( aSelectedLink.Value() );

    CCalAttachment* attachment = iEntry.AttachmentL( index );

    TBool remoteAttachment( ETrue );

    if ( CCalAttachment::EFile == attachment->Type() )
        {
        remoteAttachment = EFalse;
        }

    return remoteAttachment;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::SetRemoteAttachmentInformationL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::SetRemoteAttachmentInformationL(
            CCalRemoteAttachmentApi& aRemoteAttaApi,
            CCalRemoteAttachmentInfo& aAttachmentInfo )
    {
    FUNC_LOG;

    iRemoteAttachmentApi = &aRemoteAttaApi;
    iAttachmentInfo = &aAttachmentInfo;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::SetRemoteAttachmentInformationL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::HandleRemoteAttachmentCommandL(
        TInt aCommandId,
        const CESMRRichTextLink& aSelectedLink )
    {
    FUNC_LOG;

    // Check that remote attachment information is being set
    __ASSERT_DEBUG(
            iRemoteAttachmentApi,
            Panic(EMRAttachmentCommandHandlerRemoteInfoNotSet) );

    __ASSERT_DEBUG(
            iAttachmentInfo,
            Panic(EMRAttachmentCommandHandlerRemoteInfoNotSet) );

    HandleRemoteAttachmentCommandInternalL(
            aCommandId,
            aSelectedLink.Value() );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::RemoteOperations
// ---------------------------------------------------------------------------
//
RPointerArray<MCalRemoteAttachmentOperation>&
        CMRAttachmentCommandHandler::RemoteOperations()
    {
    FUNC_LOG;
    return iDownloadOperations;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::CurrentCommandInProgress
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentCommandHandler::CurrentCommandInProgress() const
    {
    FUNC_LOG;

    return iCommandInProgress;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::Progress
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::Progress(
         MCalRemoteAttachmentOperation* aOperation,
         TInt aPercentageCompleted )
    {
    FUNC_LOG;

    TRAP_IGNORE( NotifyProgressL(
            aOperation->AttachmentInformation().AttachmentLabel(),
            aPercentageCompleted ));
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::OperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::OperationCompleted(
        MCalRemoteAttachmentOperation* aOperation,
        RFile& aAttachment )
    {
    FUNC_LOG;

    TRAP_IGNORE( HandleOperationCompletedL( aOperation, aAttachment ) );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::OperationError
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::OperationError(
        MCalRemoteAttachmentOperation* /*aOperation*/,
        TInt aErrorCode )
    {
    FUNC_LOG;

    // Operation failed or cancelled, hide download indicator
    TRAP_IGNORE( HideDownloadIndicatorL() );
	iCommandInProgress = 0;

	if ( aErrorCode != KErrCancel )
        {
        // Operation failed, show error note
        CCoeEnv::Static()->HandleError( aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::ResolveAttachmentIndexL
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentCommandHandler::ResolveAttachmentIndexL(
        const TDesC& aAttachmentLabel )
    {
    FUNC_LOG;

    TInt index( KErrNotFound );

    TInt attachmentCount( iEntry.AttachmentCountL() );
    for (TInt i(0); i < attachmentCount && KErrNotFound == index; ++i )
        {
        CCalAttachment* attachment = iEntry.AttachmentL(i);

        TPtrC attachmentLabel( attachment->Label() );
        if ( !aAttachmentLabel.Compare(attachmentLabel) )
            {
            index = i;
            }
        }

    // Check that attachment is always found
    // If attachment is not found --> Our data is corrupted
    __ASSERT_DEBUG(
            KErrNotFound != index,
            Panic( EMRAttachmentCommandHandlerAttachmentNotFound)  );

    return index;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::ResolveAttachmentIndexL
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentCommandHandler::CommandIndex(
        const TDesC& aAttachmentLabel )
    {
    FUNC_LOG;

    TInt index( KErrNotFound );

    TInt commandCount( iDownloadOperations.Count() );
    for ( TInt i(0); i < commandCount && KErrNotFound == index; ++i )
        {
        // There are download operations in progress
        const MCalRemoteAttachment& attachInfo =
                iDownloadOperations[i]->AttachmentInformation();

        TPtrC label( attachInfo.AttachmentLabel() );
        if ( !label.Compare( aAttachmentLabel ) )
            {
            index = i;
            }
        }

    return index;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::RemoteAttachmentIndex
// ---------------------------------------------------------------------------
//
TInt CMRAttachmentCommandHandler::RemoteAttachmentIndexL(
        const TDesC& aAttachmentLabel )
    {
    FUNC_LOG;

    TInt index( KErrNotFound );

    TInt remoteAttCount( iAttachmentInfo->AttachmentCount() );
    for ( TInt i(0); i < remoteAttCount && index == KErrNotFound; ++i )
        {
        TPtrC remoteAttName( iAttachmentInfo->AttachmentL(i).AttachmentLabel() );

        if ( !remoteAttName.Compare( aAttachmentLabel ) )
            {
            index = i;
            }
        }

    return index;
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::HandleOperationCompleted
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::HandleOperationCompletedL(
        MCalRemoteAttachmentOperation* aOperation,
        RFile& aAttachment )
    {
    FUNC_LOG;

    // Notify 'download ready' event
    CESMRFieldCommandEvent* event = CESMRFieldCommandEvent::NewLC(
            NULL,
            EMRCmdHideAttachmentIndicator );

    iEventQueue.NotifyEventAsyncL( event );

    CleanupStack::Pop( event );

    // First search the correct operation from the list
    TInt operationIndex( CommandIndex(aOperation->AttachmentInformation().AttachmentLabel() ) );

    __ASSERT_DEBUG(
            operationIndex != KErrNotFound,
            Panic( EMRAttachmentCommandHandlerRemoteCommandNotFound) );

    // Resolve current download operation in progress
    MCalRemoteAttachmentOperation* operation = iDownloadOperations[ operationIndex ];
    CleanupDeletePushL( operation );
    iDownloadOperations.Remove( operationIndex );

    TInt attachmentIndex( ResolveAttachmentIndexL(
            operation->AttachmentInformation().AttachmentLabel() ) );

    if ( KErrNotFound == attachmentIndex )
        {
        User::Leave( KErrNotFound );
        }

    if ( EESMRViewerCancelAttachmentDownload != iCommandInProgress )
        {
        // Update the remote attachment to be local attachment ...
        UpdateLocalAttachmentL(
                operation->AttachmentInformation().AttachmentLabel(),
                aAttachment,
                attachmentIndex );
        }

    // Then execute normal local file attachment command
    if ( EESMRViewerOpenAttachment == iCommandInProgress ||
         EESMRViewerSaveAttachment == iCommandInProgress ||
         EESMRViewerSaveAllAttachments == iCommandInProgress )
        {
        TInt attachmentIndex(
                ResolveAttachmentIndexL(
                        operation->AttachmentInformation().AttachmentLabel() ) );

        if ( EESMRViewerOpenAttachment == iCommandInProgress )
            {
            // When opening attachment from remote storage --> Ask user to
            // save attachment first as well.
            HandleAttachmentCommandInternalL(
                    EESMRViewerOpenAndSaveAttachment, attachmentIndex );

            iCommandInProgress = 0;
            }
        else
            {
            HandleAttachmentCommandInternalL(
                    EESMRViewerSaveAttachment, attachmentIndex );

            if ( EESMRViewerSaveAllAttachments == iCommandInProgress )
                {
                iCommandInProgress = 0;

                if ( iAttachmentInfo->AttachmentCount() > 0 )
                    {
                    // There are more attachments to be downloaded --> Download next
                    SaveNextRemoteAttachmentL();
                    }
                else
                    {
                    // All remote attachments have been saved. Show info note.
                    ShowInfoNoteL( iEntry.AttachmentCountL() );
                    }
                }
            else
                {
                iCommandInProgress = 0;
                }
            }
        }

    CleanupStack::PopAndDestroy( operation );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::HandleAttachmentCommandInternalL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::HandleAttachmentCommandInternalL(
            TInt aCommandId,
            TInt aAttachmentIndex )
    {
    FUNC_LOG;

    if ( iDownloadOperations.Count() )
        {
        // Cancel ongoing remote operations ...
        iDownloadOperations.ResetAndDestroy();
        }

    CMRAttachmentCommand* command = NULL;

    // Creating command
    switch ( aCommandId )
        {
        case EESMRViewerOpenAttachment:
        case EESMREditorOpenAttachment:
            {
            command = CMROpenAttachmentCommand::NewL( *iDocHandler );
            }
            break;
        case EESMREditorRemoveAttachment:
        case EESMREditorRemoveAllAttachments:
            {
            command = CMRRemoveAttachmentCommand::NewL();
            }
            break;
        case EESMRViewerSaveAttachment:
            {
            command = CMRSaveAttachmentCommand::NewL( *iDocHandler );
            }
            break;
        case EESMRViewerOpenAndSaveAttachment:
            {
            command = CMRSaveAndOpenAttachmentCommand::NewL( *iDocHandler );
            }
            break;
        case EESMRViewerSaveAllAttachments:
            {
            SaveAllAttachmentsL();
            }
            break;
        default:
            break;
        }

    if ( command )
        {
        CleanupDeletePushL( command );

        // Executing command. TRAP errors so cancellation can be handled
        // without an extra error note.
        TRAPD( error,
                ExecuteCommandL( aCommandId, *command, aAttachmentIndex ) )

        if ( error != KErrCancel )
            {
            User::LeaveIfError( error );

            if ( aCommandId == EESMRViewerSaveAttachment
                 && iCommandInProgress != EESMRViewerSaveAllAttachments )
                {
                ShowInfoNoteL();
                }
            }
        else // KErrCancel
            {
            iCommandInProgress = 0;
            }

        CleanupStack::PopAndDestroy( command );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::UpdateLocalAttachmentL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::UpdateLocalAttachmentL(
        const TDesC& aAttachmentLabel,
        RFile& aAttachment,
        TInt aAttachmentIndex )
    {
    FUNC_LOG;

    // Create file attachment and add to calendar entry
    CCalAttachment* fileAttachment = CCalAttachment::NewFileL( aAttachment );
    CleanupStack::PushL( fileAttachment );

    RApaLsSession apaSession;
    TDataRecognitionResult dataType;
    User::LeaveIfError( apaSession.Connect() );
    apaSession.RecognizeData( aAttachment, dataType );
    apaSession.Close();

    // set attachment properties
    TPtrC8 contentType( dataType.iDataType.Des8() );

    if ( contentType.Length() )
        {
        fileAttachment->SetMimeTypeL( contentType );
        }
    else
        {
        fileAttachment->SetMimeTypeL( KUnknownDatatype );
        }

    fileAttachment->SetLabelL( aAttachmentLabel );
    iEntry.AddAttachmentL( *fileAttachment ); // calEntry takes ownership
    CleanupStack::Pop( fileAttachment );

    // Remove remote attachment
    CCalAttachment* attachment = iEntry.AttachmentL( aAttachmentIndex );
    iEntry.DeleteAttachmentL( *attachment );
    attachment = NULL;

    TInt remoteAttachmentIndex( RemoteAttachmentIndexL( aAttachmentLabel ) );
    if ( KErrNotFound != remoteAttachmentIndex )
        {
        iAttachmentInfo->RemoveAttachmentAtL( remoteAttachmentIndex );
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::NotifyProgressL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::NotifyProgressL(
            const TDesC& aAttachmentLabel,
            TInt aPercentageCompleted )
    {
    FUNC_LOG;

    CDesCArray* descArray = new (ELeave) CDesCArrayFlat( 1 );
    CleanupStack::PushL( descArray );
    descArray->AppendL( aAttachmentLabel );
    CArrayFix<TInt>* intArray = new (ELeave) CArrayFixFlat<TInt>( 1 );
    CleanupStack::PushL( intArray );
    intArray->AppendL( aPercentageCompleted );

    HBufC* statusText = StringLoader::LoadL(
            R_QTN_MEET_REQ_ATTACHMENTS_DOWNLOAD_PREFIX,
            *descArray,
            *intArray );

    CleanupStack::PushL( statusText );

    // Create field change event
    CESMRGenericFieldEvent* event = CESMRGenericFieldEvent::NewLC(
            NULL,
            MESMRFieldEvent::EESMRFieldChangeEvent );

    // Add this field as parameter
    TInt fieldId = EESMRFieldViewerAttachments;
    CESMRFieldEventValue* field = CESMRFieldEventValue::NewLC(
            MESMRFieldEventValue::EESMRInteger,
            &fieldId );
    event->AddParamL( field );
    CleanupStack::Pop( field );

    // Add new value as parameter
    CESMRFieldEventValue* value = CESMRFieldEventValue::NewLC(
            CESMRFieldEventValue::EESMRString,
            statusText );

    // Encapsulate value, so receiver gets the field value type
    event->AddParamL( value, ETrue );
    CleanupStack::Pop( value );

    // Send event
    iEventQueue.NotifyEventAsyncL( event );
    CleanupStack::Pop( event );

    // Ownership transferred to field event value
    CleanupStack::Pop( statusText );
    CleanupStack::PopAndDestroy( intArray );
    CleanupStack::PopAndDestroy( descArray );
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::HandleRemoteAttachmentCommandInternalL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::HandleRemoteAttachmentCommandInternalL(
        TInt aCommandId,
        const TDesC& aRemoteAttachmentLabel )
    {
    FUNC_LOG;

    // Fetch correct attachment from the entry
    TInt index = ResolveAttachmentIndexL( aRemoteAttachmentLabel );
    CCalAttachment* attachment = iEntry.AttachmentL( index );

    if ( CCalAttachment::EFile == attachment->Type() )
        {
        // This is local attachment ...
        User::Leave( KErrArgument );
        }

    switch ( aCommandId )
        {
        case EESMRViewerOpenAttachment:
        case EESMRViewerSaveAttachment:
            {
            TInt remoteAttIndex(
                    RemoteAttachmentIndexL( aRemoteAttachmentLabel ) );

            RPointerArray<MCalRemoteAttachment> attachmentArray;
            CleanupClosePushL( attachmentArray );
            const MCalRemoteAttachment& remoteAttachment(
                    iAttachmentInfo->AttachmentL(remoteAttIndex) );
            attachmentArray.Append( &remoteAttachment );

            if ( !iCommandInProgress )
                {
                // There are no ongoing command for this attachment
                // ==> We can start new one ...

                iCommandInProgress = aCommandId;

                // Before we can execute the actual command, we need to be sure
                // that attachments are being downloaded to device.
                iRemoteAttachmentApi->DownloadAttachmentsL(
                        attachmentArray,
                        iDownloadOperations,
                        *this );
                }

            CleanupStack::PopAndDestroy( &attachmentArray );
            }
            break;

        case EESMRViewerCancelAttachmentDownload:
            {
            if ( EESMRViewerSaveAllAttachments == iCommandInProgress )
                {
                iCommandInProgress = aCommandId;
                iDownloadOperations.ResetAndDestroy();
                }
            else
                {
                // Cancelling is done by simply deleting the command
                iCommandInProgress = aCommandId;
                TInt remoteCommand( CommandIndex(
                        aRemoteAttachmentLabel ));

                MCalRemoteAttachmentOperation* command =
                        iDownloadOperations[remoteCommand];

                iDownloadOperations.Remove( remoteCommand );
                delete command;
                }
            }
            break;
        case EESMRViewerSaveAllAttachments:
            {
            SaveAllAttachmentsL();
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::HideDownloadIndicatorL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::HideDownloadIndicatorL()
    {
    FUNC_LOG;

    CESMRFieldCommandEvent* event = CESMRFieldCommandEvent::NewLC(
        NULL,
        EMRCmdHideAttachmentIndicator );
    iEventQueue.NotifyEventAsyncL(event);
    CleanupStack::Pop(event);
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::ExecuteCommandL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::ExecuteCommandL(
            TInt aCommandId,
            CMRAttachmentCommand& aCommand,
            TInt aAttachmentIndex )
    {
    // Executing command
    switch ( aCommandId )
        {
        case EESMREditorRemoveAllAttachments:
            {
            for( TInt i = iEntry.AttachmentCountL() ; i > 0; --i )
                {
                aCommand.ExecuteAttachmentCommandL( iEntry, i - 1 );
                }
            }
        case EESMRViewerSaveAllAttachments:
             {
             for( TInt i = 0; i < iEntry.AttachmentCountL(); ++i )
                 {
                 aCommand.ExecuteAttachmentCommandL( iEntry, i );
                 }
             }
             break;
        default:
            {
            aCommand.ExecuteAttachmentCommandL( iEntry, aAttachmentIndex );
            }
            break;
        }

    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::SaveAllAttachmentsL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::SaveAllAttachmentsL()
    {
    // Save local attachments
    CMRAttachmentCommand* command =
            CMRSaveAttachmentCommand::NewL( *iDocHandler );

    TRAPD( error,
            ExecuteCommandL( EESMRViewerSaveAllAttachments, *command, 0 ) )

    delete command;

    if ( error != KErrCancel )
        {
        // Check if error happened during local attachment saving
        User::LeaveIfError( error );

        // Proceed with remote attachments
        if ( iAttachmentInfo && iAttachmentInfo->AttachmentCount() > 0 )
            {
            SaveNextRemoteAttachmentL();
            }
        else // Show Save all info note
            {
            ShowInfoNoteL( iEntry.AttachmentCountL() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRAttachmentCommandHandler::SaveNextRemoteAttachmentL
// ---------------------------------------------------------------------------
//
void CMRAttachmentCommandHandler::SaveNextRemoteAttachmentL()
    {
    __ASSERT_DEBUG( iAttachmentInfo->AttachmentCount() > 0,
                    Panic( EMRAttachmentCommandHandlerRemoteAttachmentNotFound ) );

    // Save first remote attachment of attachment info
    RPointerArray<MCalRemoteAttachment> attachmentArray;
    CleanupClosePushL( attachmentArray );
    const MCalRemoteAttachment& remoteAttachment(
            iAttachmentInfo->AttachmentL( 0 ) );
    attachmentArray.AppendL( &remoteAttachment );

    if ( !iCommandInProgress )
        {
        // There are no ongoing command for this attachment
        // ==> We can start new one ...
        iCommandInProgress = EESMRViewerSaveAllAttachments;

        // Before we can execute the actual command, we need to be sure
        // that attachments are being downloaded to device.
        iRemoteAttachmentApi->DownloadAttachmentsL(
                attachmentArray,
                iDownloadOperations,
                *this );
        }

    CleanupStack::PopAndDestroy( &attachmentArray );
    }

// EOF
