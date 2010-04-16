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

#include "NmIndicatorPlugin.h"

#include "nmindicator.h"

#include <QtPlugin>
#include <QCoreApplication>
#include <QLocale>
#include <QVariant>

Q_EXPORT_PLUGIN(NmIndicatorPlugin)

const int maxIndicatorCount = 10;

/*!
    \class NmIndicatorPlugin
    \brief Mail indicator plugin class. 
*/

/*!
     Class constructor.
*/
NmIndicatorPlugin::NmIndicatorPlugin() 
: mError(0), mTranslator(0)
{
}

/*!
    Class destructor.
*/
NmIndicatorPlugin::~NmIndicatorPlugin()
{
	delete mTranslator;
}

/*!
	Return notification types this plugin implements
 */
QStringList NmIndicatorPlugin::indicatorTypes() const
{
    QStringList types;
    for (int i=0; i<maxIndicatorCount; i++) {
		QString name = QString("com.nokia.nmail.indicatorplugin_")+i+"/1.0";
		types << name;
    }
    return types;
}

/*!
	Check if client is allowed to use notification widget
	see HbIndicatorPluginInterface
*/
bool NmIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const HbSecurityInfo *securityInfo) const
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

/*!
	Creates an indicator of type indicatorType. Ownership is passed to the caller.
 */
HbIndicatorInterface* NmIndicatorPlugin::createIndicator(
        const QString &indicatorType)
{
    if (!mTranslator) {
        mTranslator = new QTranslator();
        QString lang = QLocale::system().name();
        QString appName = "mail_";
        QString path = "z:/resource/qt/translations/";
        mTranslator->load(appName + lang, path);
        QCoreApplication::installTranslator(mTranslator);
    }
    
    HbIndicatorInterface *indicator = 0;
    indicator = new NmIndicator(indicatorType);
    return indicator;
}

/*!
	Returns the last error code.
 */
int NmIndicatorPlugin::error() const
{
    return mError;
}




