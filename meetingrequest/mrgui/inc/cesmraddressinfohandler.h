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
* Description:  ESMR GUI address info handler
*
*/



#ifndef CESMRADDRESSINFOHANDLER_H
#define CESMRADDRESSINFOHANDLER_H

// Includes
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <AiwCommon.h>

#include "resmrstatic.h"
#include "cesmrcontactmanagerhandler.h"

// Forward declarations
class RESMRStatic;
class CVPbkContactManager;
class CAiwServiceHandler;
class MESMRContactHandlerObserver;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkContactLink;
class CAknInformationNote;
class MESMRCalEntry;

/**
* CESMRAddressInfoHandler encapsulates the handling of search from contacts command
* functionality
* 
* @lib esmrgui.lib
*/
NONSHARABLE_CLASS(CESMRAddressInfoHandler) : public CBase, 
                                    public MVPbkSingleContactOperationObserver,
                                    public MAiwNotifyCallback
{
public:
    /**
     * Creates new CESMRAddressInfoHandler object. Ownership
     * is transferred to caller.
     * @return Pointer to created CESMRAddressInfoHandler object.
     */
    static CESMRAddressInfoHandler* NewL();	
    
    /**
     * C++ destructor
     */
	virtual ~CESMRAddressInfoHandler();

    /**
    * Starts single item fetch from virtual phonebook
    * @param aEntry calendar entry to update.
    * @return ETrue if aEntry was modified. 
    */
    TBool SearchAddressFromContactsL( MESMRCalEntry& aEntry );

protected: //From MVPbkSingleContactOperationObserver
    
    void VPbkSingleContactOperationComplete(
                    MVPbkContactOperationBase& aOperation,
                    MVPbkStoreContact* aContact );
	
    void VPbkSingleContactOperationFailed(
                    MVPbkContactOperationBase& aOperation, 
                    TInt aError );
    
protected: // from MAiwNotifyCallback
    
    TInt HandleNotifyL( TInt aCmdId,
                        TInt aEventId,
                        CAiwGenericParamList& aEventParamList,
                        const CAiwGenericParamList& aInParamList);
    
private: // Implementation
    
    enum TAddressType
        {
        EHomeAddress,
        EWorkAddress,
        EGeneralAddress
        };
    
    enum TAddressFormatType
        {
        EFormatOneItem = 1,
        EFormatTwoItems,
        EFormatThreeItems,
        EFormatFourItems
        };
    
    CESMRAddressInfoHandler();
    void ConstructL();
    void VPbkSingleContactOperationCompleteL(
                    MVPbkContactOperationBase& aOperation,
                    MVPbkStoreContact* aContact );
    void GetContactFieldsL( RArray<TInt>& aFieldIds,
                            RPointerArray<HBufC>& aNumbers,
                            MVPbkStoreContact* aContact );
    void FormatAddressL( RPointerArray<HBufC>& aAddresses,
                         CESMRAddressInfoHandler::TAddressType aAddressType );
    void UpdateEntryL( TInt aIndex );
    
private: //data

    RESMRStatic iESMRStatic; //not own, reference counted
    
    CVPbkContactManager* iContactManager; //not own
    
    CAiwServiceHandler* iServiceHandler; //own
    
    MVPbkContactLinkArray* iLinksSet;//own
    
    MVPbkContactOperationBase* iLinkOperationFetch;//own
    
    RArray<TInt> iAddressGeneralFields;    
    RArray<TInt> iAddressHomeFields;    
    RArray<TInt> iAddressWorkFields;
    
    RPointerArray<HBufC> iCompleteAddresses; //own
    
    MESMRCalEntry* iEntry; //not own

    CActiveSchedulerWait* iWait; // own
    
    TBool iEntryUpdated;
};

#endif /*CESMRADDRESSINFOHANDLER_H*/
