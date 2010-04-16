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
TARGET = nmpopclientplugin
QT += core 
    
DEFINES += BUILD_CLIENTPLUGIN_DLL

INCLUDEPATH += inc \
    ../inc \
    ../../inc \
    ../../emailuis/nmsettingui/inc    

HEADERS += inc/nmpopclientplugin.h \
    inc/nmpopclientpluginheaders.h

SOURCES += src/nmpopclientplugin.cpp

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002B3D2
    pluginstub.sources = nmpopclientplugin.dll
    pluginstub.path = /resource/plugins
    DEPLOYMENT += pluginstub
    LIBS += -lnmailbase \
            -lnmailuiengine
}

win32 {    
    INCLUDEPATH += ../ipssosplugin/inc
    DESTDIR = ../../bin
    pluginstub.sources = nmpopclientplugin.dll
    pluginstub.path = DESTDIR
    DEPLOYMENT += pluginstub
    LIBS += -L../../bin \
                -lnmailbase \
                -lnmailuiengine 
}

