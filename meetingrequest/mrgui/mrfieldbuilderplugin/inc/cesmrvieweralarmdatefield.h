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
* Description:  ESMR viewer alarm field.
*
*/

#ifndef __CESMRVIEWERALARMDATEFIELD_H__
#define __CESMRVIEWERALARMDATEFIELD_H__

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRImage;
class CMRLabel;

// CLASS DECLARATIONS
/**
 * Field for showing the alarm date.
 */
NONSHARABLE_CLASS( CESMRViewerAlarmDateField ): public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerAlarmDateField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerAlarmDateField* NewL();

    /**
     * C++ Destructor.
     */
    ~CESMRViewerAlarmDateField();

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
    CESMRViewerAlarmDateField();
    void ConstructL();
    void FormatAlarmTimeL();
    
private: // Data
    // Owned: Field icon
    CMRImage* iIcon;
    /// Own: lock icon
    CMRImage* iLockIcon;
    // Not own: Field text label
    CMRLabel* iLabel;
    /// Own: Alarm time
    TTime iAlarmTime;
    };

#endif // __CESMRVIEWERALARMFIELD_H__
