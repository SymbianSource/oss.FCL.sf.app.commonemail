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
* Description: Bitmap manager for MRGUI usage.
*
*/

#include "nmrbitmapmanager.h"
#include "esmrhelper.h"

#include <aknsitemid.h>
#include <aknutils.h>
#include <aknsutils.h>
#include <esmrgui.mbg>
#include <data_caging_path_literals.hrh>

using namespace NMRBitmapManager;

namespace { // codescanner::unnamednamespace

// Icon mif file.
_LIT( KESMRMifFile, "esmrgui.mif");

#ifdef _DEBUG

// Definition for panic text
_LIT( KNMRBitmapManager, "NMRBitmapManager" );

/**
 * ES MR Entry panic codes
 */
enum TNMRBitmapManager
    {
    KUnhandledBitmapId = 1
    };

/**
 * Raises panic.
 * @param aPanic Panic code
 */
void Panic(TNMRBitmapManager aPanic)
    {
    User::Panic( KNMRBitmapManager, aPanic);
    }

#endif // _DEBUG

}

// ---------------------------------------------------------------------------
// NMRBitmapManager::GetSkinBasedBitmap
// ---------------------------------------------------------------------------
//
EXPORT_C TMRBitmapStruct NMRBitmapManager::GetBitmapStruct( TMRBitmapId aBitmapId )
    {
    TMRBitmapStruct bitmapStruct;
    
    switch ( aBitmapId )
        {
        case EMRBitmapRightClickArrow:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailActionArrowRight;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_action_arrow_right;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_action_arrow_right_mask;
            break;
            }
        case EMRBitmapTrackingAccept:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarTrackingAccept;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_accept;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_accept_mask;
            break;
            }
        case EMRBitmapTrackingReject:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarTrackingReject;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_reject;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_reject_mask;
            break;
            }
        case EMRBitmapTrackingTentative:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarTrackingTentative;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_tentative;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_tentative_mask;
            break;
            }
        case EMRBitmapTrackingNone:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarTrackingNone;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_none;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_none_mask;
            break;
            }
        case EMRBitmapPriorityHigh:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarPriorityHigh;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_high;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_high_mask;
            break;
            }
        case EMRBitmapPriorityLow:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarPriorityLow;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_low;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_low_mask;
            break;
            }
        case EMRBitmapPriorityNormal:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarPriorityNormal;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_normal;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_normal_mask;
            break;
            }
        case EMRBitmapSynchronization:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailSynchronisation;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_synchronisation;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_synchronisation_mask;
            break;
            }
        case EMRBitmapSubject:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarSubject;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_subject;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_subject_mask;
            break;
            }
        case EMRBitmapOccasion:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarOccasion;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_occasion;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_occasion_mask;
            break;
            }
        case EMRBitmapRecurrence:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailCalendarRecurrence;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_calendar_recurrence;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_recurrence_mask;
            break;
            }
        case EMRBitmapBrowser:
            {
            // TODO: not available
            // bitmapStruct.iItemId = KAknsIIDQgnMenuBrowserng;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_menu_browserng;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_menu_browserng_mask;
            
            // TODO: remove me asap
            bitmapStruct.iItemId = KAknsIIDQgnMenuIntranet;
            break;
            }
        case EMRBitmapIntranet:
            {
            bitmapStruct.iItemId = KAknsIIDQgnMenuIntranet;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_menu_intranet;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_menu_intranet_mask;
            break;
            }
        case EMRBitmapBookmark:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropCmailWmlBookmark;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_cmail_wml_bookmark;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_cmail_wml_bookmark_mask;
            break;
            }
        case EMRBitmapAlarm:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailMeetReqAlarm;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_alarm;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_alarm_mask;
            break;
            }
        case EMRBitmapAlarmClock:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCalendarClockAlarm;
            // TODO: use new icon
            //bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_calendar_clock_alarm;
            //bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_calendar_clock_alarm_mask;
            
            // TODO: remove me asap, not in platform atm.
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_alarm;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_alarm_mask;
            break;
            }
        case EMRBitmapCheckBoxOn:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailMeetReqCheckboxOn;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_on;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_on_mask;
            break;
            }
        case EMRBitmapCheckBoxOff:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailMeetReqCheckboxOff;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_off;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_off_mask;
            break;
            }
        case EMRBitmapClock:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailMeetReqClock;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_clock;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_clock_mask;
            break;
            }
        case EMRBitmapLocation:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailMeetReqLocation;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_location;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_location_mask;
            break;
            }
        case EMRBitmapLocationWaypoint:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCalLocatEventAdd;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cal_locat_event_add;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cal_locat_event_add_mask;
            break;
            }
        case EMRBitmapDateStart:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailMeetReqDateStart;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_start;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_start_mask;
            break;
            }
        case EMRBitmapDateEnd:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailMeetReqDateEnd;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_end;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_end_mask;
            break;
            }
        case EMRBitmapAttachment:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailAttachment;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_attachment;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_attachment_mask;
            break;
            }
        case EMRBitmapCtrlBar:
            {
            bitmapStruct.iItemId = KAknsIIDQgnGrafCmailEmailCtrlbar;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_graf_cmail_email_ctrlbar;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_graf_cmail_email_ctrlbar_mask;
            break;
            }
        case EMRBitmapSearchContacts:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropSearchContacts;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_search_contacts;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_search_contacts_mask;
            break;
            }
        case EMRBitmapCalendarSelection:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropNrtypDate;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_nrtyp_date;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_nrtyp_date_mask;
            break;
            }
        case EMRBitmapEventTypeMeeting:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCdrMeeting;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cdr_meeting;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cdr_meeting_mask;
            break;
            }
        case EMRBitmapEventTypeMR:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCdrInvitation;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cdr_invitation;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cdr_invitation_mask;
            break;
            }
        case EMRBitmapEventTypeMemo:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCdrReminder;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cdr_reminder;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cdr_reminder_mask;
            break;
            }
        case EMRBitmapEventTypeAnniversary:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCdrBirthday;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cdr_birthday;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cdr_birthday_mask;
            break;
            }
        case EMRBitmapEventTypeTodo:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCdrTodo;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cdr_todo;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cdr_todo_mask;
            break;
            }
        case EMRBitmapListCenter:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListCenter;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_center;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_center_mask;
            break;
            }
        case EMRBitmapListTop:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListSideT;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_t;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_t_mask;
            break;
            }
        case EMRBitmapListBottom:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListSideB;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_b;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_b_mask;
            break;
            }
        case EMRBitmapListLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListSideL;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_l;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_l_mask;
            break;
            }
        case EMRBitmapListRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListSideR;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_r;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_r_mask;
            break;
            }
        case EMRBitmapListTopLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListCornerTl;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_tl;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tl_mask;
            break;
            }
        case EMRBitmapListTopRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListCornerTr;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_tr;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tr_mask;
            break;
            }
        case EMRBitmapListBottomLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListCornerBl;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_bl;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_bl_mask;
            break;
            }
        case EMRBitmapListBottomRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrListCornerBr;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_br;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_br_mask;
            break;
            }
            
        case EMRBitmapMailAttachment:
            {
            bitmapStruct.iItemId = KAknsIIDQgnFseMailAttachment;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_attachment;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_attachment_mask;
            break;
            }
        case EMRBitmapOpenAttachmentView:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropCmailActionAttachmentListOptionsViewAll;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_list_options_view_all;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_list_options_view_all_mask;
            break;
            }
        case EMRBitmapOpenAttachment:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropCmailActionAttachmentOpen;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_open;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_open_mask;
            break;
            }
        case EMRBitmapDownloadAttachment:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropCmailActionAttachmentDownload;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_download;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_download_mask;
            break;
            }
        case EMRBitmapDownloadAllAttachments:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropCmailActionAttachmentDownloadAll;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_download_all;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_download_all_mask;
            break;
            }
        case EMRBitmapSaveAttachment:
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropCmailActionAttachmentSave;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_save;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_save_mask;
            break;
            }
        case EMRBitmapSaveAllAttachments:            
            {
            bitmapStruct.iItemId = KAknsIIDQgnPropCmailActionAttachmentSaveAll;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_save_all;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_prop_cmail_action_attachment_save_all_mask;
            break;
            }
    
        // TODO: Needs correct fallbacks
        case EMRBitmapInputCenter:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputCenter;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_center;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_center_mask;
            break;
            }
        case EMRBitmapInputTop:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputSideT;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_t;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_t_mask;
            break;
            }
        case EMRBitmapInputBottom:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputSideB;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_b;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_b_mask;
            break;
            }
        case EMRBitmapInputLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputSideL;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_l;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_l_mask;
            break;
            }
        case EMRBitmapInputRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputSideR;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_r;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_r_mask;
            break;
            }
        case EMRBitmapInputTopLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputCornerTl;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_tl;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tl_mask;
            break;
            }
        case EMRBitmapInputTopRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputCornerTr;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_tr;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tr_mask;
            break;
            }
        case EMRBitmapInputBottomLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputCornerBl;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_bl;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_bl_mask;
            break;
            }
        case EMRBitmapInputBottomRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrInputCornerBr;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_br;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_br_mask;
            break;
            }
        
        // TODO: Needs correct fallbacks
        case EMRBitmapSetOptCenter:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptCenter;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_center;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_center_mask;
            break;
            }
        case EMRBitmapSetOptTop:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptSideT;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_t;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_t_mask;
            break;
            }
        case EMRBitmapSetOptBottom:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptSideB;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_b;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_b_mask;
            break;
            }
        case EMRBitmapSetOptLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptSideL;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_l;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_l_mask;
            break;
            }
        case EMRBitmapSetOptRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptSideR;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_side_r;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_side_r_mask;
            break;
            }
        case EMRBitmapSetOptTopLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptCornerTl;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_tl;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tl_mask;
            break;
            }
        case EMRBitmapSetOptTopRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptCornerTr;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_tr;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tr_mask;
            break;
            }
        case EMRBitmapSetOptBottomLeft:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptCornerBl;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_bl;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_bl_mask;
            break;
            }
        case EMRBitmapSetOptBottomRight:
            {
            bitmapStruct.iItemId = KAknsIIDQsnFrSetOptCornerBr;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQsn_fr_list_corner_br;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQsn_fr_list_corner_br_mask;
            break;
            }          
        case EMRBitmapMailPre:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailArrowPrevious;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_arrow_previous;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_arrow_previous_mask;
            break;
            }            
        case EMRBitmapMailNext:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailArrowNext;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_arrow_next;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_arrow_next_mask;
            break;            
            }
        case EMRBitmapAttachmentTypeImage:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiMceTbImage;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_mce_tb_image;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_mce_tb_image_mask;
            break;            
            }
        case EMRBitmapAttachmentTypeVideo:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiMceTbVideo;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_mce_tb_video;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_mce_tb_video_mask;
            break;            
            }
        case EMRBitmapAttachmentTypeMusic:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiMceTbAudio;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_mce_tb_audio;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_mce_tb_audio_mask;
            break;            
            }
            
        case EMRBitmapAttachmentTypeNote:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiTbNote;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_tb_note;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_tb_note_mask;
            break;            
            }
            
        case EMRBitmapAttachmentTypePresentations:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiMceTbPres;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_mce_tb_pres;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_mce_tb_pres_mask;
            break;            
            }
        case EMRBitmapAttachmentTypeOther:
            {
            bitmapStruct.iItemId = KAknsIIDQgnIndiMceTbOther;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_mce_tb_other;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_mce_tb_other_mask;
            break;            
            }
        case EMRBitmapLockField:
        	{
        	bitmapStruct.iItemId = KAknsIIDQgnIndiSettProtectedAdd;
        	bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_sett_protected_add;
        	bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_sett_protected_add_mask;
        	break;
        	}
        default:
            {
            // We shouldn't end up here.
            __ASSERT_DEBUG( EFalse, Panic( KUnhandledBitmapId ) );
            bitmapStruct.iItemId = KAknsIIDQgnIndiCmailActionArrowRight;
            bitmapStruct.iFallbackIndex = EMbmEsmrguiQgn_indi_cmail_action_arrow_right;
            bitmapStruct.iFallbackMaskIndex = EMbmEsmrguiQgn_indi_cmail_action_arrow_right_mask;
            break;
            }
        }
    
    return bitmapStruct;
    }

// ---------------------------------------------------------------------------
// NMRBitmapManager::GetSkinBasedBitmap
// ---------------------------------------------------------------------------
//
EXPORT_C TInt NMRBitmapManager::GetSkinBasedBitmap(
        TMRBitmapId aBitmapId,
        CFbsBitmap*& aBitmap,
        CFbsBitmap*& aMask,
        TSize aSize )
    {
    TInt retValue = KErrNone;
    delete aBitmap;
    aBitmap = NULL;
    delete aMask;
    aMask = NULL;

    TMRBitmapStruct bitmapStruct = GetBitmapStruct( aBitmapId );

    TFileName bitmapFilePath;
    ESMRHelper::LocateResourceFile(
            KESMRMifFile,
            KDC_APP_BITMAP_DIR,
            bitmapFilePath );

    TRAPD( error, AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                                         bitmapStruct.iItemId,
                                         aBitmap,
                                         aMask,
                                         bitmapFilePath,
                                         bitmapStruct.iFallbackIndex,
                                         bitmapStruct.iFallbackMaskIndex ) );
    
    if ( error != KErrNone )
        {
        return error;
        }
    
    AknIconUtils::SetSize( aBitmap, aSize, EAspectRatioNotPreserved );
    
    if( !aBitmap || !aMask )
        {
        retValue = KErrNotFound;
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// NMRBitmapManager::GetSkinBasedBitmapLC
// ---------------------------------------------------------------------------
//
EXPORT_C void NMRBitmapManager::GetSkinBasedBitmapLC(
        TMRBitmapId aBitmapId,
        CFbsBitmap*& aBitmap,
        CFbsBitmap*& aMask,
        TSize aSize )
    {
    delete aBitmap;
    aBitmap = NULL;
    delete aMask;
    aMask = NULL;

    TMRBitmapStruct bitmapStruct = GetBitmapStruct( aBitmapId );

    TFileName bitmapFilePath;
    ESMRHelper::LocateResourceFile(
            KESMRMifFile,
            KDC_APP_BITMAP_DIR,
            bitmapFilePath );

    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
                             bitmapStruct.iItemId,
                             aBitmap,
                             aMask,
                             bitmapFilePath,
                             bitmapStruct.iFallbackIndex,
                             bitmapStruct.iFallbackMaskIndex );
    
    AknIconUtils::SetSize( aBitmap, aSize, EAspectRatioNotPreserved );
    }

// ---------------------------------------------------------------------------
// NMRBitmapManager::GetMaskIconBitmap
// ---------------------------------------------------------------------------
//
EXPORT_C TInt NMRBitmapManager::GetMaskIconBitmap( TMRBitmapId aBitmapId,
                                  CFbsBitmap*& aBitmap,
                                  CFbsBitmap*& aMask,
                                  TInt aColorIndex,
                                  TSize aSize )
	{
    TInt retValue = KErrNone;
    delete aBitmap;
    aBitmap = NULL;
    delete aMask;
    aMask = NULL;

    TMRBitmapStruct bitmapStruct = GetBitmapStruct( aBitmapId );

    TFileName bitmapFilePath;
    ESMRHelper::LocateResourceFile(
            KESMRMifFile,
            KDC_APP_BITMAP_DIR,
            bitmapFilePath );
    
    TRgb defaultColour( KRgbBlack );    

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor( skin, defaultColour,
               KAknsIIDQsnIconColors, aColorIndex );
    
    TRAPD(error, AknsUtils::CreateColorIconL( skin, 
											  bitmapStruct.iItemId,
											  KAknsIIDQsnIconColors, 
											  aColorIndex, 
											  aBitmap, 
											  aMask,
											  bitmapFilePath, 
											  bitmapStruct.iFallbackIndex, 
											  bitmapStruct.iFallbackMaskIndex,
											  defaultColour ) );
    
    if ( error != KErrNone )
    	{
    	return error;
    	}

    AknIconUtils::SetSize( aBitmap, aSize, EAspectRatioNotPreserved );

    if( !aBitmap || !aMask )
    	{
    	retValue = KErrNotFound;
    	}
    return retValue;    
	}

// ---------------------------------------------------------------------------
// NMRBitmapManager::GetMaskIconBitmap
// ---------------------------------------------------------------------------
//
EXPORT_C void NMRBitmapManager::GetMaskIconBitmapLC( TMRBitmapId aBitmapId,
                                  CFbsBitmap*& aBitmap,
                                  CFbsBitmap*& aMask,
                                  TInt aColorIndex,
                                  TSize aSize )
	{	
	delete aBitmap;
	aBitmap = NULL;
	delete aMask;
	aMask = NULL;

	TMRBitmapStruct bitmapStruct = GetBitmapStruct( aBitmapId );

	TFileName bitmapFilePath;
	ESMRHelper::LocateResourceFile(
			KESMRMifFile,
			KDC_APP_BITMAP_DIR,
			bitmapFilePath );

	TRgb defaultColour( KRgbBlack );

	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsUtils::GetCachedColor( skin, defaultColour,
			KAknsIIDQsnIconColors, aColorIndex );

	AknsUtils::CreateColorIconLC( skin, 
								  bitmapStruct.iItemId,
								  KAknsIIDQsnIconColors, 
								  aColorIndex, 
								  aBitmap, 
								  aMask,
								  bitmapFilePath, 
								  bitmapStruct.iFallbackIndex, 
								  bitmapStruct.iFallbackMaskIndex,
								  defaultColour );
	
	AknIconUtils::SetSize( aBitmap, aSize, EAspectRatioNotPreserved );
	}
