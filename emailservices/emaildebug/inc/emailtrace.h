/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *
 */

#ifndef EMAILTRACE_H
#define EMAILTRACE_H

#include <e32debug.h>
#include <qdebug.h>
#include <qfile.h>

/*
 * The NM_TRACING_SYSTEM macro can be used to enable and disable the tracing
 * system in debug mode. The tracing system can be disabled in a specific
 * source file by defining the macro "NM_TRACING_SYSTEM 0" before this file
 * is included.
 */
#ifndef NM_TRACING_SYSTEM
#define NM_TRACING_SYSTEM 1
#endif

/*
 * The macros NM_COMMENT_TRACES, NM_ERROR_TRACES, and NM_FUNCTION_TRACES
 * control which debug messages are printed. The trace logging is controlled
 * with the NM_LOG_TO_FILE macro, whereas the NM_LOG_FILE macro defines which
 * file is to be used in logging. The print_trace() helper function implements
 * printing. If NM_LOG_TO_FILE is zero or the NM_LOG_FILE cannot be opened,
 * the messages are printed to qDebug().
 */
#if NM_TRACING_SYSTEM && (defined(DEBUG) || defined(_DEBUG))

#define NM_COMMENT_TRACES  1
#define NM_ERROR_TRACES    1
#define NM_FUNCTION_TRACES 1

#if NM_COMMENT_TRACES || NM_ERROR_TRACES || NM_FUNCTION_TRACES

#define NM_LOG_TO_FILE 0
#define NM_LOG_FILE    "c:/data/logs/nmail_trace.log"

inline void print_trace(const QString& msg)
{
    static QFile file(NM_LOG_FILE);
    if (NM_LOG_TO_FILE && !file.isOpen()) {
        file.open(QIODevice::Append | QIODevice::Text);
    }
    if (file.isWritable()) {
        QDebug(&file).nospace() << "[Nmail] " << msg << '\n';
    } else {
        qDebug().nospace() << "[Nmail] " << msg;
    }
}

#endif

#endif /* DEBUG */

/*
 * The function NM_COMMENT() prints a debug message. The INFO macros and the
 * NMLOG macro are provided for legacy compatibility. They are deprecated and
 * should not be used. If sprintf() type of formatting is desired, consider
 * using QString::arg() or QTextStream.
 */
#if NM_COMMENT_TRACES

inline void NM_COMMENT(const QString& msg)
{
    print_trace("COMMENT : " + msg);
}
#define INFO(msg) NM_COMMENT(msg)
#define INFO_1(msg,arg1)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1);\
    NM_COMMENT(__msg);\
} while (0)
#define INFO_2(msg,arg1,arg2)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,arg2);\
    NM_COMMENT(__msg);\
} while (0)
#define INFO_3(msg,arg1,arg2,arg3)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,arg2,arg3);\
    NM_COMMENT(__msg);\
} while (0)
#define NMLOG(msg) NM_COMMENT(msg)

#else

#define NM_COMMENT(msg)
#define INFO(msg)
#define INFO_1(msg,arg1)
#define INFO_2(msg,arg1,arg2)
#define INFO_3(msg,arg1,arg2,arg3)
#define NMLOG(msg)

#endif /* NM_COMMENT_TRACES */

/*
 * The function NM_ERROR() prints its second argument if the first argument
 * is non-zero. The ERROR macros are provided for legacy compatibility. They
 * are deprecated and should not be used. If sprintf() type of formatting is
 * desired, consider using QString::arg() or QTextStream.
 */
#if NM_ERROR_TRACES

inline void NM_ERROR(int err, const QString& msg)
{
    if (err) {
        print_trace("ERROR : " + msg);
    }
}
#define ERROR(err,msg) NM_ERROR(err,msg)
#define ERROR_1(err,msg,arg1)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1);\
    NM_ERROR(err,__msg);\
} while (0)
#define ERROR_2(err,msg,arg1,arg2)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,arg2);\
    NM_ERROR(err,__msg);\
} while(0)
#define ERROR_3(err,msg,arg1,arg2,arg3)\
do {\
    QString __msg;\
    __msg.sprintf(msg,arg1,srg2,arg3);\
    NM_ERROR(err,__msg);\
} while(0)
#define ERROR_GEN(msg) ERROR(KErrGeneral,msg)
#define ERROR_GEN_1(msg,arg1) ERROR_1(KErrGeneral,msg,arg1)

#else

#define NM_ERROR(err,msg)
#define ERROR(err,msg)
#define ERROR_1(err,msg,arg1)
#define ERROR_2(err,msg,arg1,arg2)
#define ERROR_3(err,msg,arg1,arg2,arg3)
#define ERROR_GEN(msg)
#define ERROR_GEN_1(msg,arg1)

#endif /* NM_ERROR_TRACES */

/*
 * The macro NM_FUNCTION, when used inside a function body, enables tracing
 * for a function. ENTER and RETURN messages are printed when entering into
 * and returning from a function, respectively. In case of an exception,
 * UNWIND (for stack unwinding) is printed. The FUNC_LOG macro is provided
 * for legacy compatibility. It is deprecated and should not be used.
 */
#if NM_FUNCTION_TRACES

class __ftracer
{
public:
    __ftracer(const QString& _fn)
    : fn(_fn)
    {
        print_trace("ENTER : " + fn);
    }
    ~__ftracer()
    {
        if (std::uncaught_exception()) {
            print_trace("UNWIND : " + fn);
        } else {
            print_trace("RETURN : " + fn);
        }
    }
private:
    QString fn;
};

#define NM_FUNCTION __ftracer __ft(__PRETTY_FUNCTION__)
#define FUNC_LOG NM_FUNCTION

#else

#define NM_FUNCTION
#define FUNC_LOG

#endif /* NM_FUNCTION_TRACES */

#endif /* EMAILTRACE_H */
