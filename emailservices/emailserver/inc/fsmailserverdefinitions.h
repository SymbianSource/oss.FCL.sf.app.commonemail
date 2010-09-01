/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Definitons for emailserver.
*
*/



#ifndef FSMAILSERVERDEFINITIONS_H
#define FSMAILSERVERDEFINITIONS_H


/**  Server Uid
 *   If you change this change it also from the pkg file.
 *   (normal and stub)
 */
// <cmail> S60 UID update
#define KFSMailServerUid 0x2001F40A
// </cmail> S60 UID update

/**  Autostarter Uid
 *   If you change this change also the resource
 *   file name in fsemailserverautostart.mmp and in
 *   fsmailserver.pkg. Rename also the actual resource file.
 */
// <cmail> S60 UID update
#define KFSMailServerAutoStartUid 0x2001F40B
// </cmail> S60 UID update

/**  Uid for Mail Server Logger Utility
 */
// <cmail> S60 UID update
#define KFSMailServerDebugLoggerUtilityUid 0x2001F452
// </cmail> S60 UID update

/**  Name of the server executable
 */
#define KFSMailServerExe "fsmailserver.exe"


#endif // FSMAILSERVERDEFINITIONS_H
