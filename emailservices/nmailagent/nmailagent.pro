#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description:
#
#

TEMPLATE = app
TARGET = nmailagent
QT += core
CONFIG += hb 
CONFIG += console

symbian:CONFIG -= symbian_i18n

MOC_DIR = moc

INCLUDEPATH += . \
               ../../inc \
               ../nmclientapi/inc \
               ../../emailuis/nmframeworkadapter/inc \
               ../../emailuis/nmailuiengine/inc

HEADERS   += inc/nmmailagent.h \
             inc/nmmailagentheaders.h
    
SOURCES   += src/main.cpp \
             src/nmmailagent.cpp

LIBS += -lnmailbase
LIBS += -lnmailclientapi

symbian*: { 
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    BLD_INF_RULES.prj_exports += "rom/nmailagent.iby $$CORE_APP_LAYER_IBY_EXPORT_PATH(nmailagent.iby)"

    TARGET.EPOCHEAPSIZE = 0x1000 0x100000 // MAX 1MB

    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x2002C326
    TARGET.CAPABILITY = CAP_APPLICATION
}

win32 {
   DESTDIR = ../../bin
}



