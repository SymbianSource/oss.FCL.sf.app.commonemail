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
 * Description:
 *
 */
#include "nmframeworkadapterheaders.h"

/*!
    \class NmFwaAddAttachmentsOperation
    
    \brief NmFwaAddAttachmentsOperation is an async operation which adds new
           attachments into the message.
           
    NmFwaAddAttachmentsOperation is an async operation which adds new attachments into the message.
    \sa NmOperation
 */

/*!
    Constructor
    
    \param message Message where new attachment to be attached.
    \param fileList File name list of the attached files
    \param mailClient Reference to mail client object.
 */
NmFwaAddAttachmentsOperation::NmFwaAddAttachmentsOperation(
    const NmMessage &message,
    const QList<QString> &fileList,
    CFSMailClient &mailClient) :
        mMessage(message),
        mMailClient(mailClient)
{
    // Take own copy of the file list
    mFileList.clear();
    for (int i=0; i<fileList.count(); ++i) {
        mFileList.append(fileList.at(i));
    }
}

/*!
    Destructor
 */
NmFwaAddAttachmentsOperation::~NmFwaAddAttachmentsOperation()
{
    mFileList.clear();
    mRequestIds.clear();
    doCancelOperation();
}

/*!
    Slot, called after base object construction via timer event, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaAddAttachmentsOperation::runAsyncOperation()
{
    CFSMailMessage *msg = NULL;

    TRAPD(err, msg = CFSMailMessage::NewL(mMessage));

    if (err == KErrNone) {
        // Go through the attachment list and add those into mail message one by one.
        for (int i=0; i<mFileList.count(); ++i) {
            HBufC *fileName = NmConverter::qstringToHBufCLC(mFileList.at(i));
            TRAP(err, mRequestIds.append(msg->AddNewAttachmentL(*fileName, *this)));
            CleanupStack::PopAndDestroy(fileName);
        }
    }
    delete msg;
    msg = NULL;
}

/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaAddAttachmentsOperation::RequestResponseL(TFSProgress aEvent,
                                                    TInt aRequestId)
{
    int err = NmNoError;
    for (int i=0;i<mRequestIds.count();++i) {
        
        if (aRequestId == mRequestIds[i]) {
            
            TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;
            
            if (status == TFSProgress::EFSStatus_RequestComplete
                && aEvent.iParam) {
                // Request completed. Let's check the result
                switch (aEvent.iError) {
                    case KErrNone: {
                        CFSMailMessagePart *fsMessagePart =
                            static_cast<CFSMailMessagePart *>(aEvent.iParam);
                        completeOperationPart(
                            mFileList.at(i),
                            fsMessagePart->GetPartId().GetNmId(),
                            NmNoError);
                        delete fsMessagePart;
                        fsMessagePart = NULL;
                        break;
                    }
                    case KErrNotFound:
                        completeOperationPart(mFileList.at(i),
                                              NULL,
                                              NmNotFoundError);
                        err = NmNotFoundError;
                        break;
                    default:
                        completeOperationPart(mFileList.at(i),
                                              NULL,
                                              NmGeneralError);
                        err = NmGeneralError;
                } // end switch case
                // remove request id and file name
                mRequestIds.removeAt(i);
                mFileList.removeAt(i);
            }
            else if (status == TFSProgress::EFSStatus_RequestCancelled) {
                mFileList.clear();
                mRequestIds.clear();
                operationCancelled();
                err = NmCancelError;
            }
            else {
                mFileList.clear();
                mRequestIds.clear();
                err = NmGeneralError;
            }
        } // end if
    } // end for
    
    // complete operation
    if (!mRequestIds.count()) {
        completeOperation(err);
    }
}
