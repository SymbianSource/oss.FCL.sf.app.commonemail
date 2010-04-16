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

#ifndef NMIPSEXTENDEDSETTINGSMANAGER_H
#define NMIPSEXTENDEDSETTINGSMANAGER_H

#include <qvariant.h>
#include "nmipssettingitems.h"

class NmId;
class XQSettingsManager;
class XQCentralRepositoryUtils;

class NmIpsExtendedSettingsManager
{
public:
    NmIpsExtendedSettingsManager(const NmId &mailboxId);
    virtual ~NmIpsExtendedSettingsManager();
    
    bool readSetting(IpsServices::SettingItem settingItem, QVariant &settingValue) const;
    bool writeSetting(IpsServices::SettingItem settingItem, const QVariant &settingValue);
    void deleteSettings();

private:
    QVariant readFromCenRep(quint32 key) const;
    bool writeToCenRep(quint32 key, const QVariant &value) const;
    void calculateMailboxOffset();
    void calculateActiveProfileOffset();

private: // data
    const NmId &mMailboxId;
    qint32 mMailboxOffset;
    qint32 mActiveProfileOffset;
    XQSettingsManager* mSettingsManager; // Owned.
};

#endif // NMIPSEXTENDEDSETTINGSMANAGER_H
