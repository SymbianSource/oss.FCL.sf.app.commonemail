/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class CFscContactActionService.
*
*/


#ifndef C_FSCCONTACTACTIONSERVICE_H
#define C_FSCCONTACTACTIONSERVICE_H

#include <e32base.h>

//<cmail>
#include "fsccontactactionservicedefines.h"
//</cmail>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MFscContactAction;
class MFscContactLinkIterator;
class MFscContactActionServiceObserver;

/**
 *  Contact Action Service API.
 *  Defines an ECom interface for the Contact Action Service.
 *
 *  @since S60 3.1
 */
class CFscContactActionService : public CBase
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @return Pointer to new instance of CFscContactActionService   
     */
    static inline CFscContactActionService* NewL( 
            CVPbkContactManager& aContactManager );

    /**
     * Destructor.
     */
    virtual ~CFscContactActionService();
    
public: // Public methods

    /**
     * Method for setting current contact set to the service.
     * 
     * @param aIterator iterator to collection of MVPbkContactLink objects.
     *                  Collection can contain links to contacts and group. 
     */
    virtual void SetContactSetL( MFscContactLinkIterator* aIterator ) = 0;
    
    /**
     * Method for setting current contact set to the service.
     * 
     * @param aStoreContactList collection of MVPbkStoreContact objects.
     *                          Collection can contain ONLY contacts 
     *                          (no groups).
     */
    virtual void SetContactSetL( 
            const RFscStoreContactList& aStoreContactList ) = 0;
    
    /**
     * Method for quering available actions
     * Method searches actions for pre-set contact/group set. 
     * Results can be received with QueryResults method.
     * Asynchronous. Data is ready when observer's QueryActionsComplete 
     * method is invoked.
     *
     * @param aObsever operation observer.
     * @param aStopWhenOneActionFound stops query when at least one 
     *                                action found.
     * @param aActionTypeFlags Action type flags which are used to filter
     *                         actions.
     * @param aMinPriority Minimum allowed priority for returned actions.
     */
    virtual void QueryActionsL(
        MFscContactActionServiceObserver* aObserver,
        TBool aStopWhenOneActionFound = EFalse,
        TUint64 aActionTypeFlags = KFscAtDefault, 
        TInt aMinPriority = 0 ) = 0;
    
    /**
     * Cancels async method QueryActionsL.
     */
    virtual void CancelQueryActions() = 0; 

    /**
     * Return action query results.
     * Empty list is returned if no queries has been done
     *
     * @return action query result list
     */
    virtual const CFscContactActionList& QueryResults() const = 0; 

    /**
     * Execute action.
     * Asynchronous. Data is ready when observer's ExecuteComplete 
     * mthod is invoked.
     *
     * @param aActionUid uid of action to be executed
     */
    virtual void ExecuteL( 
        TUid aActionUid, 
        MFscContactActionServiceObserver* aObserver ) = 0;
    
    /**
     * Cancels async method ExecuteL.
     */
    virtual void CancelExecute() = 0;
                                         
private: // data

    /** iDtor_ID_Key Instance identifier key. When instance of an
     *               implementation is created by ECOM framework, the
     *               framework will assign UID for it. The UID is used in
     *               destructor to notify framework that this instance is
     *               being destroyed and resources can be released.
     */
    TUid iDtor_ID_Key;

    };

/**
 *  Construction parameters for Action Menu
 *
 *  @since S60 3.1
 */
class TFscContactActionServiceConstructParameters
    {

public: // Public methods

    /**
     * Constructor
     *
     * @param aService Pointer to Contact Action Service
     * @param aMode Menu mode
     */
    TFscContactActionServiceConstructParameters( 
        CVPbkContactManager& aVPbkContactManager ) 
        : iVPbkContactManager( aVPbkContactManager ) {}
   
public: // Public members

    /**
     * Reference to virtual phonebook contact manager.
     */
    CVPbkContactManager& iVPbkContactManager;

    };

#include "cfsccontactactionservice.inl"

#endif // C_FSCCONTACTACTIONSERVICE_H
