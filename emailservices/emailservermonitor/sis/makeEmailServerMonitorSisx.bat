@echo off
REM * 
REM * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
REM * All rights reserved.
REM * This component and the accompanying materials are made available
REM * under the terms of "Eclipse Public License v1.0"
REM * which accompanies this distribution, and is available
REM * at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM * 
REM * Initial Contributors:
REM * Nokia Corporation - initial contribution.
REM * 
REM * Contributors:
REM * 
REM * Description:
REM * Script that creates and signs commonemail sisx packages
REM * 

@echo #################################################
@echo ### Making Email Server Monitor SISX-packages ###
@echo ###                                           ###
@echo.

REM Check if the latest added certificate folder/file exists. If not, copy certs from network drive.
IF NOT EXIST RnD_certs\s60\RDTest_02.der (
@echo ### RnD_certs not found, copying files from network drive
mkdir RnD_certs
xcopy \\Vagrp007\groups3\Myme\development\RnD_certs RnD_certs\ /S
@echo.
)


@echo ### Making Email Server Monitor urel SISX-package [EmailServerMonitor.sisx]

makesis EmailServerMonitor.pkg
@echo.
copy /Y EmailServerMonitor.SIS EmailServerMonitor.sisx
signsis EmailServerMonitor.sisx EmailServerMonitor.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email Server Monitor udeb SISX-package [EmailServerMonitor_udeb.sisx]

makesis EmailServerMonitor_udeb.pkg
@echo.
copy /Y EmailServerMonitor_udeb.SIS EmailServerMonitor_udeb.sisx
signsis EmailServerMonitor_udeb.sisx EmailServerMonitor_udeb.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email Server Monitor udeb (no auto run) SISX-package [EmailServerMonitor_udeb_norun.sisx]

makesis EmailServerMonitor_udeb_norun.pkg
@echo.
copy /Y EmailServerMonitor_udeb_norun.SIS EmailServerMonitor_udeb_norun.sisx
signsis EmailServerMonitor_udeb_norun.sisx EmailServerMonitor_udeb_norun.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.


@echo ###  Done!  ###
@echo ###############
