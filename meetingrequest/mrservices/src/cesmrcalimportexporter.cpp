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
* Description:  ESMR iCal import exporter implementation
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmrcalimportexporter.h"

#include <calentry.h>
#include <s32mem.h>
#include <utf.h>

//<cmail>
#include "cesmragnexternalinterface.h"
//</cmail>

/// Unnamed namespace for local definitions
namespace {

// Literal definition for text/calendar MIME content-type
_LIT8( KTextCalendar, "text/calendar" );

// Buffer expand size
const TInt KBufExpandSize = 64;

// Definition for first position
const TInt KFirstPos = 0;

// Definition for exporting all
const TUint KFlagExportAll = 0xFFFFFFFF;

// Definition for import flags
const TUint KFlagImport = 0;

/**
 * Cleanup operation for RPointerArray.
 * @param aArray Pointer to RPointerArray.
 */
void PointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CCalEntry>* entryArray =
        static_cast<RPointerArray<CCalEntry>*>( aArray );

    entryArray->ResetAndDestroy();
    entryArray->Close();
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::CESMRCalImportExporter
// ---------------------------------------------------------------------------
//
inline CESMRCalImportExporter::CESMRCalImportExporter()
    {
    FUNC_LOG;
    // No implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::~CESMRCalImportExporter
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCalImportExporter::~CESMRCalImportExporter()
    {
    FUNC_LOG;
    delete iImportExporter;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCalImportExporter* CESMRCalImportExporter::NewL()
    {
    FUNC_LOG;
    CESMRCalImportExporter* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCalImportExporter* CESMRCalImportExporter::NewLC()
    {
    FUNC_LOG;
    CESMRCalImportExporter* self = new (ELeave) CESMRCalImportExporter();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCalImportExporter::ConstructL()
    {
    FUNC_LOG;
    // Create IMPORT / EXPORTER for text/calendar mime type
    iImportExporter =
            CESMRAgnExternalInterface::NewL( KTextCalendar() );
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::ExportToICal8L
// ---------------------------------------------------------------------------
//
HBufC8* CESMRCalImportExporter::ExportToICal8L( CCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC8* buf = ExportToICal8LC( aEntry );
    CleanupStack::Pop( buf );
    return buf;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::ExportToICal16L
// ---------------------------------------------------------------------------
//
HBufC* CESMRCalImportExporter::ExportToICal16L( CCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC* buf = ExportToICal16LC( aEntry );
    CleanupStack::Pop( buf );
    return buf;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::ExportToICal16LC
// ---------------------------------------------------------------------------
//
HBufC* CESMRCalImportExporter::ExportToICal16LC( CCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC8* buf8 = ExportToICal8LC( aEntry );
    HBufC* buf16 = HBufC::NewLC( buf8->Length() );
    TPtr16 ptr16( buf16->Des() );

    TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr16, *buf8 );
    User::LeaveIfError( err );
    CleanupStack::Pop( buf16 );
    CleanupStack::PopAndDestroy( buf8 );
    CleanupStack::PushL( buf16 );
    return buf16;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::ExportToICalLC
// ---------------------------------------------------------------------------
//
HBufC8* CESMRCalImportExporter::ExportToICal8LC( CCalEntry& aEntry )
    {
    FUNC_LOG;
    CBufFlat* buffer = CBufFlat::NewL(KBufExpandSize);
    CleanupStack::PushL( buffer );

    RBufWriteStream wStream(*buffer);
    wStream.PushL();

    iImportExporter->ExportL(
            aEntry,
            wStream,
            KFlagExportAll,
            *this );

    HBufC8* exportBuffer = buffer->Ptr(KFirstPos).AllocL();

    wStream.Pop();
    wStream.Close();
    CleanupStack::PopAndDestroy( buffer );

    CleanupStack::PushL( exportBuffer );
    return exportBuffer;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::ImportFromIcalL
// ---------------------------------------------------------------------------
//
CCalEntry* CESMRCalImportExporter::ImportFromIcalL(const TDes8& aIcal )
    {
    FUNC_LOG;
    CCalEntry* entry = ImportFromIcalLC( aIcal );
    CleanupStack::Pop( entry );
    return entry;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::ImportFromIcalLC
// ---------------------------------------------------------------------------
//
CCalEntry* CESMRCalImportExporter::ImportFromIcalLC(const TDes8& aIcal )
    {
    FUNC_LOG;
    CCalEntry* entry = NULL;

    CBufFlat* buffer = CBufFlat::NewL(KBufExpandSize);
    CleanupStack::PushL( buffer );
    buffer->InsertL(KFirstPos, aIcal );

    RBufReadStream rStream(*buffer);
    rStream.PushL();
    RPointerArray<CCalEntry> entries;
            CleanupStack::PushL(
                        TCleanupItem(
                            PointerArrayCleanup,
                            &entries    ) );

    iImportExporter->ImportL(
            entries,
            rStream,
            KFlagImport,
            *this );

    if ( entries.Count() )
        {
        entry = entries[ KFirstPos ];
        entries.Remove( KFirstPos );
        }

    CleanupStack::PopAndDestroy( &entries );
    rStream.Pop();
    rStream.Close();
    CleanupStack::PopAndDestroy( buffer );

    CleanupStack::PushL( entry );
    return entry;
    }

// ---------------------------------------------------------------------------
// CESMRCalImportExporter::AgnImportErrorL
// ---------------------------------------------------------------------------
//
MESMRAgnImportObserver::TImpResponse CESMRCalImportExporter::AgnImportErrorL(
            TImpError /*aType*/,
            const TDesC8& /*aUid*/,
            const TDesC& /*aContext*/ )
    {
    FUNC_LOG;
    // Ignore error and continue processing
    return MESMRAgnImportObserver::EImpResponseContinue;
    }


//--------------------------------------------------------------------------
// CESMRCalImportExporter::AgnExportErrorL
//--------------------------------------------------------------------------
//
MESMRAgnExportObserver::TExpResponse CESMRCalImportExporter::AgnExportErrorL(
                TExpError /*aType*/,
                const TDesC8& /*aUid*/,
                const TDesC& /*aContext*/ )
    {
    FUNC_LOG;
    // Ignore error and continue processing
    return MESMRAgnExportObserver::EExpResponseContinue;
    }

// EOF

