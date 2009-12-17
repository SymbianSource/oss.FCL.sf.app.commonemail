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
* Description:  ESMR priority field implementation
 *
*/


#ifndef CESMRPRIORITYFIELD_H
#define CESMRPRIORITYFIELD_H

#include "cesmriconfield.h"
#include "cesmrpriority.h"

class MESMRCalEntry;
class CEikLabel;
class MESMRTitlePaneObserver;

NONSHARABLE_CLASS( CESMRPriorityField ): public CESMRIconField
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

public: // From CESMRIconField
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    void ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );
    void SetTitlePaneObserver( MESMRTitlePaneObserver* aObserver );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);

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
     * Not owned. Priority label.
     */
    CEikLabel* iPriority;

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
     * Title pane observer for setting the priority icon
     */
    MESMRTitlePaneObserver* iObserver;
    };

#endif  // CESMRPRIORITYFIELD_H

