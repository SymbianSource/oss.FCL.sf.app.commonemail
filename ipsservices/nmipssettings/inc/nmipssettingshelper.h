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
* Description:
*
*/

#ifndef NMIPSSETTINGSHELPER_H
#define NMIPSSETTINGSHELPER_H

#include <QObject>
#include <QMap>
#include <QVariant>

#include "nmsettingscommon.h"
#include "nmipssettingitems.h"

class CpSettingFormItemData;
class NmIpsSettingsManagerBase;
class NmId;

class NmIpsSettingsHelper : public QObject
{
    Q_OBJECT

public:

    NmIpsSettingsHelper(NmIpsSettingsManagerBase &settingsManager);
    ~NmIpsSettingsHelper();

    void insertContentItem(IpsServices::SettingItem key, CpSettingFormItemData *value);
    
public slots:

    void saveMyName();
    void myNameTextChange(const QString &text);
    void saveMailboxName();
    void mailboxNameTextChange(const QString &text);
    void saveMailAddress();
    void mailAddressTextChange(const QString &text);
    void saveUserName();
    void userNameTextChange(const QString &text);
    void savePassword();
    void saveReplyTo();
    void replyToTextChange(const QString &text);
    void deleteButtonPress();
    void receivingScheduleChange(int index);

signals:

    void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type);
    void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value);

private: // data

    bool mCurrentLineEditChanged;
    
    NmIpsSettingsManagerBase &mSettingsManager;

    QMap<IpsServices::SettingItem, CpSettingFormItemData *> mContentItems;
    
    Q_DISABLE_COPY(NmIpsSettingsHelper)

};

#endif // NMIPSSETTINGSHELPER_H
