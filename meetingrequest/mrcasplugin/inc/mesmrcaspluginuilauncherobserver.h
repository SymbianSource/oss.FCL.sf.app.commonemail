/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class MESMRCasPluginUiLauncherObserver
*
*/

#ifndef MESMRCASPLUGINUILAUNCHEROBSERVER_H_
#define MESMRCASPLUGINUILAUNCHEROBSERVER_H_

/**
 * MESMRCasPluginUiLauncherObserver defines observer
 * interface for asynchronous callback for
 * notifying that editor is initialized and
 * ready to be shown on screen.
 */
class MESMRCasPluginUiLauncherObserver
    {
public: // destructor
    virtual ~MESMRCasPluginUiLauncherObserver() {}
    
public: // interface

    /**
     * Called to notify observer that MR editor initialization
     * is done and it will be shown on display.
     */
    virtual void MREditorInitializationComplete() = 0;
    };

#endif // MESMRCASPLUGINUILAUNCHEROBSERVER_H_

// EOF
