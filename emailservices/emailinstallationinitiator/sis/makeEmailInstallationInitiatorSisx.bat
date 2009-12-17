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
REM * Script that creates and signs Email installation initiator sisx packages
REM * 

@echo #########################################################
@echo ### Making Email installation initiator SISX-packages ###
@echo ###                                                   ###
@echo.

REM Check if the latest added certificate folder/file exists. If not, copy certs from network drive.
IF NOT EXIST RnD_certs\s60\RDTest_02.der (
@echo ### RnD_certs not found, copying files from network drive
mkdir RnD_certs
xcopy \\Vagrp007\groups3\Myme\development\RnD_certs RnD_certs\ /S
@echo.
)


@echo ### Making Email installation initiator urel SISX-package [EmailInstallationInitiator.sisx]

makesis EmailInstallationInitiator.pkg
@echo.
copy /Y EmailInstallationInitiator.SIS EmailInstallationInitiator.sisx
signsis EmailInstallationInitiator.sisx EmailInstallationInitiator.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email installation initiator udeb SISX-package [EmailInstallationInitiator_udeb.sisx]

makesis EmailInstallationInitiator_udeb.pkg
@echo.
copy /Y EmailInstallationInitiator_udeb.SIS EmailInstallationInitiator_udeb.sisx
signsis EmailInstallationInitiator_udeb.sisx EmailInstallationInitiator_udeb.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email installation initiator udeb (no auto run) SISX-package [EmailInstallationInitiator_udeb_norun.sisx]

makesis EmailInstallationInitiator_udeb_norun.pkg
@echo.
copy /Y EmailInstallationInitiator_udeb_norun.SIS EmailInstallationInitiator_udeb_norun.sisx
signsis EmailInstallationInitiator_udeb_norun.sisx EmailInstallationInitiator_udeb_norun.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.


@echo ### Done! ###
@echo #############
