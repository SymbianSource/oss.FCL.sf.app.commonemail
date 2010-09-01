/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control key event observer.
 *
*/


#ifndef MFSCCONTROLKEYOBSERVER_H
#define MFSCCONTROLKEYOBSERVER_H

//  INCLUDES
#include <coedef.h> // TKeyResponse
#include <w32std.h> // TKeyEvent, TEventCode
// CLASS DECLARATION

/**
 * Control key event observer interface.
 */
class MFscControlKeyObserver
    {
public:
    // Interface
    /**
     * Called from OfferKeyEventL of the observed control.
     *
     * @param aKeyEvent   see CCoeControl::OfferKeyEventL.
     * @param aType       see CCoeControl::OfferKeyEventL.
     * @return see CCoeControl::OfferKeyEventL.
     * @see CCoeControl::OfferKeyEventL. If returns EKeyWasConsumed the 
     *      control should not forward the key event.
     */
    virtual TKeyResponse FscControlKeyEventL(const TKeyEvent& aKeyEvent,
            TEventCode aType) = 0;
    };

#endif // MFSCCONTROLKEYOBSERVER_H
// End of File
