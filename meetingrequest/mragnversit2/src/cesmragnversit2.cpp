/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMRAgnVersit2.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmragnversit2.h"					//CESMRAgnVersit2

// User includes.
#include "cesmragnversit2importer.h"			//CESMRAgnVersit2Importer
#include "cesmragnversit2exporter.h"			//CESMRAgnVersit2Exporter
#include "cesmragnversit2stringprovider.h"	//CESMRAgnVersit2StringProvider

//<cmail>
//</cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAgnVersit2::NewL
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2* CESMRAgnVersit2::NewL()
    {
    FUNC_LOG;

    CESMRAgnVersit2* self = new (ELeave) CESMRAgnVersit2;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2::NewL
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2::~CESMRAgnVersit2()
    {
    FUNC_LOG;

    delete iImporter;
    delete iExporter;
    delete iStringProvider;

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2::ImportL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2::ImportL( RPointerArray<CCalEntry>& aEntries,
                           RReadStream& aReadStream,
                           TUint /*aFlags*/,
                           MESMRAgnImportObserver& aObserver )
    {
    FUNC_LOG;

    if (!iImporter)
        {
        iImporter =  CESMRAgnVersit2Importer::NewL(*iStringProvider);
        }

    iImporter->ImportL(aEntries, aReadStream, aObserver);

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2::ExportL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2::ExportL( const CCalEntry& aEntry,
                           RWriteStream& aWriteStream,
                           TUint aFlags,
                           MESMRAgnExportObserver& aObserver )
    {
    FUNC_LOG;

    if (!iExporter)
        {
        iExporter = CESMRAgnVersit2Exporter::NewL(*iStringProvider);
        }

    iExporter->ExportL(aEntry, aWriteStream, aFlags, aObserver);

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2::ExportL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2::ExportL( RPointerArray<CCalEntry>& aEntries,
                           RWriteStream& aWriteStream,
                           TUint aFlags,
                           MESMRAgnExportObserver& aObserver )
    {
    FUNC_LOG;

    if (!iExporter)
        {
        iExporter = CESMRAgnVersit2Exporter::NewL(*iStringProvider);
        }

    iExporter->ExportL(aEntries, aWriteStream, aFlags, aObserver);

    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2::CESMRAgnVersit2
// ---------------------------------------------------------------------------
//
CESMRAgnVersit2::CESMRAgnVersit2()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAgnVersit2::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAgnVersit2::ConstructL()
    {
    FUNC_LOG;

    iStringProvider = CESMRAgnVersit2StringProvider::NewL();
    // Initialise the importer and exporter straight away in debug mode
    // This prevents problems when memory testing
    __DEBUG_ONLY(iImporter =  CESMRAgnVersit2Importer::NewL(*iStringProvider));
    __DEBUG_ONLY(iExporter =  CESMRAgnVersit2Exporter::NewL(*iStringProvider));

    }

// End of file.

