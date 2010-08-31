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
* Description:  Plugin factory.
*
*/

#ifndef NMMAILBOXREGISTERINTERFACEPLUGIN_H_
#define NMMAILBOXREGISTERINTERFACEPLUGIN_H_

#include <QObject>
#include <qserviceplugininterface.h>

QTM_USE_NAMESPACE

class NmMailboxRegisterInterfacePlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:
    QObject *createInstance(const QServiceInterfaceDescriptor &descriptor,
                            QServiceContext *context,
                            QAbstractSecuritySession *session);
};

#endif // NMMAILBOXREGISTERINTERFACEPLUGIN_H_
