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
#include <QGraphicsLinearLayout>
#include <hbcolorscheme.h>
#include <QTranslator>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hblabel.h>
#include "nmcommon.h"
#include "nmhswidget.h"
#include "nmhswidgetemailengine.h"
#include "nmmessageenvelope.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetemailrow.h"
#include "nmhswidgetconsts.h"
#include "nmhswidgetdatetimeobserver.h"
#include "emailtrace.h"

NmHsWidget::NmHsWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags), 
      mEngine(0),
      mRowLayout(0),
      mTitleRow(0),
      mAccountId(0),
      mAccountIconName(),
      mTranslator(0),
      mBackgroundFrameDrawer(0),
      mIsExpanded(false),
      mDateObserver(0),
      mNoMailsLabel(0)
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
    
    path.addRect(this->rect());
    if (mTitleRow){
        path.addPath(mTitleRow->shape());
    }
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
 Initializes Localization.
 /post mTranslator constructed & localization file loaded
 */
void NmHsWidget::setupLocalization()
{
    NM_FUNCTION;

    //Use correct localisation
    mTranslator = new QTranslator();
    QString lang = QLocale::system().name();
    mTranslator->load(KNmHsWidgetLocFileName + lang, KNmHsWidgetLocLocation);
    QCoreApplication::installTranslator(mTranslator);
}

/*!
 Initializes UI. Everything that is not done in docml files should be here.
 return true if ok, in error false.
 */
void NmHsWidget::setupUi()
{
    NM_FUNCTION;

    setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
        KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);

    //Setup layout
    mRowLayout = new QGraphicsLinearLayout(Qt::Vertical);

    mRowLayout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
        KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    mRowLayout->setSpacing(KNmHsWidgetContentsMargin);
    setLayout(mRowLayout);

    //background
    mBackgroundFrameDrawer = new HbFrameDrawer(KNmHsWidgetBackgroundImage,
        HbFrameDrawer::NinePieces);
    HbFrameItem* backgroundLayoutItem = new HbFrameItem(mBackgroundFrameDrawer);
    //set to NULL to indicate that ownership transferred
    mBackgroundFrameDrawer = NULL;
    setBackgroundItem(backgroundLayoutItem);
    
    //Create NoMails Label.
    mNoMailsLabel = new HbLabel(this);
    mNoMailsLabel->setPlainText(hbTrId("txt_mail_widget_info_no_messages"));
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    HbStyle style;
    qreal size;
    bool found = style.parameter(QString("hb-param-text-height-tiny"), size);
    if (found) {
        fontSpec.setTextHeight(size);
    }
    mNoMailsLabel->setFontSpec(fontSpec);
    QColor newFontColor;
    newFontColor = HbColorScheme::color("qtc_hs_list_item_content_normal");
    mNoMailsLabel->setTextColor(newFontColor);
    mNoMailsLabel->setAlignment(Qt::AlignCenter);
    mNoMailsLabel->setVisible(false);

}

/*!
 Initializes the widget.
 
 called by home screen fw when widget is added to home screen
 */
void NmHsWidget::onInitialize()
{
    NM_FUNCTION;

    QT_TRY {
        setupUi();
        setupLocalization();

        //Engine construction is 2 phased. 
        mEngine = new NmHsWidgetEmailEngine(mAccountId);
        //Client must connect to exception signals before calling the initialize function
        //because we don't want to miss any signals.
        connect(mEngine, SIGNAL( exceptionOccured(const int&) ), this,
            SLOT( onEngineException(const int&) ));
        if (!mEngine->initialize()) {
            //engine construction failed. Give up.
            emit error();
            return;
        }

        //construct and load docml for title row
        mTitleRow = new NmHsWidgetTitleRow(this);
        if (!mTitleRow->setupUI()) {
            //if docml loading fails no point to proceed
            emit error();
            return;
        }
        mRowLayout->addItem(mTitleRow);
        mTitleRow->updateAccountName(mEngine->accountName());

        //create observer for date/time change events
        mDateObserver = new NmHsWidgetDateTimeObserver();

        //Crete MailRows and associated connections
        createMailRowsList();

        updateMailData();
        mTitleRow->updateUnreadCount(mEngine->unreadCount());
        mTitleRow->setAccountIcon(mAccountIconName);
        mTitleRow->setExpandCollapseIcon(mIsExpanded);

        //Get signals about changes in mail data
        connect(mEngine, SIGNAL( mailDataChanged() ), this, SLOT( updateMailData() ));

        //Get Signals about changes in unread count
        connect(mEngine, SIGNAL( unreadCountChanged(const int&) )
                ,mTitleRow, SLOT( updateUnreadCount(const int&) ) );
        
        //Get signals about account name changes
        connect(mEngine, SIGNAL( accountNameChanged(const QString&) )
                ,mTitleRow, SLOT( updateAccountName(const QString&) ) );

	    //Get signals about user actions
	    connect(mTitleRow, SIGNAL( mailboxLaunchTriggered() )
	            ,mEngine, SLOT( launchMailAppInboxView() ) );
	    connect(mTitleRow, SIGNAL( expandCollapseButtonPressed() )
	            ,this, SLOT( handleExpandCollapseEvent() ) );

    }
    QT_CATCH(...) {
        emit error();
    }

}


/*!
 updateMailData slot
 */
void NmHsWidget::updateMailData()
{
    NM_FUNCTION;

    QList<NmMessageEnvelope> envelopes;
    int count = 0;
    if (mIsExpanded) {
        count = mEngine->getEnvelopes(envelopes, KMaxNumberOfMailsShown);
    }

    updateLayout(count);
    //count is safe for envelopes and mMailRows
    for (int i = 0; i < count; i++) {
        mMailRows.at(i)->updateMailData(envelopes.at(i));
    }
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
    toggleExpansionState();
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
    updateMailData();

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
 Updates mMailRows list to include KMaxNumberOfMailsShown mail row widgets
 /post mMailRows contains KMaxNumberOfMailsShown mailRows 
 */
void NmHsWidget::createMailRowsList()
{
    NM_FUNCTION;

    //make sure that there are as many email rows as needed
    while (mMailRows.count() < KMaxNumberOfMailsShown) {
        NmHsWidgetEmailRow *row = new NmHsWidgetEmailRow(this);
        if (!row->setupUI()) {
            NM_ERROR(1, "NmHsWidget::createMailRowsList row->setUpUI() fails");
            //if setupUI fails no point to proceed
            emit error();
            return;
        }
        connect(row, SIGNAL(mailViewerLaunchTriggered(const NmId&)), mEngine,
            SLOT(launchMailAppMailViewer(const NmId&)));
        connect(mDateObserver, SIGNAL(dateTimeChanged()), row, SLOT(updateDateTime()));
        mMailRows.append(row);
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
void NmHsWidget::updateLayout(const int mailCount)
{
    NM_FUNCTION;
    if (mIsExpanded) {
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
    }

    //resize the widget to new layout size
    setPreferredSize(mRowLayout->preferredSize());

    updateMailRowsVisibility(mailCount);
}

/*!
 Updates mNoMailsLabel visibility based on widget state
 /param mailCount defines how many mail rows is needed
 /post if mail count is 0 and mIsExpanded equals true, then
 the mNoMailLabel is added to the mRowLayout. 
 */
void NmHsWidget::addNoMailsLabelToLayout()
{
    NM_FUNCTION;
    //Use sizes defined for titlerow and mailrow docml to indentify the correct size
    //for the mNoMailslabel
    if (mNoMailsLabel->isVisible() || mMailRows.isEmpty()) {
        return;
    }
    QSizeF mailLabelSize(mTitleRow->maximumWidth(), KMaxNumberOfMailsShown
        * mMailRows.first()->maximumHeight());
    mNoMailsLabel->setPreferredSize(mailLabelSize);
    mNoMailsLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //Add mNoMailsLabel to layout if not yet there and show it
    mRowLayout->addItem(mNoMailsLabel);
    //resize the widget to new layout size
    mNoMailsLabel->show();
}

/*!
 removeNoMailsLabelFromLayout removes mNoMailsLabel from the layout
 /post mNoMailsLabel is not in mRowLayout
 */
void NmHsWidget::removeNoMailsLabelFromLayout()
{
    NM_FUNCTION;
    //remove mNoMailsLabel from Layout and hide it
    mRowLayout->removeItem(mNoMailsLabel);
    mNoMailsLabel->hide();
}

/*!
 addEmailRowsToLayout adds every emailrow to the layout
 /post all elements in mMailRows are added to mRowLayout
 */
void NmHsWidget::addEmailRowsToLayout()
{
    NM_FUNCTION;
    foreach(NmHsWidgetEmailRow *row, mMailRows)
        {
            mRowLayout->addItem(row);
        }
}

/*!
 removeEmailRowsFromLayout removes every emailrow from the layout
 /post none of the elements in mMailRows are in mRowLayout
 */
void NmHsWidget::removeEmailRowsFromLayout()
{
    NM_FUNCTION;
    foreach(NmHsWidgetEmailRow *row, mMailRows)
        {
            mRowLayout->removeItem(row);
        }
}

/*!
 Updates mail row visibilities in static widget
 /param visibleCount defines how many items do have mail data
 /post all row items having mail data are visible, other rows are hidden
 */
void NmHsWidget::updateMailRowsVisibility(const int visibleCount)
{
    NM_FUNCTION;

    // set visible as many rows as requested by visibleCount param
    bool isVisible;

    for (int i = 0; i < mMailRows.count(); i++) {
        isVisible = false;
        if ((mIsExpanded) && (i < visibleCount)) {
            isVisible = true;
        }
        mMailRows.at(i)->setVisible(isVisible);
    }
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
