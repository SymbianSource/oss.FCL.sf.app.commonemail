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
* Description:  ESMR checkbox component for es mr gui (editor)
*
*/


#include "emailtrace.h"
#include "cesmrlayoutmgr.h"

#include <e32base.h>
#include <e32cmn.h>
#include <gdi.h>
#include <fbs.h>
#include <coemain.h>
#include <eikenv.h>
#include <gulfont.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <AknFontSpecification.h>
#include <AknLayoutFont.h>
#include <AvkonInternalCRKeys.h>
#include <centralrepository.h>
#include <AknsConstants.h>
#include <aknsconstants.hrh>
#include <AknsUtils.h>
#include <aknappui.h>
#include <data_caging_path_literals.hrh>
#include <esmrgui.mbg>

#include "mesmrfontsizeobserver.h"
#include "esmrhelper.h"

// Unnamed namespace for local definitions
namespace {

// icon margins
const TInt KIconTopMarginSmallFont = 0;
const TInt KIconTopMarginNormalFont = 1;
const TInt KIconTopMarginLargeFont = 3;

/** Enumeration for font sizes */
enum TESMRFontSize
    {
    EESMRFontSmall = 21,
    EESMRFontNormal = 23,
    EESMRFontLarge = 26
    };

/** Enumeration for text top margin*/
enum TESMRTextTopMargin
    {
    EESMRTextTopMarginSmall = 2,
    EESMRTextTopMarginNormal = 2,
    EESMRTextTopMarginLarge = 3
    };

// Field heights
const TInt KRegularFieldHeightSmallFont(20);
const TInt KRegularFieldHeightNormalFont(22);
const TInt KRegularFieldHeightLargeFont(25);
const TInt KTitlePaneHeight(26);

// Basic margins
const TInt KBasicTopMargin( 3 );
const TInt KBasicBottomMargin( 3 );
const TInt KBasicLeftMargin( 9 );
const TInt KBasicTightMargin( 3 );

// Response area related
#define KResponseAreaAnswerTextSize TSize( 180, 25 )
#define KResponseAreaTopicTextSizePortrait TSize( 212, 25 )
#define KResponseAreaTopicTextSizeLandscape TSize( 280, 25 )
#define KResponseAreaAnswerTextPositionSmallFont TPoint( 27, 3 )
#define KResponseAreaAnswerTextPositionNormalFont TPoint( 27, 3 )
#define KResponseAreaAnswerTextPositionLargeFont TPoint( 27, 5 )
#define KResponseAreaAnswerIconPositionSmallFont TPoint( 8, 3 )
#define KResponseAreaAnswerIconPositionNormalFont TPoint( 8, 5 )
#define KResponseAreaAnswerIconPositionLargeFont TPoint( 8, 7 )
#define KResponseAreaAnswerTextPositionSmallFontMirrored TPoint( 46, 3 )
#define KResponseAreaAnswerTextPositionNormalFontMirrored TPoint( 46, 3 )
#define KResponseAreaAnswerTextPositionLargeFontMirrored TPoint( 46, 5 )

#define KResponseAreaAnswerTextPositionSmallFontMirroredLandscape TPoint( 127, 3 )
#define KResponseAreaAnswerTextPositionNormalFontMirroredLandscape TPoint( 127, 3 )
#define KResponseAreaAnswerTextPositionLargeFontMirroredLandscape TPoint( 127, 5 )
#define KResponseAreaAnswerIconPositionSmallFontMirroredLandscape TPoint( 280, 3 )
#define KResponseAreaAnswerIconPositionNormalFontMirroredLandscape TPoint( 280, 5 )
#define KResponseAreaAnswerIconPositionLargeFontMirroredLandscape TPoint( 280, 7 )


#define KResponseAreaAnswerIconPositionSmallFontMirrored TPoint( 194, 3 )
#define KResponseAreaAnswerIconPositionNormalFontMirrored TPoint( 194, 5 )
#define KResponseAreaAnswerIconPositionLargeFontMirrored TPoint( 194, 7 )

#define KConflictPopupPositionPortrait TPoint(30,150)
#define KConflictPopupPositionLandscape TPoint(30,100)

const TInt KResponseAreaAnserItemHeightSmallFont(20);
const TInt KResponseAreaAnserItemHeightNormalFont(22);
const TInt KResponseAreaAnserItemHeightLargeFont(26);

// Organizer field related
const TInt KOrganizerTextWidth(150);

// Color id:s KViewerAreaTextColorID
const TInt KViewerAreaTextColorID( EAknsCIQsnTextColorsCG6 );
const TInt KViewerAreaHighlightedTextColorID( EAknsCIQsnHighlightColorsCG2 );
const TInt KNormalTextColorID( EAknsCIQsnTextColorsCG26 );
const TInt KNormalHighLightedTextColorID( EAknsCIQsnTextColorsCG24 );
const TInt KTitleAreaTextColorID( EAknsCIQsnTextColorsCG2 );
const TInt KNormalTextBgColorID( EAknsCIFsOtherColorsCG12 );
const TInt KListAreaTextColorID( EAknsCIQsnTextColorsCG6 );
const TInt KListAreaBgColorID( EAknsCIFsOtherColorsCG13 );
const TInt KFieldBorderColorID( EAknsCIFsOtherColorsCG14 );


#define KDefaultListAreaBgColor TRgb( 0xffffff );
#define KDefaultBorderColor TRgb( 0x000000 );
#define KDefaultTextColor TRgb( 0x000000 );
#define KDefaultNormalTextBgColor TRgb( 0xe8e8e8 );

// Text side margin in basic field (editor & viewer )
const TInt KTextSideMargin( 5 );

TESMRFontSize MapFontSettingToFontSize(
        TInt& aCurrentFontSetting )
    {
    TESMRFontSize fontSize( EESMRFontNormal );

    if ( EAknUiZoomSmall == aCurrentFontSetting )
        {
        fontSize = EESMRFontSmall;
        }
    else if ( EAknUiZoomLarge == aCurrentFontSetting )
        {
        fontSize = EESMRFontLarge;
        }
    return fontSize;
    }

TBool FindFieldSizeInfoById(
        const CESMRLayoutManager::SFieldSize& aLhs,
        const CESMRLayoutManager::SFieldSize& aRhs )
    {
    if ( aLhs.iId == aRhs.iId )
        {
        return ETrue;
        }
    return EFalse;
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRLayoutManager::CESMRLayoutManager
// ---------------------------------------------------------------------------
//
inline CESMRLayoutManager::CESMRLayoutManager()
:   iFontSettingsValue( EAknUiZoomNormal ),
    iFontSettingsChanged( EFalse )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::~CESMRLayoutManager
// ---------------------------------------------------------------------------
//
CESMRLayoutManager::~CESMRLayoutManager()
    {
    FUNC_LOG;
    iSize.Reset();
    iSize.Close();
    delete iFont;
    delete iMfneFont;
    if ( iNotifyHandler )
        {
        iNotifyHandler->StopListening();
        }
    delete iNotifyHandler;
    delete iFontSettings;// session can be deleted only after iNotifyHandler
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::NewL
// ---------------------------------------------------------------------------
//
CESMRLayoutManager* CESMRLayoutManager::NewL()
    {
    FUNC_LOG;
    CESMRLayoutManager* self = new (ELeave) CESMRLayoutManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRLayoutManager::ConstructL()
    {
    FUNC_LOG;
    // args: field id, height(smallfont), 
    //       height(normalfont), height(largefont),
    //       width(landscape), width(portrait)

    AddSize( EESMRFieldAttendeeLabel,               20, 22, 26, 299, 218 );
    AddSize( EESMRFieldAttendee,                    20, 22, 26, 299, 218 );
    AddSize( EESMRFieldOptAttendee,                 20, 22, 26, 299, 218 );
    AddSize( EESMRFieldMeetingTime,                 22, 24, 26, 299, 218 );
    AddSize( EESMRFieldStartDate,                   22, 24, 26, 299, 218 );
    AddSize( EESMRFieldStopDate,                    22, 24, 26, 299, 218 );
    AddSize( EESMRFieldLocation,                    19, 21, 25, 299, 218 );
    AddSize( EESMRFieldAlarm,                       22, 24, 26, 299, 218 );
    AddSize( EESMRFieldAlarmDate,                   22, 24, 26, 299, 218 );
    AddSize( EESMRFieldSubject,                     19, 21, 25, 299, 218 );
    AddSize( EESMRFieldOccasion,                    19, 21, 25, 299, 218 );
    AddSize( EESMRFieldDescription,                 60, 63, 60, 299, 218 ); 
    AddSize( EESMRFieldRecurrence,                  22, 24, 28, 299, 218 );
    AddSize( EESMRFieldRecurrenceDate,              44, 49, 53, 299, 218 ); 
    AddSize( EESMRFieldPriority,                    24, 26, 26, 299, 218 );
    AddSize( EESMRFieldAllDayEvent,                 20, 22, 26, 299, 218 );
    AddSize( EESMRFieldAlarmOnOff,                  20, 22, 26, 299, 218 );
    AddSize( EESMRFieldAlarmTime,                   22, 24, 26, 299, 218 );
    AddSize( EESMRFieldSync,                        22, 24, 28, 299, 218 );
    AddSize( EESMRFieldViewerBasic,                 20, 22, 26, 299, 218 );
    AddSize( EESMRFieldOrganizer,                   40, 44, 52, 299, 218 );
    AddSize( EESMRFieldResponseArea,                93, 102, 120, 299,218 );
    AddSize( EESMRFieldResponseAreaWithOneItem,     48, 57, 65, 299, 218 );
    AddSize( EESMRFieldResponseReadyArea,           24, 26, 28, 299, 218 );
    AddSize( EESMRFieldDetailedSubject,             20, 22, 25, 299, 218 );
    AddSize( EESMRFieldAttachments,                 22, 24, 28, 299, 218 );
    AddSize( EESMRFieldReqTrack,                    20, 22, 26, 299, 218 );
    AddSize( EESMRFieldOptTrack,                    20, 22, 26, 299, 218 );
    AddSize( EESMRTrackStatus,                      20, 22, 26, 299, 218 );

    iFontSettings = CRepository::NewL( KCRUidAvkon );
    iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iFontSettings,
            CCenRepNotifyHandler::EIntKey, KAknGlobalUiZoom );
    iNotifyHandler->StartListeningL();

    // Check the current font size
    TAknUiZoom zoom;
    zoom = static_cast<CAknAppUi*>(
            CEikonEnv::Static()->AppUi()// codescanner::eikonenvstatic
                )->LocalUiZoom();

    if ( zoom == EAknUiZoomAutomatic )
        {
        CAknEnv::Static()->GetCurrentGlobalUiZoom( zoom );
        }

    iFontSettingsValue = zoom;

    // Check the font settings, note that id is not used yet (can be anything)
    Font(CCoeEnv::Static(), EESMRFieldViewerBasic );
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::CurrentFontSizeL
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::CurrentFontZoom()
    {
    FUNC_LOG;
    return iFontSettingsValue;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::FieldSize
// ---------------------------------------------------------------------------
//
TSize CESMRLayoutManager::FieldSize( TESMREntryFieldId aId )
    {
    FUNC_LOG;
    TSize size( 0, 0 );

    CESMRLayoutManager::SFieldSize fieldToFound;
    fieldToFound.iId = aId;

    TInt pos = iSize.Find( fieldToFound, FindFieldSizeInfoById );
    if ( KErrNotFound != pos )
        {
        TInt width = Layout_Meta_Data::IsLandscapeOrientation() ?
                iSize[pos].iWidthLandscape : iSize[pos].iWidthPortrait;

        TInt height;

        // Field height depends on font height
        // Field height when small font is in use
        if( iFontSettingsValue == EAknUiZoomSmall ||
            iFontSettingsValue == EAknUiZoomVerySmall )
            {
            height = iSize[pos].iFontSmallHeight;
            }
        // Field height when large font is in use
        else if( iFontSettingsValue == EAknUiZoomLarge ||
            iFontSettingsValue == EAknUiZoomVeryLarge )
            {
            height = iSize[pos].iFontLargeHeight;
            }
        // Field height if font height is not specified or it's normal
        else
            {
            height = iSize[pos].iFontNormalHeight;
            }

        size.SetSize( width, height );
        }

    return size;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::Font
// ---------------------------------------------------------------------------
//
const CFont* CESMRLayoutManager::Font(
        CCoeEnv* /*aEnv*/,
        TESMREntryFieldId aId )
    {
    FUNC_LOG;
    if ( !iFont || !iMfneFont )
        {
        TAknFontSpecification aknFontSpec( EAknLogicalFontTitleFont );
        aknFontSpec.SetUnits( TAknFontSpecification::EPixels );

        TESMRFontSize fontSize = MapFontSettingToFontSize( iFontSettingsValue );

        // text pane height for edwins (text fields)
        aknFontSpec.SetTextPaneHeight( fontSize );

        // There is nothing mrui can do if this method leaves
        delete iFont;
        iFont = NULL;
        TRAP_IGNORE( iFont = AknLayoutUtils::
                CreateLayoutFontFromSpecificationL( aknFontSpec ) );

        if ( fontSize == EESMRFontLarge )
            {
            // If large font is used:
            aknFontSpec.SetTextPaneHeight( fontSize - 2);
            }
        else
            {
            aknFontSpec.SetTextPaneHeight( fontSize - 1);
            }


        // separate font for CEikMfne based controls:
        delete iMfneFont;
        iMfneFont = NULL;

        TRAP_IGNORE( iMfneFont = AknLayoutUtils::
                CreateLayoutFontFromSpecificationL( aknFontSpec ) );

        }
    if ( aId == EESMRFieldStartDate ||
         aId == EESMRFieldStopDate ||
         aId == EESMRFieldMeetingTime ||
         aId == EESMRFieldAlarmDate ||
         aId == EESMRFieldRecurrenceDate ||
         aId == EESMRFieldAlarmTime ||
         aId == EESMRFieldDate )
        {
        return static_cast<const CFont*>(iMfneFont);
        }
    else
        {
        return static_cast<const CFont*>(iFont);
        }
    }
// ---------------------------------------------------------------------------
// CESMRLayoutManager::Margins
// ---------------------------------------------------------------------------
//
TMargins CESMRLayoutManager::Margins(
        TMarginsId /*aId*/ )
    {
    FUNC_LOG;
    TMargins m;
    m.iTop = KBasicTopMargin;
    m.iBottom = KBasicBottomMargin;
    m.iLeft = KBasicLeftMargin;
    m.iRight = KBasicTightMargin;
    return m;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::IconTopMargin
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::IconTopMargin()
    {
    FUNC_LOG;
    TInt retValue = 0;

    switch ( iFontSettingsValue )
        {
        case EAknUiZoomSmall: //Fallthrough
        case EAknUiZoomVerySmall:
            {
            retValue = KIconTopMarginSmallFont;
            break;
            }
        case EAknUiZoomNormal:
            {
            retValue = KIconTopMarginNormalFont;
            break;
            }
        case EAknUiZoomLarge: //Fallthrough
        case EAknUiZoomVeryLarge:
            {
            retValue = KIconTopMarginLargeFont;
            break;
            }
        default:
            {
            retValue = KIconTopMarginNormalFont;
            break;
            }
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::TextTopMargin
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::TextTopMargin()
    {
    FUNC_LOG;
    TInt retValue = 0;

    switch ( iFontSettingsValue )
        {
        case EAknUiZoomSmall: //Fallthrough
        case EAknUiZoomVerySmall:
            {
            retValue = EESMRTextTopMarginSmall;
            break;
            }
        case EAknUiZoomNormal:
            {
            retValue = EESMRTextTopMarginNormal;
            break;
            }
        case EAknUiZoomLarge: //Fallthrough
        case EAknUiZoomVeryLarge:
            {
            retValue = EESMRTextTopMarginLarge;
            break;
            }
        default:
            {
            retValue = KIconTopMarginNormalFont;
            break;
            }
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::TextSideMargin
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::TextSideMargin()
    {
    FUNC_LOG;
    return KTextSideMargin;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::TitlePaneHeight
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::TitlePaneHeight()
    {
    FUNC_LOG;
    return KTitlePaneHeight;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::AddSize
// ---------------------------------------------------------------------------
//
void CESMRLayoutManager::AddSize(
        TESMREntryFieldId aId,
        TInt aFontSmallHeight,
        TInt aFontNormalHeight,
        TInt aFontLargeHeight,
        TInt aWidthLandscape,
        TInt aWidthPortrait )
    {
    FUNC_LOG;
    SFieldSize size;
    size.iId = aId;
    size.iFontSmallHeight = aFontSmallHeight;
    size.iFontNormalHeight = aFontNormalHeight;
    size.iFontLargeHeight = aFontLargeHeight;
    size.iWidthLandscape = aWidthLandscape;
    size.iWidthPortrait = aWidthPortrait;
    iSize.Append( size );
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::IsMirrored
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRLayoutManager::IsMirrored()
    {
    FUNC_LOG;
    return Layout_Meta_Data::IsMirrored();
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::SetBitmapFallback
// If a new scalable graphic
// is added to the mif file, it should be mapped to fallback here.
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRLayoutManager::SetBitmapFallback(const TAknsItemID& aIconId,
                                                TInt& aFileIndex,
                                                TInt& aFileMaskIndex)
    {
// <cmail> icons changed
    FUNC_LOG;
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



// ---------------------------------------------------------------------------
// CESMRLayoutManager::ResponseAreaAnswerTextSize
// ---------------------------------------------------------------------------
//
TSize CESMRLayoutManager::ResponseAreaAnswerTextSize()
    {
    FUNC_LOG;
    return KResponseAreaAnswerTextSize;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ResponseAreaTopicTextSize
// ---------------------------------------------------------------------------
//
TSize CESMRLayoutManager::ResponseAreaTopicTextSize()
    {
    FUNC_LOG;
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        return KResponseAreaTopicTextSizeLandscape;
        }
    else
        {
        return KResponseAreaTopicTextSizePortrait;
        }
    }


// ---------------------------------------------------------------------------
// CESMRLayoutManager::ResponseAreaAnswerTextPosition
// ---------------------------------------------------------------------------
//
TPoint CESMRLayoutManager::ResponseAreaAnswerTextPosition()
    {
    FUNC_LOG;
    TPoint retValue;
    TBool mirrored = IsMirrored();

    switch ( iFontSettingsValue )
        {
        case EAknUiZoomSmall: //Fallthrough
        case EAknUiZoomVerySmall:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerTextPositionSmallFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
                     KResponseAreaAnswerTextPositionSmallFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
                     KResponseAreaAnswerTextPositionSmallFontMirrored;
                    }
                }
            break;
            }
        case EAknUiZoomNormal:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerTextPositionNormalFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
                     KResponseAreaAnswerTextPositionNormalFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
						KResponseAreaAnswerTextPositionNormalFontMirrored;
                    }
                }
            break;
            }
        case EAknUiZoomLarge: //Fallthrough
        case EAknUiZoomVeryLarge:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerTextPositionLargeFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
					 KResponseAreaAnswerTextPositionLargeFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
						KResponseAreaAnswerTextPositionLargeFontMirrored;
                    }
                }
            break;
            }
        default:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerTextPositionLargeFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
                     KResponseAreaAnswerTextPositionNormalFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
						KResponseAreaAnswerTextPositionNormalFontMirrored;
                    }
                }
            break;
            }
        }
    return retValue;
    }


// ---------------------------------------------------------------------------
// CESMRLayoutManager::ResponseAreaAnswerIconPosition
// ---------------------------------------------------------------------------
//
TPoint CESMRLayoutManager::ResponseAreaAnswerIconPosition()
    {
    FUNC_LOG;
    TPoint retValue;
    TBool mirrored = IsMirrored();

    switch ( iFontSettingsValue )
        {
        case EAknUiZoomSmall: //Fallthrough
        case EAknUiZoomVerySmall:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerIconPositionSmallFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
                     KResponseAreaAnswerIconPositionSmallFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
						KResponseAreaAnswerIconPositionSmallFontMirrored;
                    }
                }
            break;
            }
        case EAknUiZoomNormal:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerIconPositionNormalFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
                     KResponseAreaAnswerIconPositionNormalFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
						KResponseAreaAnswerIconPositionNormalFontMirrored;
                    }
                }
            break;
            }
        case EAknUiZoomLarge: //Fallthrough
        case EAknUiZoomVeryLarge:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerIconPositionLargeFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
                     KResponseAreaAnswerIconPositionLargeFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
						KResponseAreaAnswerIconPositionLargeFontMirrored;
                    }
                }
            break;
            }
        default:
            {
            if( !mirrored )
                {
                retValue = KResponseAreaAnswerIconPositionNormalFont;
                }
            else
                {
                if( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    retValue = 
					 KResponseAreaAnswerIconPositionNormalFontMirroredLandscape;
                    }
                else
                    {
                    retValue = 
						KResponseAreaAnswerIconPositionNormalFontMirrored;
                    }
                }
            break;
            }
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ResponseAreaAnswerItemheight
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::ResponseAreaAnswerItemheight()
    {
    FUNC_LOG;
    TInt retValue;

    switch ( iFontSettingsValue )
        {
        case EAknUiZoomSmall: //Fallthrough
        case EAknUiZoomVerySmall:
            {
            retValue = KResponseAreaAnserItemHeightSmallFont;
            break;
            }
        case EAknUiZoomNormal:
            {
            retValue = KResponseAreaAnserItemHeightNormalFont;
            break;
            }
        case EAknUiZoomLarge: //Fallthrough
        case EAknUiZoomVeryLarge:
            {
            retValue = KResponseAreaAnserItemHeightLargeFont;
            break;
            }
        default:
            {
            retValue = KResponseAreaAnserItemHeightNormalFont;
            break;
            }
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::RegularFieldHeight
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::RegularFieldHeight()
    {
    FUNC_LOG;
    TInt retValue;

    switch ( iFontSettingsValue )
        {
        case EAknUiZoomSmall: //Fallthrough
        case EAknUiZoomVerySmall:
            {
            retValue = KRegularFieldHeightSmallFont;
            break;
            }
        case EAknUiZoomNormal:
            {
            retValue = KRegularFieldHeightNormalFont;
            break;
            }
        case EAknUiZoomLarge: //Fallthrough
        case EAknUiZoomVeryLarge:
            {
            retValue = KRegularFieldHeightLargeFont;
            break;
            }
        default:
            {
            retValue = KRegularFieldHeightNormalFont;
            break;
            }
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::OrganizerTextWidth
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::OrganizerTextWidth()
    {
    FUNC_LOG;
    return KOrganizerTextWidth;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::OrganizerTextPosition
// ---------------------------------------------------------------------------
//
TPoint CESMRLayoutManager::OrganizerTextPosition()
    {
    FUNC_LOG;
    TPoint retValue(0,0);
    if( IsMirrored() )
        {
        TInt fieldWidth = FieldSize( EESMRFieldOrganizer ).iWidth;
        retValue = TPoint( fieldWidth - KIconSize.iWidth - 
        		           ESMRLayout::KIconBorderMargin - OrganizerTextWidth(), 
        		           RegularFieldHeight() );
        }
    else
        {
        retValue = TPoint( KIconSize.iWidth +
                           ESMRLayout::KIconBorderMargin + 
						   TextSideMargin(), 
						   RegularFieldHeight() );
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ConflictPopupPosition
// ---------------------------------------------------------------------------
//
EXPORT_C TPoint CESMRLayoutManager::ConflictPopupPosition()
    {
    FUNC_LOG;
	TPoint PopupPoint = Layout_Meta_Data::IsLandscapeOrientation() ?
		KConflictPopupPositionLandscape : KConflictPopupPositionPortrait ;

	return PopupPoint;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ListAreaBgColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::ListAreaBgColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnOtherColors,
                                     KListAreaBgColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultListAreaBgColor;
        }

    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::GeneralListAreaTextColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::GeneralListAreaTextColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnTextColors,
                                     KListAreaTextColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultTextColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::NormalTextColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::NormalTextColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnTextColors,
                                     KNormalTextColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultTextColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::NormalTextColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::HighlightedTextColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnTextColors,
                                     KNormalHighLightedTextColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultTextColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ViewerListAreaTextColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::ViewerListAreaTextColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnTextColors,
                                     KViewerAreaTextColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultTextColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ViewerListAreaHighlightedTextColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::ViewerListAreaHighlightedTextColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnHighlightColors,
                                     KViewerAreaHighlightedTextColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultTextColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::TitleAreaTextColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::TitleAreaTextColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnTextColors,
                                     KTitleAreaTextColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultTextColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::NormalTextBgColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::NormalTextBgColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnOtherColors,
                                     KNormalTextBgColorID );

    if( err != KErrNone )
        {
        bgColor = KDefaultNormalTextBgColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::FieldBorderColor
// ---------------------------------------------------------------------------
//
TRgb CESMRLayoutManager::FieldBorderColor()
    {
    FUNC_LOG;
    TRgb bgColor;
    TInt err;

    err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                     bgColor,
                                     KAknsIIDQsnOtherColors ,
                                     KFieldBorderColorID );
    if( err != KErrNone )
        {
        bgColor = KDefaultBorderColor;
        }
    return bgColor;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::NormalTextColorID
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::NormalTextColorID()
    {
    FUNC_LOG;
    return KNormalTextColorID;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::ViewerListAreaTextColorID
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::ViewerListAreaTextColorID()
    {
    FUNC_LOG;
    return KListAreaTextColorID;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::GetSkinBasedBitmap
// ---------------------------------------------------------------------------
//
TInt CESMRLayoutManager::GetSkinBasedBitmap( 
		TAknsItemID aIconID, 
		CFbsBitmap*& aBitmap, 
		CFbsBitmap*& aMask, 
		TSize aSize )
    {
    FUNC_LOG;
    TInt retValue = KErrNone;
    delete aBitmap;
    aBitmap = NULL;
    delete aMask;
    aMask = NULL;

    TInt fileIndex(-1);
    TInt fileMaskIndex(-1);

    SetBitmapFallback( aIconID, fileIndex, fileMaskIndex );

    TFileName bitmapFilePath;
    ESMRHelper::LocateResourceFile(
            KESMRMifFile,
            KDC_APP_BITMAP_DIR,
            bitmapFilePath);

    TRAPD(error, AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
										 aIconID,
										 aBitmap,
										 aMask,
										 bitmapFilePath,
										 fileIndex,
										 fileMaskIndex ));
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
// CESMRLayoutManager::IsFontChanged
// ---------------------------------------------------------------------------
//
TBool CESMRLayoutManager::IsFontChanged()
    {
    FUNC_LOG;
    return iFontSettingsChanged;
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::HandleNotifyInt
// ---------------------------------------------------------------------------
//
void CESMRLayoutManager::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    FUNC_LOG;
    if( aId == KAknGlobalUiZoom )
        {
        iFontSettingsValue = aNewValue;
        iFontSettingsChanged = ETrue;

        // Force CESMRLayoutManager::Font to create new font
        delete iFont;
        iFont = NULL;

        delete iMfneFont;
        iMfneFont = NULL;

        if ( iFontSizeObserver )
            {
            iFontSizeObserver->FontSizeSettingsChanged();//Synchronous update
            iFontSettingsChanged = EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRLayoutManager::SetObserver
// ---------------------------------------------------------------------------
//
void CESMRLayoutManager::SetObserver( MESMRFontSizeObserver* aObserver )
    {
    FUNC_LOG;
    iFontSizeObserver = aObserver;
    }

// EOF

