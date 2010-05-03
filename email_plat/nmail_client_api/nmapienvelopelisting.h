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

#ifndef NMAPIENVELOPELISTING_H_
#define NMAPIENVELOPELISTING_H_

#include <QObject>
#include <QList>

#include "nmapimessagetask.h"
#include "nmapimessageenvelope.h"
#include "nmenginedef.h"

class NmEngine;

// list messages in particular folder.
namespace EmailClientApi
{
class NmEnvelopeListingPrivate;

class NMENGINE_EXPORT NmEnvelopeListing : public NmMessageTask
{
    Q_OBJECT
public:
    /*!
     * Constructor of class. It set start values.
     */
    NmEnvelopeListing( QObject *parent, const quint64 folderId, const quint64 mailboxId );

    /*!
     * Destructor of class. It release engine to be safe if manual releasing won't work.
     */
    virtual ~NmEnvelopeListing();

    enum {EnvelopeListingFailed = -1};

    /*! 
     * \brief Returns results after envelopesListed signal is received.
     * 
     *  Caller gets ownership of envelopes. Returns true if results were available.
     *  It clears list of envelopes after be called.
     *  It also at start clear inputlist of NmMessageEnvelope.
     *  
     *  \arg envelopes 
     */
    bool getEnvelopes( QList<EmailClientApi::NmMessageEnvelope> &envelopes );

    /*!
     * \brief Return info if listing is running
     */
    bool isRunning() const;
    signals:
    /*!
     * Emitted when listing is available, count is number of mailboxes found
     * or EnvelopeListingFailed if listing failed
     */
    void envelopesListed(qint32 count);

public slots:
    /*!
     * \brief Starts gathering envelopes list.
     * 
     * In first turn it will get whole folderlist. 
     * If start works, it do nothing.
     * 
     * To asynchronous operation ce be used \sa QTimer::singleShot on this method.
     * Example:
     * <code> 
     * QTimer::singleShot(0,nmEnvelopeListing,SLOT(start());
     * </code>
     * 
     */
    virtual bool start();

    /*!
     * \brief Stop gathering envelope list.
     * 
     * In first it change state of listing.
     * Then it release engine.
     * On end it clears list of envelopes and emits \sa NmMessageTask::canceled() signal.
     */
    virtual void cancel();

private:
    NmEnvelopeListingPrivate* mListingPrivate;
};
}

#endif /* NMENVELOPELISTING_H_ */
