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


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmrglobalnote.h"
#include "esmrhelper.h"//locateresourcefile
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <AknGlobalNote.h>
#include <bautils.h>
#include <esmrgui.rsg>
#include <eikenv.h>
#include <avkon.hrh>
#include <data_caging_path_literals.hrh>

// CONSTANTS
// Unnamed namespace for local definitions
namespace {

_LIT(KResourceFileName,"esmrgui.rsc");

} // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRGlobalNote::CESMRGlobalNote
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CESMRGlobalNote::CESMRGlobalNote(
            TESMGlobalNoteType aType)
:   iType(aType)
    {
    FUNC_LOG;
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CESMRGlobalNote::~CESMRGlobalNote
// -----------------------------------------------------------------------------
//
CESMRGlobalNote::~CESMRGlobalNote()
    {
    FUNC_LOG;
    iEnv->DeleteResourceFile(iResourceOffset);    
    }

// -----------------------------------------------------------------------------
// CESMRGlobalNote::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRGlobalNote* CESMRGlobalNote::NewL(
        TESMGlobalNoteType aType)
    {
    FUNC_LOG;
    CESMRGlobalNote* self =
            new (ELeave) CESMRGlobalNote(aType);
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRGlobalNote::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRGlobalNote::ConstructL()
    {
    FUNC_LOG;
    TFileName filename;    
    iEnv = CEikonEnv::Static();// codescanner::eikonenvstatic

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
// CESMRGlobalNote::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRGlobalNote::ExecuteL(TESMGlobalNoteType aType)
    {
    FUNC_LOG;
    CESMRGlobalNote* query = CESMRGlobalNote::NewL( aType );
    CleanupStack::PushL(query);
    query->ExecuteLD();
    CleanupStack::Pop(query);
    }

// -----------------------------------------------------------------------------
// CESMRGlobalNote::NoteType
// -----------------------------------------------------------------------------
//
TAknGlobalNoteType CESMRGlobalNote::NoteType()
    {
    FUNC_LOG;
    TAknGlobalNoteType type(EAknGlobalInformationNote);
    switch ( iType )
        {
        case EESMRCorruptedMR:
            {
            type = EAknGlobalErrorNote;
            break;
            }
        case EESMRCalenLaterDate: // FALLTHROUGH
        case EESMREndsBeforeStarts: // FALLTHROUGH
        case EESMREntryEndEarlierThanItStart: // FALLTHROUGH
        case EESMRRepeatEndEarlierThanItStart: // FALLTHROUGH
        case EESMRAlarmAlreadyPassed: // FALLTHROUGH
        case EESMRDiffMoreThanMonth: // FALLTHROUGH
        case EESMREntrySaved: // FALLTHROUGH
        case EESMRTodoEntrySaved: // FALLTHROUGH
        case EESMRRepeatDifferentStartAndEndDate:
        case EESMRRepeatReSchedule:
        case EESMRCannotDisplayMuchMore:
            {
            type = EAknGlobalInformationNote;
            break;
            }
        default:
            {
            break;
            }
        }
    return type;
    }

// -----------------------------------------------------------------------------
// CESMRGlobalNote::NoteTextLC
// -----------------------------------------------------------------------------
//
HBufC* CESMRGlobalNote::NoteTextLC()
    {
    FUNC_LOG;
    HBufC* noteText = NULL;
    switch ( iType )
        {
        case EESMRCorruptedMR:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                                R_QTN_MEET_REQ_CORRUPTED_NOTE);
            break;
            }
        case EESMREndsBeforeStarts:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_MEET_REQ_INFO_NOTE_ENDS_BEFORE);
            break;
            }
        case EESMRCalenLaterDate:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_MEET_REQ_LATER_DATE);
            break;
            }
        case EESMREntryEndEarlierThanItStart:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_MEET_REQ_INFO_NOTE_ENDS_BEFORE );
            break;
            }
        case EESMRRepeatEndEarlierThanItStart:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_CALENDAR_INFO_REPEAT_INVALID );
            break;
            }
        case EESMRAlarmAlreadyPassed:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_CALENDAR_ALARM_PASSED );
            break;
            }
        case EESMRDiffMoreThanMonth:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_CALENDAR_ALARM_DIFFERENCE );
            break;
            }
        case EESMREntrySaved:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_MEET_REQ_NOTE_SAVED );
            break;
            }
        case EESMRTodoEntrySaved:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                            R_QTN_MEET_REQ_TODO_SAVED );
            break;
            }
        case EESMRRepeatDifferentStartAndEndDate:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                    R_QTN_MEET_REQ_INFO_START_END_DATE );
            }
            break;
        case EESMRRepeatReSchedule:
            {
            noteText = iEnv->AllocReadResourceAsDes16LC(
                    R_MEET_REQ_RESCHEDULE_INSTANCE );
            }
            break;
        case EESMRCannotDisplayMuchMore:
        	{
        	noteText = iEnv->AllocReadResourceAsDes16LC(
                    R_QTN_MEET_REQ_LONG_DESCRIPTION );
        	}
            break;
        default:
            {
            break;
            }
        }
    return noteText;
    }

// -----------------------------------------------------------------------------
// CESMRGlobalNote::ExecuteLD
// -----------------------------------------------------------------------------
//
void CESMRGlobalNote::ExecuteLD()
    {
    FUNC_LOG;
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    HBufC* noteText = NoteTextLC();

    note->ShowNoteL(
            NoteType(),
            *noteText );

    CleanupStack::PopAndDestroy( noteText );
    noteText = NULL;

    CleanupStack::PopAndDestroy( note );
    note = NULL;

    // Deleting 'this' object because this id D-method.
    delete this;
    }

// EOF

