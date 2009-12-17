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

REM Check if the latest added certificate folder/file exists. If not, copy certs from network drive.
IF NOT EXIST RnD_certs\s60\RDTest_02.der (
@echo ### RnD_certs not found, copying files from network drive
mkdir RnD_certs
xcopy \\Vagrp007\groups3\Myme\development\RnD_certs RnD_certs\ /S
@echo.
)

REM @echo ### Making Email installation initiator SISX-package [emailinstallationinitiator<_udeb>.sisx]

IF EXIST ..\..\..\emailservices\emailinstallationinitiator\sis (

cd ..\..\..\emailservices\emailinstallationinitiator\sis
call makeEmailInstallationInitiatorSisx.bat
cd ..\..\..\emailuis\emailui\sis
@echo.

copy /Y ..\..\..\emailservices\emailinstallationinitiator\sis\emailinstallationinitiator.sisx emailinstallationinitiator.sisx
copy /Y ..\..\..\emailservices\emailinstallationinitiator\sis\emailinstallationinitiator_udeb.sisx emailinstallationinitiator_udeb.sisx

) ELSE (
@echo ### *** Error: Can't make email installation initiator SISX-package; folder ..\..\..\emailservices\emailinstallationinitiator\sis not found! ***
)
@echo.

@echo #########################################################
@echo ###          Making MfE plugin SISX-packages          ###
@echo ###                                                   ###

REM ----------------------------------------------------------------------------------------------------------
REM MAIL FOR EXCHANGE

IF EXIST ..\..\..\..\..\complementary\eas\sis (
@echo ### Making Mail for Exchange SISX-package [eas_engine.sisx]
cd ..\..\..\..\..\complementary\eas\sis

makesis eas_engine.pkg
@echo.
copy /Y eas_engine.SIS eas_engine.sisx
signsis eas_engine.sisx eas_engine.sisx ..\..\..\app\commonemail\emailuis\emailui\sis\RnD_certs\s60\RDTest_02.der ..\..\..\app\commonemail\emailuis\emailui\sis\RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Mail for Exchange udeb SISX-package [eas_engine_udeb.sisx]

makesis eas_engine_udeb.pkg
@echo.
copy /Y eas_engine_udeb.SIS eas_engine_udeb.sisx
signsis eas_engine_udeb.sisx eas_engine_udeb.sisx ..\..\..\app\commonemail\emailuis\emailui\sis\RnD_certs\s60\RDTest_02.der ..\..\..\app\commonemail\emailuis\emailui\sis\RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Mail for Exchange localized SISX-package [eas_engine_loc.sisx]

REM python ..\..\..\app\commonemail\emailuis\emailui\sis\pkg-localizer.py eas_engine.pkg

makesis eas_engine_loc.pkg
@echo.
copy /Y eas_engine_loc.SIS eas_engine_loc.sisx
signsis eas_engine_loc.sisx eas_engine_loc.sisx ..\..\..\app\commonemail\emailuis\emailui\sis\RnD_certs\s60\RDTest_02.der ..\..\..\app\commonemail\emailuis\emailui\sis\RnD_certs\s60\RDTest_02.key
@echo.

copy /Y eas_engine.sisx ..\..\..\app\commonemail\emailuis\emailui\sis\
copy /Y eas_engine_udeb.sisx ..\..\..\app\commonemail\emailuis\emailui\sis\
copy /Y eas_engine_loc.sisx ..\..\..\app\commonemail\emailuis\emailui\sis\

cd ..\..\..\app\commonemail\emailuis\emailui\sis
) ELSE (
@echo ### *** Error: Can't make Mail for Exchange SISX-package; folder ..\..\..\..\..\complementary\eas\sis not found! ***
)
@echo.

@echo ###  Done!  ###
@echo ###############


@echo.
@echo #########################################################
@echo ###         Making Common Email SISX-packages         ###
@echo ###                                                   ###
@echo.

@echo ### Making Common Email urel SISX-package [commonemail.sisx]

makesis commonemail.pkg
@echo.
copy /Y commonemail.SIS commonemail.sisx
signsis commonemail.sisx commonemail.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Common Email udeb SISX-package [commonemail_udeb.sisx]

makesis commonemail_udeb.pkg
@echo.
copy /Y commonemail_udeb.SIS commonemail_udeb.sisx
signsis commonemail_udeb.sisx commonemail_udeb.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Common Email localized SISX-package [commonemail_loc.sisx]

REM python pkg-localizer.py commonemail.pkg

makesis commonemail_loc.pkg
@echo.
copy /Y commonemail_loc.SIS commonemail_loc.sisx
signsis commonemail_loc.sisx commonemail_loc.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.


@echo ### Making Email UI SISX-package [emailui.sisx]

makesis emailui.pkg
@echo.
copy /Y emailui.SIS emailui.sisx
signsis emailui.sisx emailui.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email UI udeb SISX-package [emailui_udeb.sisx]

makesis emailui_udeb.pkg
@echo.
copy /Y emailui_udeb.SIS emailui_udeb.sisx
signsis emailui_udeb.sisx emailui_udeb.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.

@echo ### Making Email UI localized SISX-package [emailui_loc.sisx]

python pkg-localizer.py emailui.pkg

makesis emailui_loc.pkg
@echo.
copy /Y emailui_loc.SIS emailui_loc.sisx
signsis emailui_loc.sisx emailui_loc.sisx RnD_certs\s60\RDTest_02.der RnD_certs\s60\RDTest_02.key
@echo.


@echo ###  Done!  ###
@echo ###############
