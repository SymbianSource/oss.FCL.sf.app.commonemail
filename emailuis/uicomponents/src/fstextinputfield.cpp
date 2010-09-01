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
* Description:  Implementation of the CFsTextInputField class
*
*/


#include "emailtrace.h"
#include <txtrich.h>

#include "fstextinputfield.h"
#include "fstextinputvisual.h"
#include "fstextinputfieldobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CFsTextInputField::CFsTextInputField( MFsTextInputFieldObserver& aObserver ) 
    : iObserver( aObserver ), iFsTextInputMode( ENotAtomicInput )
    {
    FUNC_LOG;
    //No implementation needed
    }

// ---------------------------------------------------------------------------
// Two phased constructor
// ---------------------------------------------------------------------------
//
void CFsTextInputField::ConstructL()
    {
    FUNC_LOG;
    CEikRichTextEditor::ConstructL( NULL,
                                    0,
                                    0,
                                    CEikEdwin::EOwnsWindow|
                                    CEikEdwin::EInclusiveSizeFixed,
                                    EGulFontControlBold|EGulFontControlItalic|
                                    EGulFontControlUnderline|
                                    EGulFontControlStrikethrough|
                                    EGulFontControlTextColor|
                                    EGulFontControlPrintPos );
    }


// ---------------------------------------------------------------------------
// Constructs and returns object of CFsTextInputField class
// ---------------------------------------------------------------------------
//
CFsTextInputField* CFsTextInputField::NewL(
    MFsTextInputFieldObserver& aObserver )
    {
    FUNC_LOG;
    CFsTextInputField* self = CFsTextInputField::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructs and returns object of CFsTextInputField class
// ---------------------------------------------------------------------------
//
CFsTextInputField* CFsTextInputField::NewLC(
    MFsTextInputFieldObserver& aObserver )
    {
    FUNC_LOG;
    CFsTextInputField* self = new ( ELeave ) CFsTextInputField( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFsTextInputField::~CFsTextInputField()
    {
    FUNC_LOG;
    //No implementation needed.
    }

// ---------------------------------------------------------------------------
// Set Text input mode
// ---------------------------------------------------------------------------
//
void CFsTextInputField::SetTextInputMode( const TFsTextInputMode aMode )
    {
    FUNC_LOG;
    iFsTextInputMode = aMode;
    }

// ---------------------------------------------------------------------------
// Returns Text input mode
// ---------------------------------------------------------------------------
//
CFsTextInputField::TFsTextInputMode CFsTextInputField::TextInputMode() const
    {
    FUNC_LOG;
    return iFsTextInputMode;
    }

// ---------------------------------------------------------------------------
// From CEikRichTextEditor
// Function receives key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CFsTextInputField::OfferKeyEventL( 
    const TKeyEvent &aKeyEvent, 
    TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse result( EKeyWasNotConsumed );
    
    if ( ( aKeyEvent.iCode == EKeyBackspace ) &&
         ( iFsTextInputMode == EAtomicInput ) &&
         !( IsReadOnly() ) )
        {
        RichText()->Reset();
        HandleTextChangedL();
        SetCursorPosL( 0, EFalse );
        result = EKeyWasConsumed;
        }
    else
        {
        result = CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// From CEikRichTextEditor
// Invoked when changes in text filed content are made.
// ---------------------------------------------------------------------------
//
void CFsTextInputField::EditObserver ( TInt aStartEdit, TInt aEditLength )
    {
    FUNC_LOG;
    CEikRichTextEditor::EditObserver( aStartEdit, aEditLength );
    // <cmail>
    TRAP_IGNORE( iObserver.HandleTextInputEventL( this,
        MFsTextInputFieldObserver::EEventTextUpdated ) );
	// </cmail>
    }


