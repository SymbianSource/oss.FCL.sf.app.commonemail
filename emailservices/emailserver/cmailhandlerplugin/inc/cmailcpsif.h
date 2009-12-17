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


#ifndef CMAILCPSIF_H_
#define CMAILCPSIF_H_

// LIW interface
#include <LiwServiceHandler.h>
#include <LiwVariant.h>
#include <LiwGenericParam.h>
#include <fbs.h>
#include <data_caging_path_literals.hrh>

#include "cmailcpshandler.h"
#include "cmailcpsifconsts.h"

/**
 *  CMail ContentPublishingService interface class
 *  Class implementing interface towards LIW FW and Content Publishing Service
 *
 *  @lib fsmailserver.exe
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CMailCpsIf ) : public CBase, public MLiwNotifyCallback
    {
public:
    /**
     * Two-phased constructor.
     */
    static CMailCpsIf* NewL( CMailCpsHandler* aMailCpsHandler );

    /**
    * Destructor.
    */
    ~CMailCpsIf();

    /**
     * Publishes setup text and action to launch setup wizard
     */
    void PublishSetupWizardL(TInt instance);
    
    /**
     * Publishes mailbox name to widget and action to launch CMail UI
     * @param aRowNumber specifies the row on widget UI
     * @param aMailboxName contains name of the mailbox
     */
    void PublishActiveMailboxNameL( const TInt aInstance, 
									const TInt aRowNumber, 
                                    const TDesC& aMailboxName,
                                    const TInt aMailBoxCount,
                                    TFSMailMsgId aMailBoxId,
                                    TFSMailMsgId aFolderId );

    /**
     * Resets mailbox name from widget
     * @param aRowNumber specifies the row on widget UI
     */
//    void ResetActiveMailboxNameL( const TInt aRowNumber );

    /**
     * Publishes unseen or outbox icon
     */
    void PublishIndicatorIconL( const TInt aInstance, 
                                const TInt aRowNumber,
                                const TInt aIcon );
    
    /**
     * 
     */
    void PublishMailboxIconL( const TInt aInstance, const TInt aRowNumber, const TInt aIcon, TFSMailMsgId aMailBoxId );

    /**
     * 
     */    
    void PublishMailDetailL(
            const TInt aInstance,
            const TInt aRowNumber, 
            const TDesC& aSender,
            TWidgetMailDetails aDetail);
    
    /**
     * Resets mail header data
     * @param aRowNumber specifies the row on widget UI
     */
    void ResetMessageL( TInt aRowNumber );

    /**
     * 
     */
	void PublisherRegisteryActionL();

    /**
     * 
     */	
    void RegisterForObserverL();

    /**
     * 
     */	
    void UnRegisterForObserverL();

    /**
     * 
     */	
	TInt HandleNotifyL(
        TInt aErrorCode,
        TInt aEventId,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& aInParamList );

    /**
     * 
     */		
	TBool PublisherStatusL(const CLiwGenericParamList& aEventParamList);

    /**
     * 
     */
	TBool AllowedToPublish( TInt aWidgetInstance );
	
    /**
     * Sets configuration data to publisher interface
     * CpsIf uses this
     * @param aRowNumber specifies the row on widget UI
     */
	void SetConfiguration( const TInt32 aConfigData );

    /**
     * 
     */	
	TInt GetWidgetInstanceCount();
	
private:
    
    CMailCpsIf( CMailCpsHandler* aMailCpsHandler );
    void ConstructL();

    /**
     * adds resource file
     */
    void CMailCpsIf::AllocateResourcesL();
    
    /**
     * initializes the LIW IF
     * @return instance of LIW IF
     */
    MLiwInterface* GetMessagingInterfaceL( );

    /**
     * Gets published item's ID from LIW. Id is later used e.g. when reseting values 
     * @param aInParamList parameter list returned from LIW IF
     * @return Id
     */
    TUint ExtractItemId( const CLiwGenericParamList& aInParamList );
    
    /**
     * Publishes descriptor
     * @param aPublisherId id of the publisher
     * @param aContentType content type of the published data
     * @param aContentId content id of the published data
     * @param aTextToAdd data to be published
     * @param aKey key of the added descriptor
     * @param aTrigger optional trigger tag
     * @param aUid optional app uid (app to be launched)
     * @return Id
     */
    TUint PublishDescriptorL(
        const TDesC& aPublisherId, 
        const TDesC& aContentType, 
        const TDesC& aContentId, 
        const TDesC& aTextToAdd,
        const TDesC& aKey,
        const TDesC8& aTrigger,
        const TInt aMailBoxCount,
        TFSMailMsgId aMailBoxId,
        TFSMailMsgId aFolderId );

    /**
     * Publishes image
     * @param aContentId
     * @param aResult
     * @param aBitmapId
     * @param aBitmapMaskId
     * @param aMailBoxId
     */

    void PublishImageL(
         const TDesC& aContentId,
         const TDesC& aContentType,
         const TDesC8& aKey,
         TInt aBitmapId,
         TInt aBitmapMaskId,
         TFSMailMsgId aMailBoxId );

    /**
     * Removes a published entity from Content Publishing Service
     * @param aId of the published data
     */
    void RemoveL( TUint aId );

    /**
     * 
     */	    
    TInt FindWidgetInstanceId(const TDesC& aContentId);
public:
	//
    RPointerArray<HBufC>    iInstIdList;
private:
	CMailCpsHandler*		iMailCpsHandler; //uses
    // LIW IF
    MLiwInterface* 			iMsgInterface;
    // LIW Service handler
    CLiwServiceHandler* 	iServiceHandler;
    // resource file offset
    TInt                    iResourceFileOffset;
    // UIDS of the published entities
    TUint 					iMailboxNameUid;
    // message count of a mailbox
    TUint 					iMessageCountUid;
    // row count
    TUint 					iRowCountUid;
    // message header details id
    TUint 					iMsgUid;
    // setup text and action id
    TUint 					iSetupUid;
	// is content allowed to publish	
	TBool 					iAllowedToPublish[KMaxMailboxCount];
    // is widget inactive    
    TBool                   iInactive[KMaxMailboxCount];	
	// From configuration: are homescreen notifications observed or ignored
	TBool                   iIgnoreHsNotifications;
    };

#endif /*CMAILCPSIF_H_*/
