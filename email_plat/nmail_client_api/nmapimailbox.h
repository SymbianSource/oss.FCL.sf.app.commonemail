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

#ifndef NMAPIMAILBOX_H
#define NMAPIMAILBOX_H

#include <QString>
#include <QExplicitlySharedDataPointer>
#include "nmenginedef.h"
/**
 * Mailbox metadata class
 */

namespace EmailClientApi
{
class NmMailboxPrivate : public QSharedData
{
public:
    NmMailboxPrivate();
    virtual ~NmMailboxPrivate();

    quint64 id;
    QString name;
    QString address;
};

class NMENGINE_EXPORT NmMailbox
{
public:
    /*
     *  constructor for nmmailbox 
     */
    NmMailbox();
    /*
     * destructor for nmmailbox
     */
    virtual ~NmMailbox();
    NmMailbox &operator=(const NmMailbox &mailbox);
    /*
     * getter for id
     */
    quint64 id() const;

    /*
     * getter for name
     */
    QString name() const;

    /*
     * getter for address
     */
    QString address() const;

    /*
     * setter for id
     */
    void setId(quint64 id);

    /* 
     * setter for name
     */
    void setName(const QString &name);

    /*
     * setter for address
     */
    void setAddress(const QString &address);
private:
    QExplicitlySharedDataPointer<NmMailboxPrivate> d;
};
}
#endif
