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

class QModelIndex;
class HbDataFormModelItem;
class HbDataFormModel;
class HbDataForm;
class HbAction;
class CpSettingFormItemData;
class NmIpsSettingsManagerBase;
class NmId;


// Constant for User Defined sync profile (EmailProfileOffsetUD)
// \sa ipssettingkeys.h - Currently used sync profile
const int NmIpsSettingsReceivingScheduleUserDefinedProfile = 3;

class NmIpsSettingsHelper : public QObject
{
    Q_OBJECT

public:

    NmIpsSettingsHelper(NmIpsSettingsManagerBase &settingsManager, HbDataForm &dataForm,
        HbDataFormModel &dataFormModel);

    ~NmIpsSettingsHelper();

    void insertContentItem(IpsServices::SettingItem key, HbDataFormModelItem *value);
    HbDataFormModelItem *contentItem(IpsServices::SettingItem key) const;
    bool isOffline();
    
    void setReceivingScheduleGroupItem(HbDataFormModelItem *item);
    void createOrUpdateReceivingScheduleGroupDynamicItem(IpsServices::SettingItem item);
    
    int getCorrectPortRadioButtonIndex(int currentPort);
    int getCorrectSecureRadioButtonIndex(QVariant secureSockets, QVariant secureSSLWrapper);
    int getCorrectInboxPathRadioButtonIndex(QVariant folderPath);
    int getCorrectOutgoingPortRadioButtonIndex(int currentPort);
    void handleReceivingScheduleSettingChange(IpsServices::SettingItem settingItem,
        const QVariant &settingValue);
    
signals:

    void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type);
    void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value);
    void goOffline(const NmId &mailboxId);
    void createUserDefinedMode();

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
    void saveIncomingMailServer();
    void incomingMailServerTextChange(const QString &text);
    void saveOutgoingMailServer();
    void outgoingMailServerTextChange(const QString &text);
    void incomingPortChange(int index);
    void incomingPortPressed(const QModelIndex &index);
    void incomingSecureConnectionItemChange(int index);
    void incomingSecureConnectionPressed(const QModelIndex &index);
    void inboxPathChange(int index);
    void inboxPathPressed(const QModelIndex &index);
    void outgoingPortChange(int index);
    void outgoingPortPressed(const QModelIndex &index);
    void outgoingSecureConnectionItemChange(int index);
    void outgoingSecureConnectionPressed(const QModelIndex &index);

private slots:

    void receivingWeekdaysModified(HbAction *action);
    void startTimeModified();
    void endTimeModified();
    void refreshPeriodModified(int index);
    
private:

    void updateShowMailInMailbox();
    void deleteReceivingScheduleGroupDynamicItem(IpsServices::SettingItem item);
    QString timeFormat();
    int showIncomingPortInputDialog(int currentPort, bool &changed);
    QString showFolderPathInputDialog(bool &changed);
    int showOutgoingPortInputDialog(int currentPort, bool &changed);
    void copyReceivingScheduleSettingsFromActiveProfile(int profileMode);

    Q_DISABLE_COPY(NmIpsSettingsHelper)

private: // data

    bool mCurrentLineEditChanged;
    bool mEmitOnline;
    NmIpsSettingsManagerBase &mSettingsManager;
    QMap<IpsServices::SettingItem, HbDataFormModelItem *> mContentItems;
    HbDataFormModelItem *mReceivingScheduleGroupItem;   // Not owned.
    HbDataForm &mDataForm;
    HbDataFormModel &mDataFormModel;
    bool mDynamicItemsVisible;
    int mRadioButtonPreviousIndex;
};

#endif // NMIPSSETTINGSHELPER_H
