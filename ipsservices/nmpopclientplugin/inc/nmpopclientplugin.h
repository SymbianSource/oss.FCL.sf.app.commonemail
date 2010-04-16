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

#ifndef NMPOPCLIENTPLUGIN_H_
#define NMPOPCLIENTPLUGIN_H_

#include "nmbaseclientplugin.h"
#include "nmuiextensioninterface.h"

/*!
 \class NmPopClientPlugin
 \brief
 */
class NmPopClientPlugin : public NmBaseClientPlugin
{
    Q_OBJECT
    Q_INTERFACES(NmUiExtensionInterface)

public:
    NmPopClientPlugin();
    virtual ~NmPopClientPlugin();
    void getActions( const NmActionRequest &request,
        QList<NmAction*> &actionList);

protected:
    virtual quint32 pluginId();

};

#endif /* NMTESTPLUGINEXTENSION_H_ */
