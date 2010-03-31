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
* Description:  ESMR priority field implementation
 *
*/


#ifndef CESMRPRIORITYFIELD_H
#define CESMRPRIORITYFIELD_H

#include "cesmrfield.h"
#include "cesmrpriority.h"
#include "mesmrcalentry.h"

class CMRImage;
class CMRLabel;

NONSHARABLE_CLASS( CESMRPriorityField ): public CESMRField
    {
public:
    /**
     * Creates new CESMRPriorityField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRPriorityField* NewL();

    /**
     * C++ Destructor.
     */
    ~CESMRPriorityField();

public: // From base class CESMRField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );

public: // From base class CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;    
    
private: // Implementation
    /**
     * Constructor.
     */
    CESMRPriorityField();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Updates priority to given index.
     *
     * @param aIndex index
     */
    void UpdateTextL( TInt aIndex );

    /**
     * Shows query dialog for selecting
     * priority
     *
     */
    void ExecutePriorityQueryL();

private:

    /**
     * Own. Array of available priorities.
     */
    RPointerArray< CESMRPriority > iArray;

    /**
     * Selected priority index.
     */
    TInt iIndex;

    /**
     * Entry type
     */
    MESMRCalEntry::TESMRCalEntryType iEntryType;
    
    /**
     * Owned: Field icon
     */
    CMRImage* iIcon;

    /**
     * Not own: Field text label
     */
    CMRLabel* iLabel;    
    };

#endif  // CESMRPRIORITYFIELD_H

