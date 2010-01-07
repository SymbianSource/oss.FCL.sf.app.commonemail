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
rem Description:  prepare the invironment for CTC data collection
rem

cd ..\..\group
del MON.sym
del MON.dat
del profile.txt
del /q CTCHTML
call bldmake bldfiles
call abld reallyclean
call ctcwrap -i m -C "EXCLUDE+*/DebugLog/*" abld build winscw udeb
cd ..\tsrc_stif\group
