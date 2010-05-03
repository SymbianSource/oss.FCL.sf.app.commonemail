#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
TARGET = nmailclientapi
QT += core

# BUILD_DLL
DEFINES += BUILD_ENGINE_DLL


INCLUDEPATH +=  inc \
                ../../email_plat/nmail_client_api \
                ../../inc 
                
DEPENDPATH += src \
              inc \
              ../../email_plat/nmail_client_api \
              ../../inc 

MOC_DIR = tmp


#headers  and sources outside api
HEADERS   += nmapitypesconverter.h \ 
             nmprivateclasses.h \
             nmapidataplugininterface.h \
             nmapidatapluginfactory.h \
             nmapiengine.h \
             nmapiconverterheader.h \
             nmmessagepart.h  \
             nmcommon_api.h
             
#headers  and sources from api             
SOURCES   += nmapitypesconverter.cpp \
             nmapidatapluginfactory.cpp \
             nmapiemailservice.cpp \
             nmapiengine.cpp 



HEADERS   += nmapiemailservice.h \
             nmapimessagetask.h \
             nmapimailboxlisting_p.h \
             nmapimailboxlisting.h \
             nmapifolderlisting_p.h \
             nmapifolderlisting.h \
             nmapienvelopelisting_p.h \
             nmapienvelopelisting.h \
	     nmapiemailaddress.h \
	     nmapieventnotifier_p.h \
	     nmapieventnotifier.h \
	     nmapifolder.h \
	     nmapimailbox.h \
	     nmapimessagebody.h \
	     nmapimessageenvelope.h
			 
LIBS += -leuser
LIBS += -llibc			


SOURCES   += nmapimessagetask.cpp \
	     nmapimailboxlisting_p.cpp \
             nmapimailboxlisting.cpp \
             nmapifolderlisting_p.cpp \
             nmapifolderlisting.cpp \
             nmapienvelopelisting_p.cpp \
             nmapienvelopelisting.cpp \ 
	     nmapiemailaddress.cpp \
	     nmapieventnotifier_p.cpp \
	     nmapieventnotifier.cpp \
	     nmapifolder.cpp \
	     nmapimailbox.cpp \
	     nmapimessagebody.cpp \
	     nmapimessageenvelope.cpp


             
             

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002C366
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	 LIBS += -lnmailbase
				   
    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/nmailclientapi.def" \
	  "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/nmailclientapi.def" \
      "$${LITERAL_HASH}endif"

    MMP_RULES += defBlock
    
     BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/nmailclientapi.iby CORE_APP_LAYER_IBY_EXPORT_PATH(nmailclientapi.iby)"    
    

}

win32 {
   DESTDIR = ../../bin
   OBJECTS_DIR = tmp 
}



