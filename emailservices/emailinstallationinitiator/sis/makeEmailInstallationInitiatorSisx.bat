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
REM * Description: Script that creates and signs Email installation initiator sisx packages
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


@echo ### Making Email installation initiator urel SISX-package [emailinstallationinitiator.sisx]

makesis emailinstallationinitiator.pkg
@echo.
copy /Y emailinstallationinitiator.SIS emailinstallationinitiator.sisx
signsis emailinstallationinitiator.sisx emailinstallationinitiator.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email installation initiator udeb SISX-package [emailinstallationinitiator_udeb.sisx]

makesis emailinstallationinitiator_udeb.pkg
@echo.
copy /Y emailinstallationinitiator_udeb.SIS emailinstallationinitiator_udeb.sisx
signsis emailinstallationinitiator_udeb.sisx emailinstallationinitiator_udeb.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

REM * This is for testing purposes only!
REM * @echo ### Making Email installation initiator udeb (no auto run) SISX-package [emailinstallationinitiator_udeb_norun.sisx]

REM * makesis emailinstallationinitiator_udeb_norun.pkg
REM * @echo.
REM * copy /Y emailinstallationinitiator_udeb_norun.SIS emailinstallationinitiator_udeb_norun.sisx
REM * signsis emailinstallationinitiator_udeb_norun.sisx emailinstallationinitiator_udeb_norun.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
REM * @echo.


@echo ### Making Email uninstallation initiator urel SISX-package [emailuninstallationinitiator.sisx]

makesis emailuninstallationinitiator.pkg
@echo.
copy /Y emailuninstallationinitiator.SIS emailuninstallationinitiator.sisx
signsis emailuninstallationinitiator.sisx emailuninstallationinitiator.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email uninstallation initiator udeb SISX-package [emailuninstallationinitiator_udeb.sisx]

makesis emailuninstallationinitiator_udeb.pkg
@echo.
copy /Y emailuninstallationinitiator_udeb.SIS emailuninstallationinitiator_udeb.sisx
signsis emailuninstallationinitiator_udeb.sisx emailuninstallationinitiator_udeb.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

REM * This is for testing purposes only!
REM * @echo ### Making Email uninstallation initiator udeb (no auto run) SISX-package [emailuninstallationinitiator_udeb_norun.sisx]

REM * makesis emailuninstallationinitiator_udeb_norun.pkg
REM * @echo.
REM * copy /Y emailuninstallationinitiator_udeb_norun.SIS emailuninstallationinitiator_udeb_norun.sisx
REM * signsis emailuninstallationinitiator_udeb_norun.sisx emailuninstallationinitiator_udeb_norun.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
REM * @echo.

@echo ### Done! ###
@echo #############
