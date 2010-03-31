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
* Description: Class for the editora focus strategy
*
*/

#ifndef CMRFOCUSSTRATEGYEDITOR_H
#define CMRFOCUSSTRATEGYEDITOR_H

// INCLUDES
#include <e32base.h>
#include "cmrfocusstrategybase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRFocusStrategyEditor ): public CMRFocusStrategyBase
    {
    public: // C'tor & d'tor
        /**
         * Static constructor
         * @param aFactory Field storage factory
         */
        static CMRFocusStrategyEditor* NewL( MESMRFieldStorage& aFactory );

        /**
         * Destructor
         */
        ~CMRFocusStrategyEditor();
        
    protected: // From CMRFocusStrategyBase
        void EventOccured( TFocusEventType aEventType );
        void InitializeFocus();
        
    private: // Implementation
        CMRFocusStrategyEditor( MESMRFieldStorage& aFactory );
    };
#endif // CMRFOCUSSTRATEGYEDITOR_H

// End of file
