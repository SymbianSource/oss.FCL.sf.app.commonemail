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

#include "nmimapclientpluginheaders.h"
#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#endif

Q_EXPORT_PLUGIN(NmImapClientPlugin)


/*!
 \class NmImapClientPlugin

 \brief The NmImapClientPlugin plugin provides ui services for IMAP mail protocol.
*/

/*!
    Constructs a new NmImapClientPlugin.
 */
NmImapClientPlugin::NmImapClientPlugin()
{
}

/*!
    Destructor.
 */
NmImapClientPlugin::~NmImapClientPlugin()
{
}

/*!
    Provides list of supported NmActions.
    Implementation of NmUiExtensionInterface.
    Parameter \a request controls list of request services.
    Parameter \a actionList is updated by supported NmActions.
 */
void NmImapClientPlugin::getActions(
      const NmActionRequest &request,
      QList<NmAction *> &actionList)
{
    NMLOG("NmImapClientPlugin::getActions()-->");
    
    // Check if this request is for the IMAP protocol.
    quint32 pluginId = this->pluginId();
    if (request.mailboxId().pluginId32() != pluginId
        #ifdef USE_POPIMAP_TESTPLUGIN
        && request.mailboxId().pluginId32() != IMAPTESTPLUGINID
        #endif
    ) {
        NMLOG("NmImapClientPlugin::getActions(): PluginId incorrect.");
        NMLOG(QString("request.mailboxId().pluginId32()=%1").arg(request.mailboxId().pluginId32()));
        return;
    }
	
    // Get the NmBaseClientPlugin implementation of the UI actions.
    NmBaseClientPlugin::getActions(request, actionList);
    NMLOG("<--NmImapClientPlugin::getActions()");
}


/*!
    Provide id of plugin.
    Implementation of NmUiExtensionInterface.
 */
quint32 NmImapClientPlugin::pluginId()
{
    NMLOG("NmImapClientPlugin::pluginId()-->");
    return IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID;
}
