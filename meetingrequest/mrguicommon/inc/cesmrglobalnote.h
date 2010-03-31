/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Edit before send list pop-up query
*
*/


#ifndef CESMRGLOBALNOTE_H
#define CESMRGLOBALNOTE_H

//  INCLUDES
#include <e32base.h>
#include <aknnotifystd.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

// CLASS DECLARATION
class CEikonEnv;
/**
 * CESMRGlobalNote encapsulates Yes/No confirmation query
 * query for ES MR Utils usage.
 */
NONSHARABLE_CLASS( CESMRGlobalNote ) : public CBase
    {
    public:
        enum TESMGlobalNoteType
            {
            EESMRCorruptedMR = 0,
            EESMREndsBeforeStarts,
            EESMRCalenLaterDate,
            EESMREntryEndEarlierThanItStart,
            EESMRAlarmAlreadyPassed,
            EESMRDiffMoreThanMonth,
            EESMRRepeatEndEarlierThanItStart,
            EESMREntrySaved,
            EESMRTodoEntrySaved,
            EESMRRepeatDifferentStartAndEndDate,
            EESMRRepeatReSchedule,
            EESMRCannotDisplayMuchMore,
            EESMRRepeatInstanceTooEarly,
            EESMRUnableToEdit,
            EESMROverlapsExistingInstance, 
            EESMRInstanceAlreadyExistsOnThisDay,
            EESMRInstanceOutOfSequence
            };
    public:
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CESMRGlobalNote* NewL(
                TESMGlobalNoteType aType);

        /**
        * Destructor.
        */
        ~CESMRGlobalNote();

    private: // Constructors

        CESMRGlobalNote( TESMGlobalNoteType aType );
        void ConstructL();
        TAknGlobalNoteType NoteType();
        HBufC* NoteTextLC();

    public: // Implementation

         /**
         * Executes the dialog and destroys itself.
         */
         IMPORT_C void ExecuteLD();

         /**
         * Static version of ExecuteLD.
         * @param aType The type of confirmation query.         
         */
         IMPORT_C static void ExecuteL(
                 TESMGlobalNoteType aType );

    private: // Data
        /// Own: Attendee status
        TESMGlobalNoteType iType;
        /// Own: Resource offset;
        TInt iResourceOffset;
        /// Ref:
        CEikonEnv* iEnv;

    };

#endif // CESMRGLOBALNOTE_H

// End of File
