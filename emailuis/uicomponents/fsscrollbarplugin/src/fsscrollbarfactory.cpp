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
* Description: Implements class TFsScrollBarFactory
*
*/



#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include <alf/alfconstants.h>
#include <alf/alflayouthandlers.h>
#include "fsscrollbarfactory.h"
#include "fsscrollbarlayouthandler.h"
#include "fsscrollbarpluginconstants.h"


// ======== LOCAL FUNCTIONS ========

MAlfExtensionFactory* Instance()
    {
    FUNC_LOG;
    TFsScrollbarFactory* me = NULL;
    me = new TFsScrollbarFactory;
    return me;
    }


const TImplementationProxy ImplementationTable[] =
    {
    //<cmail>
    IMPLEMENTATION_PROXY_ENTRY( KFsScrollbarImplementationId,
                                Instance )
    //</cmail>                              
    };


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable )
        / sizeof( TImplementationProxy ) ;
    return ImplementationTable;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// From class MAlfExtensionFactory.
// Creates new instace of required type (unique for factory) Must not take
// ownership of created object.
// ---------------------------------------------------------------------------
//
MAlfExtension* TFsScrollbarFactory::CreateExtensionL(
    const TInt aObjectId,
    const TDesC8& aInitialParams,
    MAlfInterfaceProvider& aResolver )
    {
    FUNC_LOG;
    MAlfExtension* result( NULL );
    switch ( aObjectId )
        {
        case EFsScrollbarCreateScreenLayout:
            {
            TInt2* params = ( TInt2* )aInitialParams.Ptr();

            CHuiControl* owner( NULL );
            CHuiLayout* parentLayout( NULL );

            if ( !params->iInt1 )
                {
                User::Leave( KErrArgument );
                }
            else
                {
                // currently we must use concrete owner as interface does not
                // provide means to transfer ownership
                owner = ( CHuiControl* )aResolver.GetInterfaceL(
                    EHuiObjectTypeControl, params->iInt1 );
                }

            if (params->iInt2) // not compulsory
                {
                parentLayout = ( CHuiLayout* )aResolver.GetInterfaceL(
                    EHuiObjectTypeLayout, params->iInt2 );
                }

            result = CFsScrollbarLayoutHandler::NewL(
                aResolver, owner, parentLayout );
            break;
            }

        default:
            User::Leave( KErrNotSupported );
        }
    return result;
    }


// ---------------------------------------------------------------------------
// From class MAlfExtensionFactory.
// Free resources for instance, usually delete this
// ---------------------------------------------------------------------------
//
void TFsScrollbarFactory::Release()
    {
    FUNC_LOG;
    delete this;
    }

