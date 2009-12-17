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
* Description:  This field shows the text part of recurrence information
*
*/

#ifndef __CESMRVIEWERRECURRENCEFIELD_H__
#define __CESMRVIEWERRECURRENCEFIELD_H__

// INCLUDES
#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CMRLabel;
class CMRImage;

/**
 * This class is responsible of showing the recurrence text e.g. "Daily"
 */
NONSHARABLE_CLASS( CESMRViewerRecurrenceField ): public CESMRField
    {
    public:
        /**
         * Creates new CESMRViewerRecurrenceField object. Ownership
         * is transferred to caller.
         * @return Pointer to created object,
         */
        static CESMRViewerRecurrenceField* NewL();
    
        /**
         * C++ destructor
         */
        ~CESMRViewerRecurrenceField();
    
    public: // From CESMRField
        void InternalizeL( MESMRCalEntry& aEntry );
        void InitializeL();
        void SetOutlineFocusL( TBool aFocus );
        
    public: // From CCoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
    private: // Implementation
        CESMRViewerRecurrenceField();
        void ConstructL();
        
    private: // Data
        /// Own: The recurrence topic field
        CMRLabel* iLabel;
        /// Own: Recurrence icon (default)
        CMRImage* iIcon;
    };

#endif // __CESMRVIEWERRECURRENCEFIELD_H__
