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
CONFIG += plugin
CONFIG += hb
TEMPLATE = lib
TARGET = nmimapclientplugin
QT += core 
    
DEFINES += BUILD_CLIENTPLUGIN_DLL

INCLUDEPATH += inc \
    ../inc \
    ../../inc \
    ../../emailuis/nmsettingui/inc

HEADERS += inc/nmimapclientplugin.h \
    inc/nmimapclientpluginheaders.h

SOURCES += src/nmimapclientplugin.cpp

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002B3D1
    pluginstub.sources = nmimapclientplugin.dll
    pluginstub.path = /resource/plugins
    DEPLOYMENT += pluginstub
    LIBS += -lnmailbase \
            -lnmailuiengine
}

win32 {    
    INCLUDEPATH += ../ipssosplugin/inc
    DESTDIR = ../../bin
    pluginstub.sources = nmimapclientplugin.dll
    pluginstub.path = DESTDIR
    DEPLOYMENT += pluginstub
    LIBS += -L../../bin \
                -lnmailbase \
                -lnmailuiengine 
}

