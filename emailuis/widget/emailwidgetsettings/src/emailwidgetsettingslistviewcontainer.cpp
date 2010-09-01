/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <AknIconArray.h>
#include <emailwidgetsettings.rsg>
#include <emailwidgetsettings.mbg>
#include <emailobserverinterface.hrh>
#include <memaildata.h>

#include "emailtrace.h"
#include "emailwidgetsettingslistview.h"
#include "cmailwidgetcenrepkeys.h"
#include "cfsmailclient.h"

using namespace AknLayout;
using namespace EmailInterface;

const TInt KMaxMailboxCount = 18;
const TInt KMaxExternalBoxCount = 10;
_LIT( KDissociated,"0");
const TInt KMaxDescLen = 256;
const TUid KUidWizardApp = { 0x10281c96 };
const TUid KUidEmailWizardView = { 0x10281C9A };
_LIT( KMifPath, "z:\\resource\\apps\\emailwidgetsettings.mif");
_LIT( KMifPrefix, "mif(" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::CEmailWidgetSettingsListViewContainer
// ---------------------------------------------------------------------------
//
CEmailWidgetSettingsListViewContainer::CEmailWidgetSettingsListViewContainer()
    {
    FUNC_LOG;        
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::ConstructL
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
// CEmailWidgetSettingsListViewContainer::CreateListBoxL 
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::CreateListBoxL(MEikListBoxObserver* aObserver)
    {
    FUNC_LOG;      
    
    
    TInt nativeMailboxCount = iMailboxes->MailboxCount();

    iListBox = new(ELeave) CAknSingleGraphicStyleListBox;
    iListBox->SetContainerWindowL( *this);
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    
    iAccountNames = new (ELeave) CDesCArrayFlat(nativeMailboxCount + 1); // +1 for creating new mailbox
    iDomains = new (ELeave) CDesCArrayFlat(1);
	iAccountIds = new ( ELeave ) CArrayFixFlat<TFSMailMsgId>(1);
	
    // First items on the selection list are native mailboxes.
	// Append name, domain and id information to the arrays.
	for (TInt i = 0; i < nativeMailboxCount; i++)
        {
        TBuf<KMaxDescLen> name;
        TBuf<KMaxDescLen> domain;        
        TFSMailMsgId id;
        iMailboxes->GetMailboxNameL(i, name);
        iMailboxes->GetDomainL(i, domain);
        iMailboxes->GetMailboxIdL(i, id);
        iAccountNames->AppendL(name);
        iDomains->AppendL(domain);
        iAccountIds->AppendL(id);
        }
    
    // Continue with external mailboxes.
	// List plugins implementing the interface.
    TUid interfaceUid = TUid::Uid( KEmailObserverInterfaceUid );
    RImplInfoPtrArray pluginArray;
    REComSession::ListImplementationsL( interfaceUid, pluginArray);
   
    const TInt pluginCount = pluginArray.Count();
    INFO_1("***** NUMBER OF plugins == %d", pluginCount);
    for ( TInt ii = 0; ii < pluginCount; ii++ )
        {
        TUid implUid = pluginArray[ii]->ImplementationUid();
       
        // load external email client plugin
        INFO_1("***** BEFORE instantiating plugin %d", implUid.iUid);
        EmailInterface::CEmailObserverPlugin* plugin = EmailInterface::CEmailObserverPlugin::NewL( implUid, this );
        CleanupStack::PushL( plugin );
        INFO("***** AFTER instantiating plugin ");
        
        // list mailboxes of the plugin
        MEmailData& data( plugin->EmailDataL() );
        RPointerArray<MMailboxData> extMailboxes = data.MailboxesL();
        
        const TInt extMailboxCount = extMailboxes.Count();
        
        for ( TInt jj = 0; jj < extMailboxCount; jj++ )
            {
            // Append mailbox name using indexed format ( e.g. "1\tMailboxName\t\t" )
            TBuf<KMaxDescLen> name;
            name.AppendNum(nativeMailboxCount + 1 + jj);
            name.Append(_L("\t"));
            name.Append(extMailboxes[jj]->Name());
            name.Append(_L("\t\t"));
            iAccountNames->AppendL(name);
            
            // Instead of domain information, external mail clients are requested
            // to provide a string containing a mif file path and icon/mask numbers.
            // The provided string is stored to domain list.
            iDomains->AppendL( extMailboxes[jj]->BrandingIcon());
            

            // Combine plugin uid and mailbox id and add to the list.
            TFSMailMsgId uid( implUid, extMailboxes[jj]->MailboxId() );
            iAccountIds->AppendL( uid );
            }
        CleanupStack::PopAndDestroy(plugin);
        plugin = NULL;
        }
    pluginArray.ResetAndDestroy();
  
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
    

    SetupListIconsL();

    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::SetupListIconsL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::SetupListIconsL()
    {
    FUNC_LOG;        
    CFSMailClient* mailClient = CFSMailClient::NewL();
    CleanupClosePushL(*mailClient);
    MFSMailBrandManager& brandManager = mailClient->GetBrandManagerL();
    
    TInt count = iAccountIds->Count();
    
    CArrayPtr<CGulIcon>* icons = new(ELeave) CArrayPtrFlat<CGulIcon>( count + 1 );
    CleanupStack::PushL( icons );
    
    // Add "new mailbox" icon to the list
    AppendIconL(icons, EMbmEmailwidgetsettingsQgn_prop_cmail_new_mailbox, 
                       EMbmEmailwidgetsettingsQgn_prop_cmail_new_mailbox_mask);

    for (TInt i = 0; i < count; i++)
        {
        CGulIcon* brandIcon;
        brandIcon = brandManager.GetGraphicL( EFSMailboxIcon, iDomains->MdcaPoint(i) );
        
        if (brandIcon)
            {
            // Add branded mailbox icon to the list
            icons->AppendL(brandIcon);
            }
        else
            {
            TInt err(KErrNotFound);
            
            // Check if domain list item contains mif file path (instead of domain)
            // Path follows the format: "mif(z:\\resource\\apps\\myemailplugin.mif N1 N2)"    
        
            TBufC<KMaxDescLen> temp( iDomains->MdcaPoint(i) );
            if( temp.Find(KMifPrefix) == 0 )
                {        
                TInt p1 = KMifPrefix().Length();
                TInt p2 = temp.Locate(' ') + 1;
                TInt p3 = temp.LocateReverse(' ') + 1;
                TInt end = temp.Length() - 1;

                TInt p1Len = p2-p1-1;
                TInt p2Len = p3-p2-1;
                TInt p3Len = end-p3;

                // negative lengths not allowed
                if ( p1Len > 0 && p2Len > 0 && p3Len > 0 )
                    {
                    TBufC<KMaxDescLen>  mifPath(temp.Mid( p1, p1Len ));
                    TBufC<16>        iconString(temp.Mid( p2, p2Len ));
                    TBufC<16>        maskString(temp.Mid( p3, p3Len )); 
                    
                    TLex lexIcon(iconString);
                    TLex lexMask(maskString);
                    TInt icon(0), mask(0);     
                    lexIcon.Val(icon);
                    lexMask.Val(mask);
                    
                    // Add 3rd party mailbox icon to the list
                    err = AppendExternalIconL( icons, mifPath, icon, mask );
                    }
                }   
            if ( err )
                {
                // Add default mailbox icon to the list
                AppendIconL(icons, EMbmEmailwidgetsettingsQgn_indi_cmail_drop_email_account, 
                                   EMbmEmailwidgetsettingsQgn_indi_cmail_drop_email_account_mask);
                }
            }
        }
    
    // clear any previous icon array
    CAknIconArray* oldIconArray = static_cast<CAknIconArray*>(iListBox->ItemDrawer()->ColumnData()->IconArray());
    if (oldIconArray)
        delete oldIconArray;
    
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
    CleanupStack::Pop(icons);
    CleanupStack::PopAndDestroy( mailClient );
    }

// -----------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::AppendIconL
//
// Loads and appends an icon to the icon array.
// -----------------------------------------------------------------------------
//

void CEmailWidgetSettingsListViewContainer::AppendIconL(
        CArrayPtr<CGulIcon>* aIcons,
        const TInt aFileBitmapId, 
        const TInt aFileMaskId)
    {
    FUNC_LOG;
    CGulIcon*    newIcon;
    CFbsBitmap*  newIconBmp;
    CFbsBitmap*  newIconMaskBmp;
    AknIconUtils::CreateIconLC( newIconBmp, newIconMaskBmp, KMifPath,
            aFileBitmapId,
            aFileMaskId );   
    newIcon = CGulIcon::NewL(newIconBmp, newIconMaskBmp);
    CleanupStack::Pop(newIconMaskBmp);
    CleanupStack::Pop(newIconBmp);
    CleanupStack::PushL(newIcon);
    aIcons->AppendL(newIcon);
    CleanupStack::Pop(newIcon);
    }

// -----------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::AppendExternalIconL
//
// Loads and appends an 3rd party icon to the icon array.
// -----------------------------------------------------------------------------
//
TInt CEmailWidgetSettingsListViewContainer::AppendExternalIconL(
        CArrayPtr<CGulIcon>* aIcons,
        const TDesC& aMifPath,
        const TInt aFileBitmapId, 
        const TInt aFileMaskId)
    {
    FUNC_LOG;
    CGulIcon*    newIcon;
    CFbsBitmap*  newIconBmp;
    CFbsBitmap*  newIconMaskBmp;
    // Trap when faulty mif path, missing graphic file, etc. 
    TRAPD( err, AknIconUtils::CreateIconL( newIconBmp, newIconMaskBmp, aMifPath,
                aFileBitmapId,
                aFileMaskId ) );
    if (!err)
        {
        CleanupStack::PushL(newIconBmp);
        CleanupStack::PushL(newIconMaskBmp);
        newIcon = CGulIcon::NewL(newIconBmp, newIconMaskBmp);
        CleanupStack::Pop(newIconMaskBmp);
        CleanupStack::Pop(newIconBmp);
        CleanupStack::PushL(newIcon);
        aIcons->AppendL(newIcon);
        CleanupStack::Pop(newIcon);
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::CreateCbaL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::CreateCbaL( MEikCommandObserver* aObserver )
    {
    FUNC_LOG;        
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
    FUNC_LOG;        
    delete iMailboxes;    
    delete iPopoutCba;
    delete iListBox;
    delete iAccountNames;
    delete iDomains;
    delete iAccountIds;
    REComSession::FinalClose();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::SizeChanged()
    {
    FUNC_LOG;        
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
    FUNC_LOG;        
    return iListBox ? 1 : 0;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CEmailWidgetSettingsListViewContainer::ComponentControl(TInt /*aIndex*/) const
    {
    FUNC_LOG;        
    return iListBox;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CEmailWidgetSettingsListViewContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    FUNC_LOG;        
    return iListBox->OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CEmailWidgetSettingsListViewContainer::MinimumSize()
    {
    FUNC_LOG;        
    return iEikonEnv->EikAppUi()->ClientRect().Size();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::CurrentIndex
// ---------------------------------------------------------------------------
//
TInt CEmailWidgetSettingsListViewContainer::CurrentIndex() const
    {
    FUNC_LOG;        
    return iListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::MopSupplyObject
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CEmailWidgetSettingsListViewContainer::MopSupplyObject(TTypeUid aId)
    {
    FUNC_LOG;        
    return SupplyMopObject(aId, iPopoutCba);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::SaveSelectedL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::SaveSelectedL()
    {
    FUNC_LOG;

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

    // Check whether selected box is native or external mailbox
    TInt index = CurrentIndex();
    TInt nativeBoxCount = iMailboxes->MailboxCount();

    TBool nativeBox(ETrue);
    if ( index >= nativeBoxCount )
        {
        nativeBox = EFalse;
        }
    
    if ( cenRep )
        {
        TInt setId ( GetSettingToAssociateL(args->Arg(1), nativeBox, cenRep) );
        if (setId >= 0)
            {
            TFSMailMsgId msgId = (*iAccountIds)[CurrentIndex()];
            TInt id = msgId.Id();
            TUid pluginId = msgId.PluginId();
            TInt pId(pluginId.iUid);
            TInt ret = cenRep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);

            if ( nativeBox )
                {
                ResetExtAccountWithSameId( args->Arg(1), cenRep );
            
                ret = cenRep->Set( KCMailMailboxIdBase+setId, id );
                ret = cenRep->Set( KCMailPluginIdBase+setId, pId );
                ret = cenRep->Set( KCMailWidgetContentIdBase+setId, args->Arg(1) );
                }
            else // external mailbox
                {
                ResetNatAccountWithSameId( args->Arg(1), cenRep );
            
                ret = cenRep->Set( KCMailExtMailboxBase+setId, id );
                ret = cenRep->Set( KCMailExtMailboxBase+KCMailExtPluginIdOffset+setId, pId );
                ret = cenRep->Set( KCMailExtMailboxBase+KCMailExtWidgetCidOffset+setId, args->Arg(1) );          
                }
            
            TUint32 errorKey( 0 );
            cenRep->CommitTransaction(errorKey);
            INFO_1("Errorkey == %d", errorKey);
            }
        }
    delete cenRep;
    CleanupStack::PopAndDestroy(args);
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::ResetExtAccountWithSameId
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::ResetExtAccountWithSameId( const TDesC& aContentId, CRepository* aCenRep )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < KMaxExternalBoxCount; i++ )
        {
        TBuf<10> value;
        TInt ret = aCenRep->Get( KCMailExtMailboxBase+KCMailExtWidgetCidOffset+i, value );
        if ( !value.Compare( aContentId ) )
            {
            // found
            ret = aCenRep->Set(KCMailExtMailboxBase+i, 0);
            ret = aCenRep->Set(KCMailExtMailboxBase+KCMailExtPluginIdOffset+i, 0);
            ret = aCenRep->Set(KCMailExtMailboxBase+KCMailExtWidgetCidOffset+i, KDissociated);
            }
        }
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::ResetNatAccountWithSameId
// ---------------------------------------------------------------------------
void CEmailWidgetSettingsListViewContainer::ResetNatAccountWithSameId( const TDesC& aContentId, CRepository* aCenRep )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < KMaxMailboxCount; i++ )
        {
        TBuf<10> value;
        TInt ret = aCenRep->Get( KCMailWidgetContentIdBase+i, value );
        if ( !value.Compare( aContentId ) )
            {
            // found
            ret = aCenRep->Set(KCMailMailboxIdBase+i, 0);
            ret = aCenRep->Set(KCMailPluginIdBase+i, 0);
            ret = aCenRep->Set(KCMailWidgetContentIdBase+i, KDissociated);
            }
        }
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsListViewContainer::GetSettingToAssociateL
// ---------------------------------------------------------------------------
//
TInt CEmailWidgetSettingsListViewContainer::GetSettingToAssociateL(const TDesC& aCid,
                                                                   const TBool aNativeBox,
                                                                   CRepository* aCenRep)
    {
    FUNC_LOG;
    TUint32 keyBase;
    TInt mailboxCount;
    if (aNativeBox)
        {
        keyBase = KCMailWidgetContentIdBase;
        mailboxCount = KMaxMailboxCount;
        }
    else // external mailbox
        {
        keyBase = KCMailExtMailboxBase + KCMailExtWidgetCidOffset;
        mailboxCount = KMaxExternalBoxCount;
        }
    
    TInt ret(KErrNotFound);

    // Search selected widget content id from cenrep settings 
    for (TInt i = 0; i < mailboxCount; i++)
        {
        TBuf<KMaxDescLen> value;
        aCenRep->Get( keyBase+i, value );
        TInt result = value.Compare(aCid);
        if (!result)
            {
            // Content id found
            ret = i;
            break;
            }
        }
    if (ret < 0 )
        {
        // Content id not found. Search first free space.
        for (TInt i = 0; i < mailboxCount; i++)
            {       
            TBuf<KMaxDescLen> value;
            aCenRep->Get( keyBase+i, value );
            TInt result = value.Compare(KDissociated);
            if (!result)
                {
                // Free space found
                ret = i;
                break;
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
//  CEmailWidgetSettingsListViewContainer::LaunchEmailWizardL
//  Launches Email Settings Wizard
// -----------------------------------------------------------------------------
//
void CEmailWidgetSettingsListViewContainer::LaunchEmailWizardL()
    {
    FUNC_LOG;        
    if ( iEnv )
        {
        CCoeAppUi* appui = iEnv-> EikAppUi( );
        if ( appui )
            {
            appui->ActivateViewL(TVwsViewId(KUidWizardApp, KUidEmailWizardView));
            }
        }
    }

void CEmailWidgetSettingsListViewContainer::EmailObserverEvent(
    EmailInterface::MEmailData& /*aEmailData*/ )
    {
    FUNC_LOG;
    }

// End of File
