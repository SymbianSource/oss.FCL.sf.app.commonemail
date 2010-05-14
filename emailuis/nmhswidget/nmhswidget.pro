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
 
TEMPLATE = lib
CONFIG += plugin hb mobility qtservice
QTSERVICE.DESCRIPTOR = resources/nmhswidget.xml
MOBILITY = serviceframework
TARGET = nmhswidget
QT += core
RESOURCES += nmhswidget.qrc
LIBS += -lnmailbase \
        -lnmailuiengine \
        -lxqservice \
        -leuser \
        -llibc \
        -lbafl
symbian: {
TARGET.UID3=0x2002DD15
TARGET.VID = VID_DEFAULT

load(data_caging_paths)
pluginDep.sources = nmhswidget.dll
pluginDep.path = $$QT_PLUGINS_BASE_DIR
DEPLOYMENT += pluginDep

addFiles.sources += ./resources/nmhswidget.xml
addFiles.path = xmldata
DEPLOYMENT += addFiles

BLD_INF_RULES.prj_exports += "resources/nmhswidget.xml 			z:/private/2002DD15/nmhswidget.xml"
BLD_INF_RULES.prj_exports += "rom/nmhswidget.iby				CORE_APP_LAYER_IBY_EXPORT_PATH(nmhswidget.iby)"
BLD_INF_RULES.prj_exports += "rom/nmhswidgetlanguage.iby 		LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(nmhswidgetlanguage.iby)"
TARGET.EPOCALLOWDLLDATA = 1
}

TARGET.CAPABILITY = ALL -TCB
    
SOURCES += src/nmhswidget.cpp \
    src/nmhswidgetemailengine.cpp \
    src/nmhswidgetemailrow.cpp \
    src/nmhswidgettitlerow.cpp \
    src/nmhswidgetplugin.cpp \
    src/nmhswidgetdatetimeobserver.cpp \
    src/nmhswidgetdatetimeobserver_p.cpp

HEADERS += inc/nmhswidget.h \
    inc/nmhswidgetconsts.h \
    inc/nmhswidgetemailengine.h \
    inc/nmhswidgetemailrow.h \
    inc/nmhswidgettitlerow.h \
    inc/nmhswidgetplugin.h \
    inc/nmhswidgetdatetimeobserver.h \
    inc/nmhswidgetdatetimeobserver_p.h

INCLUDEPATH += ./inc \
    ../../inc \
    ../nmailuiengine/inc \
    ../inc
DEPENDPATH += .
TRANSLATIONS = mailwidget.ts
