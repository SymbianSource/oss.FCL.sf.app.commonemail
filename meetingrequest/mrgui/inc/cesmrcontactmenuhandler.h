/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of class CESMRContactMenuHandler.
*
*/


#ifndef CESMRCONTACTMENUHANDLER_H
#define CESMRCONTACTMENUHANDLER_H

#include <e32base.h>

#include "cesmrcontactmanagerhandler.h"
#include <MVPbkContactFindObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
//<cmail>
#include "mfsccontactactionserviceobserver.h"
#include "fsccontactactionservicedefines.h"
//</cmail>
#include <eikcmobs.h>

class MVPbkContactOperationBase;
class MVPbkContactStore;
class MVPbkContactLinkArray;
class CFscContactActionService;
class CFscContactActionMenu;
class CEikMenuPane;
class CESMRContactMenuUrlHandler;
class CESMRContactMenuAttachmentHandler;
// <cmail>
class CESMRAttachmentInfo;
// </cmail>

NONSHARABLE_CLASS( CESMRContactMenuHandler ) : 
	public CBase, 
	public MVPbkContactFindObserver,
    public MVPbkSingleContactOperationObserver,
    public MFscContactActionServiceObserver, 
    public MESMRContactManagerObserver
    {
public:
    /*
     * Enumeration for value types.
     */
    enum TValueType { 
		EValueTypeURL, 
		EValueTypeEmail, 
		EValueTypePhoneNumber, 
		EValueTypeAttachment
		};

public:
    /**
     * Two-phase constructor
     *
     * @param aContactManagerHandler Handler to contact manager
     * @return new instance of the class
     */
    static CESMRContactMenuHandler* NewL( 
    		CESMRContactManagerHandler& aContactManagerHandler );

    /**
     * Destructor.
     */
    ~CESMRContactMenuHandler( );

    /**
     * Options menu available
     * @return ETrue if options menu / actions is available.
     */
    TBool OptionsMenuAvailable( );

    /**
     * Initialises actions menu pane.
     *
     * @param aActionMenuPane actions menu pane
     */
    void InitOptionsMenuL( CEikMenuPane* aActionMenuPane );

    /**
     * Executes options menu / actions command.
     *
     * @param aCommand command id
     */
    void ExecuteOptionsMenuL( TInt aCommand );

    /**
     * Resets/disables contact menu handler.
     */
    IMPORT_C void Reset( );

    /**
     * Sets new value for contact action menu. It might be of
     * type URL, Email or phone number.
     *
     * @param aValue descriptor which contains value
     * @param aValueType type of given value
     */
    IMPORT_C void SetValueL( const TDesC& aValue, TValueType aValueType );

    /**
     * Shows/executes contact action menu.
     */
    IMPORT_C void ShowActionMenuL( );

    /**
     * Sets command observer for field. Field can use command
     * observer for triggering commands.
     * @param aCommandObserver Reference to command observer
     */
    IMPORT_C void SetCommandObserver(
            MEikCommandObserver* aCommandObserver );

    IMPORT_C void SetAttachmentInfoL( CESMRAttachmentInfo* aAttachmentInfo );
    
private:
    /**
     * Private constructor.
     */
    CESMRContactMenuHandler( 
    		CESMRContactManagerHandler& aContactManagerHandler );

    /**
     * Second phase constructor.
     */
    void ConstructL( );

    void VerifyContactDetailsL();

private: // from MESMRContactManagerHandler
    void ContactManagerReady( );

private: // from MVPbkSingleContactOperationObserver
    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
    void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );

private: // from MVPbkContactFindObserver
    void FindCompleteL( MVPbkContactLinkArray* aResults );
    void FindFailed( TInt aError );

    // from MFscContactActionServiceObserver
    void QueryActionsComplete( );
    void QueryActionsFailed( TInt aError );
    void ExecuteComplete( );
    void ExecuteFailed( TInt aError );

private:
    /**
     * Searches selected richtext link from contact stores.
     */
    void SearchContactL( );

    /**
     * Creates new temporary contact for richtext link.
     */
    void CreateContactL( );

    /**
     * Searches for local contact store for temporary
     * contact creation. Leaves if none is found.
     *
     * @return local contact store
     */
    MVPbkContactStore& LocalContactStoreL( );

private: // Data
    /// Ref: Contact manager handler
    CESMRContactManagerHandler& iContactManagerHandler;
    /// Own: Url contact menu handler.
    CESMRContactMenuUrlHandler* iContactMenuUrlHandler;    
    // <cmail>
    /// Own: Attachment contact menu handler.
    CESMRContactMenuAttachmentHandler* iContactMenuAttachmentHandler;    
    // </cmail>
    /// Own: Contact search results.
    MVPbkContactLinkArray* iFindResultsArray;
    /// Own: Index for verifying search results.
    TInt iFindResultsArrayIndex;
    /// Own: Contact action service.
    CFscContactActionService* iContactActionService;
    /// Own: Contact action menu.
    CFscContactActionMenu* iContactActionMenu;
    /// Own: Contact operation base.
    MVPbkContactOperationBase* iContactOperationBase;
    /// Own: Current value for actions menu.
    HBufC* iValue;
    /// Own: Current value type.
    TValueType iValueType;
    /// Own: Contact list for temporary contact.
    RFscStoreContactList iStoreContactList;
    /// Own:  Contact action list for selected richtext link.
    CFscContactActionList iContactActionList;
    /// Own: ETrue if contact manager is initialized.
    TBool iContactManagerReady;
    /// Own: ETrue if action menu is initialized.
    TBool iActionMenuReady;
    /// Own: ETrue if options menu is initialized.
    TBool iOptionsMenuReady;
    /// Ref: Pointer to command observer
    MEikCommandObserver* iCommandObserver;
    };

#endif
