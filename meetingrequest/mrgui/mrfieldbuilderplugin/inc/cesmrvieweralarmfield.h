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


#ifndef __CESMRVIEWERALARMFIELD_H__
#define __CESMRVIEWERALARMFIELD_H__

#include "cesmrfield.h"

// Forwar declarations
class CMRImage;
class CMRLabel;

/**
 * This class is a custom field control that shows the alarms of calendar events
 */
NONSHARABLE_CLASS( CESMRViewerAlarmField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerAlarmField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerAlarmField* NewL();

    /**
     * C++ Destructor.
     */
    ~CESMRViewerAlarmField();

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void SetOutlineFocusL( TBool aFocus );
    void LockL();
    TBool ExecuteGenericCommandL( TInt aCommand );

public: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

private: // Implementation
    CESMRViewerAlarmField();
    void ConstructL();

private: // members
    // Owned: Field icon
    CMRImage* iIcon;
    // Not own: Field text label
    CMRLabel* iLabel;
    // Own: lock icon
    CMRImage* iLockIcon;
    };


#endif // __CESMRVIEWERALARMFIELD_H__
