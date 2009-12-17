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
* Description: Definition of CMRImage class.
*
*/

#include "cmrimage.h"

#include <AknsUtils.h>
#include <esmrgui.mbg>
#include <data_caging_path_literals.hrh>

#include "esmrhelper.h"
#include "cesmrlayoutmgr.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRImage::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CMRImage* CMRImage::NewL( 
        const TAknsItemID& aIconId, TScaleMode aScaleMode )
    {
    CMRImage* self = new (ELeave) CMRImage( aScaleMode );
    CleanupStack::PushL( self );
    self->ConstructL( aIconId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRImage::~CMRImage
// ---------------------------------------------------------------------------
//
EXPORT_C  CMRImage* CMRImage::NewL(
             NMRBitmapManager::TMRBitmapId aBitmapId, TScaleMode aScaleMode )
    {
    CMRImage* self = new (ELeave) CMRImage( aScaleMode );
    CleanupStack::PushL( self );
    self->ConstructL( aBitmapId );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CMRImage::~CMRImage
// ---------------------------------------------------------------------------
//
EXPORT_C CMRImage::~CMRImage()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRImage::SizeChanged
// ---------------------------------------------------------------------------
//
void CMRImage::SizeChanged()
    {
    TSize sz = Rect().Size();
    const CFbsBitmap* bitmap = Bitmap();
    AknIconUtils::SetSize( const_cast< CFbsBitmap* >( bitmap ), sz, iScaleMode );
    }

// ---------------------------------------------------------------------------
// CMRImage::CMRImage
// ---------------------------------------------------------------------------
//
CMRImage::CMRImage( TScaleMode aScaleMode )
    : iScaleMode( aScaleMode )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMRImage::ConstructL
// ---------------------------------------------------------------------------
//
void CMRImage::ConstructL( const TAknsItemID& aIconId )
    {
    CreateIconL( aIconId );
    }

// ---------------------------------------------------------------------------
// CMRImage::ConstructL
// ---------------------------------------------------------------------------
//
void CMRImage::ConstructL( NMRBitmapManager::TMRBitmapId aBitmapId )
    {
    CreateIconL( aBitmapId );
    }

// ---------------------------------------------------------------------------
// CMRImage::CreateIconL
// ---------------------------------------------------------------------------
//
void CMRImage::CreateIconL( const TAknsItemID& aIconId )
    {
    TInt fileIndex(-1);
    TInt fileMaskIndex(-1);

    SetBitmapFallback( aIconId, fileIndex, fileMaskIndex );

    TFileName bitmapFilePath;
    ESMRHelper::LocateResourceFile(
            KESMRMifFile,
            KDC_APP_BITMAP_DIR,
            bitmapFilePath );

    CFbsBitmap* bitMap;
    CFbsBitmap* bitMapMask;
    
    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                            aIconId,
                            bitMap,
                            bitMapMask,
                            bitmapFilePath,
                            fileIndex,
                            fileMaskIndex
                            );

    if( bitMap && bitMapMask )
        {
        SetPictureOwnedExternally( EFalse );
        SetBrushStyle( CWindowGc::ENullBrush );
        SetPicture( bitMap, bitMapMask );
        }
   }

// ---------------------------------------------------------------------------
// CMRImage::CreateIconL
// ---------------------------------------------------------------------------
//
void CMRImage::CreateIconL( NMRBitmapManager::TMRBitmapId aBitmapId )
    {
    CFbsBitmap* bitMap( NULL );
    CFbsBitmap* bitMapMask( NULL );    
    TSize sz( Rect().Size() );
    User::LeaveIfError( 
                    NMRBitmapManager::GetSkinBasedBitmap( 
                            aBitmapId, 
                            bitMap, bitMapMask, sz ) ); 
    
    if( bitMap && bitMapMask )
        {
        SetPictureOwnedExternally( EFalse );
        SetBrushStyle( CWindowGc::ENullBrush );
        SetPicture( bitMap, bitMapMask );
        }
    }

// ---------------------------------------------------------------------------
// CMRImage::SetBitmapFallback
// ---------------------------------------------------------------------------
//
void CMRImage::SetBitmapFallback( 
        const TAknsItemID& aIconId,
        TInt& aFileIndex,
        TInt& aFileMaskIndex )
    {
    // <cmail> icons changed
    switch ( aIconId.iMinor )
        {
        case EAknsMinorGenericQgnFscalIndiPriorityHigh:
        case EAknsMinorGenericQgnFsIndiPriorityHigh:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_high;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_high_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiPriorityNormal:
        case EAknsMinorGenericQgnFsIndiPriorityNormal:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_normal;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_normal_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiPriorityLow:
        case EAknsMinorGenericQgnFsIndiPriorityLow:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_low;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_priority_low_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiAlarmDay:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_alarm_day;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_alarm_day_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiAlarmTime:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_alarm;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_alarm_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiSubject:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_subject;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_subject_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiRecurrence:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_recurrence;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_recurrence_mask;
            break;
            }
        case EAknsMinorGenericQgnMeetReqIndiAlarm:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_alarm;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_alarm_mask;
            break;
            }
        case EAknsMinorGenericQgnMeetReqIndiCheckboxMark:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_on;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_on_mask;
            break;
            }
        case EAknsMinorGenericQgnMeetReqIndiCheckboxUnmark:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_off;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_checkbox_off_mask;
            break;
            }
        case EAknsMinorGenericQgnMeetReqIndiClock:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_clock;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_clock_mask;
            break;
            }
        case EAknsMinorGenericQgnMeetReqIndiDateEnd:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_end;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_end_mask;
            break;
            }
        case EAknsMinorGenericQgnMeetReqIndiDateStart:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_start;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_date_start_mask;
            break;
            }
        case EAknsMinorGenericQgnMeetReqIndiLocation:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_location;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_meet_req_location_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiSynchronisation:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_synchronisation;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_synchronisation_mask;
            break;
            }
        case EAknsMinorGenericQgnFscalIndiOccasion:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_occasion;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_occasion_mask;
            break;
            }

        case EAknsMinorGenericQgnFsIndiCaleTrackingTentative:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_tentative;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_tentative_mask;
            break;
            }
        case EAknsMinorGenericQgnFsIndiCaleTrackingAccept:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_accept;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_accept_mask;
            break;
            }
        case EAknsMinorGenericQgnFsIndiCaleTrackingNone:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_none;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_none_mask;
            break;
            }
        case EAknsMinorGenericQgnFsIndiCaleTrackingReject:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_reject;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_calendar_tracking_reject_mask;
            break;
            }
        case EAknsMinorGenericQgnFseMailAttachment:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_attachment;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_attachment_mask;
            break;
            }
        case EAknsMinorGenericQgnFsGrafEmailCtrlbar:
            {
            aFileIndex = EMbmEsmrguiQgn_graf_cmail_email_ctrlbar;
            aFileMaskIndex = EMbmEsmrguiQgn_graf_cmail_email_ctrlbar_mask;
            break;
            }
        case EAknsMinorQgnPropCheckboxOn:
            aFileIndex = EMbmEsmrguiQgn_prop_checkbox_on;
            aFileMaskIndex = EMbmEsmrguiQgn_prop_checkbox_on_mask;
        	break;
        case EAknsMinorQgnPropCheckboxOff:
            aFileIndex = EMbmEsmrguiQgn_prop_checkbox_off;
            aFileMaskIndex = EMbmEsmrguiQgn_prop_checkbox_off_mask;
        	break;
        case EAknsMinorQgnFsListCenter:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_center;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_center_mask;
            break;
            }
        case EAknsMinorQgnFsListSideT:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_side_t;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_side_t_mask;
            break;
            }
        case EAknsMinorQgnFsListSideL:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_side_l;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_side_l_mask;
            break;
            }
        case EAknsMinorQgnFsListSideR:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_side_r;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_side_r_mask;
            break;
            }
        case EAknsMinorQgnFsListSideB:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_side_b;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_side_b_mask;
            break;
            }
        case EAknsMinorQgnFsListCornerTl:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_corner_tl;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tl_mask;
            break;
            }
        case EAknsMinorQgnFsListCornerTr:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_corner_tr;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_corner_tr_mask;
            break;
            }
        case EAknsMinorQgnFsListCornerBl:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_corner_bl;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_corner_bl_mask;
            break;
            }
        case EAknsMinorQgnFsListCornerBr:
            {
            aFileIndex = EMbmEsmrguiQsn_fr_list_corner_br;
            aFileMaskIndex = EMbmEsmrguiQsn_fr_list_corner_br_mask;
            break;
            }
        case EAknsMinorGenericQgnFsHscrActionArrowLeft:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_arrow_right;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_arrow_right_mask;
            break;
            }
        // <cmail> removed when icons changed to common ones
        /*    
        case EAknsMinorGenericQgnFsHscrActionArrowRight:
            {
            aFileIndex = EMbmEsmrguiQgn_indi_action_menu;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_action_menu_mask;
            break;
            }
            */
        // </cmail>    
        // <cmail> Commented to enable compilation in wk04
        /*case EAknsMinorGenericQgnFseActionMenuOpenInWebIcon:
            {
            aFileIndex = EMbmEsmrguiQgn_fse_action_menu_open_in_web_icon;
            aFileMaskIndex = EMbmEsmrguiQgn_fse_action_menu_open_in_web_icon;
            break;
            }
        case EAknsMinorGenericQgnFseActionMenuOpenInIntranetIcon:
            {
            aFileIndex = EMbmEsmrguiQgn_fse_action_menu_open_in_intranet_icon;
            aFileMaskIndex = EMbmEsmrguiQgn_fse_action_menu_open_in_intranet_icon_mask;
            break;
            }
        case EAknsMinorGenericQgnFseActionMenuBookmarkIcon:
            {
            aFileIndex = EMbmEsmrguiQgn_fse_action_menu_bookmark_icon;
            aFileMaskIndex = EMbmEsmrguiQgn_fse_action_menu_bookmark_icon_mask;
            break;
            }*/
        // </cmail>
        // <cmail> removed when icons changed to common ones
        /*          
        case EAknsMinorGenericQgnPropBlidWaypoint:
            {
            aFileIndex = EMbmEsmrguiQgn_fs_action_show_on_maps;
            aFileMaskIndex = EMbmEsmrguiQgn_fs_action_show_on_maps_mask;
            break;
            }
            */
        // </cmail>             
        default:
            {
            // <cmail> icons changed    
            aFileIndex = EMbmEsmrguiQgn_indi_cmail_arrow_right;
            aFileMaskIndex = EMbmEsmrguiQgn_indi_cmail_arrow_right_mask;
            break;
            // </cmail>              
            }
// </cmail>
        }
    }

// EOF
