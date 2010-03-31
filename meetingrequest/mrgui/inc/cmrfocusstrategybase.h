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
* Description: Base class for the focus strategies
*
*/

#ifndef CMRFOCUSSTRATEGYBASE_H
#define CMRFOCUSSTRATEGYBASE_H

// INCLUDES
#include <e32base.h>
#include "mmrfocusstrategy.h"

// FORWARD DECLARATIONS
class CESMRField;
class MESMRFieldStorage;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRFocusStrategyBase ): public CBase, public MMRFocusStrategy
    {
    protected:
        void InitializeFocus();
        void EventOccured( TFocusEventType aEventType );
        TBool IsFocusVisible() const { return iVisibleFocusOn; }
        
    protected: // Implementation
        CMRFocusStrategyBase( MESMRFieldStorage& aFactory );
        ~CMRFocusStrategyBase();

        void HideFocus();
        void ShowFocus();

    protected: // Data
        /// Flag indicating the focus state
        TBool iVisibleFocusOn;
        /// Field storage reference
        MESMRFieldStorage& iFactory;
    };
#endif // CMRFOCUSSTRATEGYBASE_H

// End of file
