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
	int main
*/
int main(int argc, char *argv[])
{
    NM_FUNCTION;
    
    HbApplication app(argc, argv);
 
    // Load the translation file.
    QTranslator translator;

#ifdef Q_OS_SYMBIAN
    QString lang = QLocale::system().name();
    QString appName = "mail_";
    QString path = "Z:/resource/qt/translations/";
#else
    QString lang;
    QString appName = "mail";
    QString path = ":/translations";
#endif
    translator.load(appName + lang, path);
    app.installTranslator(&translator);

    app.setApplicationName(hbTrId("txt_mail_title_mail"));

    NmApplication *nmApplication = new NmApplication(&app);

    int ret = app.exec();
    delete nmApplication;
    return ret;
}
