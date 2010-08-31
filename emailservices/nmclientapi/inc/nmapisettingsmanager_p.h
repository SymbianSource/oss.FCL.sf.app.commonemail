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

#ifndef NMAPISETTINGSMANAGER_P_H_
#define NMAPISETTINGSMANAGER_P_H_

#include <QVariant>

#include <nmapimailboxsettingsdata.h>
#include <cemailaccounts.h>

class CImImap4Settings;
class CImSmtpSettings;
class XQSettingsManager;
class XQCentralRepositoryUtils;

namespace EmailClientApi
{

class NmApiSettingsManagerPrivate
{
public:
    NmApiSettingsManagerPrivate(const quint64 mailboxId);
    ~NmApiSettingsManagerPrivate();

    bool load(quint64 mailboxId, NmApiMailboxSettingsData &data);
    bool save(const NmApiMailboxSettingsData &data);

private:
    bool writeSettingL(const NmApiMailboxSettingsData &data);
    bool saveSettings();
    bool readSetting(NmApiMailboxSettingsData &data);
    bool readCenRepSetting(NmApiMailboxSettingsData &data);
    QVariant readFromCenRep(quint32 key) const;
    bool writeSettingToCenRep(const NmApiMailboxSettingsData &data);
    bool writeToCenRep(quint32 key, const QVariant &value) const;
    void setSecurity(QString securityType);
    QString security() const;
    QString alwaysOnlineState() const;
    void setAlwaysOnlineState(const QVariant state);
    void calculateMailboxOffset();
    void calculateActiveProfileOffset();
    qint32 convertToProfileOffset(int profile) const;
    bool checkAccountType();
    void initAccountL();

private:
    NmApiMailboxSettingsData *mSettingsData;

    CEmailAccounts *mAccount;
    CImPop3Settings *mPop3Settings;
    CImImap4Settings *mImap4Settings;
    CImSmtpSettings *mSmtpSettings;

    TSmtpAccount mSmtpAccount;
    TImapAccount mImap4Account;
    TPopAccount mPop3Account;

    QString mMailboxType;
    qint32 mMailboxOffset;
    qint32 mActiveProfileOffset;
    XQSettingsManager *mQSettingsManager;

    quint32 mMailboxId;
};

} //namespace EmailClientApi

#endif // NMAPISETTINGSMANAGER_P_H_
