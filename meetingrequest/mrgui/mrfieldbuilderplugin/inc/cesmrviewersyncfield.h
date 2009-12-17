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
* Description:  Syncronization field
*
*/


#ifndef __CESMRVIEWERSYNCFIELD_H__
#define __CESMRVIEWERSYNCFIELD_H__

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRImage;
class CMRLabel;


/**
 * This class shows state of syncronization.
 */
NONSHARABLE_CLASS( CESMRViewerSyncField ) : public CESMRField
    {
public:
    /**
     * Creates new CESMRViewerSyncField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerSyncField* NewL();

    /**
     * C++ destructor
     */
    ~CESMRViewerSyncField();
    
public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void SetOutlineFocusL( TBool aFocus );
    
public: // From CCoeControl
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
private: // Implementation
    CESMRViewerSyncField();
    void ConstructL();
    
private:
    // Owned: Field icon
    CMRImage* iIcon;
    // Owned: Field text label
    CMRLabel* iLabel;
    };

#endif //__CESMRVIEWERSYNCFIELD_H__
