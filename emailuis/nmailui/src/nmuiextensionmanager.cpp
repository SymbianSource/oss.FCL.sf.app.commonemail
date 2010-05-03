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

#include "nmuiheaders.h"

/*!
    \class NmUiExtensionManager
    \brief Handles ui extension plugins
 */
NmUiExtensionManager::NmUiExtensionManager()
{
    // load the protocol extension plugins
    loadExtensionPlugins();
}

/*!
    Destructor
 */
NmUiExtensionManager::~NmUiExtensionManager()
{
    // delete plugins from array
    qDeleteAll(mExtensions.begin(), mExtensions.end());
    mExtensions.clear();

    qDeleteAll(mPluginLoaders.begin(), mPluginLoaders.end());
    mPluginLoaders.clear();
}

/*!
    Calls all extensions
 */
void NmUiExtensionManager::getActions(
    const NmActionRequest &menuRequest,
    QList<NmAction*> &actionList)
{
    for (int i = 0; i < mExtensions.count(); i++) {
    	NmUiExtensionInterface* interface = mExtensions[i];
        interface->getActions(menuRequest, actionList);
    }
}

/*!
    Loads predifened plugins implementing the NmUiExtensionInterface interface
 */
void NmUiExtensionManager::loadExtensionPlugins()
{

#ifdef Q_OS_SYMBIAN
    // Note: In Symbian the plugin stub files (.qtplugin) must be used whenever
    // a path to plugin is needed - Symbian platform security.
    const QString KImapPluginExtensionName = "nmimapclientplugin.qtplugin";
    const QString KPopPluginExtensionName  = "nmpopclientplugin.qtplugin";
    const QString KEasPluginExtensionName = "nmeasclientplugin.qtplugin";
    const QString KPluginDirectory = "c:\\resource\\plugins";
    QDir pluginDir = QDir(KPluginDirectory);
#else
    #define NM_WINS_ENV
    const QString KImapPluginExtensionName = "nmimapclientplugin.dll";
    const QString KPopPluginExtensionName  = "nmpopclientplugin.dll";
    QDir pluginDir = QDir(QApplication::applicationDirPath());
#endif

    QPluginLoader *imapPluginLoader = new QPluginLoader(
            pluginDir.absoluteFilePath(KImapPluginExtensionName));
    QObject *imapPluginInstance = imapPluginLoader->instance();

    QPluginLoader *popPluginLoader = new QPluginLoader(
            pluginDir.absoluteFilePath(KPopPluginExtensionName));
    QObject *popPluginInstance = popPluginLoader->instance();

#ifndef NM_WINS_ENV
    QPluginLoader *easPluginLoader = new QPluginLoader(
            pluginDir.absoluteFilePath(KEasPluginExtensionName));
    QObject *easPluginInstance = easPluginLoader->instance();
#endif

    if(imapPluginInstance) {
        NmUiExtensionInterface *interface = qobject_cast<NmUiExtensionInterface*>(imapPluginInstance);
        if (interface) {
            mExtensions.append(interface);
        }
        mPluginLoaders.append(imapPluginLoader);
    } else {
        // We don't have proper plugin instance, so we don't need it's loader.
        delete imapPluginLoader;
        imapPluginLoader = NULL;
    }
    
    if(popPluginInstance) {
        NmUiExtensionInterface *interface = qobject_cast<NmUiExtensionInterface*>(popPluginInstance);
        if (interface) {
            mExtensions.append(interface);
        }
        mPluginLoaders.append(popPluginLoader);
    } else {
        // We don't have proper plugin instance, so we don't need it's loader.
        delete popPluginLoader;
        popPluginLoader = NULL;
    }

#ifndef NM_WINS_ENV
    if(easPluginInstance) {
        NmUiExtensionInterface *interface = qobject_cast<NmUiExtensionInterface*>(easPluginInstance);
        if (interface) {
            mExtensions.append(interface);
        }
        mPluginLoaders.append(easPluginLoader);
    } else {
        // We don't have proper plugin instance, so we don't need it's loader.
        delete easPluginLoader;
        easPluginLoader = NULL;
    }
#endif    

}


