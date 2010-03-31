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
* Description:  ESMR viewer start date field.
*
*/


#ifndef __CESMRVIEWERSTARTDATEFIELD_H__
#define __CESMRVIEWERSTARTDATEFIELD_H__

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRImage;
class CMRLabel;

// CLASS DECLARATIONS
/**
 * This field shows the start date of a meeting or a meeting request
 */
NONSHARABLE_CLASS( CESMRViewerStartDateField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerStartDateField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerStartDateField* NewL();

    /**
     * C++ destructor
     */
    ~CESMRViewerStartDateField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void LockL();

public: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

private: // Implementation
    CESMRViewerStartDateField();
    void ConstructL();
    void FormatDateStringL();

private: // Data
    // Owned: Field icon
    CMRImage* iIcon;
    /// Own: lock icon
    CMRImage* iLockIcon;
    // Not own: Field text label
    CMRLabel* iLabel;
    /// Own: Meeting start time
    TTime iStartTime;
    };

#endif // __CESMRVIEWERSTARTDATEFIELD_H__
