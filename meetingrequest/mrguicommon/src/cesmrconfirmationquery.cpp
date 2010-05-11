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
* Description:  Class is responsible of showing confirmation queries
*
*/

// INCLUDE FILES
#include "emailtrace.h"
#include "cesmrconfirmationquery.h"

//<cmail>
#include "esmrdef.h"
//</cmail>
#include <AknQueryDialog.h>
#include <bautils.h>
#include <esmrgui.rsg>
#include <eikenv.h>
#include <avkon.rsg>
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>
#include <AknBidiTextUtils.h>//line wrapping and mirroring
#include <aknlayoutscalable_apps.cdl.h> //xml layout data for applications
#include <aknlayoutscalable_avkon.cdl.h> //xml layout data of avkon components
#include "esmrhelper.h"//locateresourcefile

// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// Definition for panic text
_LIT( KESMRConfirmationQueryPanicTxt, "ESMRConfirmationQueryPanic" );

/** Panic code enumaration */
enum TESMRConfirmationQueryPanic
    {
    EESMRInvalidIdentifier = 0  // Invalid identifier
    };

void Panic( TESMRConfirmationQueryPanic aPanic )
    {
    User::Panic( KESMRConfirmationQueryPanicTxt, aPanic );
    }

#endif

_LIT(KResourceFileName,"esmrgui.rsc");

const TInt KVariantIndex(2); //a LAF variant value required for pop up window
const TInt KRequiredLinesPlus(2); //minimum lines needed for aknbiditextutils API array set up
const TInt KActualLine(0); //minimum lines needed for aknbiditextutils API array set up
const TInt KDummyLineForcedByAPI(1); //minimum lines needed for aknbiditextutils API array set up
} // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRConfirmationQuery::CESMRConfirmationQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CESMRConfirmationQuery::CESMRConfirmationQuery(
            TESMRConfirmationQueryType aType) :
    iType(aType)
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRConfirmationQuery::~CESMRConfirmationQuery
// -----------------------------------------------------------------------------
//
CESMRConfirmationQuery::~CESMRConfirmationQuery()
    {
    FUNC_LOG;
    iEnv->DeleteResourceFile(iResourceOffset);
    }

// -----------------------------------------------------------------------------
// CESMRConfirmationQuery::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRConfirmationQuery::ConstructL()
    {
    FUNC_LOG;
    TFileName filename;

    //cache the Eikon pointer, as Static() is slow
    iEnv = CEikonEnv::Static(); // codescanner::eikonenvstatic

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
// CESMRConfirmationQuery::ExecuteLD
// -----------------------------------------------------------------------------
//
TBool CESMRConfirmationQuery::ExecuteLD()
    {
    FUNC_LOG;
    HBufC* noteText = NULL;
    switch ( iType )
        {
        case EESMRDeleteMR:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_DELETE_QUERY);
            break;
            }
        case EESMRSendCancellationInfoToParticipants:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_SEND_CANCELLATION_QUERY);
            break;
            }
        case EESMRRemoveAppointment:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_REMOVE_APPOINTMENT_QUERY);
            break;
            }
        case EESMRSaveChanges:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_FORM_QUEST_SAVE_CHANGES);
            break;
            }
        case EESMRDeleteEntry:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_CALENDAR_OPT_DELETE_ENTRY);
            break;
            }
        case EESMRSendDecline:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_DECLINE_QUERY );
            }
            break;
        case EESMRSaveAnnivChangedStartDay:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_QUEST_CHANGE_START_DAY );
            }
            break;
        case EESMRAttachments:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_NOTE_ATTACHMENTS );
            }
            break;
        case EESMRSaveMeetingChangedStartDay:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_QUEST_CHANGE_START_DAY );
            }
            break;
        case EESMRAssignUpdatedLocation:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_ASSIGN_UPDATED_LOCATION );
            }
            break;
        case EESMRAttachmentsNotSupported:
            {
            noteText =
                iEnv->AllocReadResourceAsDes16LC(
                        R_QTN_MEET_REQ_NOTE_ATTACHMENTS_NOT_SUPPORTED );
            }
            break;
        default:
            {
            #ifdef _DEBUG
            // Invalid identifier --> panic
            Panic( EESMRInvalidIdentifier );
            #endif
            }
            break;
        }

    TBool response(EFalse);
    if ( noteText )
    	{
		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
		if( dlg->ExecuteLD(R_MR_CONFIRMATION_QUERY, *noteText) )
			{
			response = ETrue;
			}
		CleanupStack::PopAndDestroy(noteText);
    	}

    delete this;
    return response;
    }

// -----------------------------------------------------------------------------
// CESMRConfirmationQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CESMRConfirmationQuery::ExecuteL( TESMRConfirmationQueryType aType)
    {
    FUNC_LOG;
    CESMRConfirmationQuery* query = new (ELeave) CESMRConfirmationQuery(aType);
    CleanupStack::PushL( query );
    query->ConstructL( );
    TBool ret = query->ExecuteLD();
    CleanupStack::Pop( query );
    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRConfirmationQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CESMRConfirmationQuery::ExecuteL( const TDesC& aLocation )
    {
    FUNC_LOG;
    CESMRConfirmationQuery* query = new (ELeave) CESMRConfirmationQuery(EESMRDeleteMR);
    CleanupStack::PushL( query );
    query->ConstructL( );

    HBufC* location = NULL;
    
    if ( aLocation.Length() > 0 )
        {
        location = query->TruncateTextToLAFNoteL( aLocation );
        CleanupDeletePushL( location );
        }
    else
        {
        // Allocate dummy buffer for query.
        location = aLocation.AllocLC();
        }
    
    HBufC* noteText = StringLoader::LoadLC( R_MEET_REQ_REPLACE_PREVIOUS_LOCATION, *location );
    
    TBool response = EFalse;
    
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    if( dlg->ExecuteLD( R_MR_CONFIRMATION_QUERY, *noteText) )
        {
        response = ETrue;
        }
    
    CleanupStack::PopAndDestroy( noteText );
    CleanupStack::PopAndDestroy( location );    
    CleanupStack::Pop( query );
    return response;
    }

// -----------------------------------------------------------------------------
// CESMRConfirmationQuery::TruncateTextToLAFNoteL
// -----------------------------------------------------------------------------
//
HBufC* CESMRConfirmationQuery::TruncateTextToLAFNoteL( const TDesC& aLocation )
    {
    FUNC_LOG;
    //wrap long string into array of individual lines
    CArrayFixFlat<TPtrC>* textLines = new(ELeave)CArrayFixFlat<TPtrC>( KRequiredLinesPlus );
    CleanupStack::PushL( textLines );
    
    //layout stuff to fetch list font and list line width
    TAknLayoutText fontType;

    TSize main_pane_Size;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, main_pane_Size );
    TRect main_pane( main_pane_Size );

    fontType.LayoutText( main_pane, AknLayoutScalable_Avkon::popup_note_window_t4( KVariantIndex ).LayoutLine() );

    TInt lineWidth = fontType.TextRect().Width();
 
    //extra bidi space for the two lines. second line is not actually used in the query but is required by the API
    HBufC* locationText = HBufC::NewLC( aLocation.Length() + KAknBidiExtraSpacePerLine + KAknBidiExtraSpacePerLine);
    TPtr locationTextPtr = locationText->Des();
    locationTextPtr.Append(aLocation);
    
    // First wrap, then do formatting.
    CArrayFixFlat<TInt>* lineWidths = new( ELeave )CArrayFixFlat<TInt>( KRequiredLinesPlus );
    CleanupStack::PushL( lineWidths );
    lineWidths->InsertL( KActualLine, lineWidth );
    lineWidths->InsertL( KDummyLineForcedByAPI, lineWidth );
    
    TBool useEllipsis(ETrue);

    // measure, wrap and truncate location item.
    AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
            locationTextPtr,
            *lineWidths,
            *fontType.Font(),
            *textLines,
            useEllipsis );
    
    //place result to new created target buffer
    HBufC* target = HBufC::NewL( textLines->At(KActualLine).Length() );
    target->Des().Append(textLines->At( KActualLine ));
    
    CleanupStack::PopAndDestroy( lineWidths );
    CleanupStack::PopAndDestroy( locationText );
    CleanupStack::PopAndDestroy( textLines );
    delete this;
    return target;
    }

//  End of File

