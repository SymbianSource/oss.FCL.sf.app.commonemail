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

#ifndef NMAPIMAILBOXLISTING_H_
#define NMAPIMAILBOXLISTING_H_

#include <QObject>

#include "nmapimessagetask.h"
#include "nmapimailbox.h"
#include "nmenginedef.h"

namespace EmailClientApi
{
class NmMailboxListingPrivate;

/*!
 * \class Class for creating list of all mailboxes
 */
class NMENGINE_EXPORT NmMailboxListing : public NmMessageTask
{
    Q_OBJECT
public:
    /*!
     * Constructor of class. It set start values.
     */
    NmMailboxListing( QObject *parent );
    /*!
     * Destructor of class. It release engine to be safe if manual releasing won't work.
     */
    virtual ~NmMailboxListing();

    enum {MailboxListingFailed = -1};

    /*! 
     * \brief Returns results after mailboxesListed signal is received.
     * 
     *  Caller gets ownership of mailboxes. Returns true if results were available.
     *  It clears list of mailboxes (in private members) after be called.
     *  At start it clear inputlist of mailboxes.
     *  
     *  \return Return true if results were avaible
     *  \arg List of mailboxes to filled. On start is cleared. 
     */
    bool getMailboxes(QList<EmailClientApi::NmMailbox> &mailboxes);

    /*!
     * \brief Return info if listing is running
     * 
     * \return Return true if listing is running
     */
    bool isRunning() const;

    signals:
    /*!
     * 
     * emitted when listing is available, count is number of mailboxes found
     * or MailboxListingFailed if listing failed
     * 
     * \arg Count of mailboxes inside class.
     */
    void mailboxesListed(qint32 count);

public slots:
    /*!
     * \brief Starts gathering mailbox list.
     * 
     * In first turn it will get whole mailboxlist. 
     * Then it initialize core arguments and emits signal when ready.
     * 
     * To asynchronous operation can be used \sa QTimer::singleShot on this method.
     * Example:
     * <code> 
     * QTimer::singleShot(0,nmMailboxListing,SLOT(start());
     * </code>
     * 
     * \return Return true if everything go good and core of listing works good.
     * 
     */
    virtual bool start();

    /*!
     * \brief Stop gathering mailbox list.
     * 
     * In first it change state of listing.
     * Then it release engine.
     * On end it clears list of mailboxes and emits \sa NmMessageTask::canceled() signal.
     */
    virtual void cancel();

private:
    NmMailboxListingPrivate *mNmMailboxListingPrivate;
};

}

#endif /* NMMAILBOXLISTING_H_ */
