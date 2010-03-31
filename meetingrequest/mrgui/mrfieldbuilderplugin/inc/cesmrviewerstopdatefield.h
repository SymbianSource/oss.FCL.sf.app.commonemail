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
* Description:  ESMR viewer end date field.
*
*/


#ifndef __CESMRVIEWERSTOPDATEFIELD_H__
#define __CESMRVIEWERSTOPDATEFIELD_H__

// INLCUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRImage;
class CMRLabel;

// CLASS DECLARATIONS
/**
 * This class shows the end date of meeting or meeting request
 */
NONSHARABLE_CLASS( CESMRViewerStopDateField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerStopDateField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerStopDateField* NewL();

    /**
     * C++ destructor
     */
    ~CESMRViewerStopDateField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void SetOutlineFocusL( TBool aFocus );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void LockL();

public: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

private: // Implementation
    CESMRViewerStopDateField();
    void ConstructL();
    void FormatDateStringL();
    
private: // Data
    // Owned: Field icon
    CMRImage* iIcon;
    /// Own: lock icon
    CMRImage* iLockIcon;
    // Not own: Field text label
    CMRLabel* iLabel;
    /// Own: Meeting stop time
    TTime iStopTime;        
};

#endif // __CESMRVIEWERSTOPDATEFIELD_H__
