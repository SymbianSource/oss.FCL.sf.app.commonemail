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
TARGET = fsmailframework
QT += core

DEFINES += BUILD_DLL

INCLUDEPATH += ../../inc 
INCLUDEPATH += /epoc32/include/ecom 
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

HEADERS   += inc/CFSFWImplementation.h \
inc/CFSMailBrand.h \
inc/CFSMailBrandManagerImpl.h \
inc/CFSMailClient.h \
inc/CFSMailClient.hrh \
inc/CFSMailPlugin.h \
inc/CFSMailPlugin.hrh \
inc/CFSMailPluginManager.h \
inc/MailBrandManager.hrh \
inc/MailBrandManager.rh \
inc/MFSMailBoxSearchObserver.h \
inc/MFSMailBrandManager.h \
inc/MFSMailEventObserver.h \
inc/MFSMailIterator.h \
inc/MFSMailPlugin.h \
inc/MFSMailRequestObserver.h 

    
SOURCES   += src/CFSFWImplementation.cpp \
src/CFSMailClient.cpp \
src/CFSMailPluginManager.cpp \
src/CFSMailBrand.cpp \
src/CFSMailBrandManagerImpl.cpp \
inc/CFSMailPlugin.inl \
inc/CFSMailPluginInterface.inl 
 

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2001E279

    resourceBlock = \
      "$${LITERAL_HASH}include <data_caging_paths.hrh>" \
      "SOURCEPATH      data" \
      "START RESOURCE fsmailbrandmanager.rss" \
      "HEADER" \
      "targetpath RESOURCE_FILES_DIR" \
      "LANGUAGE_IDS" \
      "END" \
      
    debuggerBlock= \
      "$${LITERAL_HASH}ifdef DEBUGLOGGER_BUILD_CONFIG_DEBUG" \
      "MACRO DEBUGLOGGER_LOG_TRACE" \
      "MACRO DEBUGLOGGER_LOG_DEBUG" \
      "MACRO DEBUGLOGGER_DEBUG_ASSERTS" \
      "$${LITERAL_HASH}endif"
	   
     defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/fsmailframework.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/fsmailframework.def" \
      "$${LITERAL_HASH}endif"

      
    MMP_RULES += resourceBlock \
      debuggerBlock \
	  defBlock
    
    LIBS += -lbafl \
    -leuser \
    -lestor \
    -lefsrv \
    -lecom \
    -lgdi \
    -legul \
    -lFSFWCommonLib \
    -lcentralrepository

		BLD_INF_RULES.prj_exports += \			
        "../rom/FSEmailFramework.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(FSEmailFramework.iby)"
			
		BLD_INF_RULES.prj_extensions += \
		"START EXTENSION s60/mifconv" \
		"OPTION TARGETFILE aol_brand_graphics.mif" \
		"OPTION HEADERFILE aol_brand_graphics.mbg" \
		"OPTION SOURCEDIR data" \
		"OPTION SOURCES -c16,8 aol.svg" \
		"END" \		
		"START EXTENSION s60/mifconv" \
		"OPTION TARGETFILE google_brand_graphics.mif" \
		"OPTION HEADERFILE google_brand_graphics.mbg" \
		"OPTION SOURCEDIR data" \
		"OPTION SOURCES -c16,8 gmail44svgtest4.svg" \
		"END" \
		"START EXTENSION s60/mifconv" \
		"OPTION TARGETFILE microsoft_brand_graphics.mif" \
		"OPTION HEADERFILE microsoft_brand_graphics.mbg" \
		"OPTION SOURCEDIR data" \
		"OPTION SOURCES -c16,8 microsoft.svg" \
		"END" \		
		"START EXTENSION s60/mifconv" \
		"OPTION TARGETFILE ovi_brand_graphics.mif" \
		"OPTION HEADERFILE ovi_brand_graphics.mbg" \
		"OPTION SOURCEDIR data" \
		"OPTION SOURCES -c16,8 ovi.svg" \
		"END" \		
		"START EXTENSION s60/mifconv" \
		"OPTION TARGETFILE yahoo_brand_graphics.mif" \
		"OPTION HEADERFILE yahoo_brand_graphics.mbg" \
		"OPTION SOURCEDIR data" \
		"OPTION SOURCES -c16,8 yahoo.svg" \
		"END"
}

win32 {
   DESTDIR = ../../bin
}



