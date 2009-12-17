/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsSetUiNotes.
*
*/


#include "emailtrace.h"
#include <e32base.h>
#include <ipssossettings.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <aknclearer.h>

#include "ipssetutils.h"
#include "ipssetutilspageids.hrh"
#include "ipssetui.h"
#include "ipssetuiitembase.h"

#include "ipssetuinotes.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::CIpsSetUiNotes()
// ----------------------------------------------------------------------------
//
CIpsSetUiNotes::CIpsSetUiNotes()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::~CIpsSetUiNotes()
// ----------------------------------------------------------------------------
//
CIpsSetUiNotes::~CIpsSetUiNotes()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiNotes::ConstructL()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiNotes* CIpsSetUiNotes::NewL()
    {
    FUNC_LOG;
    CIpsSetUiNotes* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiNotes* CIpsSetUiNotes::NewLC()
    {
    FUNC_LOG;
    CIpsSetUiNotes* self = new ( ELeave ) CIpsSetUiNotes();
    CleanupStack::PushL( self );

    return self;
    }

/******************************************************************************

    STATIC FUNCTIONS

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::MakeStringLC()
// ----------------------------------------------------------------------------
//
HBufC* CIpsSetUiNotes::MakeStringLC( const TUint aTextResource )
    {
    FUNC_LOG;
    HBufC* prompt = StringLoader::LoadL( aTextResource );
    CleanupStack::PushL( prompt );

    return prompt;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::MakeString()
// ----------------------------------------------------------------------------
//
TIpsSetUtilsTextPlain CIpsSetUiNotes::MakeString(
    const TUint aTextResource,
    const TInt aValue )
    {
    FUNC_LOG;
    TIpsSetUtilsTextPlain resourceText;
    TIpsSetUtilsTextPlain finalText;
    StringLoader::Load( resourceText, aTextResource );
    StringLoader::Format(
        finalText, resourceText, KErrNotFound, aValue );

    return finalText;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::MakeString()
// ----------------------------------------------------------------------------
//
TIpsSetUtilsTextPlain CIpsSetUiNotes::MakeString(
    const TUint aTextResource,
    const TDesC& aText )
    {
    FUNC_LOG;
    TIpsSetUtilsTextPlain resourceText;
    TIpsSetUtilsTextPlain finalText;
    StringLoader::Load( resourceText, aTextResource );
    StringLoader::Format(
        finalText, resourceText, KErrNotFound, aText );

    return finalText;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::ShowQuery
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiNotes::ShowQueryL(
    const TUint aTextResource,
    const TInt  aDialogResource,
    const CAknQueryDialog::TTone aTone )
    {
    FUNC_LOG;
    // Get text to be shown
    HBufC* prompt = MakeStringLC( aTextResource );

    // Create dialog and execute the dialog
    TInt result = CIpsSetUiNotes::ShowQueryL(
        prompt->Des(), aDialogResource, aTone );

    CleanupStack::PopAndDestroy( prompt );

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::ShowQuery
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiNotes::ShowQueryL(
    const TDesC& aText,
    const TInt  aDialogResource,
    const CAknQueryDialog::TTone aTone )
    {
    FUNC_LOG;
    CAknLocalScreenClearer* localScreenClearer =
        CAknLocalScreenClearer::NewL( EFalse );
    CleanupStack::PushL( localScreenClearer );

    // Create dialog and execute the dialog
    CAknQueryDialog* dlg = CAknQueryDialog::NewL( aTone );
    TInt button = dlg->ExecuteLD( aDialogResource, aText );
    CleanupStack::PopAndDestroy( localScreenClearer );

    return button;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::ShowNoteL
// ----------------------------------------------------------------------------
//
void CIpsSetUiNotes::ShowNoteL(
    const TUint      aResource,
    const TIpsSetUiNotes& aNoteType,
    const TBool      aNoteWaiting )
    {
    FUNC_LOG;
    // Get text to be shown
    HBufC* prompt = MakeStringLC( aResource );

    // Show the note
    CIpsSetUiNotes::ShowNoteL( prompt->Des(), aNoteType, aNoteWaiting );

    CleanupStack::PopAndDestroy( prompt );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::ShowNoteL
// ----------------------------------------------------------------------------
//
void CIpsSetUiNotes::ShowNoteL(
    const TDesC&      aText,
    const TIpsSetUiNotes& aNoteType,
    const TBool      aNoteWaiting )
    {
    FUNC_LOG;
    CAknResourceNoteDialog* note = NULL;

    // Create the note and show it
    switch( aNoteType )
        {
        // Show confirmation note
        case EIpsSetUiConfirmationNote:
            note = new ( ELeave ) CAknConfirmationNote( aNoteWaiting );
            break;

        // Show information note
        case EIpsSetUiInformationNote:
            note = new ( ELeave ) CAknInformationNote( aNoteWaiting );
            break;

        // Show error note
        case EIpsSetUiErrorNote:
            note = new ( ELeave ) CAknErrorNote( aNoteWaiting );
            break;

        // Show warning note
        case EIpsSetUiWarningNote:
            note = new ( ELeave ) CAknWarningNote( aNoteWaiting );
            break;

        // Wrong note given, leave
        default:
            User::Leave( KErrUnknown );
            break;
        }

    // Show the note
    note->ExecuteLD( aText );
    }

/******************************************************************************

    Error note ui

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowDialog()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiNotes::ShowDialog(
    const CIpsSetUiItem& aBaseItem,
    const TIpsSetUiNoteErrors aError,
    const TDesC& /* aNewText */ )
    {
    FUNC_LOG;
    TIpsSetUiEventResult result = EIpsSetUiPageEventResultApproved;
     
    // Show error note based on the id
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIpsSetUiMailboxEmailAddress:
        case EIpsSetUiMailboxReplyToAddress:
            TRAP_IGNORE( result = ShowEmailAddressErrorNoteL( aError ) );
            return result;

        case EIpsSetUiIncomingMailServer:
        case EIpsSetUiOutgoingMailServer:
            TRAP_IGNORE( result = ShowServerErrorNoteL( aError ) );
            return result;

        default:
            break;
        }
   

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::ShowEmailAddressErrorNoteL()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiNotes::ShowEmailAddressErrorNoteL(
    const TIpsSetUiNoteErrors aError )
    {
    FUNC_LOG;
    // Show notes
    switch ( aError )
        {
        // Incorrect
        case EIpsSetUiItemInvalid:
            CIpsSetUiNotes::ShowNoteL(
                R_FSE_SETTINGS_MAIL_EMAIL_PROMPT,
                EIpsSetUiInformationNote, ETrue );
            return EIpsSetUiPageEventResultDisapproved;

        default:
            break;
        }

    return EIpsSetUiPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiNotes::ShowServerErrorNoteL()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiNotes::ShowServerErrorNoteL(
    const TIpsSetUiNoteErrors aError )
    {
    FUNC_LOG;
    switch ( aError )
        {
        case EIpsSetUiNoError:
            return EIpsSetUiPageEventResultApproved;

        default:
            break;
        }

    CIpsSetUiNotes::ShowQueryL(
        R_FSE_SETTINGS_MAIL_SRVR_PROMPT, R_IPS_SETUI_INFORMATION_QUERY );
    return EIpsSetUiPageEventResultDisapproved;
    }

