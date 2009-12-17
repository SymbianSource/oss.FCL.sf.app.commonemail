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
* Description:  FreestyleEmailUi main application document implementation
*
*/
 

#include "emailtrace.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiDocument.h"

CFreestyleEmailUiDocument* CFreestyleEmailUiDocument::NewL(CEikApplication& aApp)
    {
    FUNC_LOG;
    CFreestyleEmailUiDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CFreestyleEmailUiDocument* CFreestyleEmailUiDocument::NewLC(CEikApplication& aApp)
    {
    FUNC_LOG;
    CFreestyleEmailUiDocument* self = new (ELeave) CFreestyleEmailUiDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CFreestyleEmailUiDocument::ConstructL()
    {
    FUNC_LOG;
	// iEnv must be owned by Document instead of AppUI because CAlfEnv
	// must still exist when AppUI is deleted.
    iEnv = CAlfEnv::NewL();
    }    

CFreestyleEmailUiDocument::CFreestyleEmailUiDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
    FUNC_LOG;
    }

CFreestyleEmailUiDocument::~CFreestyleEmailUiDocument()
    {
    FUNC_LOG;
    delete iEnv;
    }

CEikAppUi* CFreestyleEmailUiDocument::CreateAppUiL()
    {
    FUNC_LOG;
    CEikAppUi* appUi = new (ELeave) CFreestyleEmailUiAppUi( *iEnv );
    return appUi;
    }


