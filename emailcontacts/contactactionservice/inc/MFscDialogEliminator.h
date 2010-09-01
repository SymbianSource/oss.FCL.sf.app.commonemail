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
* Description:  Generic dialog eliminator interface.
 *
*/


#ifndef MFSCDIALOGELIMINATOR_H
#define MFSCDIALOGELIMINATOR_H

// CLASS DECLARATION

/**
 * Generic dialog eliminator interface.
 */
class MFscDialogEliminator
    {
public:
    // Interface

    /**
     * Tells the dialog exit is now approved and requests it to happen.
     * The exit is done with supplied command id.
     *
     * @param aCommandId    Exit command id.
     */
    virtual void RequestExitL(TInt aCommandId) = 0;

    /**
     * Tries to exit dialog nicely, but if that does not succeed
     * exits the dialog brute force.
     */
    virtual void ForceExit() = 0;

    /**
     * Reset eliminator pointer when dialog has destroyed.
     *
     * @param aSelfPtr  Pointer to this eliminator pointer.
     */
    virtual void ResetWhenDestroyed(MFscDialogEliminator** aSelfPtr) = 0;

    };

#endif // MFSCDIALOGELIMINATOR_H
// End of File
