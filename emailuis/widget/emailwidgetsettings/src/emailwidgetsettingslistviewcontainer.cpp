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
* Description:  CEmailWidgetSettingsListViewContainer implementation
*
*/

// INCLUDE FILES
#include <eikclbd.h>
#include <eikmop.h>
#include <bacline.h>
#include <StringLoader.h>
#include <emailwidgetsettings.rsg>

#include "emailtrace.h"
#include "emailwidgetsettingslistview.h"
#include "cmailwidgetcenrepkeys.h"

using namespace AknLayout;

const TInt KMaxMailboxCount = 18;
_LIT( KDissociated,"0");
const TInt KMaxDescLen = 256;
const TUid KUidWizardApp = { 0x10281c96 };
const TUid KUidEmailWizardView = { 0x10281C9A };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmailWidgetSettingsListViewContainer::CEmailWidgetSettingsListViewContainer()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::ConstructL(CEmailWidgetSettingsListView* aView, 
                                                       const TRect& aRect)
    {
    FUNC_LOG;
    iEnv = CEikonEnv::Static( );
    iMailboxes = CEmailWidgetSettingsMailboxes::NewL();    
    CreateWindowL();
    SetBlank();

    CreateCbaL(aView);
    CreateListBoxL(aView);
    SetRect(aRect);
    ActivateL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::CreateListBoxL(MEikListBoxObserver* aObserver)
    {
    TInt count = iMailboxes->MailboxCount();

    iListBox = new(ELeave) CAknSingleStyleListBox;
    iListBox->SetContainerWindowL( *this);
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iAccountNames = new (ELeave) CDesCArrayFlat(count + 1);
    if (count)
        {
		iAccountIds = new ( ELeave ) CArrayFixFlat<TFSMailMsgId>(count);
		}
    for (TInt i = 0; i < count; i++)
        {
        TBuf<KMaxDescLen> name;        
        TFSMailMsgId id;
        iMailboxes->GetMailboxNameL(i, name);
        iMailboxes->GetMailboxIdL(i, id);
        iAccountNames->AppendL(name);
        iAccountIds->AppendL(id);
        }
    HBufC* createNewMailbox = StringLoader::LoadLC(R_EMAILWIDGETSETTINGS_CREATE_NEW_MAILBOX);
    iAccountNames->AppendL(createNewMailbox->Des());
    CleanupStack::PopAndDestroy(createNewMailbox);
    iListBox->Model()->SetItemTextArray(iAccountNames);
    
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    iListBox->SetBorder(TGulBorder::ESingleBlack);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iListBox->SetListBoxObserver(aObserver);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::CreateCbaL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::CreateCbaL( MEikCommandObserver* aObserver )
    {
    const TSize screenSize= iCoeEnv->ScreenDevice()->SizeInPixels();
          iPopoutCba = CEikButtonGroupContainer::NewL(CEikButtonGroupContainer::ECba,
            CEikButtonGroupContainer::EHorizontal, aObserver, R_AVKON_SOFTKEYS_SELECT_CANCEL);
    iPopoutCba->SetBoundingRect(TRect(screenSize));
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::~CEmailWidgetSettingsListViewContainer
// ---------------------------------------------------------------------------
//
CEmailWidgetSettingsListViewContainer::~CEmailWidgetSettingsListViewContainer()
    {
    delete iMailboxes;    
    delete iPopoutCba;
    delete iListBox;
    delete iAccountNames;
    delete iAccountIds;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::SizeChanged()
    {
    TRect mainPaneRect;
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
		TRect listBoxRect(mainPaneRect.Size());
		iListBox->SetRect(listBoxRect);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CEmailWidgetSettingsListViewContainer::CountComponentControls() const
    {
    return iListBox ? 1 : 0;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CEmailWidgetSettingsListViewContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CEmailWidgetSettingsListViewContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    return iListBox->OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CEmailWidgetSettingsListViewContainer::MinimumSize()
    {
    return iEikonEnv->EikAppUi()->ClientRect().Size();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::CurrentIndex
// ---------------------------------------------------------------------------
//
TInt CEmailWidgetSettingsListViewContainer::CurrentIndex() const
    {
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::MopSupplyObject
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CEmailWidgetSettingsListViewContainer::MopSupplyObject(TTypeUid aId)
    {
    return SupplyMopObject(aId, iPopoutCba);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::SaveSelectedL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::SaveSelectedL()
    {
    if (CurrentIndex() == iAccountNames->Count() - 1)
        {
        LaunchEmailWizardL();
        return;
        }

    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    const TInt argumentCount = args->Count();
    if (argumentCount!=2)
        {
        User::LeaveIfError(KErrNotFound);
        }  
    CRepository* cenRep = CRepository::NewL( KCRUidCmailWidget );

    if ( cenRep )
        {
        TInt setId (GetSettingToAssociateL(args->Arg(1)));
        if (setId >= 0)
            {
            TUint32 errorKey;
            TFSMailMsgId msgId = (*iAccountIds)[CurrentIndex()];
            TInt id = msgId.Id();
            TUid pluginId = msgId.PluginId();
            TInt pId(pluginId.iUid);
            cenRep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
            cenRep->Set( KCMailMailboxIdBase+setId, id );
            cenRep->Set( KCMailPluginIdBase+setId, pId );
            cenRep->Set( KCMailWidgetContentIdBase+setId, args->Arg(1) );
            cenRep->CommitTransaction(errorKey);
            }
        }
    delete cenRep;
    CleanupStack::PopAndDestroy(args);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::GetSettingToAssociateL
// ---------------------------------------------------------------------------
//
TInt CEmailWidgetSettingsListViewContainer::GetSettingToAssociateL(const TDesC& aCid)
    {
    FUNC_LOG;
    TInt ret(KErrNotFound);
    CRepository* cenRep = CRepository::NewL( KCRUidCmailWidget );
    for (TInt i = 0; i < KMaxMailboxCount; i++)
        {
        TBuf<KMaxDescLen> value;
        TUint32 key(KCMailWidgetContentIdBase+i);
        cenRep->Get( key, value );
        TInt result = value.Compare(aCid);
        if (!result)
            {
            ret = i;
            break;
            }
        }
    if (ret < 0 )
        {
        for (TInt i = 0; i < KMaxMailboxCount; i++)
            {       
            TBuf<KMaxDescLen> value;
            TUint32 key(KCMailWidgetContentIdBase+i);
            cenRep->Get( key, value );
            TInt result = value.Compare(KDissociated);
            if (!result)
                {
                ret = i;
                break;
                }
            }
        }
    delete cenRep;
    return ret;
    }

// -----------------------------------------------------------------------------
//  CEmailWidgetSettingsListViewContainer::LaunchEmailWizardL
//  Launches Email Settings Wizard
// -----------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::LaunchEmailWizardL()
    {
    if ( iEnv )
        {
        CCoeAppUi* appui = iEnv-> EikAppUi( );
        if ( appui )
            {
            appui->ActivateViewL(TVwsViewId(KUidWizardApp, KUidEmailWizardView));
            }
        }
    }

// End of File
