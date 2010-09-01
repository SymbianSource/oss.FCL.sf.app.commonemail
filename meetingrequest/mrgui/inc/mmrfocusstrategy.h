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
* Description: Interface class for the focus strategy
*
*/

#ifndef MMRFOCUSSTRATEGY_H
#define MMRFOCUSSTRATEGY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CESMRField;
class MESMRFieldStorage;

// CLASS DECLARATIONS

class MMRFocusStrategy
    {
    public: // Types
        /**
         * Event types where the strategy will react on.
         */
        enum TFocusEventType
            {
            EFocusPointerEvent,
            EFocusKeyEvent
            };

    public: // Destructor
        virtual ~MMRFocusStrategy() { }
        
    public: // Interface
        
        /**
         * Called to initialize the focus
         */
        virtual void InitializeFocus() = 0;
        
        /**
         * Called when the event occurs
         *
         * @param aEventType Type of the event e.g. key, pointer
         */
        virtual void EventOccured( TFocusEventType aEventType ) = 0;

        /**
         * Returns either the focus is on or off
         * 
         * @return ETrue if the focus is visible
         */
        virtual TBool IsFocusVisible() const = 0;
    };
#endif // MMRFOCUSSTRATEGY_H

// End of file
