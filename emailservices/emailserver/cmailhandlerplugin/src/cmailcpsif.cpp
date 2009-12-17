/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  handles interface to LIW and Content Publishing Service
*
*/

#include <cmailhandlerplugin.mbg>
#include <AknUtils.h>
#include <gulicon.h>
#include <bautils.h>
#include <StringLoader.h>
#include <emailwidget.rsg>

#include "emailtrace.h"
#include "CFSMailClient.h"
#include "cmailcpsif.h"
#include "cmailcpsifconsts.h"
#include "FreestyleEmailUiConstants.h"
#include "cmailwidgetcenrepkeysinternal.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMailCpsIf::NewL
// ---------------------------------------------------------------------------
//
CMailCpsIf* CMailCpsIf::NewL( CMailCpsHandler* aMailCpsHandler )
    {
    FUNC_LOG;
    CMailCpsIf* self = new(ELeave) CMailCpsIf( aMailCpsHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::CMailCpsIf
// ---------------------------------------------------------------------------
//
CMailCpsIf::CMailCpsIf( CMailCpsHandler* aMailCpsHandler ) :
    iMailCpsHandler( aMailCpsHandler ),
    iMsgInterface ( NULL ),
    iIgnoreHsNotifications( EFalse )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::ConstructL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::ConstructL()
    {
    FUNC_LOG;
    for (TInt i=0; i<KMaxMailboxCount; i++)
        {
        iAllowedToPublish[i] = EFalse;
        iInactive[i] = EFalse;
        }
    
    AllocateResourcesL();
    iMsgInterface = GetMessagingInterfaceL();
    RegisterForObserverL();
    PublisherRegisteryActionL();
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::~CMailCpsIf
// ---------------------------------------------------------------------------
//
CMailCpsIf::~CMailCpsIf()
    {
    FUNC_LOG;

    TRAP_IGNORE(UnRegisterForObserverL());
    
    if ( iMsgInterface )
        {
        iMsgInterface->Close();
        iMsgInterface = NULL;
        }
    delete iServiceHandler;
    iInstIdList.ResetAndDestroy();  
    CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
    }

// ---------------------------------------------------------
// CMailCpsIf::AllocateResourcesL
// ---------------------------------------------------------
//
void CMailCpsIf::AllocateResourcesL()
    {
    FUNC_LOG;
    // create resourcefile 
    CCoeEnv* env = CCoeEnv::Static();
    
    TFileName resourceFile;
    TParse parse;
    
    parse.Set( KResourceFile, &KDC_ECOM_RESOURCE_DIR, NULL ); 
    resourceFile = parse.FullName();
    
    BaflUtils::NearestLanguageFile( env->FsSession(), resourceFile );
    iResourceFileOffset = env->AddResourceFileL( resourceFile );
    }


// ---------------------------------------------------------------------------
// CMailCpsIf::GetMessagingInterfaceL
// ---------------------------------------------------------------------------
//
MLiwInterface* CMailCpsIf::GetMessagingInterfaceL()
    {
    FUNC_LOG;
    if ( !iServiceHandler )
        {
        iServiceHandler = CLiwServiceHandler::NewL();
        }

    CLiwGenericParamList* inParam = CLiwGenericParamList::NewL();
    CleanupStack::PushL( inParam );
    CLiwGenericParamList* outParam = CLiwGenericParamList::NewL();
    CleanupStack::PushL( outParam );
    CLiwCriteriaItem* crit = CLiwCriteriaItem::NewL( KLiwCmdAsStr, KCPInterface,
        KCPService  );
    crit->SetServiceClass( TUid::Uid(KLiwClassBase) );

    RCriteriaArray criteriaArray;
    criteriaArray.AppendL( crit );

    iServiceHandler->AttachL( criteriaArray );
    iServiceHandler->ExecuteServiceCmdL( *crit, *inParam, *outParam );

    delete crit;
    crit = NULL;
    criteriaArray.Reset();

    // find service interface
    TInt pos = 0;
    MLiwInterface* msgInterface = NULL;
    outParam->FindFirst( pos, KCPInterface );
    if ( pos != KErrNotFound )
        {
        msgInterface = (*outParam)[pos].Value().AsInterface(); // indexing should be safe
        }
    outParam->Reset();
    inParam->Reset();

    if( !msgInterface )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( outParam );
    CleanupStack::PopAndDestroy( inParam );
    return msgInterface;
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::PublishSetupWizardL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::PublishSetupWizardL(TInt instance)
    {
    FUNC_LOG;
    TFSMailMsgId mailBoxId;
    TFSMailMsgId folderId;

    HBufC* setupEmail = StringLoader::LoadLC(R_EMAILWIDGET_TEXT_SETUP_EMAIL);
    
    iSetupUid = PublishDescriptorL(
            KPubId, KContTypeSetupText,
            iInstIdList[instance]->Des(),
            *setupEmail, 
            KKeySetupText, KTriggerWizard8, NULL, mailBoxId, folderId);	
    
    PublishImageL(
            iInstIdList[instance]->Des(),
            KContTypeSetupIcon,
            KKeySetupIcon,
            EMbmCmailhandlerpluginQgn_prop_cmail_new_mailbox,
            EMbmCmailhandlerpluginQgn_prop_cmail_new_mailbox_mask,
            mailBoxId);

    CleanupStack::PopAndDestroy(setupEmail);    
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::PublishActiveMailboxNameL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::PublishActiveMailboxNameL(
    const TInt aInstance,
    const TInt aRowNumber, 
    const TDesC& aMailboxName,
    const TInt aMailBoxCount,
    TFSMailMsgId aMailBoxId,
    TFSMailMsgId aFolderId)
    {
    FUNC_LOG;

    TBuf<KMaxDescLen> row;
    row.Num(aRowNumber);

    TBuf<KMaxDescLen> contentType;
    contentType.Copy(KContTypeBodyText);        
    contentType.Append(row);
    
    TBuf<KMaxDescLen> textKey;
    textKey.Copy(KKeyBodyText);
    textKey.Append(row);
		
    iMailboxNameUid = PublishDescriptorL(
        KPubId, contentType, 
        iInstIdList[aInstance]->Des(),
        aMailboxName,
        textKey, KTriggerEmailUi8, aMailBoxCount,
        aMailBoxId, aFolderId);		
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::PublishIndicatorIconL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::PublishIndicatorIconL( const TInt aInstance, 
                                        const TInt aRowNumber,
                                        const TInt aIcon )
    {
    FUNC_LOG;
    TFSMailMsgId mailBoxId;
    
    TBuf<KMaxDescLen> row;
    row.Num(aRowNumber);

    TBuf<KMaxDescLen> contentType;
    contentType.Copy(KContTypeIndicatorIcon);        
    contentType.Append(row);

    TBuf8<KMaxDescLen> row8;
    row8.Num(aRowNumber);
    
    TBuf8<KMaxDescLen> key;
    key.Copy(KKeyIndicatorIcon);     
    key.Append(row8);

    PublishImageL(
              iInstIdList[aInstance]->Des(),
              contentType,
              key,
              aIcon,
              aIcon + 1,
              mailBoxId);
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::PublishMailboxIconL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::PublishMailboxIconL( const TInt aInstance,
                                      const TInt aRowNumber, 
                                      const TInt aIcon,
                                      TFSMailMsgId aMailBoxId )
    {
    FUNC_LOG;

    TBuf<KMaxDescLen> row;
    row.Num(aRowNumber);

    TBuf<KMaxDescLen> contentType;
    contentType.Copy(KContTypeMailboxIcons);        
    contentType.Append(row);

    TBuf8<KMaxDescLen> row8;
    row8.Num(aRowNumber);
    
    TBuf8<KMaxDescLen> key;
    key.Copy(KKeyMailboxIcons);     
    key.Append(row8);

    PublishImageL(
              iInstIdList[aInstance]->Des(),
              contentType,
              key,
              aIcon,
              aIcon + 1,
              aMailBoxId);    
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::PublishMailDetailL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::PublishMailDetailL( const TInt aInstance,        
                                     const TInt aRowNumber, 
                                     const TDesC& aText,
                                     TWidgetMailDetails aDetail)
    {
    FUNC_LOG;

    TBuf<KMaxDescLen> row;
    row.Num(aRowNumber);

    TBuf<KMaxDescLen> contentType;
    if (aDetail == ETime)
        {    
        contentType.Copy(KContTypeTime);
        }
    else
        {
        contentType.Copy(KContTypeBodyText);
        }
    contentType.Append(row);
    
    TBuf<KMaxDescLen> textKey;
    if (aDetail == ETime)
        {
        textKey.Copy(KKeyTime);
        }
    else
        {
        textKey.Copy(KKeyBodyText);
        }
    textKey.Append(row);

    TFSMailMsgId mailBoxId;
    TFSMailMsgId folderId;

    iMailboxNameUid = PublishDescriptorL(
        KPubId, contentType, 
        iInstIdList[aInstance]->Des(),
        aText,
        textKey, KNullDes8, NULL,
        mailBoxId, folderId);  
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::ResetMessageL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::ResetMessageL( TInt /*aRowNumber*/ )
    {
    FUNC_LOG;
    RemoveL( iMsgUid );
    iMsgUid = 0;
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::ExtractItemId
// ---------------------------------------------------------------------------
//
TUint CMailCpsIf::ExtractItemId( 
    const CLiwGenericParamList& aInParamList )
    {
    FUNC_LOG;
    TUint result ( 0 );
    TInt pos( 0 );
    aInParamList.FindFirst( pos, KItemId );
    if ( pos != KErrNotFound )
        {
        // item id present - extract and return
        aInParamList[pos].Value().Get( result ); // indexing should be safe
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::PublishDescriptorL
// ---------------------------------------------------------------------------
//
TUint CMailCpsIf::PublishDescriptorL(
    const TDesC& aPublisherId, 
    const TDesC& aContentType,
    const TDesC& aContentId, 
    const TDesC& aTextToAdd,
    const TDesC& aKey,
    const TDesC8& aTrigger,
    const TInt /*aMailBoxCount*/,
    TFSMailMsgId /*aMailBoxId*/,
    TFSMailMsgId /*aFolderId*/ )
    {
    FUNC_LOG;

    CLiwGenericParamList* inparam = &(iServiceHandler->InParamListL());
    CLiwGenericParamList* outparam = &(iServiceHandler->OutParamListL());
    TUint id(0);
    TBuf8<KMaxDescLen> key;
    key.Copy(aKey);
    if ( iMsgInterface )
        {
        TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
        inparam->AppendL( cptype );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
        map->InsertL( key,  TLiwVariant( aTextToAdd ) );
        cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
        cpdatamap->InsertL( KContentType, TLiwVariant( aContentType ));
        cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
        cpdatamap->InsertL( KDataMap, TLiwVariant( map ) );

        if( aTrigger.Length() > 0 )
            {
            CLiwDefaultMap* mapAction = CLiwDefaultMap::NewLC();
            CLiwDefaultMap* mapInterface  = CLiwDefaultMap::NewLC();
            CLiwDefaultMap* mapPlugin = CLiwDefaultMap::NewLC();
            mapInterface ->InsertL( KDataForActionHandler, TLiwVariant( mapPlugin )  );
            mapAction->InsertL( aTrigger, TLiwVariant(  mapInterface  ) ); 
            cpdatamap->InsertL( KActionMap, TLiwVariant(mapAction) );
            }

        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));
        inparam->AppendL( item );

        // Execute command
        iMsgInterface->ExecuteCmdL( KAdd , *inparam, *outparam );

        id = ExtractItemId(*outparam );

        if( aTrigger.Length() > 0 )
            {
            CleanupStack::PopAndDestroy( 3 ); // action
            }

        CleanupStack::PopAndDestroy( map );
        CleanupStack::PopAndDestroy( cpdatamap );
        item.Reset();
        cptype.Reset();
        }
    outparam->Reset();
    inparam->Reset();
    return id;
    }

// ---------------------------------------------------------
// CMailCpsIf::PublishImageL
// ---------------------------------------------------------
//
void CMailCpsIf::PublishImageL(
        const TDesC& aContentId,
        const TDesC& aContentType,
        const TDesC8& aKey,
        TInt aBitmapId,
        TInt aBitmapMaskId,
        TFSMailMsgId aMailBoxId )
    {
    FUNC_LOG;

    TBuf<KMaxDescLen> iconIds(KNullDesC);
    
    if ( aBitmapId == EMbmCmailhandlerpluginQgn_indi_cmail_drop_email_account )   
        {
        // This is mailbox icon. Try finding branded mailbox icon.
        TInt error(KErrNone);
        TBuf<KMaxDescLen> brandedIconIds(KNullDesC);
        
        MFSMailBrandManager& brandManager = iMailCpsHandler->MailClient().GetBrandManagerL();
        TRAPD( err, error = brandManager.GetGraphicIdsL( EFSMailboxIcon,  aMailBoxId, brandedIconIds ) );        

        if ( err == KErrNone && error == KErrNone )
            {            
            iconIds.Copy(KMifPrefix);
            iconIds.Append(brandedIconIds);
            }
        }

    if (!iconIds.Length())
        {
        TBuf<KMaxDescLen> id;
        iconIds.Copy(KMifPrefix);
        iconIds.Append(KMifPath);
        iconIds.Append(KSpace);
        id.Num(aBitmapId);
        iconIds.Append(id);     
        iconIds.Append(KSpace);
        id.Num(aBitmapMaskId);
        iconIds.Append(id);        
        }

    // The actual image publishing part starts here   
    CLiwGenericParamList* inparam = &(iServiceHandler->InParamListL());
    CLiwGenericParamList* outparam = &(iServiceHandler->OutParamListL());

    TLiwGenericParam type( KType, TLiwVariant(KCpData));
    inparam->AppendL( type );

    CLiwDefaultMap* pdatamap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();

    pdatamap->InsertL( KPublisherId, TLiwVariant( KPubId ));
    pdatamap->InsertL( KContentType, TLiwVariant( aContentType ));
    pdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
    
    datamap->InsertL(aKey, TLiwVariant(iconIds));

    pdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
    TLiwGenericParam item( KItem, TLiwVariant( pdatamap ));       
    inparam->AppendL( item );
    
    // Publish icon ( or remove it from widget when no id provided )
    if ( aBitmapId == KNullIcon )
        {
        iMsgInterface->ExecuteCmdL( KDelete, *inparam, *outparam);    
        }
    else
        {
        iMsgInterface->ExecuteCmdL( KAdd, *inparam, *outparam);
        }
    
    CleanupStack::PopAndDestroy( datamap );
    CleanupStack::PopAndDestroy( pdatamap );    
    
    item.Reset();
    type.Reset();
    outparam->Reset();
    inparam->Reset();
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::RemoveL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::RemoveL( TUint aId )
    {
    FUNC_LOG;
    CLiwGenericParamList* inparam = &(iServiceHandler->InParamListL());
    CLiwGenericParamList* outparam = &(iServiceHandler->OutParamListL());
    if ( iMsgInterface )
        {
        TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
        inparam->AppendL( cptype );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        cpdatamap->InsertL( KId, TLiwVariant( aId ));

        TLiwGenericParam item( KFilter, TLiwVariant( cpdatamap ));
        inparam->AppendL( item );

        iMsgInterface->ExecuteCmdL( KDelete , *inparam, *outparam);

        CleanupStack::PopAndDestroy( cpdatamap );
        item.Reset();
        cptype.Reset();
        }
    outparam->Reset();
    inparam->Reset();
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::PublisherRegisteryActionL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::PublisherRegisteryActionL()
    {
    FUNC_LOG;
    CLiwGenericParamList* inparam = &(iServiceHandler->InParamListL());
    CLiwGenericParamList* outparam = &(iServiceHandler->OutParamListL());
    
    TLiwGenericParam type( KType, TLiwVariant( KPubData ));
    inparam->AppendL( type );
    
    CLiwDefaultMap* publishermap = CLiwDefaultMap::NewLC();

    // Create the map for publisher registry
    publishermap->InsertL( KPublisherId, TLiwVariant( KPubId ));
    publishermap->InsertL( KContentId, TLiwVariant( KCPAll ));
    publishermap->InsertL( KContentType, TLiwVariant( KCPAll ));

    // Create the action map for publisher registry
    CLiwDefaultMap* mapAction = CLiwDefaultMap::NewLC();
    mapAction->InsertL(KActive, TLiwVariant( KData ));
    mapAction->InsertL(KDeActive, TLiwVariant( KData ));
    mapAction->InsertL(KSuspend, TLiwVariant( KData ));
    mapAction->InsertL(KResume, TLiwVariant( KData ));
    mapAction->InsertL(KInactive, TLiwVariant( KData ));    

    publishermap->InsertL( KActionMap, TLiwVariant(mapAction));
    CleanupStack::PopAndDestroy(mapAction);

    TLiwGenericParam item( KItem, TLiwVariant( publishermap ));
    inparam->AppendL( item );

    iMsgInterface->ExecuteCmdL( KAdd, *inparam, *outparam);

    CleanupStack::PopAndDestroy( publishermap );

    item.Reset();
    type.Reset();   

    outparam->Reset();
    inparam->Reset();
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::RegisterForObserverL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::RegisterForObserverL()
    {
    FUNC_LOG;
    CLiwGenericParamList* inParamList = &(iServiceHandler->InParamListL());
    CLiwGenericParamList* outParamList = &(iServiceHandler->OutParamListL());

    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();

    filter->InsertL( KPublisherId, TLiwVariant( KPubId ) );
    filter->InsertL( KContentId, TLiwVariant( KCPAll ) );
    filter->InsertL( KContentType, TLiwVariant( KCPAll ) );
    filter->InsertL( KOperation, TLiwVariant( KExecute ));

    // Fill in input list for RequestNotification command
    inParamList->AppendL(TLiwGenericParam(KType,TLiwVariant( KContentAndPublisher_Registry )));
    inParamList->AppendL(TLiwGenericParam(KFilter ,TLiwVariant( filter )));

    TInt errori(KErrNone);
    TRAP(errori, iMsgInterface->ExecuteCmdL(  KRequestNotification, *inParamList,
                *outParamList, 0, this ));

    CleanupStack::PopAndDestroy( filter );
    outParamList->Reset();
    inParamList->Reset();
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::UnRegisterForObserverL
// ---------------------------------------------------------------------------
//
void CMailCpsIf::UnRegisterForObserverL()
    {
    FUNC_LOG;
    CLiwGenericParamList* inparam = &(iServiceHandler->InParamListL());
    CLiwGenericParamList* outparam = &(iServiceHandler->OutParamListL());
    // Un Register for notification 

    TInt err;
    err = KErrNone;
    
    // Un Register all the reuqest notification registered in this session
    TRAP( err, iMsgInterface->ExecuteCmdL( 
            KRequestNotification,
                *inparam,
                *outparam,
                KLiwOptCancel,
                this ) ); 
    
    outparam->Reset();
    inparam->Reset();
    }

// ---------------------------------------------------------------------------
// Notification handling
// ---------------------------------------------------------------------------
//
TInt CMailCpsIf::HandleNotifyL(
        TInt aErrorCode,
        TInt /*aEventId*/,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    FUNC_LOG;
    PublisherStatusL  ( aEventParamList);
    return aErrorCode;
    }
   
// ---------------------------------------------------------------------------
// Check if Publisher was activeated or deactivated
// ---------------------------------------------------------------------------
//
TBool CMailCpsIf::PublisherStatusL(const CLiwGenericParamList& aEventParamList)
    {
    FUNC_LOG;
    TBool result(EFalse);
    TInt pos(0);
    const TInt KSAPIContentNameMaxLength = 255;
    TBuf<300> text;
    aEventParamList.FindFirst(pos, KChangeInfo);
    if (pos != KErrNotFound)
        {
        // Get list of maps
        TLiwVariant variant = (aEventParamList)[pos].Value();
        variant.PushL();
        const CLiwList* changeMapsList = variant.AsList();

        TBuf<KSAPIContentNameMaxLength> contentid;
        TBuf<KSAPIContentNameMaxLength> operation;        
        TBuf<KSAPIContentNameMaxLength> trigger;
                       
        // Iter through list content

        TInt mapslistcount (changeMapsList->Count());

        for (TInt i = 0; i < changeMapsList->Count(); ++i)
            {
             if ( changeMapsList->AtL(i, variant) ) 
                 {
                 const CLiwMap* map  = variant.AsMap();
                 
                 // Check what triggered a notification
                 if ( map->FindL(KOperation, variant) )
                     {
                     variant.Get(operation);
                     variant.Reset();
                     if( map->FindL( KContentId, variant) )
                         {
                         variant.Get(contentid );
                         variant.Reset();                         
                         if( map->FindL( KActionTrigger, variant) )
                             {
                             variant.Get(trigger );
                             }
                         if (trigger.Compare(KInactive16) == 0)
                             {
                             // Homescreen page is about to change.
                             // Ignore next deactivate event of this particular widget instance.
                             HBufC* cid = contentid.AllocLC();
                             TInt widgetInstance = FindWidgetInstanceId(cid->Des());
                             iInactive[widgetInstance] = ETrue;
                             CleanupStack::PopAndDestroy( cid );                             
                             }                         
                         else if (trigger.Compare(KSuspend16) == 0)
                             {
                             // Publishing to homescreen suspended.
                             HBufC* cid = contentid.AllocLC();
                             TInt widgetInstance = FindWidgetInstanceId(cid->Des());
                             iAllowedToPublish[widgetInstance] = EFalse;
                             CleanupStack::PopAndDestroy( cid );
                             }
                         else if (trigger.Compare(KResume16) == 0)
                             {
                             HBufC* cid = contentid.AllocLC();
                             if ( FindWidgetInstanceId(cid->Des()) < 0 )
                                 {
                                 iInstIdList.AppendL( contentid.AllocL() );
                                 }
                             TInt widgetInstance = FindWidgetInstanceId(cid->Des());
                             PublishSetupWizardL(widgetInstance);
                             iMailCpsHandler->UpdateMailboxesL(widgetInstance, cid->Des());
                             // Widget visible on the homescreen. Publishing allowed.
                             iAllowedToPublish[widgetInstance] = ETrue;
                             iInactive[widgetInstance] = EFalse;
                             CleanupStack::PopAndDestroy( cid );
                             }
                         else if (trigger.Compare(KActive16) == 0)
                             {
                             // Widget added to homescreen (or already on the screen when boot completes).
                             HBufC* cid = contentid.AllocLC();
                             TInt widgetInstance = FindWidgetInstanceId(cid->Des());
                             if (iInactive[widgetInstance])
                                 {
                                 iInactive[widgetInstance] = EFalse;
                                 }
                             else
                                 {
                                 iInstIdList.AppendL( contentid.AllocL() );
                                 }
/*                             if (!alreadyAssociated)
                                {
                                iMailCpsHandler->LaunchWidgetSettingsL(cid->Des());
                                }*/
                             CleanupStack::PopAndDestroy( cid );
                             }
                         else if (trigger.Compare(KDeActive16) == 0)
                             {                            
                             // Widget removed from homescreen.
                             HBufC* cid = contentid.AllocLC();
                             TInt widgetInstance = FindWidgetInstanceId(cid->Des());                            
                             if (!iInactive[widgetInstance])
                                 {
                                 iMailCpsHandler->DissociateWidgetFromSetting( cid->Des() );                                 
                                 iInstIdList.Remove(widgetInstance);                                 
                                 }
                             iAllowedToPublish[widgetInstance] = EFalse;
                             CleanupStack::PopAndDestroy( cid );
                             }
                         else if (trigger.Compare(KTriggerEmailUi) == 0)
                             {
                             HBufC* cid = contentid.AllocLC();
                             iMailCpsHandler->LaunchEmailUIL(cid->Des());
                             CleanupStack::PopAndDestroy( cid );
                             }
                         else if ((trigger.Compare(KTriggerWizard) == 0) ||
                                 ( trigger.Compare(KSettings16) == 0))
                             {
                             // If no accounts are created launch email wizard
                             // otherwice launch widget settings app                             
                             if (iMailCpsHandler->GetMailboxCount())
                                 {                                
                                 HBufC* cid = contentid.AllocLC();
                                 iMailCpsHandler->LaunchWidgetSettingsL(cid->Des());
                                 CleanupStack::PopAndDestroy( cid );                                 
                                 }
                             else
                                 {
                                 iMailCpsHandler->LaunchEmailWizardL();
                                 }
                             }
                         }
                     }
                 }
            }
        CleanupStack::PopAndDestroy(1);
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::AllowedToPublish
// ---------------------------------------------------------------------------
//
TBool CMailCpsIf::AllowedToPublish( TInt aWidgetInstance )
    {
    FUNC_LOG;
    if ( !iAllowedToPublish[aWidgetInstance] && !iIgnoreHsNotifications )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::SetConfiguration
// ---------------------------------------------------------------------------
//
void CMailCpsIf::SetConfiguration( const TInt32 aConfigData )
    {
    FUNC_LOG;
    iIgnoreHsNotifications = aConfigData & KCMailIgnoreHsEvents;
    }

// ---------------------------------------------------------------------------
// CMailCpsIf::GetWidgetInstanceCount
// ---------------------------------------------------------------------------
//
TInt CMailCpsIf::GetWidgetInstanceCount()
    {
    FUNC_LOG;    
    return iInstIdList.Count();
    }


// ---------------------------------------------------------------------------
// CMailCpsIf::FindWidgetInstanceId
// ---------------------------------------------------------------------------
//
TInt CMailCpsIf::FindWidgetInstanceId( const TDesC& aContentId )
    {
    FUNC_LOG;
    TInt instance(KErrNotFound);
    for (TInt i = 0; i < iInstIdList.Count(); i++)
        {
        TInt val = aContentId.Compare(iInstIdList[i]->Des());
        if (!val)
            {
            instance = i;
            break;
            }
        }
    return instance;
    }

