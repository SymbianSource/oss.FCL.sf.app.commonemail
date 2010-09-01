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
* Description:  Class which performs an operation for a set of entries.
*
*/

#ifndef CESMRENTRYCMDITERATORAO_H
#define CESMRENTRYCMDITERATORAO_H

//  INCLUDE FILES
#include <e32base.h>
#include <CMRUtils.h>

// FORWARD DECLARATIONS
class MESMRCalDbMgr;

// CLASS DECLARATION

/**
 * Active object which can be used for iterating through a set of
 * calendar entries and performing an operation for each of them.
 *
 * @lib esmrdb.lib
 */
NONSHARABLE_CLASS(CESMREntryCmdIteratorAO) : public CActive
    {
    public: // Constructors and destructors

        /**
        * Symbian constructor.
        * @param aDbMgr performs each cmd iteration and observes progress
        * @return instantiated object
        */
        static CESMREntryCmdIteratorAO* NewL(
                MESMRCalDbMgr& aDbMgr );

        /**
        * Destructor.
        */
        ~CESMREntryCmdIteratorAO();

    public: // Implementation
        /**
        * Starts iterating through entries in aCalEntryArray and performs
        * the command determined by aOpCode for each of them. This method
        * handles the case of empty array.
        * Due to asynchronous nature of this task this method creates a copy
        * of the array and takes the ownership of the CCalEntry objects!
        * @param iCalEntryArray, ownership of entries is transferred
        * @param aOpCode command to perform
        */
        void StartCmdIterationL(
                const RPointerArray<CCalEntry>& aCalEntryArray,
                TInt aOpCode );

    public: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    public: // New functions
        /**
        * Helper which creates copies entries to iCalEntryArray and
        * transfers the ownership to that.
        */
        void CopyToOwnArrayL();

    private: // Constructors and destructors
        CESMREntryCmdIteratorAO(
                MESMRCalDbMgr& aDbMgr );
        void ConstructL();

    private: // data
        // Own: Code for the utils operation being executed
        TInt iOpCode;
        // Own: Array of entries
        RPointerArray<CCalEntry> iCalEntryArray;
        // Ref: Calendar database reference
        MESMRCalDbMgr& iDbMgr;
    };

#endif // CESMRENTRYCMDITERATORAO_H

// End of File
