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

#ifndef NMAPIEMAILSERVICE_H
#define NMAPIEMAILSERVICE_H

#include <QObject>

#include "nmenginedef.h"
#include "nmapimailbox.h"
#include "nmapimessageenvelope.h"

class NmEngine;

/**
 * Client that wants to access mailboxes, messages and receive related events
 * use this class as "entry point". 
 */

namespace EmailClientApi
{
class NMENGINE_EXPORT NmEmailService : public QObject
{
    Q_OBJECT
public:

    /*
     * constructor for NmEmailService
     */
    NmEmailService(QObject* parent);

    /*
     * destructor for NmEmailService
     */
    virtual ~NmEmailService();

    /*
     * Initialises email service. this must be called and initialised signal received 
     * before services of the library are used.
     */
    void initialise();

    /*
     *  frees resources.
     */
    void uninitialise();

    /*
     *gets mail message envelope by id (see also NmEventNotifier)
     */
    bool envelope( const quint64 mailboxId, const quint64 folderId, const quint64 envelopeId, NmMessageEnvelope &envelope );

    /*
     *  gets mailbox info by id (see also NmEventNotifier)
     */
    bool mailbox( const quint64 mailboxId, NmMailbox &mailboxInfo );

    /*
     * returns isrunning flag value
     */
    bool isRunning() const;
    signals:
    /*
     * this signal is emitted when initialisation is completed
     * boolean argument tells if initialisation succeeded
     */
    void initialized(bool);

private:
    NmEngine* mEngine;
    bool mIsRunning;
};
}
#endif /* NMEMAILSERVICE_H_ */
