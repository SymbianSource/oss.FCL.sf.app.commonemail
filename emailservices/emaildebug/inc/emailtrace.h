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
* Description:  Header file describing trace utilities for commonemail
*
*/

#ifndef EMAILTRACE_H
#define EMAILTRACE_H

#include "emailtraceconfig.hrh"

#ifdef TRACE_INTO_FILE
#include <flogger.h> // RFileLogger
#else
#include <e32debug.h> // RDebug
#endif

/**
* Constants
*/
#ifdef TRACE_INTO_FILE

    _LIT(  KEmailDir, "email" );
    _LIT(  KEmailTraceFile, "email.txt" );

#endif

//-----------------------------------------------------------------------------
// Error trace macros
//-----------------------------------------------------------------------------
//
#ifdef ERROR_TRACE

    /**
    * Error trace definitions. Does not automatically log the error code!
    */
    #ifdef TRACE_INTO_FILE

        #define ERROR( aErr, aMsg )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg );\
                RFileLogger::Write(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg );\
                }\
            }
        #define ERROR_1( aErr, aMsg, aP1 )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg );\
                RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg, aP1 );\
                }\
            }
        #define ERROR_2( aErr, aMsg, aP1, aP2 )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg );\
                RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg, aP1, aP2 );\
                }\
            }
        #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg );\
                RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg, aP1, aP2, aP3 );\
                }\
            }
        #define ERROR_GEN( aMsg ) ERROR( KErrGeneral, aMsg )
        #define ERROR_GEN_1( aMsg, aP1 ) ERROR_1( KErrGeneral, aMsg, aP1 )

    #else//TRACE_INTO_FILE not defined

        #define ERROR( aErr, aMsg )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg ); RDebug::Print( KMsg );\
                }\
            }
        #define ERROR_1( aErr, aMsg, aP1 )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aP1 );\
                }\
            }
        #define ERROR_2( aErr, aMsg, aP1, aP2 )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aP1, aP2 );\
                }\
            }
        #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )\
            {\
            if( aErr != KErrNone )\
                {\
                _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aP1, aP2, aP3 );\
                }\
            }
        #define ERROR_GEN( aMsg ) ERROR( KErrGeneral, aMsg )
        #define ERROR_GEN_1( aMsg, aP1 ) ERROR_1( KErrGeneral, aMsg, aP1 )

    #endif//TRACE_INTO_FILE

#else//ERROR_TRACE not defined

    #define ERROR( aErr, aMsg )
    #define ERROR_1( aErr, aMsg, aP1 )
    #define ERROR_2( aErr, aMsg, aP1, aP2 )
    #define ERROR_3( aErr, aMsg, aP1, aP2, aP3 )
    #define ERROR_GEN( aMsg )
    #define ERROR_GEN_1( aMsg, aP1 )

#endif//ERROR_TRACE

//-----------------------------------------------------------------------------
// Info trace macros
//-----------------------------------------------------------------------------
//
#ifdef INFO_TRACE

    /**
    * Info log message definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define INFO( aMsg )\
            {\
            _LIT( KMsg, aMsg );\
            RFileLogger::Write(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg );\
            }
        #define INFO_1( aMsg, aP1 )\
            {\
            _LIT( KMsg, aMsg );\
            RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg, aP1 );\
            }
        #define INFO_2( aMsg, aP1, aP2 )\
            {\
            _LIT( KMsg, aMsg );\
            RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg, aP1, aP2 );\
            }
        #define INFO_3( aMsg, aP1, aP2, aP3 )\
            {\
            _LIT( KMsg, aMsg );\
            RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg, aP1, aP2, aP3 );\
            }

    #else//TRACE_INTO_FILE not defined

        #define INFO( aMsg )\
            {\
            _LIT( KMsg, aMsg ); RDebug::Print( KMsg );\
            }
        #define INFO_1( aMsg, aP1 )\
            {\
            _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aP1 );\
            }
        #define INFO_2( aMsg, aP1, aP2 )\
            {\
            _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aP1, aP2 );\
            }
        #define INFO_3( aMsg, aP1, aP2, aP3 )\
            {\
            _LIT( KMsg, aMsg ); RDebug::Print( KMsg, aP1, aP2, aP3 );\
            }

    #endif//TRACE_INTO_FILE

#else//INFO_TRACE not defined

    #define INFO( aMsg )
    #define INFO_1( aMsg, aP1 )
    #define INFO_2( aMsg, aP1, aP2 )
    #define INFO_3( aMsg, aP1, aP2, aP3 )

#endif//INFO_TRACE

//-----------------------------------------------------------------------------
// Function trace macros
//-----------------------------------------------------------------------------
//
#ifdef FUNC_TRACE

    // Constants
    _LIT8( KEllipse, "(" );

    /**
    * Function logging definitions.
    */
    #ifdef TRACE_INTO_FILE

        #define FUNC( aMsg, aP1 )\
            {\
            _LIT8( KMsg, aMsg ); RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KMsg, aP1 );\
            }\

    #else//TRACE_INTO_FILE not defined

        #define FUNC( aMsg, aP1 )\
            {\
            RDebug::Printf( aMsg, aP1 );\
            }\

    #endif//TRACE_INTO_FILE

    /**
    * Function trace helper class.
    */
    class TFuncLog
        {
        public:
            inline TFuncLog( const char* aFunc ): iFunc( (TUint8*)aFunc )
                {
                TInt pos = iFunc.Find( KEllipse );
                if( pos != KErrNotFound )
                    {
                    iFunc.Set( iFunc.Left( iFunc.Find( KEllipse ) ) );
                    }
                #ifdef TRACE_INTO_FILE

                    //"CMAIL" string is added in the beginning of every trace
                    //line for filtering purposes
                    FUNC( "CMAIL %S <", &iFunc );

                #else//TRACE_INTO_FILE notdefined

                    FUNC( "CMAIL %s <", iFunc.Ptr() );

                #endif//TRACE_INTO_FILE
                }
            inline ~TFuncLog()
                {
                #ifdef TRACE_INTO_FILE

                    FUNC( "CMAIL %S >", &iFunc );

                #else//TRACE_INTO_FILE not defined

                    FUNC( "CMAIL %s >", iFunc.Ptr() );

                #endif//TRACE_INTO_FILE
                }
            TPtrC8 iFunc;
        };

    #define FUNC_LOG TFuncLog _fl( __PRETTY_FUNCTION__ );

#else//FUNC_TRACE not defined

    #define FUNC_LOG

#endif//FUNC_TRACE

//-----------------------------------------------------------------------------
// Timestamp trace macros
//-----------------------------------------------------------------------------
//
#ifdef TIMESTAMP_TRACE

    #ifdef TRACE_INTO_FILE

        #define TIMESTAMP( aCaption )\
            {\
            TTime t;\
            t.HomeTime();\
            TDateTime dt = t.DateTime();\
            _LIT( KMsg, aCaption );\
            _LIT( KFormat, "[TIMESTAMP] %S %d:%02d:%02d.%d us");\
            RFileLogger::WriteFormat(  KEmailDir,  KEmailTraceFile, EFileLoggingModeAppend, KFormat,\
                &KMsg, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond() );\
            }

    #else//TRACE_INTO_FILE not defined

        #define TIMESTAMP( aCaption )\
            {\
            TTime t;\
            t.HomeTime();\
            TDateTime dt = t.DateTime();\
            _LIT( KMsg, aCaption );\
            _LIT( KFormat, "[TIMESTAMP] %S %d:%02d:%02d.%d us");\
            RDebug::Print( KFormat,\
                &KMsg, dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond() );\
            }

    #endif//TRACE_INTO_FILE

#else//TIMESTAMP_TRACE not defined

    #define TIMESTAMP( aCaption )

#endif//TIMESTAMP_TRACE

#endif // EMAILTRACE_H
