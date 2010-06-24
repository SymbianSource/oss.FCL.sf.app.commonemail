/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <email_services_api.h>
#include <e32base.h>

static const QString NmSendServiceName = "nmail.com.nokia.symbian.IFileShare";

/*!
	\class NmApplication
	\brief Application class, creates main window and handles view switching
*/

/*!
    Constructor
*/
NmApplication::NmApplication(QObject *parent, quint32 accountId)
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
mEffects(NULL),
mAttaManager(NULL),
mSettingsViewLauncher(NULL),
mViewReady(false),
mQueryDialog(NULL),
mBackButtonPressed(false)
{
    NM_FUNCTION;

    TRAP_IGNORE(mUiEngine = NmUiEngine::instance());

    // Create network access manager and cache for application use.
    mNetManager = new NmViewerViewNetManager(*mUiEngine);
    QNetworkDiskCache *cache = new QNetworkDiskCache();
    cache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
    mNetManager->setCache(cache);

    createMainWindow();

    // attachment manager can be shared between viewer and editor, ownership in application class
    mAttaManager = new NmAttachmentManager(*mUiEngine);

    mSendServiceInterface =
            new NmSendServiceInterface(NmSendServiceName, NULL, *mUiEngine, this);
    mSendServiceInterface2 =
            new NmSendServiceInterface(emailFullServiceNameSend, NULL, *mUiEngine, this);
    mUriServiceInterface =
        new NmUriServiceInterface(NULL, *mUiEngine, this);
    mMailboxServiceInterface =
            new NmMailboxServiceInterface(NULL, *mUiEngine, this);
    mViewerServiceInterface =
            new NmViewerServiceInterface(NULL, this, *mUiEngine);

    if(accountId != 0) {
        QVariant mailbox;
        mailbox.setValue(mUiEngine->getPluginIdByMailboxId(accountId).id());
        mMailboxServiceInterface->displayInboxByMailboxId(mailbox);
    }

    mEffects = new NmUiEffects(*mMainWindow);
}

/*!
    Destructor
*/
NmApplication::~NmApplication()
{
    NM_FUNCTION;

    if (mQueryDialog) {
        delete mQueryDialog;
        mQueryDialog=NULL;
    }

	delete mSendServiceInterface;
	delete mSendServiceInterface2;
	delete mUriServiceInterface;
	delete mMailboxServiceInterface;
	delete mViewerServiceInterface;

	resetViewStack();
    delete mViewStack;
    NmIcons::freeIcons();
    NmUiEngine::releaseInstance(mUiEngine);
    mUiEngine = NULL;
    delete mBackAction;
    delete mExtensionManager;
    if (mNetManager) {
        if (mNetManager->cache()) {
            mNetManager->cache()->clear();
        }
    delete mNetManager;
    mNetManager = NULL;
    }
    // Effects needs to be deleted before MainWindow.
    delete mEffects;
    if (mMainWindow) {
        // workaround: assert failure may happen otherwise
        mMainWindow->close();
    }
    delete mMainWindow;
    delete mAttaManager;
    delete mSettingsViewLauncher;
}

/*!
    Main application window creation
*/
void NmApplication::createMainWindow()
{
    NM_FUNCTION;

    bool service = XQServiceUtil::isService();

    // Register custom widget files
    HbStyleLoader::registerFilePath(":nmmessagelistviewitem.widgetml");
    HbStyleLoader::registerFilePath(":nmmessagelistviewitem.css");
    HbStyleLoader::registerFilePath(":nmviewerheader.widgetml");
    HbStyleLoader::registerFilePath(":nmviewerheader.css");

    // Create main window
    mMainWindow = new HbMainWindow();

    // Connect to lazy loading signal
    QObject::connect(mMainWindow, SIGNAL(viewReady ()),
                     this, SLOT(viewReady()));

    // Create extension manager
    mExtensionManager = new NmUiExtensionManager();

    // Create view stack
    mViewStack = new QStack<NmBaseView*>;

    // Create back action and connect it to prepareForPopView()
    if (mMainWindow) {
        mBackAction = new HbAction(Hb::BackNaviAction,this);
        connect(mBackAction, SIGNAL(triggered()), this, SLOT(prepareForPopView()));

        // Show mainwindow
        // Services will active it when the view is ready
        if (!service) {
            mMainWindow->show();
        }
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

    if (mMainWindow) {
        // Start to filter main window events to get "end key" event in all possible situations. Using
        // event() is not enough to catch the event as it is only called if the view widget has the
        // focus. Note: if key capturing (xqkeycapture.h) is required it is probably best to implement
        // an own QMainWindow class and do the capturing there, not in the views.
        mMainWindow->installEventFilter(this);
    
        // Optimize the custom paint functions. 
        // Currently effects to NmViewerHeader::paint & NmAttachmentListWidget::paint
        mMainWindow->setOptimizationFlag(QGraphicsView::DontSavePainterState);    
    }
}

/*!
    Slot. React to view ready signal and call
    current view method.
*/
void NmApplication::viewReady()
{
    NM_FUNCTION;

    mViewReady = true;
    if (mViewStack && !mViewStack->isEmpty()) {
    	NmBaseView *currentView = mViewStack->top();
        if (currentView) {
            currentView->viewReady();
        }
    }
}

/*!
    Event filter. End key is filtered from the main window and either the view takes case of the
	or the app is exited by default.
*/
bool NmApplication::eventFilter(QObject *obj, QEvent *event)
{
    NM_FUNCTION;

    bool consumed(false);

    if (obj && obj == mMainWindow && event && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_No) { // end key, the "red" key

            // exit application if no pending operations are on-going
        }
    }

    if (!consumed) {
        consumed = QObject::eventFilter(obj, event);
    }

    return consumed;
}

/*!
    Push view to view stack
*/
void NmApplication::pushView(NmBaseView *newView)
{
    NM_FUNCTION;

    if (newView && mViewStack) {
        NM_COMMENT("nmailui: view exists");
        newView->setNavigationAction(mBackAction);

        // Store view to be hidden
        NmBaseView *hideView(NULL);
        if (!mViewStack->isEmpty()) {
            hideView = mViewStack->top();
        }
		else {
			// viewReady should be informed immediatelly
			if (mViewReady) {
				newView->viewReady();
			}
		}

        // activate new view
    	NM_COMMENT("nmailui: addView");
        mMainWindow->addView(newView);
        mViewStack->push(newView);
        mMainWindow->setCurrentView(newView);
        mActiveViewId=newView->nmailViewId();

        // Set toolbars orientation
        HbToolBar *tb = newView->toolBar();
        if (tb) {
            tb->setOrientation(Qt::Horizontal);
        }

        // hide old view
        NM_COMMENT("nmailui: removeView");
        if (hideView) {
            mMainWindow->removeView(hideView);
        }

        NM_COMMENT("nmailui: pushView done");
    }
}

/*!
    Ask from view that is it ok to pop view. This kind of 2-phase popView is needed
    because view may show query dialog for user response.
 */
void NmApplication::prepareForPopView()
{
    if (mViewStack && mViewStack->size() > 0) {
        // Get view pointer
        NmBaseView *view = mViewStack->top();
        if (view){
			NmUiViewId topViewId = view->nmailViewId();
			
			// We must know in popView, are we coming with back button from message view
			// to prevent the send animation.
			if (topViewId == NmUiViewMessageEditor) {
				mBackButtonPressed = true;
			}
            
            // View will call/signal popView if exitting is ok.
            view->okToExitView();        
        }
    }
    // If the view stack is now empty quit the app
    // This happens also when the app has been started as a service
    else if (mViewStack && mViewStack->size() == 0) {
        exitApplication();
    }
}

/*!
    Pop view from view stack. View object is deleted.
*/
void NmApplication::popView()
{
    NM_FUNCTION;
    if (mViewStack && mViewStack->size() > 0) {
        NmBaseView *view = mViewStack->top();    
        if (view) {
            // Get top view id
            NmUiViewId topViewId = view->nmailViewId();
    
            // Prepare for send animation if returing from editor and message has been sent.
            if (topViewId == NmUiViewMessageEditor && mUiEngine->isSendingMessage()) {
				// If we are coming from message editor with back button
				// do not prepare the send animation.
				if (!mBackButtonPressed) {
					mEffects->prepareEffect(NmUiEffects::NmEditorSendMessageAnimation);
				}
	        }
            mBackButtonPressed = false;
            mViewStack->pop();
            // Call custom exit function
            view->aboutToExitView();
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
                mMainWindow->setCurrentView(showView);
                // Store activated view id
                mActiveViewId=showView->nmailViewId();
                // Perform send animation if requested.
                mEffects->startEffect(NmUiEffects::NmEditorSendMessageAnimation);
            }
    
            delete view;
            view = NULL;
    
            // If view was started as service, move the app now
            // to the background, unless it was started when the app
            // was already in foreground..
            if (mServiceViewId == topViewId) {
                mServiceViewId = NmUiViewNone;
                NM_COMMENT("Returned from service view");
    
                // if started as embedded or while the app was in foreground,
                // do not hide the app
                if (!XQServiceUtil::isEmbedded() &&
                    !mForegroundService) {
                    XQServiceUtil::toBackground(true);
                }
            }
        }
    }

    // If the view stack is now empty quit the app
    // This happens also when the app has been started as a service
    if (mViewStack && mViewStack->size() == 0) {
        exitApplication();
    }
}

/*!
    Reset view stack. Remove and destroy view objects.
*/
void NmApplication::resetViewStack()
{
    NM_FUNCTION;

    if (mViewStack && !mViewStack->isEmpty()) {
	    int viewCount = mViewStack->count();
        // Pop and destroy all views
	    for (int i=0 ; i < viewCount ; i++) {
		    NmBaseView *view = mViewStack->pop();
            mMainWindow->removeView(view);
            delete view;
        }
	    mActiveViewId = NmUiViewNone;
    }
}

/*!
    Function activates view based on viewId parameter.
    If requested view is already open, it is requested to reload.
    Otherwise view object is created and pushed to view stack
*/
void NmApplication::enterNmUiView(NmUiStartParam *startParam)
{
    NM_FUNCTION;

    // Check the validity of start parameter object
    if (startParam) {

        if (startParam->service() && mMainWindow) {
			// When the message list is started as a service previous views are removed
			// from the stack. Open editors are not closed.
			// Also if the view is same than the new one, keep it open (reload the content).


		    // reset the foreground service flag while popping the views
		    bool previousForegroundService = mForegroundService;
		    mForegroundService = true;

		    // at least one view must remain in the stack
			while (mViewStack->count( )> 1) {
			    NmUiViewId topId = mViewStack->top()->nmailViewId();
			    if (topId != NmUiViewMessageEditor &&
			        topId != NmUiViewMailboxList &&
			        topId != startParam->viewId()) {
			        prepareForPopView();
			    }
			    else {
			        // Editor or mailbox list in the top. Stop the loop.
			        break;
			    }
			}
			mForegroundService = previousForegroundService;
        }

        // Check whether requested view is already active
        // and if so, ask it to reload contents with new start parameter data
        // Do not reuse the view if started as service to editor view (ShareUI)
        if (mActiveViewId == startParam->viewId() &&
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
                    NmMessageListModel *messageListModel = &mUiEngine->messageListModel(
                                                startParam->mailboxId(), startParam->folderId());
                    NmMessageListView *msgList =new NmMessageListView(
                    		*this, startParam, *mUiEngine, *mMbListModel, messageListModel,
                    		new HbDocumentLoader(mMainWindow));
                    pushView(msgList);
                }
                break;
                case NmUiViewMessageSearchList:
                {
                    // Check the topmost view. If it is an editor, do not open
                    // a new mail search list view.
                    if (startParam->service() && !mViewStack->isEmpty() &&
                        mViewStack->top()->nmailViewId() == NmUiViewMessageEditor) {
                        break;
                    }

                    NmMessageListModel &model =
                        mUiEngine->messageListModelForSearch(startParam->mailboxId());

                    NmMessageSearchListView *searchListView = new NmMessageSearchListView(
                        *this, startParam, *mUiEngine, model,
                        new HbDocumentLoader(mMainWindow));

                    pushView(searchListView);
                }
                break;
                case NmUiViewMessageViewer:
                    pushView(new NmViewerView(*this, startParam, *mUiEngine,
                            mMainWindow, *mAttaManager));
                    break;
                case NmUiViewMessageEditor:
                    pushView(new NmEditorView(*this, startParam, *mUiEngine, *mAttaManager));
                    break;
                default:
                    // Reset view stack and exit application
                    delete startParam;
                    startParam = NULL;
                    resetViewStack();
                    break;
            }
        }

        if (startParam && startParam->service()) {
            // Store the view id that was launched as service
            mServiceViewId = mActiveViewId;
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
    NM_FUNCTION;

    delete mSendServiceInterface;
    mSendServiceInterface = NULL;
    delete mSendServiceInterface2;
    mSendServiceInterface2 = NULL;
    delete mUriServiceInterface;
    mUriServiceInterface = NULL;
    delete mMailboxServiceInterface;
    mMailboxServiceInterface = NULL;
    delete mViewerServiceInterface;
    mViewerServiceInterface = NULL;
    resetViewStack();
    qApp->quit();
}

/*!
   Exit the application in the next event loop
*/
void NmApplication::delayedExitApplication()
{
    NM_FUNCTION;

    // Exit the application in the next event loop
    QMetaObject::invokeMethod(this, "exitApplication", Qt::QueuedConnection);
}

/*!
    Getter for main window instance.
*/
HbMainWindow *NmApplication::mainWindow()
{
    NM_FUNCTION;

    return mMainWindow;
}

/*!
    Getter for main UI extension manager
*/
NmUiExtensionManager &NmApplication::extManager()
{
    NM_FUNCTION;

    return *mExtensionManager;
}

/*!
    Getter for network access manager
*/
NmViewerViewNetManager &NmApplication::networkAccessManager()
{
    NM_FUNCTION;

    return *mNetManager;
}

/*!
    screen size. Function returns curtent screen size
*/
QSize NmApplication::screenSize()
{
    NM_FUNCTION;

    QSize ret(0,0);
    if (mMainWindow){
        HbDeviceProfile currentP = HbDeviceProfile::current();
        HbDeviceProfile altP(currentP.alternateProfileName());
        QSize curPSize = currentP.logicalSize();
        QSize altPSize = altP.logicalSize();
        if (mMainWindow->orientation() == Qt::Horizontal) {
            // Get wide profile size in landscape
            if (curPSize.width() > altPSize.width()) {
                ret = curPSize;
            }
            else{
                ret = altPSize;
            }
        }
        else {
            // Get narrow profile size in portrait
            if (curPSize.width() < altPSize.width()) {
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
    NM_FUNCTION;

    if (event.mCompletionCode != NmNoError && event.mCompletionCode != NmCancelError) {
        if (event.mOperationType == Synch && event.mCompletionCode == NmAuthenticationError) {
            mLastOperationMailbox = event.mMailboxId;
            if (mQueryDialog) {
                delete mQueryDialog;
                mQueryDialog = NULL;
            }
            mQueryDialog = NmUtilities::displayQuestionNote(hbTrId("txt_mail_dialog_address_or_password_incorrect"),
                                                    this, SLOT(launchSettings(HbAction*)));
        }
        if (event.mOperationType == Synch && event.mCompletionCode == NmServerConnectionError) {
            mLastOperationMailbox=event.mMailboxId;
            if (mQueryDialog) {
                delete mQueryDialog;
                mQueryDialog = NULL;
            }
            mQueryDialog = NmUtilities::displayQuestionNote(hbTrId("txt_mail_dialog_server_settings_incorrect"),
                                                    this, SLOT(launchSettings(HbAction*)));
        }
        // following applies to all operation/event types
        if (event.mCompletionCode == NmConnectionError) {
            NmUtilities::displayWarningNote(hbTrId("txt_mail_dialog_mail_connection_error"));
        }
    }
}

/*!
    launches settings view of the specified mailbox
*/
void NmApplication::launchSettings(HbAction* action)
{
    NM_FUNCTION;

    // Check whether yes button was pressed
    if (mQueryDialog&& action == mQueryDialog->actions().at(0)) {
        // create settingslauncher if doesn't exist
        if(!mSettingsViewLauncher) {
            mSettingsViewLauncher = new NmSettingsViewLauncher();
            }
        // mailboxname required
        NmMailboxMetaData *mailboxMetaData = mUiEngine->mailboxById(mLastOperationMailbox); // no ownership
        if( mailboxMetaData ) {
            // launch
            mSettingsViewLauncher->launchSettingsView(mLastOperationMailbox, mailboxMetaData->name());
        }
    }
}

/*!
	Stores the visibility state, e.g. when the service was launched.
	\return true if the app was visible
*/
bool NmApplication::updateVisibilityState()
{
    // At the moment there is no good way to check the foreground state
    QWindowSurface *surface = mMainWindow->windowSurface();
	mForegroundService = (surface != 0);
	NM_COMMENT(QString("NmApplication::updateVisibilityState fg=%1").arg(mForegroundService));
	return mForegroundService;
}
