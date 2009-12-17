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
* Description:  ESMR viewer end date field for CESMRListComponent
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
        
    public: // From CCoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
    private: // Implementation
        CESMRViewerStopDateField();
        void ConstructL();
        
    private: // Data
        // Owned: Field icon
        CMRImage* iIcon;
        // Owned: Field text label
        CMRLabel* iLabel;
    };

#endif // __CESMRVIEWERSTOPDATEFIELD_H__
