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
* Description:  Definition of the class CPbkxRclCbRSender.

*
*/




#include "emailtrace.h"
#include <pbkxrclengine.rsg>

#include <AiwServiceHandler.h>

#include <AiwPoCParameters.h>

#include <CPbkContactItem.h>

#include <coemain.h>



#include "cpbkxrclcbrsender.h"

#include "pbkxrclutils.h"

#include "pbkxrclengineuids.hrh"



// ======== MEMBER FUNCTIONS ========



// ---------------------------------------------------------------------------

// CPbkxRclCbRSender::NewL

// ---------------------------------------------------------------------------

//

CPbkxRclCbRSender* CPbkxRclCbRSender::NewL()

    {
    FUNC_LOG;
    CPbkxRclCbRSender* sender = CPbkxRclCbRSender::NewLC();

    CleanupStack::Pop( sender );

    return sender;

    }



// ---------------------------------------------------------------------------

// CPbkxRclCbRSender::NewLC

// ---------------------------------------------------------------------------

//

CPbkxRclCbRSender* CPbkxRclCbRSender::NewLC()

    {
    FUNC_LOG;
    CPbkxRclCbRSender* sender = new ( ELeave ) CPbkxRclCbRSender();

    CleanupStack::PushL( sender );

    sender->ConstructL();

    return sender;

    }



// ---------------------------------------------------------------------------

// CPbkxRclCbRSender::CPbkxRclCbRSender

// ---------------------------------------------------------------------------

//

CPbkxRclCbRSender::CPbkxRclCbRSender() : CBase()

    {
    FUNC_LOG;
    }



// ---------------------------------------------------------------------------

// CPbkxRclCbRSender::~CPbkxRclCbRSender

// ---------------------------------------------------------------------------

//

CPbkxRclCbRSender::~CPbkxRclCbRSender()

    {
    FUNC_LOG;
    delete iServiceHandler;

    }



// ---------------------------------------------------------------------------

// CPbkxRclCbRSender::ConstructL

// ---------------------------------------------------------------------------

//

void CPbkxRclCbRSender::ConstructL()

    {
    FUNC_LOG;
    iServiceHandler = CAiwServiceHandler::NewL();

    iServiceHandler->AttachL( R_RCL_POC_INTEREST );

    }



// ---------------------------------------------------------------------------

// CPbkxRclCbRSender::SendCallbackRequestL

// ---------------------------------------------------------------------------

//

void CPbkxRclCbRSender::SendCallbackRequestL( CPbkContactItem& aContactItem )

    {
    FUNC_LOG;


    // check for PTT settings

    HBufC* ptt = PbkxRclUtils::FieldTextL( &aContactItem, EPbkFieldIdPushToTalk );

    if ( ptt == NULL )

        {

        User::Leave( KErrNotSupported );

        }

    else

        {

        CleanupStack::PushL( ptt );

        }

    

    CAiwGenericParamList& params = iServiceHandler->InParamListL();

    TAiwPocParameterData pocParameter;

    pocParameter.iConsumerAppUid = TUid::Uid( KPbkxRclEngineUid );

    pocParameter.iConsumerWindowGroup =

        CCoeEnv::Static()->RootWin().Identifier();

        

    pocParameter.iCommandId = EAiwPoCCmdSendCallBackRequest;



    TAiwGenericParam param = TAiwGenericParam(

        EGenericParamPoCData,

        TAiwVariant( TAiwPocParameterDataPckg( pocParameter ) ) );

    params.AppendL(param);            

    

    TAiwGenericParam addressParam = TAiwGenericParam( 

        EGenericParamSIPAddress, 

        TAiwVariant( *ptt ) );

  

    params.AppendL( addressParam );            

    

    iServiceHandler->ExecuteServiceCmdL(

        KAiwCmdPoC,

        params,

        iServiceHandler->OutParamListL(), 

        0,

        NULL );

      

    CleanupStack::PopAndDestroy( ptt );



    }



