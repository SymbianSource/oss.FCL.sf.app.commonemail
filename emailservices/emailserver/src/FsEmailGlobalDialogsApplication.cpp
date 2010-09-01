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
* Description:  FSMailServer application entry point implementation
*
*/


#include "emailtrace.h"
#include "FsEmailGlobalDialogsApplication.h"
#include "FsEmailGlobalDialogsDocument.h"
#include "fsmailserverdefinitions.h"

const TUid KFSMailServerUidAsTUid = { KFSMailServerUid };

CApaDocument* CFsEmailGlobalDialogsApplication::CreateDocumentL()
    {
    FUNC_LOG;
    CApaDocument* document = CFsEmailGlobalDialogsDocument::NewL( *this );
    return document;
    }

TUid CFsEmailGlobalDialogsApplication::AppDllUid() const
    {
    FUNC_LOG;
    return KFSMailServerUidAsTUid;
    }

