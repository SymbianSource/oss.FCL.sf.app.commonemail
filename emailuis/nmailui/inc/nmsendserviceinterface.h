/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: NMail Application service interface used for interfacing between
*              QT highway and other applications
*
*/

#ifndef NMSENDSERVICEINTERFACE_H_
#define NMSENDSERVICEINTERFACE_H_

//  INCLUDES
#include <QObject>
#include <QVariant>

#ifdef Q_OS_SYMBIAN
#include <xqserviceprovider.h>
#else
#define NM_WINS_ENV
#endif

//  FORWARD DECLARATIONS
class NmDataManager;
class NmMailboxListModel;
class NmUiEngine;
class NmApplication;
class NmUiStartParam;
class NmMailboxSelectionDialog;
class NmId;
class HbView;


class NmSendServiceInterface
#ifndef NM_WINS_ENV
    : public XQServiceProvider
#else
    : public QObject
#endif
{
    Q_OBJECT

public:

    NmSendServiceInterface(QString interfaceName,
                           QObject *parent,
                           NmUiEngine &mailboxListModel,
                           NmApplication* application);
    virtual ~NmSendServiceInterface();

public slots:

    void send(QVariant data);
    void selectionDialogClosed(NmId &mailboxId);

private:

    void launchEditorView(NmId mailboxId);
    void cancelService();

private: // Data

    NmApplication *mApplication; // Not owned
    NmUiEngine &mUiEngine;
    int mAsyncReqId;
    NmUiStartParam *mStartParam;
    NmMailboxSelectionDialog *mSelectionDialog;
    HbView *mCurrentView;
};

#endif /* NMSENDSERVICEINTERFACE_H_ */

// End of file.
