rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:  calls ctc_preprocess, run_unit_test, and ctc_post_process
rem

@echo off
REM -------------------------------
REM Preparing CTC Instrumentation
REM -------------------------------
call ctc_preprocess.bat

REM -------------------------------
REM Build test modules
REM -------------------------------
call bldmake bldfiles
call abld reallyclean winscw
call abld build winscw udeb

REM -------------------------------
REM Run the test modules
REM -------------------------------
call run_unit_tests.bat

REM -------------------------------
REM Post process CTC data
REM -------------------------------
call ctc_postprocess.bat
