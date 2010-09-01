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
* Description:  Interface definition for ESMR iCal import exporter
*
*/


#ifndef CESMRCALIMPORTEXPORTER_H
#define CESMRCALIMPORTEXPORTER_H

#include <e32base.h>
//<cmail>
#include "mesmragnimportobserver.h"
#include "mesmragnexportobserver.h"
//</cmail>
#include "mesmrcalimportexporter.h"

class CESMRAgnExternalInterface;

/**
 *  CESMRCalImportExporter implements interface for
 *  importing and exporting calendar entries to / from
 *  iCal format.
 *
 *  @see MESMRCalImportExporter
 *  @see CAgnExternalInterface
 *  @lib esmrservices.lib
 */
NONSHARABLE_CLASS(CESMRCalImportExporter) :
        public CBase,
        public MESMRCalImportExporter,
        public MESMRAgnImportObserver,
        public MESMRAgnExportObserver
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CESMRCalImportExporter object.
     * Ownership is transferred to caller.
     *
     * @return Pointer to CESMRCalImportExporter object.
     */
    IMPORT_C static CESMRCalImportExporter* NewL();

    /**
     * Creates and initializes new CESMRCalImportExporter object.
     * Ownership is transferred to caller and created object is
     * left to cleanup stack.
     *
     * @return Pointer to CESMRCalImportExporter object.
     */
    IMPORT_C static CESMRCalImportExporter* NewLC();

    /**
     * C++ destructor.
     */
    IMPORT_C ~CESMRCalImportExporter();

public: // From MESMRCalImportExporter
    HBufC8* ExportToICal8L( CCalEntry& aEntry );
    HBufC8* ExportToICal8LC( CCalEntry& aEntry );
    HBufC* ExportToICal16L( CCalEntry& aEntry );
    HBufC* ExportToICal16LC( CCalEntry& aEntry );
    CCalEntry* ImportFromIcalL(const TDes8& aIcal );
    CCalEntry* ImportFromIcalLC(const TDes8& aIcal );

public: // From MAgnImportObserver
    TImpResponse AgnImportErrorL(
            TImpError aType,
            const TDesC8& aUid,
            const TDesC& aContext);

public: // From MAgnExportObserver
    TExpResponse AgnExportErrorL(
            TExpError aType,
            const TDesC8& aUid,
            const TDesC& aContext);

private: // Implementation
    CESMRCalImportExporter();
    void ConstructL();

private: // Data
    /**
     * Import/export implementation
     * Own.
     */
    CESMRAgnExternalInterface* iImportExporter;
    };

#endif  // CESMRCALIMPORTEXPORTER_H
