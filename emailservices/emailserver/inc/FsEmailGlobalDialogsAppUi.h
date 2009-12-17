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
* Description:  Declaration of FSMailServer global dialogs app ui
*
*/


#ifndef C_FSEMAILGLOBALDIALOGSAPPUI_H
#define C_FSEMAILGLOBALDIALOGSAPPUI_H

#include <aknappui.h>

class CFSNotificationHandlerMgr;
class CEmailServerShutdownObserver;

/**
 *  Application UI class for FSMailServer.
 *  Needed to be able to show Avkon dialogs from FSMailServer.
 */
class CFsEmailGlobalDialogsAppUi : public CAknAppUi
    {
public:
    CFsEmailGlobalDialogsAppUi();
    ~CFsEmailGlobalDialogsAppUi();
    void ConstructL();
    
    void SendToBackground();
    void BringToForeground();

public:    // From base class CAknAppUi
    void Exit();

private:

#ifdef _DEBUG
    void EnsureDebugDirExists();
#endif
    
private:

    /**
     * Notification handler manager.
     * Own.
     */
    CFSNotificationHandlerMgr* iNotificationHandlerMgr;

    /**
     * Shutdown watcher.
     * Own.
     */
    CEmailServerShutdownObserver* iShutdownObserver;
    };


#endif // C_FSEMAILGLOBALDIALOGSAPPUI_H

// End of File
