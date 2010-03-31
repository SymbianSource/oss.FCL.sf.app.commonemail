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
* Description: This file implements class CIpsSetView.
*
*/


#include "emailtrace.h"
#include <ecom/implementationproxy.h>
#include <msvstd.h>
#include <aknViewAppUi.h>
#include <aknclearer.h>
//<cmail>
#include "FreestyleEmailUiConstants.h"
//</cmail>
#include <ipssossettings.rsg>

#include "ipssetview.h"
#include "ipssetui.h"
#include "../../ipssosplugin/inc/ipsplgsosbaseplugin.hrh"

// CONTANTS
const TInt KSubViewCountZero = 0;

// interface implementation table
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
    	IPS_SET_ECOM_IMPLEMENTATION_UID,
        CIpsSetView::NewL )
    };

// CESMailSettingsUiPlugin implementation

CIpsSetView* CIpsSetView::NewL()
    {
    FUNC_LOG;
    CIpsSetView* self = new (ELeave) CIpsSetView();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
    return self;
    }

void CIpsSetView::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    TCallBack callbackFunc( RunSettingsDlgL, this );
    iAsyncCallback = new (ELeave) CAsyncCallBack( callbackFunc, CActive::EPriorityStandard );
    }

CIpsSetView::~CIpsSetView()
    {
    FUNC_LOG;
    delete iAsyncCallback;
    delete iCustomMessage;
    if (iLocalScreenClearer != NULL)
        {
        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        }
    }

// Constructor
CIpsSetView::CIpsSetView()
    {
    FUNC_LOG;
    }

TUid CIpsSetView::Id() const
    {
    FUNC_LOG;
    return TUid::Uid( IPS_SET_ECOM_IMPLEMENTATION_UID );
    }

void CIpsSetView::DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid /*aCustomMessageId*/,
		const TDesC8& aCustomMessage )
    {
    FUNC_LOG;
    iPrevViewId = aPrevViewId;
    delete iCustomMessage;
    iCustomMessage = NULL;
    iCustomMessage = aCustomMessage.AllocL();
    iAsyncCallback->CallBack();
    iLocalScreenClearer = CAknLocalScreenClearer::NewL( EFalse );
    }

void CIpsSetView::DoDeactivate()
    {
    FUNC_LOG;
    if (iLocalScreenClearer != NULL)
        {
        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        }
    }

TPtrC CIpsSetView::MailSettingsSubviewCaption(
		TFSMailMsgId /*aAccountId*/,
		TInt /*aSubviewIndex*/,
		TBool /*aLongCaption*/ )
    {
    FUNC_LOG;
    return TPtrC( KNullDesC );
    }

TInt CIpsSetView::MailSettingsSubviewCount()
    {
    FUNC_LOG;
    return KSubViewCountZero;
    }

TBool CIpsSetView::CanHandlePIMSync()
    {
    FUNC_LOG;
    return EFalse;
    }

TPtrC CIpsSetView::LocalizedProtocolName()
    {
    FUNC_LOG;
    return TPtrC( KNullDesC );
    }

TBool CIpsSetView::IsSelectedForPIMSync()
    {
    FUNC_LOG;
    return EFalse;
    }

void CIpsSetView::SelectForPIMSync( TBool /*aSelectForSync*/ )
    {
    FUNC_LOG;
    }

TInt CIpsSetView::PIMSettingsSubviewCount()
    {
    FUNC_LOG;
    return KSubViewCountZero;
    }

TPtrC CIpsSetView::PIMSettingsSubviewCaption(
		TFSMailMsgId /*aAccountId*/,
		TInt /*aSubviewIndex*/,
		TBool /*aLongCaption*/ )
    {
    FUNC_LOG;
    return TPtrC( KNullDesC );
    }



EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

TInt CIpsSetView::RunSettingsDlgL( TAny* aSelfPtr )
    {
    FUNC_LOG;
    CIpsSetView* self = static_cast<CIpsSetView*>( aSelfPtr );

    TSubViewActivationData subView;
    subView.iSubviewId = 0;
    TPckgBuf<CESMailSettingsPlugin::TSubViewActivationData> subViewData( subView ); //<cmail>
    subViewData.Copy( *self->iCustomMessage );
    subView = subViewData();

    TInt buttonId( KErrNotFound );
    CIpsSetUi* setUi = CIpsSetUi::NewL( subView.iAccount.Id(), subView.iLaunchFolderSettings );//<cmail>

    // leaves if user choose cancel in queries,
    // then just exit settings
    TRAP_IGNORE( buttonId = setUi->ExecuteLD( R_IPS_SET_SETTINGS_DIALOG ) );

    if( buttonId == EAknSoftkeyExit )
        {
        // Exit Freestyle app
        User::Exit( KErrCancel );
        }
    else
        {
        //refresh background
        self->iLocalScreenClearer->DrawNow();
        
    	//check if composer is active view; needed to handle special case when email is plugin settings and composer is activated externaly
    	TVwsViewId aViewId;
    	self->AppUi()->GetActiveViewId(aViewId);
    	if (aViewId.iViewUid == MailEditorId )
    		{
    		//just skip 
    		}
    	else
    		{
    		// go back to previous view
    		self->AppUi()->ActivateViewL( self->iPrevViewId,
										  TUid::Uid(KMailSettingsReturnFromPluginSettings),
										  *self->iCustomMessage );
    		}
        }
    return KErrNone;
    }
// End of File

