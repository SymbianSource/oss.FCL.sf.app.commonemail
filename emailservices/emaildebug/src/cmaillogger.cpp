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
* Description:  commonemail file logger implementation
*
*/

#include "cmaillogger.h"
#include <e32std.h>

_LIT( KLogFileExt, ".txt" );
_LIT( KLogFileBaseName, "c:\\data\\cmail_" );

// log file is replaced after certain amount of rows
const TInt KLogFileMaxRowCount = 3000;

#ifdef _DEBUG
#define CMAILLOGGERTACE(x) {RDebug::Print(_L(x) );}
#define CMAILLOGGERTACE2(x,y) {RDebug::Print(_L(x),y );}
#else
#define CMAILLOGGERTACE(x)
#define CMAILLOGGERTACE2(x,y)
#endif

// local functions
RFileWriteStream* File();

void ReplaceIfSizeExceeded(LogFile& aLogFile );

// ---------------------------------------------------------------------------
// Exception handler that closes and renames current log file and passes
// exception to original handler.
// ---------------------------------------------------------------------------
void LoggerExcHandler( TExcType aType )
    {
    CMAILLOGGERTACE2( "cmail exception handler %d", aType )
    LogFile* logFile = reinterpret_cast<LogFile*>( Dll::Tls() );
    TExceptionHandler storedExcHandler = NULL;
    if ( logFile )
        {
        _LIT8( KExceptionText, "CMAIL catched exception %d \n" );
        TBuf8<50> buf8;
        buf8.Format( KExceptionText(), aType );
        TRAP_IGNORE( logFile->iFile.WriteL( buf8 ) );
        logFile->iFile.Close();

        // rename file to prevent over writing
        TBuf<100> newName;
        TTime time;
        time.HomeTime();
        TDateTime dt = time.DateTime();
        TInt pos = logFile->iFileName.Locate(TChar('.'));
        if ( pos != KErrNotFound )
            {
            _LIT( KDateTimeFormat, "%C" ); // microsecond form
            TBuf<20> timeBuf;
            TRAP_IGNORE( time.FormatL( timeBuf, KDateTimeFormat() ) )
            newName = logFile->iFileName.Left( pos );
            newName.Append( timeBuf );
            newName.Append( KLogFileExt() );
            TInt err = logFile->iFs.Rename( logFile->iFileName, newName);
            CMAILLOGGERTACE2( "CMAIL log file rename err=%d", err )
            }

        storedExcHandler = logFile->iHandler;
        (*storedExcHandler)(aType);
        }

    }

// ---------------------------------------------------------------------------
// CmailLogger::Write
// ---------------------------------------------------------------------------
//
EXPORT_C void CmailLogger::Write( const TDesC8& aData )
{
    RFileWriteStream* file = File();
    if ( file /*&& file->SubSessionHandle() */)
        {
        TRAP_IGNORE( 
            {
            file->WriteL( aData );
            TBuf8<2> crLf;
            crLf.Append( 13 );
            crLf.Append( 10 );
            file->WriteL( crLf ); 
            } );            
        }
}

// ---------------------------------------------------------------------------
// CmailLogger::Close
// ---------------------------------------------------------------------------
//
EXPORT_C void CmailLogger::Close()
{
    CMAILLOGGERTACE("Cmaillogger Close")
    TAny* tls = Dll::Tls();
    if ( tls )
        {
        LogFile* logFile = reinterpret_cast<LogFile*>( tls );
        logFile->iFile.Close();
        logFile->iFs.Close();
        logFile->iFileName.Zero();
        User::SetExceptionHandler( logFile->iHandler,0xFFFFFFFF );
        delete logFile;
        Dll::SetTls( NULL );
        }
}

// ---------------------------------------------------------------------------
// Returns log file pointer. When called first time file handle container
// LogFile is stored to TLS.
// ---------------------------------------------------------------------------
//
RFileWriteStream* File()
    {
    CMAILLOGGERTACE("Cmaillogger File")
    TAny* tls = Dll::Tls();
    LogFile* logFile = NULL;
    if ( !tls )
        {
        CMAILLOGGERTACE("Cmaillogger File no tls")
        RThread t;
        TName n = t.Name();
        logFile = new LogFile;
        if ( !logFile )
            {
            return NULL;
            }
        Dll::SetTls( logFile );
        logFile->iThreadName.Copy( t.Name().Left(20));
        logFile->iLinesWritten = KErrNotFound; // for ReplaceIfSizeExceeded()
        TInt res = logFile->iFs.Connect();
        CMAILLOGGERTACE2("Cmaillogger iFs.Connect res=%d", res )
        ReplaceIfSizeExceeded( *logFile );

        // store default exception handler pointer
        logFile->iHandler = User::ExceptionHandler();

        // set own exception handler (see above)
        User::SetExceptionHandler( LoggerExcHandler,
            KExceptionAbort |
            KExceptionAbort |
            KExceptionUserInterrupt |
            KExceptionFault );
        }
    else
        {
        logFile = reinterpret_cast<LogFile*>( tls );
        ReplaceIfSizeExceeded( *logFile );
        logFile->iLinesWritten++;
        if ( logFile->iLinesWritten % 100 == 0 )
            { // flush every now and then to prevent loss of data if thread
              // is paniced (exceptions like ke-3 are not a problem, they are
              // catched by LoggerExcHandler).
            TRAP_IGNORE( logFile->iFile.CommitL() )
            }
        }
    return &logFile->iFile;
    }

// ---------------------------------------------------------------------------
// ReplaceIfSizeExceeded
// Replaces log file if determined log row count is exceeded.
// ---------------------------------------------------------------------------
//
void ReplaceIfSizeExceeded( LogFile& aLogFile )
    {
    CMAILLOGGERTACE("Cmaillogger ReplaceIfSizeExceeded<")
    if ( ( aLogFile.iLinesWritten >= KLogFileMaxRowCount ||
         aLogFile.iLinesWritten < 0 ) && // initial creation
         aLogFile.iFs.Handle() )
        {
        if ( !aLogFile.iFileName.Length() )
            {
            aLogFile.iFileName = KLogFileBaseName();
            aLogFile.iFileName.Append( aLogFile.iThreadName );
            aLogFile.iFileName.Append( KLogFileExt() );
            }
        aLogFile.iLinesWritten = 0;
        aLogFile.iFile.Close();
        CMAILLOGGERTACE2("Cmaillogger  open file '%S'",&aLogFile.iFileName)
        TInt res = aLogFile.iFile.Replace( aLogFile.iFs, aLogFile.iFileName,
            EFileShareReadersOrWriters );
        CMAILLOGGERTACE2("Cmaillogger open result=%d", res )
        }
    CMAILLOGGERTACE("Cmaillogger ReplaceIfSizeExceeded>")
    }

