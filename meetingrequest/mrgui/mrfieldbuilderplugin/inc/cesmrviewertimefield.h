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
* Description:  ESMR viewer time field.
*
*/

#ifndef __CESMRVIEWERTIMEFIELD_H__
#define __CESMRVIEWERTIMEFIELD_H__

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRImage;
class CMRLabel;
class CMRBackground;

/**
 * This field is responsible of showing the scheduled time.
 */
NONSHARABLE_CLASS( CESMRViewerTimeField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerTimeField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerTimeField* NewL();

    /**
     * C++ destructor
     */
    ~CESMRViewerTimeField();

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
    CESMRViewerTimeField();
    void ConstructL();
    void FormatTimeFieldStringL();

private:
    // Owned: Field icon
    CMRImage* iIcon;
    /// Own: lock icon
    CMRImage* iLockIcon;
    // Not own: Field text label
    CMRLabel* iLabel;
    /// Own: Entry start time
    TTime iStartTime;
    /// Own: Entry end time
    TTime iEndTime;
    };

#endif // __CESMRVIEWERTIMEFIELD_H__
