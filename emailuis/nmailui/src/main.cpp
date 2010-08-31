/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

const QString NmActivityName("EmailInboxView");

/*!
    The main function.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc,argv,Hb::NoSplash);
    
    // Load the translation file.
    QTranslator translator;
    QString lang = QLocale::system().name();
    QString appName = "mail_";
    QString path = "Z:/resource/qt/translations/";
    translator.load(appName + lang, path);
    app.installTranslator(&translator);
    
    app.setApplicationName(hbTrId("txt_mail_title_mail"));
    
    NmApplication *nmApplication = NULL;
    quint64 accountId = 0;
    QString activateId = app.activateId();
    if (app.activateReason() == Hb::ActivationReasonActivity &&
            activateId.startsWith(NmActivityName) ) {
        QString accountIdString = activateId.mid(NmActivityName.length());
        accountId = accountIdString.toULongLong();
        nmApplication = new NmApplication(&app,accountId);
    } else {
        nmApplication = new NmApplication(&app);
    }
    
    int ret = app.exec();
    delete nmApplication;
    return ret;
}
