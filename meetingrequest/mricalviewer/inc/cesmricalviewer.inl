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
* Description:  esmricalviewer inline methods 
*
*/


//<cmail>
#include "mruiuids.h"
//</cmail>
#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// CESMRIcalViewer::NewL
// -----------------------------------------------------------------------------
//
inline CESMRIcalViewer* CESMRIcalViewer::NewL(
        MESMRIcalViewerCallback& aCallback )
    {
    const TUid KMRUIImplUid = { KESMRIcalViewerImplementationUid };

    TAny* callBack = static_cast<TAny*>( &aCallback );

    TAny* ptr = REComSession::CreateImplementationL(
            KMRUIImplUid,
            _FOFF(CESMRIcalViewer, iDtor_ID_Key ),
            callBack );

    return reinterpret_cast<CESMRIcalViewer*>(ptr);
    }

// -----------------------------------------------------------------------------
// CESMRIcalViewer::~CESMRIcalViewer
// -----------------------------------------------------------------------------
//
inline CESMRIcalViewer::~CESMRIcalViewer()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// EOF
