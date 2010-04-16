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

#include "nmuiengineheaders.h"

#include <QTimer>

/*!
    \brief Constructor
    Constructor of NmOperation
    \param parent Parent object.
 */
NmOperation::NmOperation(QObject *parent)
: QObject(parent),
mProgress(0),
mIsRunning(true)
{
    // operation is started immediately
    mTimer = new QTimer(this);
    mTimer->setSingleShot(TRUE);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(runAsyncOperation()));
    mTimer->start(1);
}

/*!
    \brief Destructor
    Does nothing
 */
NmOperation::~NmOperation()
{
}

/*!
    \brief Tells whether the operation is running or not
 */
bool NmOperation::isRunning() const
{
    return mIsRunning;
}

/*!
    \brief Slot, complete
    The performer of the asynchronous function call should use this slot when
    the operation is completed, this will emit the actionCompleted signal * 
    \param result Result from operation
 */
void NmOperation::completeOperation(int result)
{
    // Operation is completed, emit the signal
    doCompleteOperation();
    emit this->operationCompleted(result);
    mIsRunning = false;
}

/*!
    \brief Slot, cancel
    The observer of the asynchronous function call should use this slot if it
    wants to cancel the ongoing operation, it will emit the actionCancelled signal
 */
void NmOperation::cancelOperation()
{
    // Operation is canceled, emit the signal
    mTimer->stop();
    this->doCancelOperation();
    emit this->operationCancelled();
    mIsRunning = false;
}
/*!
    \brief Slot, update progress
    The performer of the asynchronous function call should use this slot when
    updating the operation progress, this will emit the progressChanged signal
 */
void NmOperation::updateOperationProgress(int progress)
{
    mProgress = progress;
    this->doUpdateOperationProgress();
    emit this->operationProgressChanged(mProgress);
}

/*!
    \brief Virtual function to be implemented by subclasses
    This function is called from the completeAction before the signal is
    emitted. Derived class can override this is some special actions
    are needed.
 */ 
void NmOperation::doCompleteOperation()
{
    
}

/*!
    \brief Virtual function to be implemented by subclasses
    This function is called from the cancelAction after the trigger timer
    and before the signal is emitted. Derived class can override this
    and put its own cancellation operations here.
 */
void NmOperation::doCancelOperation()
{
}

/*!
    \brief Virtual function to be implemented by subclasses
    This function is called from the updateProgress after the progress
    value has been updated and before the signal is emitted. Derived class
    can override this.
 */
void NmOperation::doUpdateOperationProgress()
{
}


