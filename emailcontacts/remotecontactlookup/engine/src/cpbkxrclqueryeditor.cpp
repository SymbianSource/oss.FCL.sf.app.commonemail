/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CPbkxRclQueryEditor.
*
*/


#include "emailtrace.h"
#include <pbkxrclengine.rsg>
#include <StringLoader.h>
#include <aknedsts.h>

#include "pbkxremotecontactlookuppanic.h"
#include "cpbkxrclqueryeditor.h"
#include "pbkxrclengineconstants.h"
#include "pbkxrclutils.h"
#include "engine.hrh"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclQueryEditor::CPbkxRclQueryEditor
// ---------------------------------------------------------------------------
//
CPbkxRclQueryEditor::CPbkxRclQueryEditor( TDes& aQueryText ) :
    CAknTextQueryDialog( aQueryText )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclQueryEditor::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclQueryEditor::ConstructL( const TDesC& aAccountName )
    {
    FUNC_LOG;

    PrepareLC( R_RCL_QUERY_DIALOG );

    HBufC* promptBuf = StringLoader::LoadLC( R_QTN_RCL_SEARCH, aAccountName );
    SetPromptL( *promptBuf );
    CleanupStack::PopAndDestroy( promptBuf );

    }

// ---------------------------------------------------------------------------
// CPbkxRclQueryEditor::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclQueryEditor* CPbkxRclQueryEditor::NewL(
    const TDesC& aAccountName,
    TDes& aQueryText )
    {
    FUNC_LOG;
    CPbkxRclQueryEditor* self = new ( ELeave ) CPbkxRclQueryEditor( aQueryText );
    CleanupStack::PushL( self );
    self->ConstructL( aAccountName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclQueryEditor::~CPbkxRclQueryEditor
// ---------------------------------------------------------------------------
//
CPbkxRclQueryEditor::~CPbkxRclQueryEditor()
    {
    FUNC_LOG;
    }
    
// ---------------------------------------------------------------------------
// CPbkxRclQueryEditor::ExecuteDialogLD
// ---------------------------------------------------------------------------
//
TBool CPbkxRclQueryEditor::ExecuteDialogLD()
    {
    FUNC_LOG;

    SetEditorInputModeL();
    TInt ret = CAknQueryDialog::RunLD();
    return ( TBool )ret;
    }

// ---------------------------------------------------------------------------
// CPbkxRclQueryEditor::SetEditorInputModeL
// ---------------------------------------------------------------------------
//
void CPbkxRclQueryEditor::SetEditorInputModeL()
    {
    FUNC_LOG;

    CAknQueryControl* control = QueryControl();
    CEikEdwin* edwin = static_cast<CEikEdwin*>( 
        control->ControlByLayoutOrNull( EDataLayout ) );
    
    User::LeaveIfNull( edwin );
    
    TDes& text = Text();

    TCoeInputCapabilities inputCapabilities( edwin->InputCapabilities() ); 
    if ( inputCapabilities.FepAwareTextEditor() 
         && inputCapabilities.FepAwareTextEditor()->Extension1() ) 
        { 
        CAknEdwinState* state = static_cast<CAknEdwinState*>(
            inputCapabilities.FepAwareTextEditor()->Extension1()->State( 
                KNullUid ) );
        
        if ( text == KNullDesC || 
             PbkxRclUtils::HasAlphaCharsInString( text ) )
            {
            state->SetCurrentInputMode( EAknEditorTextInputMode );
            }
        else
            {
            state->SetCurrentInputMode( EAknEditorNumericInputMode ); 
            } 
        }
    }

