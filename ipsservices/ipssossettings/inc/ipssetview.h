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
* Description:  Declearation for exception handling.
*
*/

#ifndef IPSSETVIEW_H_
#define IPSSETVIEW_H_

// SYSTEM INCLUDES
//<cmail>
#include "CFSMailCommon.h"
#include "ESMailSettingsPlugin.h"
//</cmail>


// class declaration
class CIpsSetView : public CESMailSettingsPlugin
	{
public:

    static CIpsSetView* NewL();
    ~CIpsSetView();
	
	/**
	 * from base class CAknView
	 */
	TUid Id() const;
		
protected:

	// from base class CAknView

	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );

	void DoDeactivate();

private:
	CIpsSetView();
	void ConstructL();
	
	static TInt RunSettingsDlgL( TAny* aSelfPtr );
public: 


    // from CESMailSettingsUiPlugin

	TInt MailSettingsSubviewCount();
	  
	TPtrC MailSettingsSubviewCaption( 
			TFSMailMsgId aAccountId, 
			TInt aSubviewIndex, 
			TBool aLongCaption );

	TBool CanHandlePIMSync();
	
	TPtrC LocalizedProtocolName();
	
	TBool IsSelectedForPIMSync();
	
	void SelectForPIMSync( TBool aSelectForSync );

	TInt PIMSettingsSubviewCount();
	
	TPtrC PIMSettingsSubviewCaption( 
			TFSMailMsgId aAccountId, 
			TInt aSubviewIndex, 
			TBool aLongCaption );

private:
    CAsyncCallBack* iAsyncCallback;
    TVwsViewId iPrevViewId;
    HBufC8* iCustomMessage;
	};

#endif //IPSSETVIEW_H_

// End of File
