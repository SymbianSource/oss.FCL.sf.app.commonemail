/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  File logger for commonemail. Log file size is limited. When
*               predefined size is reached, file is replaced. 
*
*/

#ifndef CMAILLOGGER_H
#define CMAILLOGGER_H

#include <e32cmn.h>
#include <f32file.h>
#include <s32file.h>

/**
 * File logger that logs to root of c-drive with thread name as part of file 
 * name.
 *
 */
class CmailLogger
{
public:
    // writes line to log
    IMPORT_C static void Write( const TDesC8& aData );
    // releases log file
    IMPORT_C static void Close();
};

/**    
 * Log file container
 */
class LogFile {
public:
    // log file handle
    RFileWriteStream iFile;
    // thread name designates filename
    TBuf<20>     iThreadName;
    // current row in log file
    TInt         iLinesWritten;        
    // file server handle
    RFs          iFs;
    // log file name
    TFileName   iFileName;
    // stored default exception handler
    TExceptionHandler iHandler;
};

#endif //  CMAILLOGGER_H