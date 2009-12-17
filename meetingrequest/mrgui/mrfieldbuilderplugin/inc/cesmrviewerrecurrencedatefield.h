/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Recurrence date field
*
*/


#ifndef CESMRVIEWERRECURRENCEDATEFIELD_H
#define CESMRVIEWERRECURRENCEDATEFIELD_H

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRLabel;
class CLayoutManager;

// CLASS DECLARATIONS

/**
 * Class is responsible of showing the recurrence event's repeat details.
 */
NONSHARABLE_CLASS( CESMRViewerRecurrenceDateField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerRecurrenceDateField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerRecurrenceDateField* NewL();

    /**
     * C++ destructor
     */
    ~CESMRViewerRecurrenceDateField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();

public: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TSize MinimumSize();
    
private: // Implementation
    CESMRViewerRecurrenceDateField();
    void ConstructL();
    
private:
    /// Own: topic label
    CMRLabel* iRepeatTopic;
    /// Own: date label
    CMRLabel* iRepeatDate;
    };

#endif  // CESMRVIEWERRECURRENCECONTAINER_H
