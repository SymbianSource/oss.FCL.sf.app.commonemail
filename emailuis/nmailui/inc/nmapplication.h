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
*
*/

#ifndef NMAPPLICATION_H
#define NMAPPLICATION_H

#include <QStack>
#include <QObject>

#include "nmuiviewids.h"

class HbMainWindow;
class NmBaseView;
class NmUiEngine;
class NmMailboxListModel;
class HbAction;
class NmUiExtensionManager;
class NmSendServiceInterface;
class NmMailboxServiceInterface;
class NmViewerServiceInterface;
class NmViewerViewNetManager;
class NmUtilities;

class NmApplication : public QObject
{
    Q_OBJECT
public:
    NmApplication(QObject *parent);
    ~NmApplication();
    void enterNmUiView(NmUiStartParam *startParam);
    HbMainWindow* mainWindow();
    NmUiExtensionManager &extManager();
    NmViewerViewNetManager &networkAccessManager();
    QSize screenSize();


public slots:
    void popView();
    void exitApplication();
    void delayedExitApplication();
    void handleOperationCompleted(const NmOperationCompletionEvent &event);

private:
    void createMainWindow();
    void pushView(NmBaseView *view);
    void resetViewStack();

private:
    HbMainWindow *mMainWindow;              // Not owned
    QStack<NmBaseView*> *mViewStack;        // Owned
    NmUiViewId mActiveViewId;
    NmUiEngine *mUiEngine;                  // Owned
    HbAction *mBackAction;                  // Owned
    NmUiExtensionManager *mExtensionManager;// Owned
    NmSendServiceInterface *mSendServiceInterface; // Owned
    NmSendServiceInterface *mSendServiceInterface2; // Owned
    NmMailboxServiceInterface *mMailboxServiceInterface; // Owned
    NmViewerServiceInterface *mViewerServiceInterface; // Owned
    NmMailboxListModel *mMbListModel;       // Not owned
    NmUiViewId mServiceViewId;
    NmViewerViewNetManager *mNetManager;     // Owned
    bool mForegroundService;	
    NmUtilities *mUtilities; // Owned
};

#endif // NMAPPLICATION_H
