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
* Description:  MR multi calendar field impl.
 *
*/

#ifndef CMRMULTICALENFIELD_H
#define CMRMULTICALENFIELD_H

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRLabel;
class CMRImage;
class CESMRTextItem;
class CESMRMultiCalenItem;
class MESMRCalDbMgr;

/**
 * This class is a custom field control that shows the calendar type of multi calendar
 */
NONSHARABLE_CLASS( CMRMultiCalenField ) : public CESMRField
    {
public:
    /**
     * Creates new CMRMultiCalenField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CMRMultiCalenField* NewL();

    /**
     * Destructor.
     */
    ~CMRMultiCalenField();

public: // From CESMRField
	void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void LockL();

public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );

private:
    /**
     * Constructor.
     */
    CMRMultiCalenField();

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
     * Sets the label text color dimmed
     */
    void SetTextDimmed();
    
    /**
     * Tests if calendar type change is enabled or not.
     * 
     * @return Boolean, ETrue if type change is enabled, otherwise EFalse. 
     */
    TBool TypeChangeEnabledL();

private:
    /**
     * Not owned. Event type label.
     */
    CMRLabel* iCalenName;

    /// Own: Icon field
    CMRImage* iIcon;
    
    /// Own: lock icon
    CMRImage* iLockIcon;

    /**
     * Index of selected event type.
     */
    TInt iIndex;

    /**
     * ref to cur entry
     */
    MESMRCalEntry* iEntry;

    /**
     * reserve multi-calendar name list
     */
    RArray<TPtrC> iCalenNameList;
    
    /**
     * Own: Indicates if calendar type change is enabled or disabled
     */
    TBool iTypeChangeEnabled;
    };

#endif  // CMRMULTICALENFIELD_H

