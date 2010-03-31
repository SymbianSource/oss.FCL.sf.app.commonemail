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
* Description:  Class is responsible of showing list queries
*
*/

// INCLUDE FILES
#include "emailtrace.h"
#include "cesmrguilistquery.h"
#include "esmrhelper.h"

#include <aknlistquerydialog.h>
#include <bautils.h>
#include <esmrgui.rsg>
#include <eikenv.h>
#include <avkon.rsg>
#include <data_caging_path_literals.hrh>

// Unnamed namespace for local definitions
namespace {

// Resource file definitions
_LIT(KResourceFileName,"esmrgui.rsc");

} // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRGUIListQuery::CESMRConfirmationQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CESMRGUIListQuery::CESMRGUIListQuery(
        TESMRListQueryType aType) :
    iType(aType)
    {
    FUNC_LOG;
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CESMRGUIListQuery::~CESMRConfirmationQuery
// -----------------------------------------------------------------------------
//
CESMRGUIListQuery::~CESMRGUIListQuery()
    {
    FUNC_LOG;
    iEnv->DeleteResourceFile(iResourceOffset);
    }

// -----------------------------------------------------------------------------
// CESMRGUIListQuery::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRGUIListQuery::ConstructL()
    {
    FUNC_LOG;
    TFileName filename;
   
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
// CESMRGUIListQuery::ExecuteLD
// -----------------------------------------------------------------------------
//
TInt CESMRGUIListQuery::ExecuteLD()
    {
    FUNC_LOG;
    TInt resourceId( KErrNotFound );

    switch ( iType )
        {
        case EESMRSendUpdateToAllQuery:
            {
            resourceId = R_MEET_REQ_SEND_UPDATE_QUERY;
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            break;
            }            
        }

    TInt response( KErrCancel );
    CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog( &response );
    if( !dlg->ExecuteLD(resourceId ) )
        {
        response = KErrCancel;
        }
    dlg = NULL;

    delete this;
    return response;
    }

// -----------------------------------------------------------------------------
// CESMRGUIListQuery::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRGUIListQuery::ExecuteL(
        TESMRListQueryType aType)
    {
    FUNC_LOG;
    CESMRGUIListQuery* query =
            new (ELeave) CESMRGUIListQuery(aType);

    CleanupStack::PushL( query );
    query->ConstructL( );

    TInt ret = query->ExecuteLD();

    CleanupStack::Pop( query );

    return ret;
    }

//  End of File

