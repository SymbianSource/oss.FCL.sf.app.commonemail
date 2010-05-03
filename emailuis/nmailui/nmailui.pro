# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
CONFIG += debug
CONFIG += hb
CONFIG += service

SERVICE.FILE = service_conf.xml

TEMPLATE = app
TARGET = nmailui
DEPENDPATH += .
INCLUDEPATH += . \
    ./inc \
    ../../inc \
    ../nmailuiwidgets/inc \
    /epoc32/include/app
DEFINES += UI_EXTENSION_INTERFACE
QT += webkit
QT += network
MOC_DIR = moc

LIBS += -leuser
LIBS += -llibc

symbian*: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    INCLUDEPATH += /epoc32/include \
    /epoc32/include/app \
    /epoc32/include/mw \
    /sf/mw/qtextensions/qthighway/inc \
    /sf/mw/qtextensions/qtmobileextensions/include 
    
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x200255BA
    TARGET.CAPABILITY = CAP_APPLICATION

    # TARGET.EPOCSTACKSIZE = 0x6000
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x1000000
    LIBS += -lnmailbase \
        -lnmailuiengine \
        -lnmailuiwidgets \
        -lxqservice \
        -lxqserviceutil \
        -lqtcontacts \
        -lmobcntmodel

    # Remove following system includes, when qmake is able to generate those
    MMP_RULES += "SYSTEMINCLUDE /epoc32/include/mw/hb/hbtools"
    MMP_RULES += "SYSTEMINCLUDE /epoc32/include/mw/hb/hbfeedback"
}

win32 {
    INCLUDEPATH += . \
        ../../emailservices/nmailbase \
        ../nmailuiengine
    DESTDIR = ../../bin
    LIBS += -L../../bin \
        -lnmailbase \
        -lnmailuiengine \
        -lnmailuiwidgets \
}
HEADERS += inc/nmeditorcontent.h \
    inc/nmeditorheader.h \
    inc/nmuiheaders.h \
    inc/nmmailboxlistview.h \
    inc/nmeditorview.h \
    inc/nmviewerview.h \
    inc/nmviewerwebview.h \
    inc/nmviewerheader.h \
    inc/nmviewerviewnetmanager.h \
    inc/nmviewerviewnetreply.h \
    inc/nmmessagelistview.h \
    inc/nmbaseview.h \
    inc/nmapplication.h \
    inc/nmmailboxlistviewitem.h \
    inc/nmmessagelistviewitem.h \
    inc/nmuiviewids.h \
    inc/nmaction.h \
    inc/nmactionobserver.h \
    inc/nmactionrequest.h \
    inc/nmactionresponse.h \
    inc/nmuiextensionmanager.h \
    inc/nmuiextensioninterface.h \
    inc/nmuidef.h \
    inc/nmrecipientfield.h \
    inc/nmsendserviceinterface.h \
    inc/nmmailboxselectiondialog.h \
    inc/nmuidocumentloader.h \
    inc/nmmailboxserviceinterface.h \
    inc/nmviewerserviceinterface.h \
    inc/nmutilities.h \
    inc/nmattachmentlist.h \
    inc/nmeditortextdocument.h \
    inc/nmattachmentpicker.h

SOURCES += src/nmeditorcontent.cpp \
    src/nmeditorheader.cpp \
    src/nmviewerview.cpp \
    src/nmviewerwebview.cpp \
    src/nmviewerheader.cpp \
    src/nmviewerviewnetmanager.cpp \
    src/nmviewerviewnetreply.cpp \
    src/nmmessagelistview.cpp \
    src/nmmailboxlistview.cpp \
    src/nmeditorview.cpp \
    src/nmbaseview.cpp \
    src/main.cpp \
    src/nmapplication.cpp \
    src/nmmailboxlistviewitem.cpp \
    src/nmmessagelistviewitem.cpp \
    src/nmuiextensionmanager.cpp \
    src/nmrecipientfield.cpp \
    src/nmsendserviceinterface.cpp \
    src/nmmailboxselectiondialog.cpp \
    src/nmuidocumentloader.cpp \
    src/nmmailboxserviceinterface.cpp \
    src/nmviewerserviceinterface.cpp \
    src/nmutilities.cpp \
    src/nmattachmentlist.cpp \
    src/nmeditortextdocument.cpp \
    src/nmattachmentpicker.cpp    

RESOURCES += nmailui.qrc
RESOURCES += nmmessagelistviewitem.qrc
RESOURCES += nmviewerheader.qrc
TRANSLATIONS = mail.ts
