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
* Description:  Declares list view class for application.
 *
*/

#ifndef __EMAILWIDGETSETTINGSLISTVIEW_H_
#define __EMAILWIDGETSETTINGSLISTVIEW_H_

#include <aknlists.h>
#include <aknPopup.h>
#include <AknQueryDialog.h>
#include <aknenv.h>
#include <aknsettingitemlist.h>

#include "emailwidgetsettingsappui.h"
#include "emailwidgetsettingsmailboxes.h"

extern const TUid KEmailWidgetSettingsListViewId;

class CEmailWidgetSettingsListView;
class CEmailWidgetSettingsListViewContainer : public CCoeControl
	{
public:
	CEmailWidgetSettingsListViewContainer();
	void ConstructL(CEmailWidgetSettingsListView* aView, 
	                const TRect& aRect);
	~CEmailWidgetSettingsListViewContainer();

public: 	
	TInt  CurrentIndex() const;
	void  SizeChanged();
	TInt  CountComponentControls() const;
	CCoeControl* ComponentControl(TInt /*aIndex*/) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	TSize MinimumSize();
    void SaveSelectedL();

private:
	void CreateCbaL(MEikCommandObserver* aObserver);
	void CreateListBoxL(MEikListBoxObserver* aObserver);
	TInt GetSettingToAssociateL(const TDesC& aCid);
	void LaunchEmailWizardL();
	
private: // from MObjectProvider
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

public:
	CEikButtonGroupContainer*       iPopoutCba;
	CEikColumnListBox*	            iListBox;
	CDesCArrayFlat*                 iAccountNames;
    CArrayFixFlat<TFSMailMsgId>*    iAccountIds;
	CEmailWidgetSettingsMailboxes*  iMailboxes;
    CEikonEnv*                      iEnv;	
	};

class CEmailWidgetSettingsListView : public CAknView, public MEikListBoxObserver
	{
public:
	void ConstructL();
	~CEmailWidgetSettingsListView();

public:
	TUid Id() const;
	void HandleCommandL(TInt aCommand);

	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

private: 
	void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage);
	void DoDeactivate();

private:
	void HandleForegroundEventL(TBool aForeground);
	void HandleStatusPaneSizeChange();

private:
	TVwsViewId                             iPrevViewId;
	CEmailWidgetSettingsListViewContainer* iView;
	HBufC*                                 iCid;
	};

#endif
