/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  VCal exporter for send ui wrapper
*
*/


//debug
// INCLUDE FILES
#include "emailtrace.h"
#include "esmrvcalexport.h"

#include <esmrgui.rsg> // r_qtn_meet_req_conflict_unnamed
#include <calentry.h>
#include <calenexporter.h>
#include <bldvariant.hrh> // For FeatureIds
#include <eikenv.h>
#include <featmgr.h>
#include <stringloader.h>
#include <vtoken.h>
#include <s32mem.h>

namespace {

// LOCAL CONSTANTS AND MACROS
const TInt KDynBufExpandSize(100);

// This flag definition should match EUTCTime flag
// in \s60\AgnVersit\inc\S60AgnVersit.h
#define S60AGNVERSIT_EUTCTIME 0x1000
#define S60AGNVERSIT_EJAPAN 0x2000

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TESMRVCalExport::TESMRVCalExport()
// ---------------------------------------------------------------------------
//
TESMRVCalExport::TESMRVCalExport(
		CCalSession& aCalSession,
        RFs& aFileServerSession): 
    iSession(aCalSession),
    iFs(aFileServerSession)
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// TESMRVCalExport::ExportVCalLC()
// ---------------------------------------------------------------------------
//
HBufC8* TESMRVCalExport::ExportVCalLC(CCalEntry& aEntry)
    {
    FUNC_LOG;

    const TDesC& subject=aEntry.SummaryL();

    TBool emptySubject = subject.Length() ==0;
    if (emptySubject)
        {
        // If subject is empty, we add <unnamed> text to it
        HBufC* unnamed =
            StringLoader::LoadLC(
                    R_QTN_MEET_REQ_CONFLICT_UNNAMED,
                    CEikonEnv::Static()); // codescanner::eikonenvstatic

        aEntry.SetSummaryL(*unnamed);
        CleanupStack::PopAndDestroy(unnamed);
        }

    CBufFlat* buf = CBufFlat::NewL( KDynBufExpandSize );
    CleanupStack::PushL(buf);
    ConvertVCalL(buf, aEntry);

    HBufC8* result = HBufC8::NewL( buf->Size() );
    result->Des().Copy(buf->Ptr(0));
    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PushL(result);

    return result;
    }

// ---------------------------------------------------------------------------
// TESMRVCalExport::ConvertVCalL()
// ---------------------------------------------------------------------------
//
void TESMRVCalExport::ConvertVCalL(CBufFlat* aBuf, CCalEntry& aEntry)
    {
    FUNC_LOG;

    // CCalEntry is converted to VCal
    RBufWriteStream writeStream;
    writeStream.Open(*aBuf);
    CleanupClosePushL(writeStream);
    CCalenExporter* exporter=CCalenExporter::NewL(iSession);
    CleanupStack::PushL(exporter);
    exporter->ExportVCalL( aEntry, writeStream);
    CleanupStack::PopAndDestroy(exporter);
    CleanupStack::PopAndDestroy(&writeStream);

    }

// ---------------------------------------------------------------------------
// TESMRVCalExport::ConvertICalL()
// ---------------------------------------------------------------------------
//
void TESMRVCalExport::ConvertICalL(CBufFlat* aBuf, CCalEntry& aEntry)
    {
    FUNC_LOG;

    // CCalEntry is converted to iCal
    RBufWriteStream writeStream;
    writeStream.Open(*aBuf);
    CleanupClosePushL(writeStream);
    CCalenExporter* exporter=CCalenExporter::NewL(iSession);
    CleanupStack::PushL(exporter);
    exporter->ExportICalL( aEntry, writeStream);
    CleanupStack::PopAndDestroy(exporter);
    CleanupStack::PopAndDestroy(&writeStream);

    }


//  End of File

