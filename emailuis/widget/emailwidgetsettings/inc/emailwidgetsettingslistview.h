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
#include <ecom/ecom.h>
#include <emailobserverplugin.h>
#include <memailobserverlistener.h>

#include "emailwidgetsettingsappui.h"
#include "emailwidgetsettingsmailboxes.h"

extern const TUid KEmailWidgetSettingsListViewId;

class CEmailWidgetSettingsListView;
class CEmailWidgetSettingsListViewContainer :
    public CCoeControl, 
    public EmailInterface::MEmailObserverListener
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
    
    // From MEmailObserverListener
    virtual void EmailObserverEvent( EmailInterface::MEmailData& aEmailData );

private:
	void CreateCbaL(MEikCommandObserver* aObserver);
	void CreateListBoxL(MEikListBoxObserver* aObserver);
    void SetupListIconsL();
    void AppendIconL(
            CArrayPtr<CGulIcon>* aIcons,
            const TInt aFileBitmapId, 
            const TInt aFileMaskId);
    TInt AppendExternalIconL(
            CArrayPtr<CGulIcon>* aIcons,
            const TDesC& aMifPath,
            const TInt aFileBitmapId, 
            const TInt aFileMaskId);

	TInt GetSettingToAssociateL(const TDesC& aCid, const TBool aNativeBox, CRepository* aCenRep);
	void LaunchEmailWizardL();
	
	void ResetExtAccountWithSameId( const TDesC& aContentId, CRepository* aCenRep );
    void ResetNatAccountWithSameId( const TDesC& aContentId, CRepository* aCenRep );

private: // from MObjectProvider
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

public:
	CEikColumnListBox*                     iListBox;
	
private:
	CEikButtonGroupContainer*              iPopoutCba;
	CDesCArrayFlat*                        iAccountNames;
	CDesCArrayFlat*                        iDomains;
	CArrayFixFlat<TFSMailMsgId>*           iAccountIds;
	CEmailWidgetSettingsMailboxes*         iMailboxes;
	CEikonEnv*                             iEnv;
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
	};

#endif
