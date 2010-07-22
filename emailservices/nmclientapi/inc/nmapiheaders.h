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

#ifndef NMAPIHEADERS_H
#define NMAPIHEADERS_H

// Qt
#include <QObject>
#include <QVariant>
#include <QString>
#include <QHash>
#include <QList>
#include <QPluginLoader>
#include <QDir>
#include <QApplication>
#include <QLocale>
#include <QList>
#include <QTimer>
#include <QDateTime>
#include <QExplicitlySharedDataPointer>
#include <QSharedData>

#include <imapset.h>
#include <pop3set.h>
#include <iapprefs.h>
#include <cemailaccounts.h>


#include <SMTPSET.H>

#include <xqconversions.h>

#include <nmdataplugininterface.h>

#include <nmmailbox.h>
#include <nmmessage.h>
#include <nmfolder.h>
#include <nmmessagepart.h>
#include <nmcommon.h>

#include <nmoperation.h>

#include <emailtrace.h>



// nmail_settings_api
#include <nmapimailboxsettings.h>
#include <nmapimailboxsettingsdata.h>

// nmail_client_api
#include <nmapicommon.h>
#include <nmapidef.h>
#include <nmapiemailaddress.h>
#include <nmapiemailservice.h>
#include <nmapienvelopelisting.h>
#include <nmapieventnotifier.h>
#include <nmapifolder.h>
#include <nmapifolderlisting.h>
#include <nmapimailbox.h>
#include <nmapimailboxlisting.h>
#include <nmapimessagebody.h>
#include <nmapimessageenvelope.h>
#include <nmapimessagetask.h>
#include <nmapimessagemanager.h>


// own headers
#include "nmapidatapluginfactory.h"
#include "nmapiemailaddress_p.h"
#include "nmapiengine.h"
#include "nmapienvelopelisting_p.h"
#include "nmapieventnotifier_p.h"
#include "nmapifolder_p.h"
#include "nmapifolderlisting_p.h"
#include "nmapimailbox_p.h"
#include "nmapimailboxlisting_p.h"
#include "nmapimailboxsettings_p.h"
#include "nmapimailboxsettingsdata_p.h"
#include "nmapimessagebody_p.h"
#include "nmapimessageenvelope_p.h"
#include "nmapimessagemanager_p.h"
#include "nmapipopimapsettingsmanager.h"
#include "nmapiprivateclasses.h"
#include "nmapitypesconverter.h"


#endif // NMAPIHEADERS_H
