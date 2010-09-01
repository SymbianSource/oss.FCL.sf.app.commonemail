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

#ifndef CMRFOCUSSTRATEGYVIEWER_H
#define CMRFOCUSSTRATEGYVIEWER_H

// INCLUDES
#include <e32base.h>
#include "cmrfocusstrategybase.h"   // CMRFocusStrategyBase

// FORWARD DECLARATIONS

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRFocusStrategyViewer ):
    public CMRFocusStrategyBase    
    {
    public: // C'tor & d'tor
        /**
         * Static constructor
         * @param aFactory Field storage factory
         */
        static CMRFocusStrategyViewer* NewL( MESMRFieldStorage& aFactory );

        /**
         * Destructor
         */
        ~CMRFocusStrategyViewer();
        
    protected: // From CMRFocusStrategyBase
        void EventOccured( TFocusEventType aEventType );
        void InitializeFocus();

    private: // Implementation
        CMRFocusStrategyViewer( MESMRFieldStorage& aFactory );
        void ConstructL();
        void DoHideFocus();    
    };
#endif // CMRFOCUSSTRATEGYVIEWER_H

// End of file
