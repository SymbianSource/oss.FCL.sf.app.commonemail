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
* Description:  Definition for static helper methods
*
*/

#ifndef ESMRHELPER_H
#define ESMRHELPER_H

#include <e32base.h>
#include <msvstd.h>
#include <calentry.h>

class CCalAttendee;
class CCalUser;
class CMsvSession;

/**
 *  Static helpers methods ES MR UI subsystem
 *
 */
NONSHARABLE_CLASS(ESMRHelper)
    {
public: // Data types
    /**
    * Enumeration for entry copy method
    */
    enum TCopyFields
        {
        ECopyFull,
        ECopyOrganizer,
        ECopySkeleton
        };

public: // New functions
    /**
     * Creates a new copy of the given attendee.
     *
     * @param aSource
     * @return attendee, ownership is transferred to caller.
     */
    IMPORT_C static CCalAttendee* CopyAttendeeL(
            CCalAttendee& aSource );

    /**
     * Creates a new copy of the given attendee. Item is
     * left in the cleanup stack.
     *
     * @param aSource
     * @return attendee, ownership is transferred to caller.
     */
    IMPORT_C static CCalAttendee* CopyAttendeeLC(
            CCalAttendee& aSource );

    /**
     * Creates a new copy of the given cal user.
     *
     * @param aSource
     * @return user, ownership is transferred to caller.
     */
    IMPORT_C static CCalUser* CopyUserL(
            CCalUser& aSource );

    /**
     * Creates a new copy of the given cal user. Item is
     * left in the cleanup stack.
     *
     * @param aSource
     * @return user, ownership is transferred to caller.
     */
    IMPORT_C static CCalUser* CopyUserLC(
            CCalUser& aSource );

    /**
     * Creates a copy with given method type.
     * Start/end time and DTSTAMP are set even for a skeleton.
     * @param aEntry source entry
     * @param aCopyType specifies copied data
     * @return instantiated entry, ownership transferred to caller
     */
    IMPORT_C static CCalEntry* CopyEntryL(
            const CCalEntry& aEntry,
            CCalEntry::TMethod aMethod,
            TCopyFields aCopyType );

    /**
     * Creates a copy with given method type.
     * Start/end time and DTSTAMP are set even for a skeleton.
     * Copied entry is left in the cleanup stack.
     * @param aEntry source entry
     * @param aCopyType specifies copied data
     * @return instantiated entry, ownership transferred to caller
     */
    IMPORT_C static CCalEntry* CopyEntryLC(
            const CCalEntry& aEntry,
            CCalEntry::TMethod aMethod,
            TCopyFields aCopyType );

    /**
     * Copies entry fields according to the copy type specification.
     * Unlike CCalEntry::CopyFromL() this function never copies
     * the entry method property.
     * @param aSource source for copying
     * @param aTarget target for copying
     * @param aCopyType specifies what data to copy
     */
    IMPORT_C static void CopyFieldsL(
            const CCalEntry& aSource,
            CCalEntry& aTarget,
            TCopyFields aCopyType );

    /**
     * Returns TPtrC to all/part of the aAddress parameter
     * so that possible "mailto:" or "MAILTO:" prefix is 
     * stripped out.
     *
     * @param aAddress address which might contain the prefix
     * @return TPtrC which represents only the address
     */
    IMPORT_C static TPtrC AddressWithoutMailtoPrefix(
            const TDesC& aAddress );

    /**
     * Parses drive name of dll(eg. "c:") and sets it to
     * aDriveName parameter.
     *
     * @param aDriveName TFileName where parsed drive letter
     * shall be set.
     */
    IMPORT_C static void GetCorrectDllDriveL(
            TFileName& aDriveName );

    /**
     * Loads resource file into CONE environment.
     * @param aResourceFile Resource file name.
     * @param aResourcePath Resource file path.
     * @return Resource file offset.
     */
    IMPORT_C static TInt LoadResourceL(
            const TDesC& aResourceFile,
            const TDesC& aResourcePath );
    /**
     * Resolves corresponding SMTP service for received MTM
     * @param aRelatedService Related service
     * @param aMsvSession Reference to MSV session.
     * @return TMsvId for sending service.
     */
    IMPORT_C static TMsvId CorrespondingSmtpServiceL(
            TMsvId aRelatedService,
            CMsvSession& aMsvSession );

    /**
     * Populates empty fields of a child entry with data from the
     * parent entry.
     * @param aChild Child (modifying) entry to be populated .
     * @param Parent (originating) entry
     */
    IMPORT_C static void PopulateChildFromParentL (
            CCalEntry &aChild,
            const CCalEntry &aParent);

     /**
     * Locates the calentry policy resource file.
     *
     * @param aResource Resourcefile to be located.
     * @param aPath Path from which file is searched
     * @param aResourceFile On return contains the resource file name.
     * @param aFs File server session reference. Ownership is not transfered.
     * @return KErrNone on success, otherwise system wide error code.
     */
    IMPORT_C static TInt LocateResourceFile(
            const TDesC& aResource,
            const TDesC& aPath,
            TFileName &aResourceFile,
            RFs* aFs = NULL );
    
    //<cmail> getting temporary file name is not hard coded
    /**
    * Creates and append process'es private directory to begining of file.
    * It also appends two back slashes, so aFileName should not contain any
    * backslashes in the begining.
    *
    * @param aFileName private directory is appended to this file name
    * @return error code
    */
    IMPORT_C static TInt CreateAndAppendPrivateDirToFileName(TFileName& aFileName);
    //</cmail>
    
    };

#endif      // ESMRHELPER_H
