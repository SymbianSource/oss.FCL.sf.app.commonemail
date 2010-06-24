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

//parameter values
const QString NmActivityName("EmailInboxView");

/*!
	int main
*/
int main(int argc, char *argv[])
{
    NM_FUNCTION;
    
    HbApplication app(argc, argv);
 
    // Load the translation file.
    QTranslator translator;
    QString lang = QLocale::system().name();
    QString appName = "mail_";
    QString path = "Z:/resource/qt/translations/";
    translator.load(appName + lang, path);
    app.installTranslator(&translator);

    app.setApplicationName(hbTrId("txt_mail_title_mail"));

    NmApplication *nmApplication = NULL;
    
    quint32 accountId = 0;
    if (app.activateReason() == Hb::ActivationReasonActivity && app.activateId() == NmActivityName)
        {
        QVariant data = app.activateParams().take("accountId");
        QString accountIdString = data.toString();
        accountId = accountIdString.toULongLong();
        nmApplication = new NmApplication(&app,accountId);
        }
    else
        {
        nmApplication = new NmApplication(&app);
        }

    int ret = app.exec();
    delete nmApplication;
    return ret;
}
