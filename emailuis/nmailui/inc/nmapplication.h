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
class NmUriServiceInterface;
class NmMailboxServiceInterface;
class NmViewerServiceInterface;
class NmViewerViewNetManager;
class NmUtilities;
class NmAttachmentManager;
class NmSettingsViewLauncher;
class NmUiEffects;
class HbMessageBox;

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
    bool eventFilter(QObject *obj, QEvent *event);
    bool updateVisibilityState();

public slots:
    void prepareForPopView();
    void popView();
    void exitApplication();
    void delayedExitApplication();
    void handleOperationCompleted(const NmOperationCompletionEvent &event);
    void viewReady();
    void launchSettings(HbAction* action);
    
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
    NmSendServiceInterface *mSendServiceInterface;       // Owned
    NmSendServiceInterface *mSendServiceInterface2;      // Owned
    NmUriServiceInterface *mUriServiceInterface;         // Owned
    NmMailboxServiceInterface *mMailboxServiceInterface; // Owned
    NmViewerServiceInterface *mViewerServiceInterface;   // Owned
    NmMailboxListModel *mMbListModel;       // Not owned
    NmUiViewId mServiceViewId;
    NmViewerViewNetManager *mNetManager;    // Owned
    bool mForegroundService;
    NmUiEffects *mEffects;                  // Owned
    NmAttachmentManager *mAttaManager;      // Owned
    NmSettingsViewLauncher* mSettingsViewLauncher; // Owned
    bool mViewReady;
    NmId mLastOperationMailbox;
    HbMessageBox *mQueryDialog;             // Owned
};

#endif // NMAPPLICATION_H
