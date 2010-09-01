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
* Description:  ESMRUI tombstone extension interface definition.
*
*/


#ifndef MESMRUTILSTOMBSEXT_H
#define MESMRUTILSTOMBSEXT_H

// INCLUDES

#include <e32base.h>
#include <calcommon.h>
//<cmail>
#include "esmrutilsapiext.h"
#include "esmrtombsextuid.h"
//</cmail>

// FORWARD DECLARATIONS
class CCalEntryView;
class CCalInstance;

// CLASS DECLARATION

/**
 * Extension API for CMRUtils.
 *
 * Usage (assuming you have CMRUtils* iUtils):
 * MESMRUtilsTombsExt* ext = ExtensionApiL<MESMRUtilsTombsExt>( iUtils );
 */
class MESMRUtilsTombsExt
    {
    public: // Data types
        /**
        * Result codes for database operations. Evalutation is done purely on
        * entry basis, not on request or method level. E.g. EStoredNew means
        * that given originating or modifying entry does not exist in the phone.
        * EStoredUpdate on the other hand means that entry with the same UID and
        * RECURRENCE-ID existed and was updated.
        * Another example: a cancellation may be a valid new entry from the
        * database point of view, but it is still questionable whether it
        * is a valid cancellation if the corresponding request does not exist.
        */
        enum TESMRUtilsDbResult
            {
            EErrorContent = -100, // General error in entry content
            EErrorRecurrence,     // Entry has invalid RECURRENCE-ID
            EErrorObsolete,       // Entry is obsolete (old SEQUENCE or
                                  // DTSTAMP), does not tell whether meeting
                                  // occurred in past
            EErrorHasBeenDeleted, // Deleted and exists as a tombstone, this
                                  // can be considered as a special case of
                                  // EErrorCancelled
            EErrorIdenticalExists,// Identical already exists in db
            EErrorCancelled,      // Entry exists with status "ECancelled"
            EUndefined = 0,       // Result undefined (used only internally)
            ECheckedValidNew,     // Entry was checked, is valid new entry
            ECheckedValidUpdate,  // Entry was checked, is valid update
            EStoredNew,           // Stored new entry (new in this phone)
            EStoredUpdate,        // Stored update (newer SEQUENCE or DTSTAMP)
            EResurrectedDeleted,  // Stored entry which existed as a tombstone,
                                  // can only happen if explicitely requested
            EResurrectedCancelled // Stored entry which was earlier "ECancelled",
                                  // can only happen if explicitely requested
            };

    public: // New functions
        /**
         * Fetches the extension UID for Thombstone extension
         */
        static TUid ExtensionUid()
            { return TUid::Uid( KESMRUTilsTombsExtUid ); }

        /**
        * Store entry to Agenda Model conditionally using entry validation
        * and tombstone logic. Attempts to resolves and set entry's phone
        * owner field before saving if currently unset.
        * Resurrection of a modifying entry will leave if originating entry
        * does not exist in the Agenda Model.
        * @param aCalEntry entry to store, phone owner is set if resolvable
        * @param aResurrect if ETrue then entry stored even if it has been
        * previously deleted
        * @return result code
        */
        virtual TESMRUtilsDbResult StoreL(
                CCalEntry& aCalEntry,
                TBool aResurrect ) = 0;

        /**
        * Delete entry from Agenda Model if found and create
        * tombstone(s).
        * This method causes deletion of all entries with given GUID.
        * @param aUid determines entry to be deleted
        */
        virtual void DeleteL(
                const TDesC8& aUid ) = 0;

        /**
        * Delete entry from Agenda Model if found and create
        * tombstone(s).
        * This method causes deletion of a modifying entry, or entire
        * recurring event if aLocalUid corresponds to an originating entry.
        * @param aLocalUid determines entry to be deleted
        */
        virtual void DeleteL(
                const TCalLocalUid& aLocalUid ) = 0;

        /**
        * Delete entry from Agenda Model if found and create
        * tombstone(s).
        * This method causes deletion of a modifying entry, or entire
        * recurring event if aCalEntry is an originating entry.
        * @param aCalEntry entry to be deleted
        */
        virtual void DeleteL(
                const CCalEntry& aCalEntry ) = 0;

        /**
        * Delete entries from Agenda Model if found and create
        * tombstone(s).
        * This is asynchronous, user gets notified with
        * MMRUtilsObserver::HandleOperation( EDeleteEntries,... )
        * @param aCalTimeRange determines entries to be deleted
        */
        virtual void DeleteL(
                const CalCommon::TCalTimeRange& aCalTimeRange ) = 0;
    };

#endif  // MESMRUTILSTOMBSEXT_H

// End of File
