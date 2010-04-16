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
* Description: Nmail UI plugin for custom widgets
*
*/

#ifndef NMAILUIWIDGETSPLUGIN_H_
#define NMAILUIWIDGETSPLUGIN_H_

#include <QtPlugin>
#include <hbdocumentloaderplugin.h>

class NMailUiWidgetsPlugin : public HbDocumentLoaderPlugin
{
public:
    QObject *createObject(const QString& type, const QString &name);
    QList<const QMetaObject *> supportedObjects();
};

Q_EXPORT_PLUGIN(NMailUiWidgetsPlugin)

#endif /* NMAILUIWIDGETSPLUGIN_H_ */
