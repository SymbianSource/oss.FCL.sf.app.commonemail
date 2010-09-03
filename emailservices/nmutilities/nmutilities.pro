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

CONFIG += debug

TEMPLATE = lib
TARGET = nmutilities
QT += core
DEFINES += BUILD_UTILITIES_DLL

CONFIG += mobility
MOBILITY += systeminfo

INCLUDEPATH += inc \
               ../../inc \
               ../../../inc \
               $$MW_LAYER_SYSTEMINCLUDE

DEPENDPATH += src \
              inc \
              ../../../inc 

MOC_DIR = tmp

HEADERS   += nmcenrepkeys.h \
             emailutilitiesdef.h \
             emailmailboxinfo_p.h \
             emailmailboxinfo.h \
             nmuieventsnotifier.h \
             nmutilitiescommonheaders.h
             
SOURCES   += emailmailboxinfo_p.cpp \
             emailmailboxinfo.cpp \
             nmuieventsnotifier.cpp

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002EA97 

    INCLUDEPATH += /epoc32/include/ecom \ 
                   $$MW_LAYER_SYSTEMINCLUDE
                      
    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/nmutilities.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/nmutilities.def" \
      "$${LITERAL_HASH}endif"
    
    MMP_RULES += defBlock
    
    LIBS += -lcentralrepository 
    LIBS += -ltimezonelocalization 
    LIBS += -lxqsettingsmanager 
}

win32 {
   DESTDIR = ../../bin
}

# Build.inf rules  
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             "rom/nmutilities.iby $$CORE_APP_LAYER_IBY_EXPORT_PATH(nmutilities.iby)" \
                             "data/icons/gmail.svg                  /epoc32/release/winscw/udeb/z/resource/apps/gmail.svg" \
                             "data/icons/gmail.svg                  /epoc32/release/winscw/urel/z/resource/apps/gmail.svg" \
                             "data/icons/gmail.svg                  /epoc32/data/z/resource/apps/gmail.svg" \
                             "data/icons/microsoft.svg              /epoc32/release/winscw/udeb/z/resource/apps/microsoft.svg" \
                             "data/icons/microsoft.svg              /epoc32/release/winscw/urel/z/resource/apps/microsoft.svg" \
                             "data/icons/microsoft.svg              /epoc32/data/z/resource/apps/microsoft.svg" \
                             "data/icons/ovi.svg                    /epoc32/release/winscw/udeb/z/resource/apps/ovi.svg" \
                             "data/icons/ovi.svg                    /epoc32/release/winscw/urel/z/resource/apps/ovi.svg" \
                             "data/icons/ovi.svg                    /epoc32/data/z/resource/apps/ovi.svg" \
                             "data/icons/yahoo.svg                  /epoc32/release/winscw/udeb/z/resource/apps/yahoo.svg" \
                             "data/icons/yahoo.svg                  /epoc32/release/winscw/urel/z/resource/apps/yahoo.svg" \
                             "data/icons/yahoo.svg                  /epoc32/data/z/resource/apps/yahoo.svg" \
                             "data/icons/aol.svg                    /epoc32/release/winscw/udeb/z/resource/apps/aol.svg" \
                             "data/icons/aol.svg                    /epoc32/release/winscw/urel/z/resource/apps/aol.svg" \
                             "data/icons/aol.svg                    /epoc32/data/z/resource/apps/aol.svg" \
                             "data/2001E277.txt                     /epoc32/release/winscw/udeb/z/private/10202be9/2001E277.txt" \
                             "data/2001E277.txt                     /epoc32/release/winscw/urel/z/private/10202be9/2001E277.txt" \
                             "data/2001E277.txt                     /epoc32/data/z/private/10202be9/2001E277.txt" \
                             "data/nmutilities.confml               APP_LAYER_CONFML(nmutilities.confml)" \
                             "data/nmutilities_200255BA.crml        APP_LAYER_CRML(nmutilities_200255BA.crml)" \
                             "inc/emailmailboxinfo.h                APP_LAYER_PLATFORM_EXPORT_PATH(emailmailboxinfo.h)" \
                             "inc/nmutilitiescommonheaders.h        APP_LAYER_PLATFORM_EXPORT_PATH(nmutilitiescommonheaders.h)" \
                             "inc/emailutilitiesdef.h               APP_LAYER_PLATFORM_EXPORT_PATH(emailutilitiesdef.h)" \
                             "inc/nmcenrepkeys.h                    APP_LAYER_PLATFORM_EXPORT_PATH(nmcenrepkeys.h)" \ 
                             "inc/nmuieventsnotifier.h              APP_LAYER_PLATFORM_EXPORT_PATH(nmuieventsnotifier.h)"
