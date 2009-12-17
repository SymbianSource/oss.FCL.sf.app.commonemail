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
* Description:  FSMailServer main application document implementation
*
*/


#include "emailtrace.h"
#include "FsEmailGlobalDialogsAppUi.h"
#include "FsEmailGlobalDialogsDocument.h"

CFsEmailGlobalDialogsDocument* CFsEmailGlobalDialogsDocument::NewL( CEikApplication& aApp )
    {
    FUNC_LOG;
    CFsEmailGlobalDialogsDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

CFsEmailGlobalDialogsDocument* CFsEmailGlobalDialogsDocument::NewLC( CEikApplication& aApp )
    {
    FUNC_LOG;
    CFsEmailGlobalDialogsDocument* self = new (ELeave) CFsEmailGlobalDialogsDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CFsEmailGlobalDialogsDocument::ConstructL()
    {
    FUNC_LOG;
    }    

CFsEmailGlobalDialogsDocument::CFsEmailGlobalDialogsDocument( CEikApplication& aApp )
    : CAknDocument(aApp)
    {
    FUNC_LOG;
    }

CFsEmailGlobalDialogsDocument::~CFsEmailGlobalDialogsDocument()
    {
    FUNC_LOG;
    }

CEikAppUi* CFsEmailGlobalDialogsDocument::CreateAppUiL()
    {
    FUNC_LOG;
    CEikAppUi* appUi = new (ELeave) CFsEmailGlobalDialogsAppUi;
    return appUi;
    }

