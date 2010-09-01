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
rem Description:  processing data collected by CTC
rem

cd ..\..\group
REM Force Function Coverage.  The data was collected for MultiCondition Coverage, but
REM We can just show the Function Coverage.
call ctcpost -ff -p profile.txt
call ctc2html -i profile.txt
cd ..\tsrc_stif\group
