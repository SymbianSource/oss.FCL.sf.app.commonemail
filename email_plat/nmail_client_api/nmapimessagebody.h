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

#ifndef NMAPIMESSAGEBODY_H
#define NMAPIMESSAGEBODY_H

#include <QString>
#include <QExplicitlySharedDataPointer>
#include "nmenginedef.h"

namespace EmailClientApi
{

class NmMessageBodyPrivate : public QSharedData
{
public:
    NmMessageBodyPrivate();
    virtual ~NmMessageBodyPrivate();

    quint64 totalSize;
    quint64 fetchedSize;
    QString content;
};

class NMENGINE_EXPORT NmMessageBody
{
public:
    /*
     * constructor for nmmessagebody
     */
    NmMessageBody();

    virtual ~NmMessageBody();

    /*!
     * getter for total size of message body in bytes
     */
    quint64 totalSize() const;

    /*!
     * getter for bytes available in local mailbox store
     */
    quint64 fetchedSize() const;

    /*!
     * getter for fetched content
     */
    QString content() const;

    /*
     * setter for total size of message body in bytes
     */
    void setTotalSize(quint64 size);

    /*
     * setter for bytes available on local mailbox store
     */
    void setFetchedSize(quint64 size);

    /*
     * setter for content
     */
    void setContent(const QString& content);

private:
    QExplicitlySharedDataPointer<NmMessageBodyPrivate> d;
};
}
#endif
