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

#ifndef NMMAILBOXSETTINGVIEW_H
#define NMMAILBOXSETTINGVIEW_H

#include <cpbasesettingview.h>
#include <nmcommon.h>
#include "nmsettingscommon.h"

class HbDataForm;
class HbDataFormModel;
class NmMailboxSettingsManager;
class QGraphicsItem;
class NmId;


class NmMailboxSettingView : public CpBaseSettingView
{
    Q_OBJECT

public:

    explicit NmMailboxSettingView(const NmId &mailboxId,
        const QString &mailboxName,
        NmMailboxSettingsManager& settingsManager,
        QGraphicsItem *parent = 0);

    ~NmMailboxSettingView();

    NmId mailboxId();

private slots:

    void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type);

    void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value);


private:

    Q_DISABLE_COPY(NmMailboxSettingView)


private: // Data

    HbDataForm *mForm; // Owned
    HbDataFormModel *mModel; // Owned
    const NmId mMailboxId;
};

#endif // NMMAILBOXSETTINGVIEW_H

// End of file.
