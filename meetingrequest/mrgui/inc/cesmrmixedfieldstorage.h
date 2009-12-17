/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*  Version     : %version: tr1sido#3 %
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
            CESMRPolicy* aPolicy,
            MESMRResponseObserver* aResponseObserver,
            MESMRCalEntry& aEntry );

    /**
     * Destructor.
     */
    ~CESMRMixedFieldStorage();

public: // from CESMRFieldStorage
    void ExternalizeL( MESMRCalEntry& aEntry );
    void InternalizeL( MESMRCalEntry& aEntry );
    TInt Validate( TESMREntryFieldId& aUpdatedFocus, TBool aForceValidate );

private: // Implemantation
    /**
     * Constructor.
     *
     * @param aResponseObserver response observer
     */
    CESMRMixedFieldStorage(
            MESMRFieldEventObserver& aEventObserver,
            MESMRResponseObserver* aResponseObserver,
            MESMRCalEntry& aEntry  );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     */
    void ConstructL( CESMRPolicy* aPolicy );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     * @param aValidator validator object
     */
    void ConstructForwardEventL(
            CESMRPolicy* aPolicy,
            MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     * @param aValidator validator object
     */
    void ConstructRecurrentEventL(
            CESMRPolicy* aPolicy,
            MESMRFieldValidator* aValidator );

    /**
     * Second phase constructor.
     *
     * @param aPolicy policy states visible fields
     * @param aValidator validator object
     */
    void ConstructEditSeriesEventL(
            CESMRPolicy* aPolicy,
            MESMRFieldValidator* aValidator );

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
    MESMRCalEntry& iEntry;
    /// Own: Created event type
    TMixedFieldStorageEventType iEventType;
    };

#endif  // CESMRMIXEDFIELDSTORAGE_H

