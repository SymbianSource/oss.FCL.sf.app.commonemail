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

#include "nmindicatorplugin.h"
#include "nmindicator.h"

#include <QCoreApplication>
#include <QLocale>
#include <QVariant>
#include <QTimer>

#include <xqservicerequest.h>
#include <email_services_api.h>

Q_EXPORT_PLUGIN(NmIndicatorPlugin)

const int NmMaxIndicatorCount = 10;

/*!
    \class NmIndicatorPlugin
    \brief Mail indicator plugin class.
*/

/*!
     Class constructor.
*/
NmIndicatorPlugin::NmIndicatorPlugin()
: mError(0), mTranslator(0), mStatusBarIndicator(0), mSending(false)
{
}

/*!
    Class destructor.
*/
NmIndicatorPlugin::~NmIndicatorPlugin()
{
    NMLOG("NmIndicatorPlugin::~NmIndicatorPlugin");
	delete mTranslator;
}

/*!
	Return notification types this plugin implements
 */
QStringList NmIndicatorPlugin::indicatorTypes() const
{
    QStringList types;
    for (int i=0; i<NmMaxIndicatorCount; i++) {
		QString name = QString("com.nokia.nmail.indicatorplugin_%1/1.0").arg(i);
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
    Called when any of the indicator receive updated status of the global status
 */
void NmIndicatorPlugin::globalStatusChanged(bool sending)
{
    mSending = sending;

    // Pass the information to the indicator handling the status bar icon
    if (mStatusBarIndicator) {
        mStatusBarIndicator->acceptIcon(sending);
    }
    else {
		// No indicator is showing the status now.
		indicatorIconLost();
	}
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

    NmIndicator* indicator = new NmIndicator(indicatorType);
    connect(indicator, SIGNAL(indicatorIconLost()), this, SLOT(indicatorIconLost()));
    connect(indicator, SIGNAL(destroyed(QObject *)), this, SLOT(indicatorDeactivated(QObject *)));
    connect(indicator, SIGNAL(globalStatusChanged(bool)), this, SLOT(globalStatusChanged(bool)));
    connect(indicator, SIGNAL(mailboxLaunched(quint64)), this, SLOT(showMailbox(quint64)));
    mIndicators.append(indicator);

    return indicator;
}

/*!
	Returns the last error code.
 */
int NmIndicatorPlugin::error() const
{
    return mError;
}

/*!
    Called when an indicator signals about lost indicator.
    \param true if a new indicator was found
 */
bool NmIndicatorPlugin::indicatorIconLost()
{
    bool found(false);
    mStatusBarIndicator = NULL;

    foreach (NmIndicator* indicator, mIndicators) {
        // Find a new candidate to handle the status bar icon
        if (indicator->acceptIcon(mSending)) {
			mStatusBarIndicator = indicator;
			found = true;
            break;
        }
    }
    return found;
}

/*!
    Remove destroyed indicators from the list.
    \param indicator item that has been deleted
 */
void NmIndicatorPlugin::indicatorDeactivated(QObject *indObject)
{
    NMLOG(QString("NmIndicatorPlugin::indicatorDeactivated %1").arg((int)indObject));
    NmIndicator *indicator = static_cast<NmIndicator*>(indObject);
    mIndicators.removeAll(indicator);
    if (mStatusBarIndicator == indicator) {
		mStatusBarIndicator = NULL;

		// Find new indicator to take care of the status bar icon
		indicatorIconLost();
	}
}

/*!
    Opens inbox view to specific mailbox
    \return true if inbox is succesfully opened
*/
bool NmIndicatorPlugin::showMailbox(quint64 mailboxId)
{
    NMLOG("NmIndicatorPlugin::showMailbox");
    XQServiceRequest request(
        emailInterfaceNameMailbox,
        emailOperationViewInbox,
        true);

    QList<QVariant> list;
    list.append(QVariant(mailboxId));

    request.setArguments(list);

    int returnValue(-1);
    return request.send(returnValue);
}


