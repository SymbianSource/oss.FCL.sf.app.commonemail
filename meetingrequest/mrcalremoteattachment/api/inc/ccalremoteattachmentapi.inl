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
* Description:
*
*/

#include "calremoteattachmentapidefs.h"
#include <ecom/ecom.h>

// simple wrapper class with array deletion on close
class RImplInfoPtrArrayCalRemoteAttachment : public RImplInfoPtrArray
    {
    public:
        void Close()
            {
            ResetAndDestroy();
            RImplInfoPtrArray::Close();
            }
    };

// -----------------------------------------------------------------------------
// CCalRemoteAttachmentApi::NewL
// -----------------------------------------------------------------------------
//
inline CCalRemoteAttachmentApi* CCalRemoteAttachmentApi::NewL(
        const TDesC8& aUri )
    {
    TAny* ptr( NULL );

    RImplInfoPtrArrayCalRemoteAttachment implArray;
    CleanupClosePushL( implArray );
    REComSession::ListImplementationsL(
            TUid::Uid( KCalRemoteAttachmentAPIInterface ),
            implArray );

    TInt implCount( implArray.Count() );
    if ( !implCount )
        {
        User::Leave( KErrNotFound );
        }

    for ( TInt i(0); i < implCount && !ptr; ++i )
        {
        TPtrC8 datatype( implArray[i]->DataType() );
        if ( KErrNotFound != aUri.Find( datatype) )
            {
            TInt32 keyOffset = _FOFF( CCalRemoteAttachmentApi, iDtor_ID_Key );
            TUid implUid = implArray[i]->ImplementationUid();
            ptr = REComSession::CreateImplementationL( implUid,
                                                       keyOffset,
                                                       NULL );
            }
        }

    User::LeaveIfNull( ptr );
    CleanupStack::PopAndDestroy( &implArray );
    return reinterpret_cast<CCalRemoteAttachmentApi*>(ptr);
    }

// -----------------------------------------------------------------------------
// CCalRemoteAttachmentApi::~CCalRemoteAttachmentApi
// -----------------------------------------------------------------------------
//
inline CCalRemoteAttachmentApi::~CCalRemoteAttachmentApi()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// EOF
