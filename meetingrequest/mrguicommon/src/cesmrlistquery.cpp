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
* Description:  Implementation for mrui list queries
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmrlistquery.h"

#include <aknlists.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <aknPopup.h>
#include <bautils.h>
#include <esmrgui.rsg>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>

#include "esmrgui.hrh"
#include "cesmralarm.h"
#include "cesmrrecurrence.h"
#include "esmrhelper.h"//locateresourcefile

// CONSTANTS
// Unnamed namespace for local definitions
namespace {

// resource file:
_LIT( KResourceFileName, "esmrgui.rsc");

} // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRListQuery::CESMRListQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CESMRListQuery::CESMRListQuery( TESMRListQueryType aType )
    : iType( aType )
    {
    FUNC_LOG;
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::CESMRListQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CESMRListQuery::CESMRListQuery(
        TESMRListQueryType aType, 
        RArray<TInt>& aFilter )
    : iType( aType ), iFilter( aFilter )
    {
    FUNC_LOG;
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::~CESMRListQuery
// -----------------------------------------------------------------------------
//
CESMRListQuery::~CESMRListQuery()
    {
    FUNC_LOG;
    iAlarmArray.ResetAndDestroy();
    iRecurrenceArray.ResetAndDestroy();
    iEnv->DeleteResourceFile(iResourceOffset);
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRListQuery*
    CESMRListQuery::NewL( TESMRListQueryType aType )
    {
    CESMRListQuery* self = new (ELeave) CESMRListQuery(aType);
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRListQuery*
    CESMRListQuery::NewL( TESMRListQueryType aType, RArray<TInt>& aFilter )
    {
    CESMRListQuery* self = new (ELeave) CESMRListQuery(aType, aFilter);
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRListQuery::ConstructL()
    {
    FUNC_LOG;
    TFileName filename;

    iEnv = CEikonEnv::Static();  // codescanner::eikonenvstatic

    ESMRHelper::LocateResourceFile(
            KResourceFileName,
            KDC_RESOURCE_FILES_DIR,
            filename,
            &iEnv->FsSession() );

    //for localization
    BaflUtils::NearestLanguageFile(iEnv->FsSession(),filename); 
    iResourceOffset = iEnv->AddResourceFileL(filename);
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::SetAttendeeStatus
// -----------------------------------------------------------------------------
//
void CESMRListQuery::SetAttendeeStatus(TESMRAttendeeStatus aStatus)
    {
    FUNC_LOG;
    iStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRListQuery::ExecuteEditBeforeSendL(
		TESMRAttendeeStatus aStatus)
    {
    FUNC_LOG;
    CESMRListQuery* query =
            CESMRListQuery::NewL( EESMREditBeforeSendQuery );
    CleanupStack::PushL(query);
    query->SetAttendeeStatus(aStatus);
    TInt ret = query->ExecuteLD();
    CleanupStack::Pop(query);
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRListQuery::ExecuteEditBeforeSendWithSendOptOnlyL(
                 TESMRAttendeeStatus aStatus )
    {
    FUNC_LOG;
    CESMRListQuery* query =
            CESMRListQuery::NewL( 
            		EESMREditBeforeSendQueryWithSendOptionsOnly );
    CleanupStack::PushL(query);
    query->SetAttendeeStatus(aStatus);
    TInt ret = query->ExecuteLD();
    CleanupStack::Pop(query);
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRListQuery::ExecuteL( TESMRListQueryType aType )
    {
    FUNC_LOG;
    CESMRListQuery* query = CESMRListQuery::NewL( aType );
    CleanupStack::PushL( query );
    TInt ret = query->ExecuteLD();
    CleanupStack::Pop( query );
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRListQuery::ExecuteL( TESMRListQueryType aType, 
					RArray<TInt>& aFilter )
    {
    FUNC_LOG;
    CESMRListQuery* query = CESMRListQuery::NewL( aType, aFilter );
    CleanupStack::PushL( query );
    TInt ret = query->ExecuteLD();
    CleanupStack::Pop( query );
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::SetListQueryTextsL
// -----------------------------------------------------------------------------
//
void CESMRListQuery::SetListQueryTextsL(
        CDesCArrayFlat* aItemArray,
        CAknPopupList* aPopupList)
    {
    FUNC_LOG;
    if ( iType == EESMREditBeforeSendQuery ||
         iType == EESMREditBeforeSendQueryWithSendOptionsOnly)
        {
        // Set title:
        if (iStatus == EESMRAttendeeStatusAccept)
            {
            aPopupList->SetTitleL(*iEnv->AllocReadResourceAsDes16LC(
                    R_QTN_MEET_REQ_SEND_QUERY_ACCEPTED));
            CleanupStack::PopAndDestroy(); // Resource string
            }
        else if (iStatus == EESMRAttendeeStatusTentative)
            {
            aPopupList->SetTitleL(*iEnv->AllocReadResourceAsDes16LC(
                    R_QTN_MEET_REQ_SEND_QUERY_TENTATIVE));
            CleanupStack::PopAndDestroy(); // Resource string
            }
        else if (iStatus == EESMRAttendeeStatusDecline)
            {
            aPopupList->SetTitleL(*iEnv->AllocReadResourceAsDes16LC(
                    R_QTN_MEET_REQ_SEND_QUERY_DECLINED));
            CleanupStack::PopAndDestroy(); // Resource string
            }

        // Set list items:
        aItemArray->AppendL(*iEnv->AllocReadResourceAsDes16LC(
                R_QTN_MEET_REQ_SEND_NOW));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*iEnv->AllocReadResourceAsDes16LC(
                R_QTN_MEET_REQ_EDIT_BEFORE_SEND));
        CleanupStack::PopAndDestroy(); // Resource string

        if ( iType == EESMREditBeforeSendQuery )
            {
            aItemArray->AppendL(*iEnv->AllocReadResourceAsDes16LC(
                    R_QTN_MEET_REQ_DONT_SEND_RESPONSE));
            CleanupStack::PopAndDestroy(); // Resource string
            }
        }

    else if ( iType == EESMRSendUpdateToAllQuery )
        {
        // Set title:
        aPopupList->SetTitleL(*iEnv->AllocReadResourceAsDes16LC(
                R_QTN_MEET_REQ_SEND_UPDATE_QUERY));
        CleanupStack::PopAndDestroy(); // Resource string

        // Set list items:
        aItemArray->AppendL(*iEnv->AllocReadResourceAsDes16LC(
                R_QTN_MEET_REQ_SEND_UPDATE_ALL_QUERY));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*iEnv->AllocReadResourceAsDes16LC(
                R_QTN_MEET_REQ_SEND_UPDATE_UPDATED_QUERY));
        CleanupStack::PopAndDestroy(); // Resource string
        }
    else if ( iType == EESMRRecurrenceQuery )
        {
        // Set title:
        aPopupList->SetTitleL(*iEnv->AllocReadResourceAsDes16LC(
                R_QTN_MEET_REQ_SET_RECURRENCE_HEADER));
        CleanupStack::PopAndDestroy(); // Resource string
        const TInt count(iRecurrenceArray.Count());
        for ( TInt i=0; i<count; i++ )
            {
            
            aItemArray->AppendL(iRecurrenceArray[i]->RecurrenceText());
            }
        }
    else if ( iType == EESMRRelativeAlarmTimeQuery )
        {
        aPopupList->SetTitleL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_SET_ALARM_HEADER));
        CleanupStack::PopAndDestroy(); // Resource string

        const TInt count(iAlarmArray.Count());
        for ( TInt i=0; i<count; i++ )
            {
            aItemArray->AppendL(*iAlarmArray[i]->Text());
            }
        }
    else if ( iType == EESMROpenThisOccurenceOrSeriesQuery )
        {
        // Set title:
        aPopupList->SetTitleL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_HEADING_OPEN));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_THIS_OCCURRENCE));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_SERIES));
        CleanupStack::PopAndDestroy(); // Resource string
        }
    else if (iType == EESMRDeleteThisOccurenceOrSeriesQuery )
        {
        // Deleting recurrent event
        aPopupList->SetTitleL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_HEADING_DELETE));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_THIS_OCCURRENCE_DELETE));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_SERIES_DELETE));
        CleanupStack::PopAndDestroy(); // Resource string
        }
    else if ( iType == EESMRForwardThisOccurenceOrSeriesQuery )
        {
        // Set title:
        aPopupList->SetTitleL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_HEADING_FORWARD));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_RECURRENT_FW_THIS_OCCURRENCE));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_FW_RECURRENT_SERIES));
        CleanupStack::PopAndDestroy(); // Resource string
        }
    else if ( iType == EESMRPriorityPopup || iType == EESMRTodoPriorityPopup)
        {
        // Set title:
        aPopupList->SetTitleL(*StringLoader::LoadLC(
                R_QTN_MEET_REQ_OPT_PRIORITY));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_CALENDAR_TASK_PRIORITY_HIGH));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_CALENDAR_TASK_PRIORITY_NORMAL));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_CALENDAR_TASK_PRIORITY_LOW));
        CleanupStack::PopAndDestroy(); // Resource string
        }
    else if ( iType == EESMRSynchronizationPopup )
        {
        // Set title:
        aPopupList->SetTitleL(*StringLoader::LoadLC(
                R_QTN_CALENDAR_TASK_SYNC_TITLE));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_CALENDAR_TASK_SYNC_PRIVATE));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_CALENDAR_TASK_SYNC_PUBLIC));
        CleanupStack::PopAndDestroy(); // Resource string
        aItemArray->AppendL(*StringLoader::LoadLC(
                R_QTN_CALENDAR_TASK_SYNC_NONE));
        CleanupStack::PopAndDestroy(); // Resource string
        }
    else if ( iType == EESMRUnifiedEditorPopup )
		{
		// Set title:
		aPopupList->SetTitleL(*StringLoader::LoadLC(
				R_QTN_CALENDAR_TASK_UNIFIED_TITLE));
		CleanupStack::PopAndDestroy(); // Resource string
		aItemArray->AppendL(*StringLoader::LoadLC(
				R_QTN_MEET_REQ_TITLE_MEETING));
		CleanupStack::PopAndDestroy(); // Resource string
		aItemArray->AppendL(*StringLoader::LoadLC(
				R_QTN_CALENDAR_TITLE_MEETING));
		CleanupStack::PopAndDestroy(); // Resource string
		aItemArray->AppendL(*StringLoader::LoadLC(
				R_QTN_CALENDAR_TITLE_MEMO));
		CleanupStack::PopAndDestroy(); // Resource string
		aItemArray->AppendL(*StringLoader::LoadLC(
				R_QTN_CALENDAR_TITLE_ANNIVERSARY));
		CleanupStack::PopAndDestroy(); // Resource string
		aItemArray->AppendL(*StringLoader::LoadLC(
				R_QTN_CALENDAR_TITLE_TODO));
		CleanupStack::PopAndDestroy(); // Resource string
		}
	    else if ( iType == EESMRNormalResponseQuery)
	        {
	        aPopupList->SetTitleL(*StringLoader::LoadLC(
	                R_QTN_MEET_REQ_RESPONSE_PLEASE_RESPOND));
	        CleanupStack::PopAndDestroy(); // Resource string
	        aItemArray->AppendL(*StringLoader::LoadLC(
	                R_QTN_MEET_REQ_RESPONSE_ACCEPT));
	        CleanupStack::PopAndDestroy(); // Resource string
	        aItemArray->AppendL(*StringLoader::LoadLC(
	                R_QTN_MEET_REQ_RESPONSE_TENTATIVE));
	        CleanupStack::PopAndDestroy(); // Resource string
	        aItemArray->AppendL(*StringLoader::LoadLC(
	                R_QTN_MEET_REQ_RESPONSE_DECLINE));
	        CleanupStack::PopAndDestroy(); // Resource string
	        }
	    else if ( iType == EESMRRemoveResponseQuery)
	        {
	        aPopupList->SetTitleL(*StringLoader::LoadLC(
	                R_QTN_MEET_REQ_RESPONSE_PLEASE_RESPOND));
	        CleanupStack::PopAndDestroy(); // Resource string
	        aItemArray->AppendL(*StringLoader::LoadLC(
	        		R_QTN_MEET_REQ_OPTIONS_REMOVE_FROM_CAL));
	        CleanupStack::PopAndDestroy(); // Resource string
	        }
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::MapSelectedIndexToReturnValue
// -----------------------------------------------------------------------------
//
TInt CESMRListQuery::MapSelectedIndexToReturnValue(TInt aIndex)
    {
    FUNC_LOG;
    TInt response (KErrCancel);

    if (iType == EESMREditBeforeSendQuery ||
        iType == EESMREditBeforeSendQueryWithSendOptionsOnly )
        {
        switch( aIndex )
            {
            case 0:
                {
                response = EESMRResponsePlain;
                break;
                }
            case 1:
                {
                response = EESMRResponseMessage;
                break;
                }
            case 2:
                {
                response = EESMRResponseDontSend;
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if (iType == EESMRSendUpdateToAllQuery )
        {
        switch( aIndex )
            {
            case 0:
                {
                response = EESMRSendToAllParticipants;
                break;
                }
            case 1:
                {
                response = EESMRSendToAddedOrRemovedParticipants;
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( iType == EESMRRecurrenceQuery )
        {
        return iRecurrenceArray[aIndex]->RecurrenceValue();
        }
    else if ( iType == EESMRRelativeAlarmTimeQuery )
        {
        return iAlarmArray[aIndex]->Id();
        }
    else if ( iType == EESMROpenThisOccurenceOrSeriesQuery ||
              iType == EESMRDeleteThisOccurenceOrSeriesQuery ||
              iType == EESMRForwardThisOccurenceOrSeriesQuery )
        {
        switch ( aIndex )
            {
            case 0:
                {
                response = EESMRThisOccurence;
                break;
                }
            case 1:
                {
                response = EESMRSeries;
                break;
                }
            default:
                break;
            }
        }
    else if ( iType == EESMRPriorityPopup )
        {
        switch ( aIndex )
            {
            case 0:
                {
                response = EFSCalenMRPriorityHigh;
                break;
                }
            case 1:
                {
                response = EFSCalenMRPriorityNormal;
                break;
                }
            case 2:
                {
                response = EFSCalenMRPriorityLow;
                break;
                }
            default:
                break;
            }
        }
    else if ( iType == EESMRTodoPriorityPopup )
        {
        switch ( aIndex )
            {
            case 0:
                {
                response = EFSCalenTodoPriorityHigh;
                break;
                }
            case 1:
                {
                response = EFSCalenTodoPriorityNormal;
                break;
                }
            case 2:
                {
                response = EFSCalenTodoPriorityLow;
                break;
                }
            default:
                break;
            }

        }
    else if ( iType == EESMRSynchronizationPopup )
        {
        switch ( aIndex )
            {
            case 0:
                {
                response = ESyncPublic;
                break;
                }
            case 1:
                {
                response = ESyncPrivate;
                break;
                }
            case 2:
                {
                response = ESyncNone;
                break;
                }
            default:
                break;
            }
        }
    else if ( iType == EESMRUnifiedEditorPopup )
        {
        switch ( aIndex )
            {
            case 0:
                {
                response = EMRTypeMeetingRequest;
                break;
                }
            case 1:
                {
                response = EMRTypeMeeting;
                break;
                }
            case 2:
                {
                response = EMRTypeMemo;
                break;
                }
            case 3:
				{
				response = EMRTypeAnniversary;
				break;
				}
            case 4:
				{
				response = EMRTypeToDo;
				break;
				}
            default:
                break;
            }
        }
        else if( iType == EESMRNormalResponseQuery )
        	{
            switch ( aIndex )
                {
                case 0:
                    {
                    response = EESMRCmdAcceptMR;
                    break;
                    }
                case 1:
                    {
                    response = EESMRCmdTentativeMR;
                    break;
                    }
                case 2:
                    {
                    response = EESMRCmdDeclineMR;
                    break;
                    }
                default:
                    break;
                }
        	}
        else if( iType == EESMRRemoveResponseQuery )
        	{
            switch ( aIndex )
                {
                case 0:
                    {
                    response = EESMRCmdRemoveFromCalendar;
                    break;
                    }
                default:
                    break;
                }
        	}
        
    return response;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteLD
// -----------------------------------------------------------------------------
//
void CESMRListQuery::LoadTextsFromResourceL()
    {
    FUNC_LOG;
    if ( iType == EESMRRelativeAlarmTimeQuery )
        {
        TResourceReader reader;
        iEnv->CreateResourceReaderLC( reader, R_ESMREDITOR_ALARMS );

        // Read alarm items to array
        iAlarmArray.Reset();
        TInt count = reader.ReadInt16();
        for ( TInt i(0); i < count; i++ )
            {
            CESMRAlarm* alarm = new (ELeave) CESMRAlarm;
            CleanupStack::PushL( alarm );
            alarm->ConstructFromResourceL( reader );
            alarm->LoadTextL( iEnv );
            CleanupStack::Pop( alarm );
            iAlarmArray.Append( alarm );
            }
        CleanupStack::PopAndDestroy(); // resource reader
        }
    else if ( iType == EESMRRecurrenceQuery )
        {
        TResourceReader reader;
        iEnv->CreateResourceReaderLC( reader, R_ESMREDITOR_RECURRENCE );

        // Read alarm items to array
        iRecurrenceArray.Reset();
        TInt count = reader.ReadInt16();
        for ( TInt i(0); i < count; i++ )
            {
            CESMRRecurrence* recurrence = new (ELeave) CESMRRecurrence;
            CleanupStack::PushL( recurrence );
            recurrence->ConstructFromResourceL( iEnv, reader );
            CleanupStack::Pop( recurrence );
            iRecurrenceArray.Append( recurrence );
            }
        CleanupStack::PopAndDestroy(); // resource reader
        }
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRListQuery::ExecuteLD()
    {
    FUNC_LOG;
    TInt response(KErrCancel);

    CAknSinglePopupMenuStyleListBox* list =
        new(ELeave)CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

    CAknPopupList* popupList = CAknPopupList::NewL(list,
            R_AVKON_SOFTKEYS_OK_CANCEL,
            AknPopupLayouts::EMenuGraphicHeadingWindow);
    CleanupStack::PushL(popupList);

    list->ConstructL(popupList, 0);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
                                                    CEikScrollBarFrame::EOn);

    CDesCArrayFlat* itemArray = new(ELeave)CDesCArrayFlat(2);
    CleanupStack::PushL(itemArray);

    LoadTextsFromResourceL();

    SetListQueryTextsL(itemArray, popupList);

    CleanupStack::Pop(itemArray);
    
    // Dynamic popup for Unified Editor does not include every item
    if ( iType == EESMRUnifiedEditorPopup )
    	{
    	// Looped backwards. When removing multiple indexes from array,
    	// incorrect items would otherwise be removed.
        for ( TInt index( iFilter.Count() - 1 ); index >= 0; --index )
            {
            itemArray->Delete( ( TInt )iFilter[index] );
            }
    	}
    
    list->Model()->SetItemTextArray(itemArray);
    list->Model()->SetOwnershipType(ELbmOwnsItemArray);

    // Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();

    if (popupOk)
        {
        if ( iType == EESMRUnifiedEditorPopup )
            {
            response = MapUnifiedEditorItemTextToIndexL( *list );
            ASSERT( KErrNotFound != response );
            }
        else
            {
            response = MapSelectedIndexToReturnValue( 
												list->CurrentItemIndex() );
            }
        }

    CleanupStack::Pop(popupList);
    CleanupStack::PopAndDestroy(list);
    delete this; // red key works correctly and no double destruction possible

    return response;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRListQuery::ExecuteLD( RArray<TPtrC>& aCalenNameList )
    {
    TInt response(KErrCancel);

    CAknSinglePopupMenuStyleListBox* list =
        new(ELeave)CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

    CAknPopupList* popupList = CAknPopupList::NewL(list,
            R_AVKON_SOFTKEYS_OK_CANCEL,
            AknPopupLayouts::EMenuGraphicHeadingWindow);
    CleanupStack::PushL(popupList);

    list->ConstructL(popupList, 0);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
                                                        CEikScrollBarFrame::EOn);

    CDesCArrayFlat* itemArray = new(ELeave)CDesCArrayFlat(2);
    CleanupStack::PushL(itemArray);
    
    popupList->SetTitleL(*StringLoader::LoadLC(
            R_QTN_CALENDAR_TASK_CALENDAR_TITLE));
    CleanupStack::PopAndDestroy(); // Resource string
            
    TInt count = aCalenNameList.Count();
    
    for ( TInt i(0); i < count; i++ )
        {
        itemArray->AppendL( aCalenNameList[i] );        
        }         
    
    CleanupStack::Pop(itemArray);
    list->Model()->SetItemTextArray(itemArray);
    list->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
    // Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();
    
    if (popupOk)
        {
        response = list->CurrentItemIndex();
        }
    
    CleanupStack::Pop(popupList);
    CleanupStack::PopAndDestroy(list);
    delete this; // red key works correctly and no double destruction possible
    
    return response;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRListQuery::ExecuteL(
        TESMRListQueryType aType,  RArray<TPtrC>& aCalenNameList )
    {
    CESMRListQuery* query = CESMRListQuery::NewL(aType);
    CleanupStack::PushL(query);
    TInt ret = query->ExecuteLD( aCalenNameList );
    CleanupStack::Pop(query);
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRListQuery::MapUnifiedEditorItemTextToIndexL
// -----------------------------------------------------------------------------
//
TInt CESMRListQuery::MapUnifiedEditorItemTextToIndexL(
        CAknSinglePopupMenuStyleListBox& aList )
    {
    FUNC_LOG;
    
    TInt index( KErrNotFound );
    
    TInt itemIndex( aList.CurrentItemIndex() );
    TPtrC selItemText(
            aList.Model()->ItemText( itemIndex ) );
    
    // Meeting Request
    HBufC* mrText      = StringLoader::LoadLC( R_QTN_MEET_REQ_TITLE_MEETING );
    HBufC* meetingText = StringLoader::LoadLC( R_QTN_CALENDAR_TITLE_MEETING );
    HBufC* memoText    = StringLoader::LoadLC( R_QTN_CALENDAR_TITLE_MEMO );
    HBufC* annivText   = StringLoader::LoadLC( R_QTN_CALENDAR_TITLE_ANNIVERSARY );
    HBufC* todoText    = StringLoader::LoadLC( R_QTN_CALENDAR_TITLE_TODO );
    
    if ( !selItemText.Compare( *mrText ) )
        {
        index = EESMREventTypeMeetingRequest;
        }
    else if ( !selItemText.Compare( *meetingText ) )
        {
        index = EESMREventTypeAppt;
        }    
    else if ( !selItemText.Compare( *memoText ) )
        {
        index = EESMREventTypeEEvent;
        }
    else if ( !selItemText.Compare( *annivText ) )
        {
        index = EESMREventTypeEAnniv;
        }
    else if ( !selItemText.Compare( *todoText ) )
        {
        index = EESMREventTypeETodo;
        }
    
    CleanupStack::PopAndDestroy( todoText );
    CleanupStack::PopAndDestroy( annivText ); 
    CleanupStack::PopAndDestroy( memoText ); 
    CleanupStack::PopAndDestroy( meetingText ); 
    CleanupStack::PopAndDestroy( mrText );     
    
    return index;
    }

//  End of File
