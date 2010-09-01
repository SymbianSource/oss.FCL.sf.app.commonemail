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
*  Description : Fieldstorage with both viewer and editor fields
*  Version     : %version: e002sa33#5 %
*
*/


#ifndef CESMRMIXEDFIELDSTORAGE_H
#define CESMRMIXEDFIELDSTORAGE_H

#include "cesmrfieldstorage.h"
#include "mesmrfieldvalidator.h"

class CESMRPolicy;
class TESMREntryField;
class MESMRResponseObserver;
class MESMRCalEntry;

/**
 * Field storage which contains both viewer and editor fields. Storage is
 * used with following use cases:
 *  - Forward
 *  - Edit recurrent event single instance
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRMixedFieldStorage ): public CESMRFieldStorage
    {
public:
    /**
     * Two phase constructor.
     */
    static CESMRMixedFieldStorage* NewL(
            MESMRFieldEventObserver& aEventObserver,
            const CESMRPolicy& aPolicy,
            MESMRResponseObserver* aResponseObserver,
            MESMRCalEntry& aEntry );

    /**
     * Destructor.
     */
    ~CESMRMixedFieldStorage();

protected: // from CESMRFieldStorage
    void ExternalizeL( MESMRCalEntry& aEntry );
    void InternalizeL( MESMRCalEntry& aEntry );
    TInt Validate( TESMREntryFieldId& aUpdatedFocus, TBool aForceValidate );
    void ChangePolicyL(
            const CESMRPolicy& aNewPolicy,
            MESMRCalEntry& aEntry );

private: // Implemantation
    /**
     * Constructor.
     *
     * @param aResponseObserver response observer
     */
    CESMRMixedFieldStorage(
            MESMRFieldEventObserver& aEventObserver,
            MESMRResponseObserver* aResponseObserver );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     * @param aEntry current entry for policy
     */
    void ConstructL( const CESMRPolicy& aPolicy,
                     MESMRCalEntry& aEntry );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     * @param aValidator validator object
     */
    void ConstructForwardEventL(
            const CESMRPolicy& aPolicy,
            MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     * @param aValidator validator object
     */
    void ConstructRecurrentEventL(
            const CESMRPolicy& aPolicy,
            MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     * @param aValidator validator object
     */
    void ConstructEditSeriesEventL(
            const CESMRPolicy& aPolicy,
            MESMRFieldValidator* aValidator );

    /**
     * Resets this storage
     */
    void Reset();
    
    /**
     * Non-virtual function to change policy.
     * @param aNewPolicy policy states visible fields
     * @param aEntry the entry to handle
     */
    void DoChangePolicyL(
            const CESMRPolicy& aNewPolicy,
            MESMRCalEntry& aEntry );
    
private:

    /** Enumeration for mixed event types */
    enum TMixedFieldStorageEventType
        {
        /** Unknown event type*/
        EMixedFieldStorageUnknown = 0,
        /** Forward event type*/
        EMixedFieldStorageForward,
        /** Recurrent instance event type*/
        EMixedFieldStorageRecurrentEvent,
        /** Recurrent series event type*/
        EMixedFieldStorageEditSeriesEvent
        };

private:
    /// Own: Validator object.
    MESMRFieldValidator* iValidator;
    /// Ref: Response observer
    MESMRResponseObserver* iResponseObserver;
    /// Ref: Reference to used calendar entry
    MESMRCalEntry* iEntry;
    /// Own: Created event type
    TMixedFieldStorageEventType iEventType;
    };

#endif  // CESMRMIXEDFIELDSTORAGE_H

