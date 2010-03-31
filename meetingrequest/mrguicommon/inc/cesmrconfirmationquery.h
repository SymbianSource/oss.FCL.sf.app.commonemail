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
* Description:  Class is responsible of showing confirmation queries
*
*/


#ifndef CESMRCONFIRMATIONQUERY_H
#define CESMRCONFIRMATIONQUERY_H

//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32base.h>

class CEikonEnv;
/**
 * CESMRConfirmationQuery encapsulates Yes/No confirmation query
 * query for ES MR Utils usage.
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRConfirmationQuery ) : public CBase
    {
public:
    /**
    * Enumeration definition for different kind of
    * confirmation queries
    */
    enum TESMRConfirmationQueryType
        {
        EESMRDeleteMR = 0,
        EESMRSendCancellationInfoToParticipants,
        EESMRRemoveAppointment,
        EESMRSaveChanges,
        EESMRDeleteEntry,
        EESMRSendDecline,
        EESMRSaveAnnivChangedStartDay,
        EESMRAttachments,
        EESMRSaveMeetingChangedStartDay,
        EESMRAssignUpdatedLocation,
        EESMRAttachmentsNotSupported
        };
public:
    /**
     * C++ Destructor.
     */
    ~CESMRConfirmationQuery();

     /**
     * Static version of ExecuteLD.
     * @param aType The type of confirmation query.
     * @return TBool ETrue if Yes, EFalse for no.
     */
     IMPORT_C static TBool ExecuteL( TESMRConfirmationQueryType aType );
     
     /**
     * Static version of ExecuteLD, for location text overwrite queries.
     * only used internally by esmrgui and doesn't need to be exported
     * @param aLocation location text to be used and truncated to the query
     * @return TBool ETrue if Yes, EFalse for no.
     */
     static TBool ExecuteL( const TDesC& aLocation );
              
private: // Implementation
    CESMRConfirmationQuery( TESMRConfirmationQueryType aType );
    void ConstructL();
    TBool ExecuteLD();
    HBufC* TruncateTextToLAFNoteL( const TDesC& aLocation );

private: // Data
    /**
    * Attendee status
    */
    TESMRConfirmationQueryType iType;

    /**
    * Resource offset
    */
    TInt iResourceOffset;

    // Ref:
    CEikonEnv* iEnv;
    };

#endif // CESMRCONFIRMATIONQUERY_H
