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
* Description:
* Declaration of common functions of Email Server Monitor
*
*/
//  Include Files  

#ifndef EMAILSERVERMONITORUTILITIES_H
#define EMAILSERVERMONITORUTILITIES_H

#include <e32base.h>
#include <e32cmn.h>     // TSecureId

const TInt KDefaultTimeToWaitInSeconds = 3;

/////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION DECLARATIONS

/**
 * Wait some time to give other applications and servers some time to shut
 * down themselves gracefully or to start themselves
 * 
 * @param aWaitTimeInSeconds Time to wait in seconds
 */
void Wait( TInt aWaitTimeInSeconds = KDefaultTimeToWaitInSeconds );

/**
 * Check is specified process running. If we are checking other UID than
 * ourselves, we just check is there specified process running. If we are
 * checking our own UID, then we check is there another instance already
 * running.
 * 
 * @param aUid Process UID (=Secure ID) to be searched
 * @param aProcess If process handle is given, it will updated to have a
 *                 a handle to the found process
 * @param aProcessUpdated On return ETrue, if aProcess handle was given and
 *                        it was updated succesfully
 * @return ETrue if the specified process is running, otherwise EFalse.
 */
TBool IsProcessRunning( TSecureId aUid, RProcess* aProcess = NULL );


#endif // EMAILSERVERMONITORUTILITIES_H
