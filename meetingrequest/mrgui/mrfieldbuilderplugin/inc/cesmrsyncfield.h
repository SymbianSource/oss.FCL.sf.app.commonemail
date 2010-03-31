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
* Description:  ESMR synchronization field impl.
 *
*/

#ifndef CESMRSYNCFIELD_H
#define CESMRSYNCFIELD_H

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRLabel;
class CMRImage;
class CESMRTextItem;

/**
 * This class is a custom field control that shows the sync status of calendar events
 */
NONSHARABLE_CLASS( CESMRSyncField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRSyncField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRSyncField* NewL();

    /**
     * Destructor.
     */
    ~CESMRSyncField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void SizeChanged();
    
private:
    /**
     * Constructor.
     */
    CESMRSyncField();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Updates synchronization text to aIndex.
     *
     * @param aIndex new synchronization value index
     */
    void UpdateLabelL( TInt aIndex );

    /**
    * Shows query dialog for selecting
    * priority
    *
    */
    void ExecuteSyncQueryL();

private:
    /**
     * Not owned. Synchronization label.
     */
    CMRLabel* iSync;
    
    /// Own: Icon field
    CMRImage* iIcon;

    /**
     * Own. Array of synchronization objects.
     */
    RPointerArray< CESMRTextItem > iArray;

    /**
     * Index of selected synchronization.
     */
    TInt iIndex;
    };

#endif  // CESMRSYNCFIELD_H

