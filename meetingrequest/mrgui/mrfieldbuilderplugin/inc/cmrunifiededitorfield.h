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
* Description:  MR Unified editor field impl.
 *
*/

#ifndef CMRUNIFIEDEDITORFIELD_H
#define CMRUNIFIEDEDITORFIELD_H

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRLabel;
class CMRImage;
class CESMRTextItem;

/**
 * This class is a custom field control that shows the event type of calendar events
 */
NONSHARABLE_CLASS( CMRUnifiedEditorField ) : public CESMRField
    {
public:
    /**
     * Creates new CMRUnifiedEditorField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CMRUnifiedEditorField* NewL();

    /**
     * Destructor.
     */
    ~CMRUnifiedEditorField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void LockL();

public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void SizeChanged();

private:
    /**
     * Constructor.
     */
    CMRUnifiedEditorField();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Updates event type text to aIndex.
     *
     * @param aIndex new event type index
     */
    void UpdateLabelL( TInt aIndex );

    /**
    * Shows query dialog for selecting
    * event type
    */
    void ExecuteTypeQueryL();
    
    /**
    * Sets the correct icon for the field according
    * to the entry type.
    */
    void SetIconL( MESMRCalEntry& aEntry );

private:
    /**
     * Not owned. Event type label.
     */
    CMRLabel* iType;

    /// Own: Icon field
    CMRImage* iIcon;
    
    /// Own: lock icon
    CMRImage* iLockIcon;

    /**
     * Own. Array of event type objects.
     */
    RPointerArray< CESMRTextItem > iArray;

    /**
     * Index of selected event type.
     */
    TInt iIndex;

    /**
     * Own: Filter for dynamic popup, not showing all items
     */
    RArray<TInt> iFilter;
    };

#endif  // CMRUNIFIEDEDITORFIELD_H

