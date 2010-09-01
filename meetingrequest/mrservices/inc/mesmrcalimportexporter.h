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


#ifndef MESMRCALIMPORTEXPORTER_H
#define MESMRCALIMPORTEXPORTER_H

class CCalEntry;

/**
 *  MESMRCalImportExporter defines interface for
 *  importing and exporting calendar entries to / from
 *  iCal format.
 *
 *  @lib esmrservices.lib
 */
class MESMRCalImportExporter
    {
public: // Destruction
    /**
     * Virtual C++ destructor.
     */
    virtual ~MESMRCalImportExporter() { }

public: // Interface
    /**
     * Exports calendar entry to iCal format. Ownership of the returned
     * descriptor is transferred to caller.
     *
     * @param aEntry Reference to CCalEntry object.
     * @return CCalEntry in iCal format.
     */
    virtual HBufC8* ExportToICal8L(
            CCalEntry& aEntry ) = 0;

    /**
     * Exports calendar entry to iCal format. Created object is left to
     * cleanup stack. Ownership of the returned descriptor is
     * transferred to caller.
     *
     * @param aEntry Reference to CCalEntry object.
     * @return CCalEntry in iCal format.
     */
    virtual HBufC8* ExportToICal8LC(
            CCalEntry& aEntry ) = 0;

    /**
     * Export calendar entry to iCal format. Ownership of the
     * returned descriptor is transferred to caller.
     *
     * @param aEntry Reference to CCalEntry object.
     * @return CCalEntry in iCal format.
     */
    virtual HBufC* ExportToICal16L(
            CCalEntry& aEntry ) = 0;

    /**
     * Export calendar entry to iCal format. Created object is left to
     * cleanup stack. Ownership of the returned descriptor is
     * transferred to caller.
     *
     * @param aEntry Reference to CCalEntry object.
     * @return CCalEntry in iCal format.
     */
    virtual HBufC* ExportToICal16LC(
            CCalEntry& aEntry ) = 0;

    /**
     * Imports iCal entry to CCalEntry. Ownership of the returned object is
     * transferred to caller.
     *
     * @param aICal Buffer containing entry in iCal format.
     * @return Pointer to imported CCalEntry object.
     */
    virtual CCalEntry* ImportFromIcalL(
            const TDes8& aIcal ) = 0;

    /**
     * Imports iCal entry to CCalEntry. Created object is left to
     * cleanup stack. Ownership of the returned object is transferred
     * to caller.
     *
     * @param aICal Buffer containing entry in iCal format.
     * @return Pointer to imported CCalEntry object.
     */
    virtual CCalEntry* ImportFromIcalLC(
            const TDes8& aIcal ) = 0;
    };

#endif  // MESMRCALIMPORTEXPORTER_H
