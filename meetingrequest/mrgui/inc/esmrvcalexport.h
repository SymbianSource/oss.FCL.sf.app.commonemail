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


#ifndef TESMRVCALEXPORT_H
#define TESMRVCALEXPORT_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h> // for TParse

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;


// CLASS DECLARATION

/**
 *  Exports Agenda entry to VCalendar data
 *
 */
NONSHARABLE_CLASS( TESMRVCalExport )
    {
public:  // Constructors and destructor
    /**
     * C++ default constructor.
     */
    TESMRVCalExport(CCalSession& aCalSession, RFs& aFileServerSession);

public: // New functions
    /**
     * Exports CCalEntry into VCalendar format
     *
     * @param aEntry - entry to be exported
     * @return Descriptor that points to the exported data
     */
    HBufC8* ExportVCalLC(CCalEntry& aEntry);

protected: // New functions
    void ConvertVCalL(CBufFlat* aBuf, CCalEntry& aEntry);
    void ConvertICalL(CBufFlat* aBuf, CCalEntry& aEntry);

private:    // Data

    // Ref: calendar session
    CCalSession& iSession;

    // Ref: file server session
    RFs& iFs;
    };

#endif


// End of File
