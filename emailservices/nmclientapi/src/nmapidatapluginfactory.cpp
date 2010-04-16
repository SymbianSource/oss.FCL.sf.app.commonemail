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

#include "nmapidataplugininterface.h"
#include "nmapidatapluginfactory.h"
// Qt
#include <QList>
#include <QPluginLoader>
#include <QDir>
#include <QApplication>
#include <QLocale>

// nmailbase
#include "nmcommon.h"
#include "nmmailbox.h"
#include "../../../inc/nmmessageenvelope.h"
#include "nmmessage.h"
#include "nmmessagepart.h"
#include "nmfolder.h"

/*!
 \class NmDataPluginFactory
 \brief The NmDataPluginFactory class creates NmDataPlugin instance.

 */
NmDataPluginFactory *NmDataPluginFactory::mInstance = NULL;
int NmDataPluginFactory::mReferenceCount = 0;
QList<QObject*> *NmDataPluginFactory::mPluginArray = NULL;
QList<QPluginLoader*> NmDataPluginFactory::mPluginLoaderArray = QList<QPluginLoader*> ();
const int INTERFACEPLUGININDEX = 0;

/*!

 */
NmDataPluginFactory::NmDataPluginFactory()
{

}

/*!

 */
NmDataPluginFactory::~NmDataPluginFactory()
{
    if (mPluginArray) {
        qDeleteAll(mPluginArray->begin(), mPluginArray->end());
        mPluginArray->clear();
        delete mPluginArray;
        mPluginArray = NULL;
    }

    qDeleteAll(mPluginLoaderArray.constBegin(), mPluginLoaderArray.constEnd());
    mPluginLoaderArray.clear();
}

/*!

 */
NmDataPluginFactory *NmDataPluginFactory::instance()
{
    if (!mInstance) {
        mInstance = new NmDataPluginFactory();
    }
    mReferenceCount++;
    return mInstance;
}

/*!

 */
void NmDataPluginFactory::releaseInstance(NmDataPluginFactory *&instance)
{
    //can't have passed out instances if we don't have any
    if (mInstance) {
        if (instance == mInstance) {
            instance = NULL;
            mReferenceCount--;
        }
        if (0 >= mReferenceCount) {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

/*!

 */
NmDataPluginInterface *NmDataPluginFactory::interfaceInstance(QObject *plugin)
{
    NmDataPluginInterface *pluginInterface = NULL;
    if (plugin) {
        pluginInterface = qobject_cast<NmDataPluginInterface*> (plugin);
    }
    return pluginInterface;
}

/*!

 */
NmDataPluginInterface *NmDataPluginFactory::interfaceInstance(NmId mailboxId)
{
    return interfaceInstance(pluginInstance(mailboxId));
}

/*!

 */
QObject *NmDataPluginFactory::pluginInstance(NmId mailboxId)
{
    Q_UNUSED(mailboxId);
    return pluginInstances()->at(INTERFACEPLUGININDEX);
}

/*!

 */
QList<QObject*> *NmDataPluginFactory::pluginInstances()
{
    //if plugins have not been already created, do it now.
    if (!mPluginArray) {
        mPluginArray = new QList<QObject*> ();
    }
    if (mPluginArray->count() == 0) {
#ifdef Q_OS_SYMBIAN

        const QString KPluginDirectory = "c:\\resource\\plugins";
        QDir pluginDir = QDir(KPluginDirectory);
        const QString KFrameworkPluginName = "nmframeworkadapter.qtplugin";
        QObject *frameworkPlugin = loadPlugin(pluginDir, KFrameworkPluginName);
        mPluginArray->append(frameworkPlugin);

#endif
    }

    return mPluginArray;
}

/*!

 */
QObject *NmDataPluginFactory::loadPlugin(const QDir& pluginDir, const QString& pluginName)
{
    /*!
     This creates plugin entity.
     */
    QPluginLoader *loader = new QPluginLoader(pluginDir.absoluteFilePath(pluginName));

    QObject *pluginInstance = loader->instance();

    if (pluginInstance) {
        // Store QPluginLoader instances to keep plugins alive.
        // If QPluginLoader instance for a plugin is deleted, then there
        // is a risk that some component outside will load and unload
        // plugin, which will lead into situation where plugin is deleted.
        // This means that instance in mPluginArray will be invalid.
        mPluginLoaderArray.append(loader);
    }
    else {
        // We don't have proper plugin instance, so we don't need it's loader.
        delete loader;
        loader = NULL;
    }
    return pluginInstance;
}
