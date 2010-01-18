/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Asynchronous reloading of web page
*
*/



#include "FSHtmlReloadAO.h"
#include "FreestyleEmailUiHtmlViewerView.h"

CFSHtmlReloadAO* CFSHtmlReloadAO::NewL(  CFsEmailUiHtmlViewerView& aView  )
    {
    CFSHtmlReloadAO* self = new( ELeave ) CFSHtmlReloadAO( aView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
CFSHtmlReloadAO::CFSHtmlReloadAO( CFsEmailUiHtmlViewerView& aView  )
        : CActive( CActive::EPriorityStandard ), iView( aView )
    {
    CActiveScheduler::Add( this );
    }

CFSHtmlReloadAO::~CFSHtmlReloadAO()
    {
    Cancel();
    }


void CFSHtmlReloadAO::ReloadPageAysnc()
    {
    if ( !IsActive() )
        {
        //Complete request immediately
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }



void CFSHtmlReloadAO::ConstructL()
    {
    // Do nothing
    }

// pure virtuals from CActive implemented in this derived class
void CFSHtmlReloadAO::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        iView.ReloadPageL();
        }
    }

void CFSHtmlReloadAO::DoCancel()
    {
    }
