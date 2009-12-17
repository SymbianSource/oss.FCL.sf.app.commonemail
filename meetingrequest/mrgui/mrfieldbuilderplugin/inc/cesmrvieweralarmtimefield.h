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
* Description:  ESMR viewer alarm field for CESMRListComponent
*
*/


#ifndef CESMRVIEWERALARMTIMEFIELD_H
#define CESMRVIEWERALARMTIMEFIELD_H

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRImage;
class CMRLabel;

/**
 * This class is a custom field control that shows the alarm time of calendar events
 */
NONSHARABLE_CLASS( CESMRViewerAlarmTimeField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerAlarmTimeField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerAlarmTimeField* NewL();
    
    /**
     * C++ Destructor.
     */
    ~CESMRViewerAlarmTimeField();
    
public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void SetOutlineFocusL( TBool aFocus );

public: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;

private: // Implementation
    CESMRViewerAlarmTimeField();
    void ConstructL();
    
private:
    // Owned: Field icon
    CMRImage* iIcon;
    // Owned: Field text label
    CMRLabel* iLabel;
    };

#endif // CESMRVIEWERALARMTIMEFIELD_H
