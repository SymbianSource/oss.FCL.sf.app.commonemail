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
* Description:  ESMR viewer start date field for CESMRListComponent
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
        void InitializeL();
        void SetOutlineFocusL( TBool aFocus );
        
    public: // From CCoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
    private: // Implementation
        CESMRViewerStartDateField();
        void ConstructL();
        
    private: // Data
        // Owned: Field icon
        CMRImage* iIcon;
        // Owned: Field text label
        CMRLabel* iLabel;
    };

#endif // __CESMRVIEWERSTARTDATEFIELD_H__
