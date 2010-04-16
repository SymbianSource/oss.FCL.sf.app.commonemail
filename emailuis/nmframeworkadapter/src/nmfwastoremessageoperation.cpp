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

NmFwaStoreMessageOperation::NmFwaStoreMessageOperation(
        CFSMailMessage *message,
        CFSMailClient &mailClient) :
		mMessage(message),
        mMailClient(mailClient),
        mRequestId(NmNotFoundError),
        mStatus(EStoreHeader)
{
}

NmFwaStoreMessageOperation::~NmFwaStoreMessageOperation()
{
    doCancelOperation();
    delete mMessage;
}

void NmFwaStoreMessageOperation::runAsyncOperation()
{
    TInt err = KErrNone;

    if (mMessage) {
        switch (mStatus) {
            case EStoreHeader: {
                TRAP(err, mRequestId = mMessage->SaveMessageL(*this));
                if (err != KErrNone) {
                    completeOperation(NmGeneralError);
                }
                mStatus = EStoreSubParts;
                break;
            }
            case EStoreSubParts: {
                TRAP(err, mRequestId = mMessage->SaveMessagePartsL(*this));
                if (err != KErrNone) {
                    completeOperation(NmGeneralError);
                }
                mStatus = EComplete;
                break;
            }
            case EComplete: {
                completeOperation(NmNoError);
                break;
            }

            default: {
                completeOperation(NmNotFoundError);
                break;
            }
        }
    }
}

/*!
    
 */
void NmFwaStoreMessageOperation::doCompleteOperation()
{
    mRequestId = NmNotFoundError;
}

void NmFwaStoreMessageOperation::doCancelOperation()
{
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
}

/**
 * asynchronous request response message
 *
 * @param aEvent plugin event description
 * @param aRequestId request id of asyncronous operation
 */
void NmFwaStoreMessageOperation::RequestResponseL(TFSProgress aEvent,
                                                     TInt aRequestId)
{
    if (aRequestId == mRequestId) {
        if (aEvent.iProgressStatus ==
                TFSProgress::EFSStatus_RequestComplete) {
            if(mStatus != EComplete) {
                runAsyncOperation();
            } else {
                completeOperation(NmNoError);
            }
        }
        else if (aEvent.iProgressStatus ==
                TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError);
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
}
