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
* Constant definitions of Email Installation Initiator
*
*/

#ifndef EMAILINSTALLATIONINITIATORCONST_H
#define EMAILINSTALLATIONINITIATORCONST_H


#include <e32base.h>


/**  One second in micro seconds and default waiting time to wait services
 *   to launch themselves
 */
const TInt KEmailInitiatorOneSecond = 1000000;
const TInt KDefaultTimeToWaitInSeconds = 1;

/**  Name of the Email Installation Initiator process/thread
 */
_LIT( KEmailInstallatioInitiatorName, "EmailInstallationInitiator" );


#endif // EMAILINSTALLATIONINITIATORCONST_H
