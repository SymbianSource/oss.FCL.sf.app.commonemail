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


#include "nmuiheaders.h"

/*!
    \class NmViewerViewNetReply
    \brief Mail viewer network reply class
*/

/*!
    Constructor
*/
NmViewerViewNetReply::NmViewerViewNetReply(QVariant data)
    : QNetworkReply(),
      mDataArray(data.toByteArray()),
      mReadIndex(0)
{
    open(ReadWrite);
    setReadBufferSize(mDataArray.length());
    // QNetworkAccessManager starts listening the signals only
    // after the construction, so we cannot signal reply to be
    // ready instantly. We need to emit the signals after the
    // construction.
    QTimer::singleShot(100, this, SLOT(signalReady()));
}

/*!
    Destructor
*/
NmViewerViewNetReply::~NmViewerViewNetReply()
{
}

/*!
    signalReady
*/
void NmViewerViewNetReply::signalReady()
{
    emit readyRead();
    emit finished();
}

/*!
    setOriginalRequest. This function is created to provide access to call
    base class' protected function setRequest
*/
void NmViewerViewNetReply::setOriginalRequest(const QNetworkRequest &request)
{
    setRequest(request);
}

/*!
    readData
*/
qint64 NmViewerViewNetReply::readData(char *data, qint64 maxlen)
{
    qint64 i = 0;
    const qint64 dataLength(mDataArray.length());

    if (0 >= dataLength || !data) {
        return -1;
    }

    for (i = 0; i < maxlen && mReadIndex < dataLength; i++, mReadIndex++) {
        data[i] = mDataArray.at(mReadIndex);
    }
    return i;
}

/*!
    readBufferSize. Returns the size of the read buffer, in bytes.
*/
qint64 NmViewerViewNetReply::readBufferSize() const
{
    return mDataArray.length();
}

/*!
    bytesAvailable. Returns the number of bytes that are available for reading.
    This function is commonly used with sequential devices to determine the number of bytes to
    allocate in a buffer before reading. Base implementation is called in order to
    include the size of QIODevices' buffer.
*/
qint64 NmViewerViewNetReply::bytesAvailable() const
{
    return mDataArray.length() + QIODevice::bytesAvailable();
}

/*!
    isSequential. Returns true if this device is sequential.
*/
bool NmViewerViewNetReply::isSequential () const
{
    return false;
}

/*!
    abort. Aborts the operation immediately and close down any
    network connections still open
*/
void NmViewerViewNetReply::abort()
{
}

