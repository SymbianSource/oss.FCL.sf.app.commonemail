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
* Description:  Implements the import functionality of CESMRAgnVersit2
*
*
*/

#ifndef CESMRAGNVERSIT2_H
#define CESMRAGNVERSIT2_H

// User includes
#include "cesmragnexternalinterface.h"  //CESMRAgnExternalInterface

// Forward class declarations.
class CESMRAgnVersit2StringProvider;
class CESMRAgnVersit2Exporter;
class CESMRAgnVersit2Importer;
class RReadStream;
class RWriteStream;

/**
 * Implementation of CESMRAgnExternalInterface for iCalendar files. Objects of this
 * type are instantiated through the ECOM plugin mechanism in response to a mime
 * type of text/calendar.
 * @internalTechnology
 */
class CESMRAgnVersit2 : public CESMRAgnExternalInterface
    {
public:     // Construction/destruction.
    /**
     * Two stage constructor for AgnVersit2 objects. This is used during plugin
     * instantiation and should not be called directly from client applications.
     * @return A new CESMRAgnVersit2 object.
     * @publishedAll
     */
    static CESMRAgnVersit2* NewL();
    
    /**
     * C++ Destructor.
     */
    ~CESMRAgnVersit2();

public:     // Methods.
    /**
     * Takes an array of agenda entries and a stream of iCalendar formatted text, and
     * adds entries to the array representing the components in the stream. This
     * implements CESMRAgnExternalInterface::ImportL.
     * @see CESMRAgnExternalInterface::ImportL()
     * @param aEntries An array onto which new entries will be appended.
     * @param aReadStream A stream from which to read the iCalendar information. The
     * client is responsible for opening the stream before calling this function, and
     * is also responsible for closing it again.
     * @param aFlags not used in this implementation.
     * @param aObserver A mechanism for error reporting and handling.
     * @publishedAll
     */
    void ImportL(RPointerArray<CCalEntry>& aEntries, 
                 RReadStream& aReadStream, 
                 TUint aFlags, 
                 MESMRAgnImportObserver& aObserver);
    
    /**
     * This implements CESMRAgnExternalInterface::ExportL.
     * @see CESMRAgnExternalInterface::ExportL()
     * @param aEntry Calendar entry to be exported.
     * @param aWriteStream Stream to write output to. The client is responsible for
     * opening the stream before calling this function, and is also responsible for
     * closing it again.
     * @param aFlags Not used in this implementation.
     * @param aObserver Export observer to handle errors.
     * @publishedAll
     */
    void ExportL(const CCalEntry& aEntry, 
                 RWriteStream& aWriteStream, 
                 TUint aFlags, 
                 MESMRAgnExportObserver& aObserver);
    
    /**
     * This implements CESMRAgnExternalInterface::ExportL.
     * @see CESMRAgnExternalInterface::ExportL()
     * @param aEntries array of Calendar entries to be exported.
     * @param aWriteStream Stream to write output to. The client is responsible for
     * opening the stream before calling this function, and is also responsible for
     * closing it again.
     * @param aFlags Not used in this implementation.
     * @param aObserver Export observer to handle errors.
     * @publishedAll
     */
    void ExportL(RPointerArray<CCalEntry>& aEntries, 
                 RWriteStream& aWriteStream, 
                 TUint aFlags, 
                 MESMRAgnExportObserver& aObserver);

private:    // Construction/destruction.
    CESMRAgnVersit2();
    void ConstructL();

private:    // Members.
    CESMRAgnVersit2StringProvider* iStringProvider;
    CESMRAgnVersit2Exporter* iExporter;
    CESMRAgnVersit2Importer* iImporter;
    };

#endif

// End of file.
