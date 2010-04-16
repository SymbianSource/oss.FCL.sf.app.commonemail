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

#include "nmuiheaders.h"
#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#else
#define NM_WINS_ENV
#endif

static const qreal nmCacheSize = 2097152;

/*!
	\class NmApplication
	\brief Application class, creates main window and handles view switching
*/

/*!
    Constructor
*/
NmApplication::NmApplication(QObject *parent)
:QObject(parent),
mMainWindow(NULL),
mViewStack(NULL),
mActiveViewId(NmUiViewNone),
mUiEngine(NULL),
mBackAction(NULL),
mExtensionManager(NULL),
mMbListModel(NULL),
mServiceViewId(NmUiViewNone)
{
    // Create network access manager and cache for application use.
    mNetManager = new NmViewerViewNetManager();
    QNetworkDiskCache *cache = new QNetworkDiskCache();
    cache->setMaximumCacheSize(nmCacheSize);
    mNetManager->setCache(cache);

    // TEMPORARY WORKAROUND TO PREVENT PANIC User panic 66, where there is
    // a PushL call to cleanup stack without any TRAP.
    #ifdef Q_OS_SYMBIAN
    TRAP_IGNORE(mUiEngine = NmUiEngine::instance());
    #else
    mUiEngine = NmUiEngine::instance();
    #endif
    createMainWindow();

#ifndef NM_WINS_ENV
    mSendServiceInterface =
            new NmSendServiceInterface(NULL, *mUiEngine, this);
    mMailboxServiceInterface =
            new NmMailboxServiceInterface(NULL, *mUiEngine, this);
    mViewerServiceInterface =
            new NmViewerServiceInterface(NULL, this, *mUiEngine);
#endif
}

/*!
    Destructor
*/
NmApplication::~NmApplication()
{
#ifndef NM_WINS_ENV
	delete mSendServiceInterface;
	delete mMailboxServiceInterface;
	delete mViewerServiceInterface;
#endif

	resetViewStack();
    delete mViewStack;
    NmIcons::freeIcons();
    NmUiEngine::releaseInstance(mUiEngine);
    mUiEngine = NULL;
    delete mBackAction;
    delete mExtensionManager;
    if (mNetManager){
        if (mNetManager->cache()){
            mNetManager->cache()->clear();
        }
    delete mNetManager;
    mNetManager=NULL;
    }
}

/*!
    Main application window creation
*/
void NmApplication::createMainWindow()
{
    NMLOG("nmailui: createMainWindow enter");

#ifndef NM_WINS_ENV
    bool service = XQServiceUtil::isService();
    if (service && !XQServiceUtil::isEmbedded()) {
        // If started as service, keep it hidden until everything is initialised
        // In embedded mode, the client app should not get hidden
        XQServiceUtil::toBackground(true);
    }
#else
    bool service = false;
#endif

    // Register custom widget files
    HbStyleLoader::registerFilePath(":nmmessagelistviewitem.widgetml");
    HbStyleLoader::registerFilePath(":nmmessagelistviewitem.css");
    HbStyleLoader::registerFilePath(":nmviewerheader.widgetml");
    HbStyleLoader::registerFilePath(":nmviewerheader.css");

    // Create main window
    mMainWindow = new HbMainWindow();
    mMainWindow->show();

    // Create extension manager
    mExtensionManager = new NmUiExtensionManager();

    // Create view stack
    mViewStack = new QStack<NmBaseView*>;

    // Create back action and connect it to popView()
    if (mMainWindow) {
        mBackAction = new HbAction(Hb::BackNaviAction,this);
        connect(mBackAction, SIGNAL(triggered()), this, SLOT(popView()));
    }

    mMbListModel = &mUiEngine->mailboxListModel();

    // Start application to mailbox view
    // If started as service, there is no need to create views
    if (!service) {
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMailboxList);
        enterNmUiView(startParam);
    }
}

/*!
    Push view to view stack
*/
void NmApplication::pushView(NmBaseView *view)
{
    NMLOG("nmailui: pushView enter");
    if (view) {
        view->setNavigationAction(mBackAction);
        NMLOG("nmailui: view exists");
    	if (!mViewStack->isEmpty()) {
        	NmBaseView *hideView = mViewStack->top();
        	NMLOG("nmailui: removeView");
            mMainWindow->removeView(hideView);
        }
    	NMLOG("nmailui: addView");
        mMainWindow->addView(view);
        mViewStack->push(view);
        mActiveViewId=view->nmailViewId();

        //set toolbars orientation
        HbToolBar *tb = view->toolBar();
        if(tb){
            tb->setOrientation(Qt::Horizontal);
        }

        NMLOG("nmailui: pushView done");
    }
}

/*!
    Pop view from view stack. View object is deleted.
*/
void NmApplication::popView()
{
    NMLOG("nmailui: popView enter");
    if (mViewStack->size() > 0) {
        // Get view pointer
        NmBaseView *view = mViewStack->top();

        // ask view if it's ok to exit
        if (view->okToExitView()) {
            mViewStack->pop();
            // Call custom exit function
            view->aboutToExitView();
            NmUiViewId topViewId = view->nmailViewId();
            // Remove view from stack.
            mMainWindow->removeView(view);

            if (!mViewStack->isEmpty()) {
                // Activate next view in stack
                NmBaseView *showView = mViewStack->top();
                mMainWindow->addView(showView);
                // Store activated view id
                mActiveViewId=showView->nmailViewId();
            }

            delete view;
            view = NULL;

#ifndef NM_WINS_ENV
            // If view was started as service, move the app now to the background
            if (mServiceViewId == topViewId) {
                mServiceViewId = NmUiViewNone;

                // if started as embedded, do not hide the app
		        if (!XQServiceUtil::isEmbedded()) {
		            XQServiceUtil::toBackground(true);
                }
            }
#endif
        }
    }

    // If the view stack is now empty quit the app
    // This happens also when the app has been started as a service
    if (mViewStack->size()==0) {
        exitApplication();
    }
}

/*!
    Reset view stack. Remove and destroy view objects.
*/
void NmApplication::resetViewStack()
{
    NMLOG("nmailui: resetViewStack enter");
    if (mViewStack && !mViewStack->isEmpty()) {
	    int viewCount = mViewStack->count();
        // Pop and destroy all views
	    for (int i=0;i<viewCount;i++) {
		    NmBaseView *view = mViewStack->pop();
            mMainWindow->removeView(view);
            delete view;
        }
	    mActiveViewId=NmUiViewNone;
    }
}

/*!
    Function activates view based on viewId parameter.
    If requested view is already open, it is requested to reload.
    Otherwise view object is created and pushed to view stack
*/
void NmApplication::enterNmUiView(NmUiStartParam* startParam)
{
    // Check the validity of start parameter object
    if (startParam) {

        // Check whether requested view is already active
        // and if so, ask it to reload contents with new start parameter data
        // Do not reuse the view if started as service (ShareUI or Launch API)
        if (mActiveViewId==startParam->viewId() && !startParam->service()) {
            mViewStack->top()->reloadViewContents(startParam);
        }
        else {
            switch (startParam->viewId()) {
                case NmUiViewMailboxList:
                {
                    NmMailboxListView *mbListView = new NmMailboxListView(
                    		*this, startParam, *mUiEngine,
                    		*mMbListModel, new HbDocumentLoader(mMainWindow));
                    pushView(mbListView);
                }
                break;
                case NmUiViewMessageList:
                {
                    NmMessageListModel &messageListModel = mUiEngine->messageListModel(
                                                startParam->mailboxId(), startParam->folderId());
                    NmMessageListView *msgList =new NmMessageListView(
                    		*this, startParam, *mUiEngine, *mMbListModel, messageListModel,
                    		new HbDocumentLoader(mMainWindow));
                    pushView(msgList);
                }
                break;
                case NmUiViewMessageViewer:
                    pushView(new NmViewerView(*this, startParam, *mUiEngine, mMainWindow));
                    break;
                case NmUiViewMessageEditor:
                    pushView(new NmEditorView(*this, startParam, *mUiEngine));
                    break;
                default:
                    // Reset view stack and exit application
                    delete startParam;
                    resetViewStack();
                    break;
            }

            if (startParam->service()) {
				// Store the view id that was launched as service
        		mServiceViewId = mActiveViewId;
			}
        }
    }
}

/*!
    Function can be used from views to exit the application
    View stack is cleared. Views can connect exit menu
    selection to this slot.
*/
void NmApplication::exitApplication()
{
	NMLOG("NmApplication::exitApplication");
#ifndef NM_WINS_ENV
    delete mSendServiceInterface;
    mSendServiceInterface = NULL;
    delete mMailboxServiceInterface;
    mMailboxServiceInterface = NULL;
    delete mViewerServiceInterface;
    mViewerServiceInterface = NULL;
#endif
    resetViewStack();
    // Do housekeeping if needed.
    qApp->quit();
}

/*!
   Exit the application in the next event loop
*/
void NmApplication::delayedExitApplication()
{
    NMLOG("NmApplication::delayedExitApplication");
    // Exit the application in the next event loop
    QTimer::singleShot(0, this, SLOT(exitApplication()));
}

/*!
    Getter for main window instance.
*/
HbMainWindow* NmApplication::mainWindow()
{
    return mMainWindow;
}

/*!
    Getter for main UI extension manager
*/
NmUiExtensionManager& NmApplication::extManager()
{
    return *mExtensionManager;
}

/*!
    Getter for network access manager
*/
NmViewerViewNetManager* NmApplication::networkAccessManager()
{
    return mNetManager;
}

/*!
    screen size. Function returns curtent screen size
*/
QSize NmApplication::screenSize()
{
    QSize ret(0,0);
    if (mMainWindow){
        HbDeviceProfile currentP = HbDeviceProfile::current();
        HbDeviceProfile altP(currentP.alternateProfileName());
        QSize curPSize = currentP.logicalSize();
        QSize altPSize = altP.logicalSize();
        if (mMainWindow->orientation()==Qt::Horizontal){
            // Get wide profile size in landscape
            if (curPSize.width()>altPSize.width()){
                ret = curPSize;
            }
            else{
                ret = altPSize;
            }
        }
        else {
            // Get narrow profile size in portrait
            if (curPSize.width()<altPSize.width()){
                ret = curPSize;
            }
            else{
                ret = altPSize;
            }
        }
    }
    return ret;
}
