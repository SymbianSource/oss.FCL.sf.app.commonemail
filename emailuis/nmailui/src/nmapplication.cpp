/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

static const QString NmActivityName = "EmailInboxView";

/*!
    \class NmApplication
    \brief Application class, creates main window and handles view switching.
*/


/*!
    Constructor.
*/
NmApplication::NmApplication(QObject *parent, quint64 accountId)
: QObject(parent),
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
    TRAP_IGNORE(mUiEngine = NmUiEngine::instance());
    
    // Create network access manager and cache for application use.
    mNetManager = new NmViewerViewNetManager(*mUiEngine);
    QNetworkDiskCache *cache = new QNetworkDiskCache();
    cache->setCacheDirectory(
        QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
    mNetManager->setCache(cache);
    
    createMainWindow();
    
    // Attachment manager can be shared between viewer and editor.
    // The application class has the ownership.
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
        mailbox.setValue(accountId);
        mMailboxServiceInterface->displayInboxByMailboxId(mailbox);
    }
    
    mEffects = new NmUiEffects(*mMainWindow);
    
    QObject::connect(parent, SIGNAL(activate()), this, SLOT(activityActivated()));
}


/*!
    Destructor.
*/
NmApplication::~NmApplication()
{
    // Remove the event filter early since catching application activated/
    // deactivated events now may cause a crash.
    QCoreApplication::instance()->removeEventFilter(this);

    if (mQueryDialog) {
        delete mQueryDialog;
        mQueryDialog = NULL;
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
    
    // Workaround: the main window has to be closed (hidden) before deleting
    // the extension manager in order to prevent the main window's title bar
    // from showing when the application is closed.
    if (mMainWindow) {
        // Workaround: assert failure may happen if an open main window is
        // deleted.
        mMainWindow->close();
    }
    
    delete mExtensionManager;
    
    if (mNetManager) {
        if (mNetManager->cache()) {
            mNetManager->cache()->clear();
        }

        delete mNetManager;
        mNetManager = NULL;
    }
    
    // Effects need to be deleted before MainWindow.
    delete mEffects;
    delete mMainWindow;
    delete mAttaManager;
    delete mSettingsViewLauncher;
}


/*!
    Main application window creation.
*/
void NmApplication::createMainWindow()
{
    NM_FUNCTION;
    
    bool service = XQServiceUtil::isService();
    
    // Register custom widget files.
    HbStyleLoader::registerFilePath(":nmmessagelistviewitem.widgetml");
    HbStyleLoader::registerFilePath(":nmmessagelistviewitem.css");
    HbStyleLoader::registerFilePath(":nmviewerheader.widgetml");
    HbStyleLoader::registerFilePath(":nmviewerheader.css");
    
    // Create main window.
    mMainWindow = new HbMainWindow();
    
    // Connect to lazy loading signal.
    QObject::connect(mMainWindow, SIGNAL(viewReady ()),
                     this, SLOT(viewReady()));
    
    // Create extension manager.
    mExtensionManager = new NmUiExtensionManager();
    
    // Create view stack.
    mViewStack = new QStack<NmBaseView*>;
    
    // Create back action and connect it to prepareForPopView().
    if (mMainWindow) {
        mBackAction = new HbAction(Hb::BackNaviAction,this);
        connect(mBackAction,
                SIGNAL(triggered()),
                this,
                SLOT(prepareForPopView()));
        
        // Show mainwindow.
        // Services will active it when the view is ready.
        if (!service) {
            mMainWindow->show();
        }
    }
    
    // Asynchronous operation completion related notifications.
    connect(mUiEngine,
            SIGNAL(operationCompleted(const NmOperationCompletionEvent &)),
            this,
            SLOT(handleOperationCompleted(const NmOperationCompletionEvent &)));
    
    mMbListModel = &mUiEngine->mailboxListModel();
    
    // ----------------------------------------------------------------------
    // TODO: Remove or comment out the following code block when the mail
    //       wizard starts to work with Mfe.
    // ----------------------------------------------------------------------
    // Start application to mailbox view. If started as service, there is no
    // need to create views.
    if (!service) {
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMailboxList);
        enterNmUiView(startParam);
    }
    // ----------------------------------------------------------------------
    
    if (mMainWindow) {
        // Start to filter main window events to get "end key" event in all
        // possible situations. Using event()is not enough to catch the event
        // as it is only called if the view widget has the focus. Note: if
        // key capturing (xqkeycapture.h) is required it is probably best to
        // implement an own QMainWindow class and do the capturing there, not
        // in the views.
        mMainWindow->installEventFilter(this);
        // Optimize the custom paint functions.
        // Currently effects to NmViewerHeader::paint() and
        // NmAttachmentListWidget::paint().
        mMainWindow->setOptimizationFlag(QGraphicsView::DontSavePainterState);    
    }
    
	// Install the event filter in order to receive ApplicationActivate/Deactivate
    // events.
    QCoreApplication::instance()->installEventFilter(this);
}


/*!
    Slot. React to view ready signal and call current view method.
*/
void NmApplication::viewReady()
{
    mViewReady = true;

    if (mViewStack && !mViewStack->isEmpty()) {
    	NmBaseView *currentView = mViewStack->top();

        if (currentView) {
            currentView->viewReady();
            emit applicationReady();
        }
    }
}


/*!
    Event filter. End key is filtered from the main window and either the view
    takes case or the app is exited by default.
*/
bool NmApplication::eventFilter(QObject *obj, QEvent *event)
{
    bool consumed(false);

    if (obj && obj == mMainWindow && event && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_No) {
            // End key, the "red" key.
            // Exit application if no pending operations are on-going.
        }
    }
    else if (event && event->type() == QEvent::ApplicationActivate) {
        NM_COMMENT("NmApplication::eventFilter ApplicationActivate");
		// Update task switcher name & screenshot, we could have activated into some other mailbox
        updateActivity();
    }
    else if (event && event->type() == QEvent::ApplicationDeactivate) {
        NM_COMMENT("NmApplication::eventFilter ApplicationDeactivate");
        // Update the screenshot in the taskswitcher to represent current state
        updateActivity();
        // hide the sync indicator when app goes to background
        mUiEngine->enableSyncIndicator(false);
    }
    
    if (!consumed) {
        consumed = QObject::eventFilter(obj, event);
    }
    
    return consumed;
}


/*!
    Push view to view stack.
*/
void NmApplication::pushView(NmBaseView *newView)
{
    if (newView && mViewStack) {
        NM_COMMENT("NmApplication::pushView() : view exists.");
        newView->setNavigationAction(mBackAction);
        
        // Store view to be hidden.
        NmBaseView *hideView(NULL);
        if (!mViewStack->isEmpty()) {
            hideView = mViewStack->top();
        }
		else {
			// viewReady should be informed immediately.
			if (mViewReady) {
				newView->viewReady();
			}
		}
        
        // Activate new view.
    	NM_COMMENT("NmApplication::pushView() : add view.");
        mMainWindow->addView(newView);
        mViewStack->push(newView);
        mMainWindow->setCurrentView(newView);
        mActiveViewId=newView->nmailViewId();
        
        // Set toolbars orientation.
        HbToolBar *tb = newView->toolBar();
        if (tb) {
            tb->setOrientation(Qt::Horizontal);
        }
        
        // Hide old view.
        NM_COMMENT("NmApplication::pushView() : remove view.");
        if (hideView) {
            mMainWindow->removeView(hideView);
        }
    }
}

/*!
    Ask from view that is it ok to pop view. This kind of 2-phase popView is
    needed because view may show query dialog for user response.
 */
void NmApplication::prepareForPopView()
{
    if (mViewStack && mViewStack->size() > 0) {
        // Get view pointer.
        NmBaseView *view = mViewStack->top();
        if (view){
			NmUiViewId topViewId = view->nmailViewId();
			
			// We must know in popView, are we coming with back button from
			// message view to prevent the send animation.
			if (topViewId == NmUiViewMessageEditor) {
				mBackButtonPressed = true;
			}
            
            // View will call/signal popView if exitting is ok.
            view->okToExitView();        
        }
    }
    // If the view stack is now empty quit the app. This happens also when
    // the app has been started as a service
    else if (mViewStack && mViewStack->size() == 0) {
        exitApplication();
    }
}


/*!
     Hide the application
*/
void NmApplication::hideApplication()
{
    // Hide the application
    XQServiceUtil::toBackground(true);

    // hide the sync indicator as well
    mUiEngine->enableSyncIndicator(false);
    
    // Hide the mail from the task switcher 
    TsTaskSettings taskSettings;
    taskSettings.setVisibility(false);
    
    // Remove also the mailbox item from the task switcher
    HbApplication* hbApp = dynamic_cast<HbApplication*>(parent());
    if (hbApp) {
        hbApp->activityManager()->removeActivity(NmActivityName);
    }
}


/*!
    Pop view from view stack. View object is deleted.
*/
void NmApplication::popView()
{
    if (mViewStack && mViewStack->size() > 0) {
        NmBaseView *view = mViewStack->top();    
        if (view) {
            // Get top view id.
            NmUiViewId topViewId = view->nmailViewId();
            
            // Prepare for send animation if returing from editor and message
            // has been sent.
            if (topViewId == NmUiViewMessageEditor && mUiEngine->isSendingMessage()) {
				// If we are coming from message editor with back button.
				// do not prepare the send animation.
				if (!mBackButtonPressed) {
					mEffects->prepareEffect(NmUiEffects::NmEditorSendMessageAnimation);
				}
	        }
            mBackButtonPressed = false;

            // Move the application to background if closing the message list view
            if (topViewId == NmUiViewMessageList && mViewStack->size() == 1) {
                hideApplication();
                return;
            }

            mViewStack->pop();
            // Call custom exit function.
            view->aboutToExitView();
            // Remove view from stack.
            mMainWindow->removeView(view);
            
            // If we were in editor and sent a message, pop viewer from stack
            // first so we can go straight to mail list.
            if (!mViewStack->isEmpty() && topViewId == NmUiViewMessageEditor &&
                mUiEngine->isSendingMessage() &&
                mViewStack->top()->nmailViewId() == NmUiViewMessageViewer) {
                NmBaseView *tmpView = mViewStack->pop();
                mMainWindow->removeView(tmpView);
                delete tmpView;
                tmpView = NULL;
            }
            
            if (!mViewStack->isEmpty()) {
                // Activate next view in stack.
                NmBaseView *showView = mViewStack->top();
                mMainWindow->addView(showView);
                mMainWindow->setCurrentView(showView);
                // Store activated view id.
                mActiveViewId=showView->nmailViewId();
                // Perform send animation if requested.
                mEffects->startEffect(NmUiEffects::NmEditorSendMessageAnimation);
            }
            
            delete view;
            view = NULL;
            
            // If view was started as service, move the app now to the
            // background, unless it was started when the app was already in
            // foreground.
            if (mServiceViewId == topViewId) {
                mServiceViewId = NmUiViewNone;
                NM_COMMENT("NmApplication::popView() : returned from service view.");
                // If started as embedded or while the app was in foreground,
                // do not hide the app.
                if (!XQServiceUtil::isEmbedded() &&
                    !mForegroundService) {
                    XQServiceUtil::toBackground(true);
                }
            }
        }
    }
    
    // If the view stack is now empty quit the app. This happens also when
    // the app has been started as a service.
    if (mViewStack && mViewStack->size() == 0) {
        exitApplication();
    }
}


/*!
    Reset view stack. Remove and destroy view objects.
*/
void NmApplication::resetViewStack()
{
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
    Function activates view based on viewId parameter. If requested view is
    already open, it is requested to reload. Otherwise view object is created
    and pushed to view stack.
*/
void NmApplication::enterNmUiView(NmUiStartParam *startParam)
{
    NM_FUNCTION;
    
    // Check the validity of start parameter object.
    if (startParam) {
    
        mCurrentMailboxId = startParam->mailboxId();
        
        if (startParam->service() && mMainWindow) {
			// When the message list is started as a service previous views
            // are removed from the stack. Open editors are not closed. Also
            // if the view is same than the new one, keep it open (reload the
            // content).
            
		    // Reset the foreground service flag while popping the views.
		    bool previousForegroundService = mForegroundService;
		    mForegroundService = true;
		    
		    // At least one view must remain in the stack.
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
        
        // Check whether requested view is already active and if so, ask it
        // to reload contents with new start parameter data. Do not reuse the
        // view if started as service to editor view (ShareUI).
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
                    // a new mail list view.
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
                    // Reset view stack and exit application.
                    delete startParam;
                    startParam = NULL;
                    resetViewStack();
                    break;
            }
        }
        
        if (startParam && startParam->service()) {
            // Store the view id that was launched as service.
            mServiceViewId = mActiveViewId;
		}
    }
}


/*!
    Function can be used from views to exit the application. View stack is
    cleared. Views can connect exit menu selection to this slot.
*/
void NmApplication::exitApplication()
{
    NM_FUNCTION;
    
    HbApplication* hbApp = dynamic_cast<HbApplication*>(parent());
    hbApp->activityManager()->removeActivity("EmailInboxView");
    
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
   Exit the application in the next event loop.
*/
void NmApplication::delayedExitApplication()
{
    // Exit the application in the next event loop.
    QMetaObject::invokeMethod(this, "exitApplication", Qt::QueuedConnection);
}


/*!
    Getter for main window instance.
*/
HbMainWindow *NmApplication::mainWindow()
{
    return mMainWindow;
}


/*!
    Getter for main UI extension manager.
*/
NmUiExtensionManager &NmApplication::extManager()
{
    return *mExtensionManager;
}


/*!
    Getter for network access manager.
*/
NmViewerViewNetManager &NmApplication::networkAccessManager()
{
    return *mNetManager;
}


/*!
    Get the screen size. Function returns curtent screen size.
*/
QSize NmApplication::screenSize()
{
    QSize ret(0,0);
    if (mMainWindow){
        HbDeviceProfile currentP = HbDeviceProfile::current();
        HbDeviceProfile altP(currentP.alternateProfileName());
        QSize curPSize = currentP.logicalSize();
        QSize altPSize = altP.logicalSize();
        if (mMainWindow->orientation() == Qt::Horizontal) {
            // Get wide profile size in landscape.
            if (curPSize.width() > altPSize.width()) {
                ret = curPSize;
            }
            else{
                ret = altPSize;
            }
        }
        else {
            // Get narrow profile size in portrait.
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
    Handles all asynchronous operation's completions at UI level.
*/
void NmApplication::handleOperationCompleted(const NmOperationCompletionEvent &event)
{
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
        // Following applies to all operation/event types.
        if (event.mCompletionCode == NmConnectionError) {
            NmUtilities::displayWarningNote(hbTrId("txt_mail_dialog_mail_connection_error"));
        }
    }
}


/*!
    Launches settings view of the specified mailbox.
*/
void NmApplication::launchSettings(HbAction* action)
{
    // Check whether yes button was pressed.
    if (mQueryDialog&& action == mQueryDialog->actions().at(0)) {
        // Create settingslauncher if doesn't exist.
        if(!mSettingsViewLauncher) {
            mSettingsViewLauncher = new NmSettingsViewLauncher();
            }
        // Mailboxname required.
        NmMailboxMetaData *mailboxMetaData = mUiEngine->mailboxById(mLastOperationMailbox); // No ownership.
        if( mailboxMetaData ) {
            // Launch.
            mSettingsViewLauncher->launchSettingsView(mLastOperationMailbox, mailboxMetaData->name());
        }
    }
}


/*!
	Stores the visibility state, e.g. when the service was launched.
	\return true if the app was visible.
*/
bool NmApplication::updateVisibilityState()
{
    // At the moment there is no good way to check the foreground state.
    QWindowSurface *surface = mMainWindow->windowSurface();
	mForegroundService = (surface != 0);
	NM_COMMENT(QString("NmApplication::updateVisibilityState() : mForegroundService == %1").arg(mForegroundService));
	return mForegroundService;
}


/*!
    Update the thumbnail in the task switcher.
*/
void NmApplication::updateActivity()
{
    NmMailboxMetaData *meta = mUiEngine->mailboxById(mCurrentMailboxId);
    HbApplication* hbApp = dynamic_cast<HbApplication*>(parent());

    if (hbApp) {
        if (meta) {
            TsTaskSettings tasksettings;
            tasksettings.setVisibility(false);
            QVariantHash metadata;
            metadata.insert(ActivityScreenshotKeyword, QPixmap::grabWidget(mainWindow(), mainWindow()->rect()));
            metadata.insert(ActivityApplicationName, meta->name());
            metadata.insert(ActivityVisibility, true);
            hbApp->activityManager()->removeActivity(NmActivityName);
            hbApp->activityManager()->addActivity(NmActivityName, QVariant(), metadata);
        }
        else {
            hbApp->activityManager()->removeActivity(NmActivityName);
            TsTaskSettings tasksettings;
            tasksettings.setVisibility(true);
        }
    }
}


/*!
	Switch to activated mailbox
*/
void NmApplication::activityActivated()
{
    HbApplication* hbApp = dynamic_cast<HbApplication*>(parent());
    if (hbApp) {
        quint64 accountId(0);
        QString activateId = hbApp->activateId();
        if (hbApp->activateReason() == Hb::ActivationReasonActivity &&
                activateId.startsWith(NmActivityName) ) {
            QString accountIdString = activateId.mid(NmActivityName.length());
            accountId = accountIdString.toULongLong();
            QVariant mailbox;
            mailbox.setValue(accountId);
            mMailboxServiceInterface->displayInboxByMailboxId(mailbox);
        }
    }
}

