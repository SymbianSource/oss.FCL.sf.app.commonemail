/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CEmailWidgetSettingsAppUi implementation
*
*/

// INCLUDE FILES
#include <bacline.h>
#include <eikstart.h>

#include "emailtrace.h"
#include "emailwidgetsettingsappui.h"
#include "emailwidgetsettingslistview.h"

// ============================ MEMBER FUNCTIONS ===============================

//
// CEmailWidgetSettingsAppUi
//

// -----------------------------------------------------------------------------
// CEmailWidgetSettingsAppUi::~CEmailWidgetSettingsAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CEmailWidgetSettingsAppUi::~CEmailWidgetSettingsAppUi()
    {
    FUNC_LOG;	
    }

// -----------------------------------------------------------------------------
// CEmailWidgetSettingsAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEmailWidgetSettingsAppUi::ConstructL()
    {
    FUNC_LOG;	
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    CreateListViewL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailWidgetSettingsAppUi::CreateListViewL()
    {
    FUNC_LOG;	
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    const TInt count = args->Count();
    if (count!=2)
        {
        CleanupStack::PopAndDestroy(args);
        Exit();
        }
  
    CEmailWidgetSettingsListView* view = new(ELeave) CEmailWidgetSettingsListView;
    CleanupStack::PushL(view);

    AddViewL(view);     // transfer ownership to CAknViewAppUi
    CleanupStack::Pop(view); //view

    view->ConstructL();
    SetDefaultViewL(*view);
    CleanupStack::PopAndDestroy(args);    
    }

// -----------------------------------------------------------------------------
// CEmailWidgetSettingsAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CEmailWidgetSettingsAppUi::HandleCommandL(TInt aCommand)
    {
    FUNC_LOG;	
    switch (aCommand)
        {
        case EAknSoftkeyCancel:
        case EEikCmdExit:
            Exit();
            break;
        default:
            break;
        }
    }
	
// --------- emailwidgetsettingsdocument.cpp

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CEikAppUi* CEmailWidgetSettingsDocument::CreateAppUiL()
    {
    FUNC_LOG;	
    return(new(ELeave) CEmailWidgetSettingsAppUi);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailWidgetSettingsDocument::ConstructL()
    {
    FUNC_LOG;
    }

// --------- emailwidgetsettingsapplication.cpp

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUid CEmailWidgetSettingsApplication::AppDllUid() const
    {
    FUNC_LOG;	
    return KUidWidgetSettApp;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CApaDocument* CEmailWidgetSettingsApplication::CreateDocumentL()
    {
    FUNC_LOG;	
    CEmailWidgetSettingsDocument* document=new(ELeave) CEmailWidgetSettingsDocument(*this);
    CleanupStack::PushL(document);
    document->ConstructL();
    CleanupStack::Pop();
    return(document);
    }

// --------- emailwidgetsettings.cpp

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    FUNC_LOG;	
    return new CEmailWidgetSettingsApplication;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    FUNC_LOG;	
    return EikStart::RunApplication(NewApplication);
    }

// End of File
