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
    \class NmFwaRemoveAttachmentOperation
    
    \brief NmFwaRemoveAttachmentOperation is an async operation which removes
           attachment from the message.
           
    NmFwaRemoveAttachmentOperation is an async operation which removes attachment from the message.
    \sa NmOperation
 */

/*!
    Constructor
    
    \param message Message where attachment to be removed.
    \param fileName File name of the removed file.
    \param mailClient Reference to mail client object.
 */
NmFwaRemoveAttachmentOperation::NmFwaRemoveAttachmentOperation(
    const NmMessage &message,
    const NmId &attachmentPartId,
    CFSMailClient &mailClient) :
        mMessage(message),
        mMailClient(mailClient)
{
    // Take a copy because original will be deleted during the operation
    mAttachmentPartId.setId(attachmentPartId.id());
}

/*!
    Destructor
 */
NmFwaRemoveAttachmentOperation::~NmFwaRemoveAttachmentOperation()
{
    doCancelOperation();
}

/*!
    Slot, called after base object construction via timer event, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaRemoveAttachmentOperation::runAsyncOperation()
{
    CFSMailMessage *msg = NULL;

    TRAPD(err, msg = CFSMailMessage::NewL(mMessage));
    
    if (err == KErrNone) {
    
        // Get attachment list from the message
        RPointerArray<CFSMailMessagePart> attachments;
        attachments.Reset();
        TRAP(err, msg->AttachmentListL(attachments));
        
        if (err == KErrNone) {
            err = KErrNotFound;
            
            // Search through all attachments from message and remove attachment
            // if message part match.
            for (int i=0; i<attachments.Count(); ++i) {
                if (mAttachmentPartId.id() == attachments[i]->GetPartId().GetNmId().id()) {
                    TRAP(err, msg->RemoveChildPartL(attachments[i]->GetPartId()));
                    break;
                }
            }
        }
        attachments.ResetAndDestroy();
    }
    
    delete msg;
    msg = NULL;
    
    // Send signal for completion of the operation
    if (err == KErrNone) {
        completeOperation(NmNoError);
    }
    else if (err == KErrNotFound) {
        completeOperation(NmNotFoundError);
    }
    else {
        completeOperation(NmGeneralError);
    }
}
