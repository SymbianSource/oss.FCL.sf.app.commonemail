/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtGui>
#include <QTranslator>
#include <QGraphicsLinearLayout>
#include <QTimer>
#include <hbcolorscheme.h>
#include <hbdocumentloader.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hblabel.h>
#include <hbstyleloader.h>
#include <hblistview.h>
#include <hbdeviceprofile.h>
#include <hbevent.h>

#include "nmcommon.h"
#include "nmmessageenvelope.h"
#include "nmhswidget.h"
#include "nmhswidgetemailengine.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetconsts.h"
#include "nmhswidgetdatetimeobserver.h"
#include "nmhswidgetlistviewitem.h"
#include "nmhswidgetlistmodel.h"
#include "emailtrace.h"


NmHsWidget::NmHsWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags), 
      mMainContainer(0),
      mEmptySpaceContainer(0),
      mWidgetContainer(0),
      mTitleRow(0),
      mContentContainer(0),
      mNoMailsLabel(0),
      mContentLayout(0),              
      mBackgroundFrameDrawer(0),
      mTranslator(0),
      mEngine(0),      
      mAccountId(0),
      mAccountIconName(),
      mDateObserver(0),
      mIsExpanded(false),
      mListView(0),
	  mListModel(0),
	  mListActivityTimer(0)
{
    NM_FUNCTION;
}

/*!
 Destructor
 */
NmHsWidget::~NmHsWidget()
{
    NM_FUNCTION;

    delete mTranslator;
    mTranslator = NULL;

    delete mEngine;
    mEngine = NULL;

    delete mBackgroundFrameDrawer;
    mBackgroundFrameDrawer = NULL;

    delete mDateObserver;
    mDateObserver = NULL;
     
    if (mListActivityTimer){
        mListActivityTimer->stop();
        delete mListActivityTimer;
        mListActivityTimer = NULL;
    }
}

/*!
 \fn QPainterPath NmHsWidget::shape()

 Called by home screen fw to check widget boundaries, needed to draw
 outside widget boundingRect.
 /return QPainterPath path describing actual boundaries of widget 
  including child items
 */
QPainterPath NmHsWidget::shape() const
{
    NM_FUNCTION;
    
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    if (mWidgetContainer){
        //add mWidgetContainer using geometry to get
        //correct point for top-left-corner
        QRectF widgetRect = mWidgetContainer->geometry();
        path.addRect(widgetRect); 
        
        //then fetch shape from title row 
        QPainterPath titlepath;
        titlepath.addPath(mTitleRow->shape());
        //translate it's location to be inside mWidgetContainer
        titlepath.translate(widgetRect.topLeft());
        //and finally add it to path
        path.addPath(titlepath);    
    }
    //simplified path, i.e. only outlines
    return path.simplified();
}

/*!
 \fn void NmHsWidget::onShow()

 called by home screen fw when widget gets visible
 */
void NmHsWidget::onShow()
{
    NM_FUNCTION;
    if (mEngine) {
        mEngine->activate();
    }
}

/*!
 \fn void NmHsWidget::onHide()

 called by home screen fw when widget gets hidden
 */
void NmHsWidget::onHide()
{
    NM_FUNCTION;
    if (mEngine) {
        mEngine->suspend();
    }
}

/*!
 \fn bool NmHsWidget::loadDocML(HbDocumentLoader &loader)
 
 loads layout data and child items from docml file. Must be called after constructor.
 /return true if loading succeeded, otherwise false. False indicates that object is unusable
 */
bool NmHsWidget::loadDocML(HbDocumentLoader &loader)
{
    NM_FUNCTION;
    
    bool ok(false);
    loader.load(KNmHsWidgetDocML, &ok);
    
    if(ok) {
        mMainContainer = static_cast<HbWidget*> (loader.findWidget(KNmHsWidgetMainContainer));  
        mWidgetContainer = static_cast<HbWidget*> (loader.findWidget(KNmHsWidgetContainer));
        mContentContainer = static_cast<HbWidget*> (loader.findWidget(KNmHsWidgetContentContainer));
        mEmptySpaceContainer = static_cast<HbWidget*> (loader.findWidget(KNmHsWidgetEmptySpaceContainer));
        mNoMailsLabel = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetNoMailsLabel));
        mListView = static_cast<HbListView*> (loader.findWidget(KNmHsWidgetMailListView));
        if (!mMainContainer || !mWidgetContainer || !mContentContainer 
                || !mEmptySpaceContainer || !mNoMailsLabel || !mListView) {
            //something failed in documentloader, no point to continue
            NM_ERROR(1,"NmHsWidget::loadDocML fail @ containers or label");
            ok = false;
        }
    }
    return ok;
}

/*!
 Initializes Localization.
 /post mTranslator constructed & localization file loaded
 */
void NmHsWidget::setupLocalization()
{
    NM_FUNCTION;

    //Use correct localisation
    mTranslator = new QTranslator();
    QString lang = QLocale::system().name();

    //try to load translation from c drive. If not found then try z.
    bool loadingSucceed = mTranslator->load(KNmHsWidgetLocFileName + lang, "c:" + KNmHsWidgetLocLocation);
    if(!loadingSucceed){
        mTranslator->load(KNmHsWidgetLocFileName + lang, "z:" + KNmHsWidgetLocLocation);
    }
    
    QCoreApplication::installTranslator(mTranslator);
}


/*!
 Initializes UI. Everything that is not done in docml files should be here.
 return true if ok, in error false.
 */
void NmHsWidget::setupUi()
{
    NM_FUNCTION;

    //main level layout needed to control docml objects
    QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout(Qt::Vertical);
    widgetLayout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    widgetLayout->setSpacing(KNmHsWidgetContentsMargin);
    widgetLayout->addItem(mMainContainer);
    this->setLayout(widgetLayout);

    //fetch pointer to content container layout
    //to be able to add/remove email rows and no mails label
    mContentLayout = (QGraphicsLinearLayout*) mContentContainer->layout();
    
    //set noMailsLabel properties not supported by doc loader 
    QColor newFontColor;
    newFontColor = HbColorScheme::color("qtc_hs_list_item_content_normal");
    mNoMailsLabel->setTextColor(newFontColor);
    mNoMailsLabel->setVisible(true);   
    
    mContentLayout->removeItem(mNoMailsLabel);
    
    //widget background
    mBackgroundFrameDrawer = new HbFrameDrawer(KNmHsWidgetBackgroundImage,
        HbFrameDrawer::NinePieces);
    HbFrameItem* backgroundLayoutItem = new HbFrameItem(mBackgroundFrameDrawer);
    //set to NULL to indicate that ownership transferred
    mBackgroundFrameDrawer = NULL;
    mContentContainer->setBackgroundItem(backgroundLayoutItem);
	
	HbEffect::add(mContentContainer, "combo_disappear_up", "collapse");
    HbEffect::add(mContentContainer, "combo_appear_down", "expand");
    
}

/*!
 Sets correct properties and mail item proto type for mail list view 
 */
void NmHsWidget::createMailRowsList()
{
	  NM_FUNCTION;

    //construct list model
    mListModel = new NmHsWidgetListModel(); 
    
	//Connect item activation (click) to message view launching
	connect(mListView, SIGNAL(activated(const QModelIndex&)), this,
        SLOT(openMessage(const QModelIndex&)));
        
    //When new mails arrive, scroll list to the start
    connect(mListModel, SIGNAL( messagesAddedToModel() ), 
        this, SLOT( handleMessagesAddedToModel() ));
    
    //connect item removal signal to update layout, so that nomailslabel & listview visibility
    //can be determined
    connect(mListModel, SIGNAL( modelIsEmpty(bool) ), 
            this, SLOT( updateLayout() ));
    
    // Set the list widget properties.
    mListView->setItemRecycling(false);  //This should be changed in future
    NmHsWidgetListViewItem *prototype = new NmHsWidgetListViewItem(mListView);
    mListView->setItemPrototype(prototype);
    mListView->setModel(mListModel);  
}

/*!
 Initializes the widget.
 
 called by home screen fw when widget is added to home screen
 */
void NmHsWidget::onInitialize()
{
    NM_FUNCTION;
    
    QT_TRY {
	    HbStyleLoader::registerFilePath(":/layout/nmhswidgetlistviewitem.widgetml");
	    HbStyleLoader::registerFilePath(":/layout/nmhswidgetlistviewitem.css");
	    
	    // Use document loader to load the contents
	    HbDocumentLoader loader;
		//setup localization before docml loading
	    setupLocalization();
		
	    //load containers and mNoMailsLabel
        if (!loadDocML(loader)) {
            NM_ERROR(1,"NmHsWidget::onInitialize Fail @ loader");
            emit error(); //failure, no point to continue
            return;
        }

        //construct title row
        mTitleRow = new NmHsWidgetTitleRow(this);
        if (!mTitleRow->setupUI(loader)) {
            //title row creation failed
            NM_ERROR(1,"NmHsWidget::onInitialize fail @ titlerow");
            emit error(); //failure, no point to continue
            return;            
        }
				
        setupUi();

        //Crete list for mail items
        createMailRowsList();        

        //Engine construction is 2 phased. 
        mEngine = new NmHsWidgetEmailEngine(mAccountId);
        connect(mEngine,    SIGNAL( mailDataRefreshed(const QList<NmMessageEnvelope*>&) ), 
                mListModel, SLOT  ( refresh          (const QList<NmMessageEnvelope*>&) ));
        connect(mEngine,    SIGNAL( mailDataCleared() ), 
                mListModel, SLOT  ( removeAllMessages() ));
        connect(mEngine,    SIGNAL( mailsReceived (const QList<NmMessageEnvelope*>&) ), 
                mListModel, SLOT  ( addMessages   (const QList<NmMessageEnvelope*>&) ));
        connect(mEngine,    SIGNAL( mailsUpdated  (const QList<NmMessageEnvelope*>&) ), 
                mListModel, SLOT  ( updateMessages(const QList<NmMessageEnvelope*>&) ));
        connect(mEngine,    SIGNAL( mailsDeleted  (const QList<NmId>&)), 
                mListModel, SLOT  ( removeMessages(const QList<NmId>&) ));
        //Client must connect to exception signals before calling the initialize function
        //because we don't want to miss any signals.
        connect(mEngine, SIGNAL( exceptionOccured(const int&) ), this,
            SLOT( onEngineException(const int&) ));
        if (!mEngine->initialize()) {
            //engine construction failed. Give up.
            NM_ERROR(1,"NmHsWidget::onInitialize fail @ engine");
            emit error();
            return;
        }

		//set account name to title row
        mTitleRow->updateAccountName(mEngine->accountName());

        //create observer for date/time change events
        mDateObserver = new NmHsWidgetDateTimeObserver();

        //Create timer for delaying events after list scrolling is ended
        mListActivityTimer = new QTimer(this);
        mListActivityTimer->setInterval(KNmHsWidgetDelayAfterScrollingEnded);
        connect(mListActivityTimer, SIGNAL(timeout()), this, SLOT(scrollListToStart()) );
        //Connect scrolling ended to scrolling timer activation
        //so that after timeout the list will be scrolled to first item
        connect(mListView, SIGNAL( scrollingEnded() ), 
                mListActivityTimer, SLOT( start() ));
        
        //if timer is running, stop it right away when new activity happens
        connect(mListView, SIGNAL( scrollingStarted() ), 
                mListActivityTimer, SLOT( stop() ));
        
        mTitleRow->updateUnreadCount(mEngine->unreadCount());
        mTitleRow->setAccountIcon(mAccountIconName);
        mTitleRow->setExpandCollapseIcon(mIsExpanded);
       
        //Get Signals about changes in unread count
        connect(mEngine, SIGNAL( unreadCountChanged(const int&) )
                ,mTitleRow, SLOT( updateUnreadCount(const int&) ) );
        
        //Get signals about account name changes
        connect(mEngine, SIGNAL( accountNameChanged(const QString&) )
                ,mTitleRow, SLOT( updateAccountName(const QString&) ) );

	    //Get signals about user actions
        //Qt::QueuedConnection used to improve ui responsiveness 
	    connect(mTitleRow, SIGNAL( mailboxLaunchTriggered() )
	            ,mEngine, SLOT( launchMailAppInboxView() ), Qt::QueuedConnection);
	    
	    connect(mTitleRow, SIGNAL( expandCollapseButtonPressed() )
	            ,this, SLOT( handleExpandCollapseEvent() ) );
	    
	    //Get date/time events from date observer
	    connect(mDateObserver, SIGNAL(dateTimeChanged())
	            , mEngine, SLOT(forceUpdate()));
	    
	    setMinimumSize(mTitleRow->minimumWidth(), 
	            mEmptySpaceContainer->minimumHeight() + mTitleRow->minimumHeight());
    }
    QT_CATCH(...) {
        NM_ERROR(1,"NmHsWidget::onInitialize fail @ catch");
        emit error();
    }
}

/*!
 Uninitializes the widget.
 
 called by home screen fw when widget is removed from home screen
 */
void NmHsWidget::onUninitialize()
{
    NM_FUNCTION;
    HbStyleLoader::unregisterFilePath(":/layout/nmhswidgetlistviewitem.widgetml");
    HbStyleLoader::unregisterFilePath(":/layout/nmhswidgetlistviewitem.css");
}


/*!
 Sets monitored account id from given string
 Needed for home screen framework which supports only QString type properties
 */
void NmHsWidget::setAccountId(const QString &text)
{
    NM_FUNCTION;

    bool ok;
    quint64 id = text.toULongLong(&ok);
    if (!ok) {
        NM_ERROR(1, "NmHsWidget::setAccountId: invalid account ID data, signal finished()!!!");
        //No valid account id so give up
        emit finished();
        return;
    }

    mAccountId.setId(id);
}

/*!
 Returns monitored account id as a string
 Needed for home screen framework which supports only QString type properties
 */
QString NmHsWidget::accountId() const
{
    NM_FUNCTION;
    return QString::number(mAccountId.id());
}

/*!
 Sets monitored account icon name from given string
 */
void NmHsWidget::setAccountIconName(const QString &text)
{
    NM_FUNCTION;
    mAccountIconName = text;
}

/*!
 Returns monitored account icon name
 */
QString NmHsWidget::accountIconName() const
{
    NM_FUNCTION;
    return mAccountIconName;
}

/*!
 Slot to handle expand/collapse trigger event
 */
void NmHsWidget::handleExpandCollapseEvent()
{
    NM_FUNCTION;
    if(mIsExpanded){
        HbEffect::start( mContentContainer, "collapse", this, "toggleExpansion");
    }
    else{
        toggleExpansionState();
        HbEffect::start( mContentContainer, "expand");
    }
}

/*!
 Sets widget expand/collapse state
 /post widget expansion state is changed
 */
void NmHsWidget::toggleExpansionState()
{
    NM_FUNCTION;

    mIsExpanded = !mIsExpanded;

    //save new state to home screen
    QStringList propertiesList;
    propertiesList.append("widgetState");
    emit setPreferences(propertiesList);

    //handle state change drawing
    updateLayout();

    mTitleRow->setExpandCollapseIcon(mIsExpanded);
}

/*!
 Sets expand/collapse state from given string (needed by homescreen)
 */
void NmHsWidget::setWidgetStateProperty(QString value)
{
    NM_FUNCTION;
    if (value == KNmHsWidgetStateCollapsed) {
        mIsExpanded = false;
    }
    else {
        mIsExpanded = true;
    }
}

/*!
 Returns widget expand/collapse state as string (needed by homescreen) 
 */
QString NmHsWidget::widgetStateProperty()
{
    NM_FUNCTION;
    if (mIsExpanded) {
        return KNmHsWidgetStateExpanded;
    }
    else {
        return KNmHsWidgetStateCollapsed;
    }
}


/*!
 Updates the Layout to contain the right items
 /param mailCount defines how many emails is to be shown
 /post If widget is collapsed, the layout contains only titleRow widget.
 If widget is expanded and mailCount is 0 layout will contain
 titlerow & noMailsLabel. 
 If widget is expanded and mailCount is greter
 than zero, layout will contain titlerow and KMaxNumberOfMailsShown times
 emailrow(s)
 */
void NmHsWidget::updateLayout()
{
    NM_FUNCTION;
    int mailCount = mListModel->rowCount();
	//collapsed size
    qreal totalHeight = mEmptySpaceContainer->preferredHeight() + mTitleRow->containerHeight();
    
    if (mIsExpanded) {
        //when expanded, grow as big as possible
        totalHeight = KNmHsWidgetHSMaxWidgetHeightInUnits * HbDeviceProfile::current().unitValue();
        mContentContainer->setVisible(true);
        if (mailCount == 0) {
            addNoMailsLabelToLayout();
            removeEmailRowsFromLayout();
        }
        else {
            removeNoMailsLabelFromLayout();
            addEmailRowsToLayout();
        }
    }
    else {
        removeNoMailsLabelFromLayout();
        removeEmailRowsFromLayout();
        mContentContainer->setVisible(false);        
    }

    //set maximum sizes, otherwise widget will stay huge also when collapsed
    setMaximumHeight(totalHeight);
    mMainContainer->setMaximumHeight(totalHeight);
    mWidgetContainer->setMaximumHeight(totalHeight - mEmptySpaceContainer->preferredHeight());
    //resize here or widget cannot draw mail rows when expanding
    //TODO: check if this is still needed as list used
    resize(mTitleRow->maximumWidth(), totalHeight);
    mMainContainer->resize(mTitleRow->maximumWidth(), totalHeight);
    mWidgetContainer->resize(mTitleRow->maximumWidth(), totalHeight - mEmptySpaceContainer->preferredHeight());
}

/*!
 Updates mNoMailsLabel visibility based on widget state
 /param mailCount defines how many mail rows is needed
 /post if mail count is 0 and mIsExpanded equals true, then
 the mNoMailLabel is added to the mContentLayout. 
 */
void NmHsWidget::addNoMailsLabelToLayout()
{
    NM_FUNCTION;

    if ( mNoMailsLabel->isVisible() ) {
        return;
    }
    //Add mNoMailsLabel to layout if not yet there and show it
    mContentLayout->addItem(mNoMailsLabel);
    //resize the widget to new layout size
    mNoMailsLabel->show();
}

/*!
 removeNoMailsLabelFromLayout removes mNoMailsLabel from the layout
 /post mNoMailsLabel is not in mContentLayout
 */
void NmHsWidget::removeNoMailsLabelFromLayout()
{
    NM_FUNCTION;
    //remove mNoMailsLabel from Layout and hide it
    mContentLayout->removeItem(mNoMailsLabel);
    mNoMailsLabel->hide();
}

/*!
 addEmailRowsToLayout adds list widget to content layout
 */
void NmHsWidget::addEmailRowsToLayout()
{
    NM_FUNCTION;
    
    mContentLayout->addItem(mListView);
    
    mListView->setVisible(true);
    
}

/*!
 removeEmailRowsFromLayout removes and hides list widget 
 from content layout
 */
void NmHsWidget::removeEmailRowsFromLayout()
{
    NM_FUNCTION;
    mContentLayout->removeItem(mListView);
    mListView->setVisible(false);
}

/*!
 openMessage slot
 */
void NmHsWidget::openMessage(const QModelIndex& index)
{
    NM_FUNCTION;
    QVariant var = mListModel->data(index,Qt::DisplayRole);
    if(!var.isNull()){
        NmMessageEnvelope *envelope = var.value<NmMessageEnvelope*>();
        mEngine->launchMailAppMailViewer(envelope->messageId());
    }
}

/*!
 handleMessagesAddedToModel slo
 Slot is called when new messages is added to model. This function should scroll 
 the list right away to the first item, unless there is user activity ongoing or
 the scrolling timer is active
 */
void NmHsWidget::handleMessagesAddedToModel()
{
    NM_FUNCTION;
    if(mListActivityTimer->isActive() || mListView->isScrolling()){
        //No need to scroll right away as user is active and we don't want to interupt it
        //or the scrolling event will occur within KNmHsWidgetDelayAfterScrollingEnded msecs.
        return; 
    }else{
        //if the list is idle, scroll to the first item
        scrollListToStart();
    }
}

/*!
 scrollListToStart slot - Scrolls the mListView to the beginning of the list with
 KListScrollUpTime as a time to do it.
 */
void NmHsWidget::scrollListToStart()
{
    NM_FUNCTION;
    QModelIndex index = (mListModel->item(0,0))->index();
    //next line will fail if item recycling is enabled!!
    QPointF pos = (mListView->itemByIndex(index))->pos();
    if(mListView->contentWidget()->pos() != pos){
        //dont move if already there. Otherwise new scrolling events are emitted
        mListView->scrollContentsTo(pos, KListScrollUpTime);
    }
    mListActivityTimer->stop(); //stop the timer
}

/*!
 onEngineException (NmHsWidgetEmailEngineExceptionCode exc)
 signals widget to be finalized
 /param exc exception type
 */
void NmHsWidget::onEngineException(const int& exc)
{
    NM_FUNCTION;
    switch (exc) {
        case (NmEngineExcAccountDeleted):
            emit finished(); //succesful ending
            break;
        case (NmEngineExcFailure):
            emit error(); //failure
            break;
        default:
            break;
    }
}

/*!
    toggleExpansion
*/
void NmHsWidget::toggleExpansion(const HbEffect::EffectStatus &status)
    {
    NM_FUNCTION;

    Q_UNUSED(status);
    
    toggleExpansionState();
    }


/*
 * NmHsWidget::event()
 */
bool NmHsWidget::event( QEvent *event )
{
    NM_FUNCTION;
    QEvent::Type eventType = event->type();
    if( eventType == HbEvent::ThemeChanged ){
        QColor newFontColor;
        newFontColor = HbColorScheme::color("qtc_hs_list_item_content_normal");
        mNoMailsLabel->setTextColor(newFontColor);
        return true;
    }
    return HbWidget::event(event);
}
