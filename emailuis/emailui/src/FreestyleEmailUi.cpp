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
* Description:  FreestyleEmailUi application entry point implementation
*
*/


#include "emailtrace.h"
#include "FreestyleEmailUiApplication.h"
#include "FreestyleEmailUiDocument.h"
#include "FreestyleEmailUiConstants.h"
#include <eikstart.h>

CApaDocument* CFreestyleEmailUiApplication::CreateDocumentL()
    {
    FUNC_LOG;
    CApaDocument* document = CFreestyleEmailUiDocument::NewL(*this);
    return document;
    }

TUid CFreestyleEmailUiApplication::AppDllUid() const
    {
    FUNC_LOG;
    return KFSEmailUiUid;
    }

LOCAL_C CApaApplication* NewApplication()
    {
    FUNC_LOG;
    return new CFreestyleEmailUiApplication;
    }


GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }
    

