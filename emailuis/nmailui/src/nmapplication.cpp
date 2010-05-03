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
#include <email_services_api.h>
#include <e32base.h>
#else
#define NM_WINS_ENV
#endif

static const qreal nmCacheSize = 2097152;
static const char *NMUI_CACHE_DIR = "cache";
static const QString NmSendServiceName = "com.nokia.symbian.IMessage.Send";

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
mServiceViewId(NmUiViewNone),
mForegroundService(false),
mUtilities(NULL)
{
    // Create network access manager and cache for application use.
    mNetManager = new NmViewerViewNetManager();
    QNetworkDiskCache *cache = new QNetworkDiskCache();
    cache->setCacheDirectory(NMUI_CACHE_DIR);
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
            new NmSendServiceInterface(NmSendServiceName, NULL, *mUiEngine, this);
    mSendServiceInterface2 =
            new NmSendServiceInterface(emailInterfaceNameSend, NULL, *mUiEngine, this);
    mMailboxServiceInterface =
            new NmMailboxServiceInterface(NULL, *mUiEngine, this);
    mViewerServiceInterface =
            new NmViewerServiceInterface(NULL, this, *mUiEngine);
#endif
    mUtilities = new NmUtilities();
}

/*!
    Destructor
*/
NmApplication::~NmApplication()
{
#ifndef NM_WINS_ENV
	delete mSendServiceInterface;
	delete mSendServiceInterface2;
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
    delete mUtilities;
    delete mMainWindow;
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

    // Create extension manager
    mExtensionManager = new NmUiExtensionManager();

    // Create view stack
    mViewStack = new QStack<NmBaseView*>;

    // Create back action and connect it to popView()
    if (mMainWindow) {
        mBackAction = new HbAction(Hb::BackNaviAction,this);
        connect(mBackAction, SIGNAL(triggered()), this, SLOT(popView()));
        // Show main window
        mMainWindow->show();
    }

    // async operation completion related notifications
    connect(
        mUiEngine, SIGNAL(operationCompleted(const NmOperationCompletionEvent &)),
        this, SLOT(handleOperationCompleted(const NmOperationCompletionEvent &)));

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

            // if we were in editor and sent a message, pop viewer from stack first
            // so we can go straight to mail list
            if (!mViewStack->isEmpty() && topViewId == NmUiViewMessageEditor &&
                mUiEngine->isSendingMessage() &&
                mViewStack->top()->nmailViewId() == NmUiViewMessageViewer) {
                NmBaseView *tmpView = mViewStack->pop();
                mMainWindow->removeView(tmpView);
                delete tmpView;
                tmpView = NULL;
            }

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
            // If view was started as service, move the app now
            // to the background, unless it was started when the app
            // was already in foreground..
            if (mServiceViewId == topViewId) {
                mServiceViewId = NmUiViewNone;

                // if started as embedded, do not hide the app
		        if (!XQServiceUtil::isEmbedded() &&
		            !mForegroundService) {
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

        if (startParam->service() && mMainWindow) {
			// Store the visibility state when the service was launched
			mForegroundService = mMainWindow->isVisible();

			// When the message list is started as a service previous views are removed
			// from the stack. Open editors are not closed.
			// Also if the view is same than the new one, keep it open (reload the content).
			
		    // at least one view must remain in the stack
			while (mViewStack->count()>1) { 
			    NmUiViewId topId = mViewStack->top()->nmailViewId();
			    if (topId!=NmUiViewMessageEditor && 
			        topId!=NmUiViewMailboxList &&
			        topId!=startParam->viewId()) {
			        popView();
			    }
			    else {
			        // Editor or mailbox list in the top. Stop the loop.
			        break;
			    }
			}
        }
        
        // Check whether requested view is already active
        // and if so, ask it to reload contents with new start parameter data
        // Do not reuse the view if started as service to editor view (ShareUI)
        if (mActiveViewId==startParam->viewId() &&
        	(!startParam->service() || mActiveViewId!=NmUiViewMessageEditor)) {
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
                    // Check the topmost view. If it is an editor, do not open
                    // a new mail list view
                    if (startParam->service() && !mViewStack->isEmpty() && 
                        mViewStack->top()->nmailViewId()==NmUiViewMessageEditor) {
                        break;
                    }
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
                    startParam=NULL;
                    resetViewStack();
                    break;
            }

            if (startParam && startParam->service()) {
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
    delete mSendServiceInterface2;
    mSendServiceInterface2 = NULL;
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
NmViewerViewNetManager& NmApplication::networkAccessManager()
{
    return *mNetManager;
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

/*!
    handles all asynchronous operation's completions at UI level
*/
void NmApplication::handleOperationCompleted(const NmOperationCompletionEvent &event)
{
    mUtilities->displayOperationCompletionNote(event);
}
