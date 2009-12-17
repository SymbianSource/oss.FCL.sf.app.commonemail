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
* Description:  ESMR viewer time field for CESMRListComponent
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

public: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
private: // Implementation
    CESMRViewerTimeField();
    void ConstructL();
    
private:
    // Owned: Field icon
    CMRImage* iIcon;
    // Owned: Field text label
    CMRLabel* iLabel;
    };

#endif // __CESMRVIEWERTIMEFIELD_H__
