/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines and implements an ECOM plugin interface for the
*  import and export of data into and out of Agenda using the Agenda API.
*
*
*/


#ifndef CESMRAGNEXTERNALINTERFACE_H
#define CESMRAGNEXTERNALINTERFACE_H

// System includes.
#include <ecom/ecom.h> // REComSession

// Forward class declarations.
class CCalEntry;
class MESMRAgnExportObserver;
class MESMRAgnImportObserver;
class RReadStream;
class RWriteStream;

/**
 * Defines and implements an ECOM plugin interface for the import and export of
 * data into and out of Agenda using the Agenda API and RReadStream/RWriteStream.
 * @publishedAll
 */
class CESMRAgnExternalInterface : public CBase
    {
public:     // Construction/destruction.
    static CESMRAgnExternalInterface* NewL(const TDesC8& aMimeType);
    ~CESMRAgnExternalInterface();

public:     // Methods.
    virtual void ImportL(RPointerArray<CCalEntry>& aEntries, RReadStream& aReadStream, TUint aFlags, MESMRAgnImportObserver& aObserver) = 0;
    virtual void ExportL(const CCalEntry& aEntries, RWriteStream& aWriteStream, TUint aFlags, MESMRAgnExportObserver& aObserver) = 0;
    virtual void ExportL(RPointerArray<CCalEntry>& aEntries, RWriteStream& aWriteStream, TUint aFlags, MESMRAgnExportObserver& aObserver) = 0;


private:
    TUid iDtorIdKey;
    };

//Flags to determine which fields to export from agenda entries
enum TAgnEntryExport
    {
    KAgnExportDtStart           =  1 << 0,
    KAgnExportDtEnd             =  1 << 1,
    KAgnExportLastModified      =  1 << 2,
    KAgnExportCompleted         =  1 << 3,
    KAgnExportRRule             =  1 << 4,
    KAgnExportRDate             =  1 << 5,
    KAgnExportExDate            =  1 << 6,
    KAgnExportAlarm             =  1 << 7,
    KAgnExportSummary           =  1 << 8,
    KAgnExportDescription       =  1 << 9,
    KAgnExportLocation          =  1 << 10,
    KAgnExportCategory          =  1 << 11,
    KAgnExportAttendee          =  1 << 12,
    KAgnExportOrganizer         =  1 << 13,
    KAgnExportPhoneOwner        =  1 << 14,
    KAgnExportStatus            =  1 << 15,
    KAgnExportReplicationStatus =  1 << 16,
    KAgnExportPriority          =  1 << 17,
    KAgnExportMethod            =  1 << 18,
    KAgnExportSequence          =  1 << 19,
    KAgnExportUID               =  1 << 20,
    KAgnExportRecurrenceId      =  1 << 21,
    KAgnExportTzRules           =  1 << 22,
    KAgnExportCreated           =  1 << 23,
    KAgnExportDtStamp           =  1 << 24,
    KAgnExportClass             =  1 << 25,
    KAgnExportXProp             =  1 << 26,

    };

#include "cesmragnexternalinterface.inl"

#endif

// End of file.
