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

#ifndef NMBASECLIENTPLUGIN_H_
#define NMBASECLIENTPLUGIN_H_

#include <QVariant>

#include "nmcommon.h"
#include "nmuienginedef.h"
#include "nmuiextensioninterface.h"
#include "nmactionrequest.h"
#include "nmactionresponse.h"
#include "nmsettingscommon.h"

class NmUiEngine;
class NmSettingsViewLauncher;

/*!
 \class NmBaseClientPlugin
 \brief
 */
class NMUIENGINE_EXPORT NmBaseClientPlugin : public QObject, public NmUiExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(NmUiExtensionInterface)

public:
    NmBaseClientPlugin();
    virtual ~NmBaseClientPlugin();
    virtual void getActions(const NmActionRequest &request,
                            QList<NmAction*> &actionList);

public slots:
    void refresh();
    void openMessage();
    void deleteMessage();
    void deleteMessageFromViewerViewToolbar();
    void createNewMailViewerToolBar();
    void createNewMail();
    void settings();
    void sendMail();
    void replyMail();
    void replyAllMail();
    void forwardMail();
    void setPriorityHigh();
    void setPriorityNormal();
    void setPriorityLow();
    void markAsRead();
    void markAsUnread();
    void attach();

private slots:
    void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type);
    void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value);

protected:
    virtual quint32 pluginId() = 0;

private:
    void createMessageListCommands(const NmActionRequest &request, QList<NmAction*> &actionList);
    void createEditorViewCommands(const NmActionRequest &request, QList<NmAction*> &actionList);
    void createViewerViewCommands(const NmActionRequest &request, QList<NmAction*> &actionList);
    void handleRequest(NmActionResponseCommand command, const NmActionRequest &request);
    void updateEnvelopeProperty(NmEnvelopeProperties property);

private:
    NmActionRequest mMenuRequest;
    NmActionRequest mEditorToolBarRequest;
    NmActionRequest mViewerToolBarRequest;
    NmActionRequest mViewerViewToolbarRequest;
    NmUiEngine *mUiEngine; // Singleton instance
    NmSettingsViewLauncher *mSettingsViewLauncher; // owned
};

#endif /* NMBASECLIENTPLUGIN_H_ */
