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
* Description:  CEmailWidgetSettingsListView implementation
*
*/

// INCLUDE FILES
#include <eikclbd.h>
#include <eikmop.h>
#include <bacline.h>

#include "emailtrace.h"
#include "emailwidgetsettingslistview.h"
#include "cmailwidgetcenrepkeys.h"

const TUid KEmailWidgetSettingsListViewId = {1};

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::ConstructL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListView::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::~CEmailWidgetSettingsListView
// ---------------------------------------------------------------------------
//	
CEmailWidgetSettingsListView::~CEmailWidgetSettingsListView()
    {
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::DoActivateL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListView::DoActivateL(const TVwsViewId& aPrevViewId,TUid /*aCustomMessageId*/,const TDesC8& /*aCustomMessage*/)
    {
    iPrevViewId = aPrevViewId;
    iView = new(ELeave) CEmailWidgetSettingsListViewContainer;
    iView->SetMopParent(this);
    iView->ConstructL(this,AppUi()->ClientRect());
    AppUi()->AddToViewStackL(*this, iView);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::DoDeactivate
// ---------------------------------------------------------------------------
//	
void CEmailWidgetSettingsListView::DoDeactivate()
    {
    if (iView)
        {
        AppUi()->RemoveFromViewStack(*this, iView);
        }
    delete iView;
    iView = NULL;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::Id
// ---------------------------------------------------------------------------
//	
TUid CEmailWidgetSettingsListView::Id() const
    {
    return KEmailWidgetSettingsListViewId;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::HandleListBoxEventL
// ---------------------------------------------------------------------------
//	
void CEmailWidgetSettingsListView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemSingleClicked )
        {    
        iView->SaveSelectedL();
        AppUi()->HandleCommandL(EAknSoftkeyCancel); // Exit app
		}
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::HandleCommandL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListView::HandleCommandL(TInt aCommandId)
    {
    switch (aCommandId)
        {
        case EAknSoftkeyCancel:
            AppUi()->HandleCommandL(aCommandId);
            break;
        case EAknSoftkeySelect:            
            iView->SaveSelectedL();
            AppUi()->HandleCommandL(EAknSoftkeyCancel); // Exit app
            break;            
        default:
            //Handled by App UI
            break;
        }
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::HandleForegroundEventL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListView::HandleForegroundEventL(TBool /*aForeground*/)
    {
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListView::HandleStatusPaneSizeChange
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListView::HandleStatusPaneSizeChange()
    {
    if (iView && iView->iListBox)
        {
        iView->SetRect(ClientRect());
        }
    }

// End of File
