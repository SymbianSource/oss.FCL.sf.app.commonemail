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

#ifndef NMUIENGINEHEADERS_H_
#define NMUIENGINEHEADERS_H_

// Qt
#include <QList>
#include <QPluginLoader>
#include <QDir>
#include <QApplication>
#include <QLocale>
#include <QTimer>
#ifdef Q_OS_SYMBIAN
#include <xqsharablefile.h>
#endif

// Orbit
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbnamespace.h>
#include <hbicon.h>
#include <hbiconanimationmanager.h>


// nmailuiengine
#include "nmuienginedef.h"
#include "nmuiengine.h"
#include "nmdatamanager.h"
#include "nmmailboxlistmodel.h"
#include "nmfolderlistmodel.h"
#include "nmmessagelistmodel.h"
#include "nmdatapluginfactory.h"
#include "nmdataplugininterface.h"
#include "nmmailboxlistmodelitem.h"
#include "nmmessagelistmodelitem.h"
#include "nmmailboxmetadata.h"
#include "nmfoldermetadata.h"
#include "nmactionrequest.h"
#include "nmactionresponse.h"
#include "nmactionobserver.h"
#include "nmaction.h"
#include "nmicons.h"
#include "nmbaseclientplugin.h"
#include "nmoperation.h"
#include "nmstoreenvelopesoperation.h"
#include "nmmessagesendingoperation.h"

// nmailbase
#include "nmcommon.h"
#include "nmmailbox.h"
#include "nmmessage.h"
#include "nmmessagepart.h"
#include "nmmessageenvelope.h"
#include "nmfolder.h"
#include "nmoperation.h"

// nmsettings
#include "nmsettingsviewlauncher.h"
#include "nmsettingscommon.h"

#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#else
#define NM_WINS_ENV
#endif


#endif /* NMUIENGINEHEADERS_H_ */
