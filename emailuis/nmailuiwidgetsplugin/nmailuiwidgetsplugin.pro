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
TARGET = nmailuiwidgetsplugin

INCLUDEPATH += ./inc \
  ../../inc

HEADERS += inc/nmailuiwidgetsplugin.h

SOURCES += src/nmailuiwidgetsplugin.cpp

symbian*: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002C349

    pluginstub.sources = nmailuiwidgetsplugin.dll
    pluginstub.path = /resource/plugins
    DEPLOYMENT += pluginstub
    LIBS += -lnmailuiwidgets.dll
}

win32 {
    DESTDIR = ../../bin
    LIBS += -L../../bin \
        -lnmailuiwidgets
}
