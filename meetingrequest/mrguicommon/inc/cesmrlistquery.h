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
* Description:  Definition for mrui list queries
*
*/


#ifndef CESMRLISTQUERY_H
#define CESMRLISTQUERY_H

//  INCLUDES
#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

// FORWARD DECLARATIONS:
class CDesC16ArrayFlat;
class CAknPopupList;
class CESMRAlarm;
class CESMRRecurrence;
class CAknSinglePopupMenuStyleListBox;

// CLASS DECLARATION
class CEikonEnv;
/**
 * CESMRListQuery encapsulates edit before send pop-up list
 * query for ES MR Utils usage.
 *
 * Usage:
 *
 * TESMRResponseType resp = (TESMRResponseType)
 *      CESMRListQuery::ExecuteEditBeforeSendL(EESMRAttendeeStatusTentative);
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRListQuery ) : public CBase
    {
    public:
        enum TESMRListQueryType
            {
            EESMREditBeforeSendQuery = 0,
            EESMRSendUpdateToAllQuery,
            EESMRRecurrenceQuery,
            EESMRRelativeAlarmTimeQuery,
            EESMROpenThisOccurenceOrSeriesQuery,
            EESMRPriorityPopup,
            EESMRTodoPriorityPopup,
            EESMRSynchronizationPopup,
            EESMRDeleteThisOccurenceOrSeriesQuery,
            EESMREditBeforeSendQueryWithSendOptionsOnly,
            EESMRUnifiedEditorPopup,
            EESMRNormalResponseQuery,
            EESMRRemoveResponseQuery,
            EESMRMultiCalenQuery,
            EESMRForwardThisOccurenceOrSeriesQuery
            };
        
    public:
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CESMRListQuery* NewL(TESMRListQueryType aType);
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CESMRListQuery* NewL(TESMRListQueryType aType, 
						RArray<TInt>& aFilter);

        /**
        * Destructor.
        */
        ~CESMRListQuery();

    private: // Constructors

        CESMRListQuery( TESMRListQueryType aType );
        CESMRListQuery( TESMRListQueryType aType, RArray<TInt>& aFilter );
        void ConstructL();

    public: // Implementation

         /**
         * Sets the attendee response status to Edit before send query's
         * title.
         * @param aStatus Response status
         */
         void SetAttendeeStatus(TESMRAttendeeStatus aStatus);


         /**
         * Executes the dialog and destroys itself.
         * @return TESMRResponseType, or KErrCancel if dialog has
         *         been cancelled.
         */
         IMPORT_C TInt ExecuteLD();

         /**
         * Static version of ExecuteLD for edit before send dialog
         * @param aStatus attendee status.
         * @return TESMRResponseType, or KErrCancel if dialog has
         *         been cancelled.
         */
         IMPORT_C static TInt ExecuteEditBeforeSendL(
                 TESMRAttendeeStatus aStatus);

         /**
         * Static version of ExecuteLD for edit before send dialog
         * @param aStatus attendee status.
         * @return TESMRResponseType, or KErrCancel if dialog has
         *         been cancelled.
         */
         IMPORT_C static TInt ExecuteEditBeforeSendWithSendOptOnlyL(
                 TESMRAttendeeStatus aStatus );

         /**
         * Static version of ExecuteLD for other list querys
         * @param aStatus attendee status.
         * @return
         *         TESMRSendUpdateQueryResponeStatus if the type is EESMRSendUpdateAll
         *         TESMRGUIAlarmValue if the type is EESMRRelativeAlarmTime
         *         TESMRGUIRecurrenceValue if the type is EESMRRecurrence
         *         TESMRThisOccurenceOrSeriesQuery if the type is
         *              EESMRThisOccurenceOrSeriesQuery
         *         or KErrCancel if dialog has
         *         been cancelled.
         */
         IMPORT_C static TInt ExecuteL(
                 TESMRListQueryType aType);
         
         /**
         * Static version of ExecuteLD for other list querys
         * @param aStatus attendee status.
         * @param aFilter contains information about items that should be
         * 		  excluded from popup list.
         * @return
         *         TESMRSendUpdateQueryResponeStatus if the type is EESMRSendUpdateAll
         *         TESMRGUIAlarmValue if the type is EESMRRelativeAlarmTime
         *         TESMRGUIRecurrenceValue if the type is EESMRRecurrence
         *         TESMRThisOccurenceOrSeriesQuery if the type is
         *              EESMRThisOccurenceOrSeriesQuery
         *         or KErrCancel if dialog has
         *         been cancelled.
         */
         IMPORT_C static TInt ExecuteL(
                 TESMRListQueryType aType, RArray<TInt>& aFilter);
         
         /**
          * For multi-calendar query
          * @param aCalenNameList multi-calendar names for display
          * @return index of user choose 
          */
         IMPORT_C TInt ExecuteLD( RArray<TPtrC>& aCalenNameList );
                  
         /**
          * Static version of ExecuteLD for multi-calendar querys
          * @param aCalenNameList multi-calendar name list
          * @return index of user choose
          */
         IMPORT_C static TInt ExecuteL(
                 TESMRListQueryType aType, RArray<TPtrC>& aCalenNameList );

    private: // Implementation:

        void SetListQueryTextsL(
                CDesC16ArrayFlat* aItemArray,
                CAknPopupList* aPopupList);
        TInt MapSelectedIndexToReturnValue(TInt aIndex);
        TInt MapUnifiedEditorItemTextToIndexL(
                CAknSinglePopupMenuStyleListBox& aList );        
        void LoadTextsFromResourceL();

    private: // Data
        /// Own: Attendee status
        TESMRAttendeeStatus iStatus;
        /// Own: Resource offset;
        TInt iResourceOffset;
        /// Own: List query type
        TESMRListQueryType iType;
        /// Own: Array for Alarm items
        RPointerArray< CESMRAlarm > iAlarmArray;
        /// Own: Array for Recurrence items
        RPointerArray< CESMRRecurrence > iRecurrenceArray;
        /// Ref:
        CEikonEnv* iEnv;
        /// Own: Filter for dynamic popup, not showing all items
        RArray<TInt> iFilter;
    };

#endif // CESMRLISTQUERY_H

// End of File
